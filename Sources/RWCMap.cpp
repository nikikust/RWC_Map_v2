/**
 * Copyright 2023 Glazunov Nikita <lis291@yandex.ru>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../Include/RWCMap.h"


RWCMap::RWCMap(const std::string& app_title)
    : window_      (data_storage_, app_title),
      data_loader_ (data_storage_),
      data_editor_ (data_storage_, window_),
      renderer_    (data_storage_, data_loader_, window_),
      interface_   (data_storage_, data_loader_, window_, data_storage_diff_),

      data_loader_diff_(data_storage_diff_) {}
RWCMap::~RWCMap()
{
    
}


int RWCMap::run()
{
    int result = load_data();

    if (result != 0)
        return result;

    return main_loop();
}

int RWCMap::load_data()
{
    std::cout << "Init data loader ... ";
    if (data_loader_.init() != 0)
        return 1;
    std::cout << "Done" << std::endl;

    std::cout << "\nLoading current data ... " << std::endl;
    if (data_loader_.load_railroads_data("Data\\data.rr") != 0)
        return 1;

    std::cout << "\nLoading reference data ... " << std::endl;
    if (data_loader_diff_.load_railroads_data("Data\\data-backup-for-differentiated-length-top.rr") != 0)
        return 1;

    std::cout << "Loading completed" << std::endl;

    return 0;
}
int RWCMap::main_loop()
{
    while (window_.is_open())
    {
        // --- Poll
        poll_events   ();
        process_inputs();

        // --- Update
        window_   .update();
        renderer_ .update();
        interface_.update();

        // --- Draw
        window_.cls();
        renderer_.draw();
        window_.flip();

        data_editor_.check_screenshot_status();
    }

    return 0;
}

void RWCMap::poll_events()
{
    sf::Event event {};

    while (window_.poll_event(event))
    {
        ImGui::SFML::ProcessEvent(event);

        switch (event.type)
        {
        case sf::Event::Closed:
            window_.close();
            break;
        case sf::Event::Resized:
            window_.on_resize(event);
            break;
        case sf::Event::MouseButtonPressed:
            if (utils::in(0, event.mouseButton.button, sf::Mouse::ButtonCount - 1))
                utils::mouse_states[event.mouseButton.button] = utils::ButtonState::Pressed;
            break;
        case sf::Event::MouseButtonReleased:
            if (utils::in(0, event.mouseButton.button, sf::Mouse::ButtonCount - 1))
                utils::mouse_states[event.mouseButton.button] = utils::ButtonState::Released;
            break;
        case sf::Event::KeyPressed:
            if (utils::in(0, event.key.code, sf::Keyboard::KeyCount - 1))
                utils::key_states[event.key.code] = utils::ButtonState::Pressed;
            break;
        case sf::Event::KeyReleased:
            if (utils::in(0, event.key.code, sf::Keyboard::KeyCount - 1))
                utils::key_states[event.key.code] = utils::ButtonState::Released;
            break;
        default:
            break;
        }
    }
}
void RWCMap::process_inputs()
{
    // --- Camera movement

    if (utils::mouse_pressed  (sf::Mouse::Left) && data_editor_.mouse_io_is_released())
    {
        data_editor_.save_mouse_position_on_click();

        if (data_editor_.is_add_line_mode())
        {
            if (data_editor_.hovered_point())
            {
                data_editor_.create_line();
            }
            else if (data_editor_.hovered_line())
            {
                data_editor_.split_line();

                data_editor_.create_line();
            }
            else
            {
                data_editor_.create_point();

                data_editor_.create_line();
            }

            if (data_storage_.menus.fields.lines_edit.auto_line_repeat)
                data_editor_.select_overlapped_point();
            else
                data_editor_.release_point();
        }
        else
        {
            if (data_editor_.hovered_point())
                data_editor_.start_moving_point();
            else if (data_editor_.hovered_line())
                data_editor_.start_railroad_selection();
            else
                data_editor_.start_moving_camera();
        }
    }
    if (utils::mouse_down     (sf::Mouse::Left))
    {
        auto mouse_diff = window_.get_cursor_position() - data_storage_.status.mouse_position_on_click;

        mouse_diff.x = int(mouse_diff.x / data_storage_.camera.scale_modifier_as_pow2);
        mouse_diff.y = int(mouse_diff.y / data_storage_.camera.scale_modifier_as_pow2);


        if (data_editor_.is_camera_moving_mode())
        {
            data_editor_.move_camera(mouse_diff);

            if (!data_editor_.mouse_io_is_released())
                data_editor_.restore_camera_position();
        }
        else if (data_editor_.is_point_moving_mode())
        {
            data_editor_.move_point(mouse_diff);

            if (!data_editor_.mouse_io_is_released())
                data_editor_.restore_point_position();
        }
    }
    if (utils::mouse_released (sf::Mouse::Left) && data_editor_.mouse_io_is_released())
    {
        if (data_editor_.is_railroad_selection_mode())
        {
            if (data_editor_.mouse_kept_position())
            {
                if (data_editor_.hovered_line())
                    data_editor_.select_railroad();
                else
                    data_editor_.release_railroad();
            }
        }
        else if (data_editor_.is_point_moving_mode())
        {
            if (data_editor_.mouse_kept_position())
            {
                if (!data_editor_.railroad_is_selected())
                    data_editor_.create_message("Select a Railroad to add line!");
                else if (!data_editor_.zone_is_selected())
                    data_editor_.create_message("Select a RR Zone to add line!");
                else
                    data_editor_.start_add_line_mode();

                data_storage_.status.movement_mode = DataStorage::Status::MovementMode::IDLE;
            }
            else
            {
                data_editor_.release_point();
            }
        }
        else if (data_editor_.is_camera_moving_mode())
        {
            if (data_editor_.mouse_kept_position())
                data_editor_.release_railroad();

            data_editor_.stop_moving_camera();
        }
    }
                              
    if (utils::mouse_pressed  (sf::Mouse::Right) && data_editor_.mouse_io_is_released())
    {
        if (data_editor_.is_add_line_mode())
        {
            data_editor_.save_mouse_position_on_click();

            if (!data_editor_.hovered_point())
                data_editor_.start_moving_camera();
        }
    }
    if (utils::mouse_down     (sf::Mouse::Right))
    {
        if (data_editor_.is_add_line_mode())
        {
            auto mouse_diff = window_.get_cursor_position() - data_storage_.status.mouse_position_on_click;

            mouse_diff.x = int(mouse_diff.x / data_storage_.camera.scale_modifier_as_pow2);
            mouse_diff.y = int(mouse_diff.y / data_storage_.camera.scale_modifier_as_pow2);

            if (data_editor_.is_camera_moving_mode())
            {
                data_editor_.move_camera(mouse_diff);

                if (!data_editor_.mouse_io_is_released())
                    data_editor_.restore_camera_position();
            }
        }
    }
    if (utils::mouse_released (sf::Mouse::Right) && data_editor_.mouse_io_is_released())
    {
        if (data_editor_.is_add_line_mode())
            if (data_editor_.is_camera_moving_mode())
                data_editor_.stop_moving_camera();
    }

    // --- Zooming
    if (utils::key_pressed(sf::Keyboard::Subtract))
    {
        if (data_editor_.keyboard_io_is_released())
            data_editor_.zoom_out();
    }
    if (utils::key_pressed(sf::Keyboard::Add))
    {
        if (data_editor_.keyboard_io_is_released())
            data_editor_.zoom_in();
    }
    if (utils::key_pressed(sf::Keyboard::LBracket))
    {
        if (data_editor_.keyboard_io_is_released())
            data_editor_.zoom_out();
    }
    if (utils::key_pressed(sf::Keyboard::RBracket))
    {
        if (data_editor_.keyboard_io_is_released())
            data_editor_.zoom_in();
    }

    // --- Hotkeys
    if (utils::key_pressed(sf::Keyboard::F1 ))
    {
        data_storage_.menus.Help = 1 - data_storage_.menus.Help;
    }
    if (utils::key_pressed(sf::Keyboard::F2 ))
    {
        data_storage_.menus.LinesEdit = 1 - data_storage_.menus.LinesEdit;
    }
    if (utils::key_pressed(sf::Keyboard::F3 ))
    {
        data_storage_.menus.Settings = 1 - data_storage_.menus.Settings;
    }
    if (utils::key_pressed(sf::Keyboard::F4 ))
    {
        data_storage_.menus.Railroads = 1 - data_storage_.menus.Railroads;
    }
    if (utils::key_pressed(sf::Keyboard::F5 ))
    {
        data_storage_.menus.Players = 1 - data_storage_.menus.Players;
    }
    if (utils::key_pressed(sf::Keyboard::F6 ))
    {
        data_storage_.menus.LengthTop = 1 - data_storage_.menus.LengthTop;

        data_storage_.menus.fields.length_top.initialized = false;
    }
    if (utils::key_pressed(sf::Keyboard::F10))
    {
        data_storage_.menus.Debug = 1 - data_storage_.menus.Debug;
    }

    if (utils::key_pressed(sf::Keyboard::M))
    {
        if (data_editor_.keyboard_io_is_released())
            data_editor_.open_livemap();
    }
    if (utils::key_pressed(sf::Keyboard::P))
    {
        if (data_editor_.keyboard_io_is_released())
            if (data_storage_.settings.disable_interface_on_screenshot)
                data_editor_.screenshot_without_interface();
            else
                data_editor_.screenshot_with_interface();
    }
    if (utils::key_pressed(sf::Keyboard::H))
    {
        if (data_editor_.keyboard_io_is_released() && data_editor_.railroad_is_selected())
            data_storage_.selection_info.railroad.lock()->hide = 1 - data_storage_.selection_info.railroad.lock()->hide;
    }

    if (utils::key_pressed(sf::Keyboard::A))
    {
        if (data_editor_.keyboard_io_is_released() && data_editor_.mouse_io_is_released())
        {
            if (data_editor_.hovered_line())
                data_editor_.split_line();
            else
                data_editor_.create_point();
        }
    }
    if (utils::key_pressed(sf::Keyboard::D) ||
        utils::key_pressed(sf::Keyboard::Delete))
    {
        if (data_editor_.keyboard_io_is_released())
        {
            if (data_editor_.hovered_line())
                data_editor_.delete_hovered_line();

            if (data_editor_.hovered_point() && !data_editor_.is_point_moving_mode())
            {
                if (!(data_editor_.is_add_line_mode() && 
                      data_storage_.status.overlapped_point.lock()->id == data_storage_.selection_info.point.lock()->id))
                    data_editor_.delete_hovered_point();
            }
        }
    }

    if (utils::key_pressed(sf::Keyboard::Escape))
    {
        if (data_editor_.message_is_opened())
            data_editor_.close_message();
        else if (utils::mouse_down(sf::Mouse::Left))
        {
            if (data_editor_.is_camera_moving_mode())
                data_editor_.restore_camera_position();
            else if (data_editor_.is_point_moving_mode())
                data_editor_.restore_point_position();
        }
        else if (data_editor_.is_add_line_mode())
            data_editor_.stop_add_line_mode();
        else
            data_editor_.flip_exit_popup_state();
    }

    if (utils::key_down(sf::Keyboard::LControl))
    {
        if (data_editor_.keyboard_io_is_released())
        {
            if (utils::key_pressed(sf::Keyboard::S))
                data_loader_.save_railroads_data();
        }
    }
    // --- //
}

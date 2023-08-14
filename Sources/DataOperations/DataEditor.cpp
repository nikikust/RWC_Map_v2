#include "../../Include/DataOperations/DataEditor.h"


DataEditor::DataEditor(DataStorage& data_storage, Window& window)
    : data_storage_(data_storage),
      window_      (window)
{

}

DataEditor::~DataEditor()
{

}

// --- //

void DataEditor::save_mouse_position_on_click()
{
    data_storage_.status.mouse_position_on_click = window_.get_cursor_position();
}
bool DataEditor::mouse_kept_position()
{
    return data_storage_.status.mouse_position_on_click == window_.get_cursor_position();
}

void DataEditor::select_overlapped_point()
{
    data_storage_.status.point_position_on_click = data_storage_.status.overlapped_point.lock()->position;

    data_storage_.selection_info.point = data_storage_.status.overlapped_point;
}
void DataEditor::release_point()
{
    data_storage_.selection_info.point.reset();

    data_storage_.status.movement_mode = DataStorage::Status::MovementMode::IDLE;
}
void DataEditor::restore_point_position()
{
    data_storage_.selection_info.point.lock()->position = data_storage_.status.point_position_on_click;

    data_storage_.status.movement_mode = DataStorage::Status::MovementMode::IDLE;
}
void DataEditor::start_moving_point()
{
    select_overlapped_point();

    data_storage_.status.movement_mode = DataStorage::Status::MovementMode::MovingPoint;
}
void DataEditor::move_point(sf::Vector2i delta)
{
    if (data_storage_.menus.fields.lines_edit.align_at_movement == true)
    {
        bool horizontal = utils::is_horizontal_move(delta);

        delta.x = (horizontal ? delta.x : 0);
        delta.y = (horizontal ? 0 : delta.y);
    }

    auto point = data_storage_.selection_info.point.lock();
    point->position = data_storage_.status.point_position_on_click + delta;
}

void DataEditor::start_railroad_selection()
{
    data_storage_.status.movement_mode = DataStorage::Status::MovementMode::RailroadSelection;
}
void DataEditor::select_railroad()
{
    data_storage_.menus.fields.railroad_info.initialized = false;
    data_storage_.menus.fields.lines_edit   .initialized = false;

    auto railroad = data_storage_.status.overlapped_line.lock()->zone.lock()->railroad.lock();

    data_storage_.selection_info.railroad = railroad;
    data_storage_.selection_info.zone.reset();

    data_storage_.menus.RailroadInfo = true;

    data_storage_.status.movement_mode = DataStorage::Status::MovementMode::IDLE;
}
void DataEditor::release_railroad()
{
    data_storage_.selection_info.railroad.reset();
    data_storage_.selection_info.zone.reset();

    data_storage_.menus.fields.railroad_info.initialized = false;
    data_storage_.menus.fields.lines_edit.initialized = false;

    data_storage_.status.movement_mode = DataStorage::Status::MovementMode::IDLE;
}

void DataEditor::save_camera_position()
{
    data_storage_.status.camera_position_on_click = data_storage_.camera.position;
}
void DataEditor::restore_camera_position()
{
    data_storage_.camera.position = data_storage_.status.camera_position_on_click;

    data_storage_.status.movement_mode = DataStorage::Status::MovementMode::IDLE;
}
void DataEditor::start_moving_camera()
{
    save_camera_position();

    data_storage_.status.movement_mode = DataStorage::Status::MovementMode::MovingCamera;
}
void DataEditor::stop_moving_camera()
{
    data_storage_.status.movement_mode = DataStorage::Status::MovementMode::IDLE;
}
void DataEditor::move_camera(sf::Vector2i delta)
{
    data_storage_.camera.position = data_storage_.status.camera_position_on_click - delta;
}

bool DataEditor::mouse_io_is_released()
{
    return !data_storage_.status.mouse_captured;
}
bool DataEditor::keyboard_io_is_released()
{
    return !data_storage_.status.keyboard_captured;
}
bool DataEditor::hovered_point()
{
    return !data_storage_.status.overlapped_point.expired();
}
bool DataEditor::hovered_line()
{
    return !data_storage_.status.overlapped_line.expired();
}

bool DataEditor::is_idle_mode()
{
    return data_storage_.status.movement_mode == DataStorage::Status::MovementMode::IDLE;
}
bool DataEditor::is_camera_moving_mode()
{
    return data_storage_.status.movement_mode == DataStorage::Status::MovementMode::MovingCamera;
}
bool DataEditor::is_railroad_selection_mode()
{
    return data_storage_.status.movement_mode == DataStorage::Status::MovementMode::RailroadSelection;
}
bool DataEditor::is_point_moving_mode()
{
    return data_storage_.status.movement_mode == DataStorage::Status::MovementMode::MovingPoint;
}

bool DataEditor::railroad_is_selected()
{
    return !data_storage_.selection_info.railroad.expired();
}
bool DataEditor::zone_is_selected()
{
    return !data_storage_.selection_info.zone.expired();
}

void DataEditor::start_add_line_mode()
{
    data_storage_.status.edit_mode = DataStorage::Status::EditMode::AddLine;
}
bool DataEditor::is_add_line_mode()
{
    return data_storage_.status.edit_mode == DataStorage::Status::EditMode::AddLine;
}
void DataEditor::stop_add_line_mode()
{
    data_storage_.status.edit_mode = DataStorage::Status::EditMode::IDLE;
}

void DataEditor::create_point()
{
    sf::Vector2i cursor_position = window_.get_cursor_position() - data_storage_.screen_size / 2;

    cursor_position.x = int(cursor_position.x / data_storage_.camera.scale_modifier_as_pow2);
    cursor_position.y = int(cursor_position.y / data_storage_.camera.scale_modifier_as_pow2);

    cursor_position += data_storage_.camera.position;


    if (data_storage_.menus.fields.lines_edit.align_at_creation == true && is_add_line_mode() &&
        !data_storage_.selection_info.point.expired())
    {
        auto point_pos       = data_storage_.selection_info.point.lock()->position;
        auto new_point_shift = cursor_position - point_pos;

        bool horizontal = utils::is_horizontal_move(new_point_shift);

        new_point_shift.x = (horizontal ? new_point_shift.x : 0);
        new_point_shift.y = (horizontal ? 0 : new_point_shift.y);

        cursor_position = point_pos + new_point_shift;
    }


    RR_Point new_point{ 0, cursor_position };

    auto id = data_storage_.railroads_data.add_RR_Point(new_point);

    data_storage_.status.overlapped_point = data_storage_.railroads_data.get_RR_Point_by_ID(id);
}
void DataEditor::create_line()
{
    RR_Line new_line{
        0,
        data_storage_.selection_info.point,
        data_storage_.status.overlapped_point,
        data_storage_.selection_info.zone
    };

    auto id = data_storage_.railroads_data.add_RR_Line(new_line);

    auto line = data_storage_.railroads_data.get_RR_Line_by_ID(id);

    link_points_and_line(
        data_storage_.selection_info.point,
        data_storage_.status.overlapped_point,
        line
    );
}
void DataEditor::split_line()
{
    // --- Current selection
    auto current_railroad = data_storage_.selection_info.railroad; //
    auto current_zone     = data_storage_.selection_info.zone    ; // is swap copy
    auto current_point    = data_storage_.selection_info.point   ; //

    // --- Splitted line info
    auto splitted_line = data_storage_.status.overlapped_line.lock();

    auto pointA = splitted_line->pointA.lock();
    auto pointB = splitted_line->pointB.lock();

    auto second_zone     = splitted_line->zone    .lock();
    auto second_railroad = second_zone  ->railroad.lock();

    splitted_line->erase_from_lists(data_storage_);
    splitted_line.reset();

    // --- Create new lines

    create_point();

    data_storage_.selection_info.railroad = second_railroad;
    data_storage_.selection_info.zone     = second_zone;

    data_storage_.selection_info.point = pointA;

    create_line();

    data_storage_.selection_info.point = pointB;

    create_line();

    // --- Return old selection

    data_storage_.selection_info.railroad = current_railroad;
    data_storage_.selection_info.zone     = current_zone;

    data_storage_.selection_info.point    = current_point;
}
void DataEditor::create_message(const std::string& message, int timeout)
{
    data_storage_.menus.CursorMessage = true;
    data_storage_.menus.fields.cursor_message.elapse_at = time(0) + timeout;
    data_storage_.menus.fields.cursor_message.message = message;
}

bool DataEditor::message_is_opened()
{
    return data_storage_.menus.CursorMessage == true;
}

void DataEditor::delete_hovered_line()
{
    auto line = data_storage_.status.overlapped_line.lock();

    line->erase_from_lists(data_storage_);
}
void DataEditor::delete_hovered_point()
{
    auto point = data_storage_.status.overlapped_point.lock();

    point->erase_from_lists(data_storage_);

    data_storage_.railroads_data.erase_RR_Point_by_ID(point->id);
}
void DataEditor::close_message()
{
    data_storage_.menus.fields.cursor_message.elapse_at = time(0);
}

void DataEditor::zoom_out()
{
    if (data_storage_.settings.super_scale)
    {
        data_storage_.camera.scale_modifier = utils::minmax(
            -2.f - 100 * data_storage_.settings.super_scale,
             data_storage_.camera.scale_modifier - 1,
             3.f + 100 * data_storage_.settings.super_scale
        );
    }
    else
    {
        data_storage_.camera.scale_modifier = utils::minmax(
            -2.f,
             round(data_storage_.camera.scale_modifier) - 1,
             3.f
        );
    }

    data_storage_.camera.scale_modifier_as_pow2 = powf(2, data_storage_.camera.scale_modifier);

    data_storage_.map_data.reset_images_states();
}
void DataEditor::zoom_in()
{
    if (data_storage_.settings.super_scale)
    {
        data_storage_.camera.scale_modifier = utils::minmax(
            -2.f - 100 * data_storage_.settings.super_scale,
             data_storage_.camera.scale_modifier + 1,
             3.f + 100 * data_storage_.settings.super_scale
        );
    }
    else
    {
        data_storage_.camera.scale_modifier = utils::minmax(
            -2.f,
            round(data_storage_.camera.scale_modifier) + 1,
            3.f
        );
    }

    data_storage_.camera.scale_modifier_as_pow2 = powf(2, data_storage_.camera.scale_modifier);

    data_storage_.map_data.reset_images_states();
}

void DataEditor::open_livemap()
{
    std::string livemap_link = utils::to_map_coords(
        data_storage_.camera.position, utils::minmax(0, (int)data_storage_.camera.scale_modifier + 2, 5));

    ShellExecute(0, 0, std::wstring(livemap_link.begin(), livemap_link.end()).c_str(), 0, 0, SW_SHOW);
}

void DataEditor::screenshot_with_interface()
{
    data_storage_.status.make_screenshot         = true;
    data_storage_.status.screenshot_frames_delay = 0;
}
void DataEditor::screenshot_without_interface()
{
    data_storage_.status.make_screenshot         = true;
    data_storage_.status.screenshot_frames_delay = 2;
}
void DataEditor::check_screenshot_status()
{
    if (data_storage_.status.make_screenshot == false)
        return;

    if (data_storage_.status.screenshot_frames_delay > 0)
    {
        data_storage_.status.screenshot_frames_delay--;

        return;
    }

    make_screenshot();

    data_storage_.status.make_screenshot = false;
}
void DataEditor::make_screenshot()
{
    auto& render_window = window_.get_render_area();

    sf::Texture texture;
    texture.create(render_window.getSize().x, render_window.getSize().y);
    texture.update(render_window);

    texture.copyToImage().saveToFile("Data/screenshots/" + utils::get_time_string() + ".png");
}

void DataEditor::flip_exit_popup_state()
{
    data_storage_.menus.ConfirmExit.should_be_open = !data_storage_.menus.ConfirmExit.is_open;
}

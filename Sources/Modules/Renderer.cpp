#include "../../Include/Modules/Renderer.h"


Renderer::Renderer(DataStorage& data_storage, DataLoader& data_loader, Window& window)
    : data_storage_(data_storage),
      data_loader_ (data_loader),
      window_      (window),
      painter_     (data_storage_, window)
{

}
Renderer::~Renderer()
{

}


void Renderer::update()
{
    auto& map_data = data_storage_.map_data;
    auto& camera   = data_storage_.camera;

    sf::Vector2i screen_size_scaled {
        int(data_storage_.screen_size.x / camera.scale_modifier_as_pow2),
        int(data_storage_.screen_size.y / camera.scale_modifier_as_pow2)
    };

    map_data.central_view_sector = position_to_sector(camera.position);
    map_data.view_from           = position_to_sector(camera.position - screen_size_scaled);
    map_data.view_to             = position_to_sector(camera.position + screen_size_scaled);

    if (map_data.loading_images == false && !view_area_is_loaded())
    {
        map_data.loading_images = true;
    
        std::thread load_map_thread(&DataLoader::load_map, &data_loader_, map_data.view_from, map_data.view_to);
        load_map_thread.detach();
    }
}

void Renderer::draw()
{
    draw_map    ();
    draw_lines  ();
    draw_points ();
}


void Renderer::draw_map   ()
{
    if (data_storage_.settings.hide_map == true)
        return;

    // --- //
    
    auto& camera   = data_storage_.camera;
    auto& map_data = data_storage_.map_data;

    sf::Vector2i &from = map_data.view_from; // copy to clip
    sf::Vector2i &to   = map_data.view_to;   // 

    // --- //

    // from.x = std::max(0, from.x);
    // from.y = std::max(0, from.y);
    // 
    // to.x = std::min(to.x, SPRITES_AMOUNT_HORIZONTAL - 1);
    // to.y = std::min(to.y, SPRITES_AMOUNT_VERTICAL   - 1);

    // --- //

    for (int vertical = 0; vertical < SPRITES_AMOUNT_VERTICAL; ++vertical)
    {
        for (int horizontal = 0; horizontal < SPRITES_AMOUNT_HORIZONTAL; ++horizontal)
        {
            int index = vertical * SPRITES_AMOUNT_HORIZONTAL + horizontal;

            if (map_data.map_images[index].container_state.load() != ImageContainer::LOADED)
                continue;

            sf::Vector2f shifted_camera_position {
                -camera.position.x * camera.scale_modifier_as_pow2 - map_data.sprite_0_0_pos_shift.x * camera.scale_modifier_as_pow2,
                -camera.position.y * camera.scale_modifier_as_pow2 - map_data.sprite_0_0_pos_shift.y * camera.scale_modifier_as_pow2,
            };

            shifted_camera_position += sf::Vector2f {
                map_data.sprite_size.x * camera.scale_modifier_as_pow2 * (horizontal - map_data.sprite_with_0_0_pos.x),
                map_data.sprite_size.y * camera.scale_modifier_as_pow2 * (vertical   - map_data.sprite_with_0_0_pos.y)
            };

            shifted_camera_position += sf::Vector2f(data_storage_.screen_size / 2);

            if (utils::in(from.x, horizontal, to.x) && utils::in(from.y, vertical, to.y))
            {
                painter_.draw_image(shifted_camera_position, index);
            }
            else
            {
                map_data.map_images[index].current_map_spr.reset();
                map_data.map_images[index].current_map_tex.reset();

                map_data.map_images[index].difference_map_spr.reset();
                map_data.map_images[index].difference_map_tex.reset();

                map_data.map_images[index].old_map_spr.reset();
                map_data.map_images[index].old_map_tex.reset();

                map_data.map_images[index].container_state.store(ImageContainer::EMPTY);
            }
        }
    }
}
void Renderer::draw_lines ()
{
    if (data_storage_.menus.fields.lines_edit.show_lines == false)
        return;


    // --- Preparation
    auto mouse_pos          = window_.get_cursor_position();
    auto screen_size_halved = data_storage_.screen_size / 2;
    auto brush_shift        = screen_size_halved;
    auto screen_radius      = utils::pif(screen_size_halved);


    // --- Lines
    data_storage_.status.overlapped_line.reset();

    for (auto& line : data_storage_.railroads_data.RR_Lines)
    {
        auto  zone = line->zone.lock();
        // - Hide if railroad is hidden
        if (!data_storage_.settings.show_hidden_RRs && zone->railroad.lock()->hide)
            continue;

        // - Hide if line type is hidden
        auto& fields = data_storage_.menus.fields.lines_edit;

        if (!fields.show_Plans      && zone->state == RR_Zone::Plan       ||
            !fields.show_InProgress && zone->state == RR_Zone::InProgress || 
            !fields.show_Built      && zone->state == RR_Zone::Built      )
        {
            continue;
        }

        // - Calculate points screen position
        auto pointA_pos = line->pointA.lock()->position - data_storage_.camera.position;
        auto pointB_pos = line->pointB.lock()->position - data_storage_.camera.position;

        pointA_pos.x = int(pointA_pos.x * data_storage_.camera.scale_modifier_as_pow2);
        pointA_pos.y = int(pointA_pos.y * data_storage_.camera.scale_modifier_as_pow2);

        pointB_pos.x = int(pointB_pos.x * data_storage_.camera.scale_modifier_as_pow2);
        pointB_pos.y = int(pointB_pos.y * data_storage_.camera.scale_modifier_as_pow2);

        pointA_pos += brush_shift;
        pointB_pos += brush_shift;

        // - Should we draw it?
        if (utils::pif(pointB_pos - pointA_pos) < 1 ||
            utils::distance(pointA_pos, pointB_pos, screen_size_halved) > screen_radius)
            continue;

        // - Setup line width
        float width = 4.f;

        if (data_storage_.status.edit_mode == DataStorage::Status::EditMode::AddLine)
        {
            width = 4.f;
        }
        else
        {
            if (!data_storage_.selection_info.railroad.expired())
            {
                if (zone->railroad.lock()->id == data_storage_.selection_info.railroad.lock()->id)
                {
                    width = 8.f;

                    if (!data_storage_.selection_info.zone.expired())
                        if (zone->id != data_storage_.selection_info.zone.lock()->id)
                            width = 4.f;
                }
            }
        }

        if (!data_storage_.status.mouse_captured)
        {
            if (data_storage_.status.overlapped_point.expired())
            {
                if (utils::distance(pointA_pos, pointB_pos, mouse_pos) <= 4 &&
                    utils::pif((pointA_pos + pointB_pos) / 2 - mouse_pos) <= utils::pif(pointB_pos - pointA_pos) / 2)
                {
                    width = 8.f;
                    data_storage_.status.overlapped_line = line;
                }
            }
        }
        
        // - Draw
        auto railroad = zone->railroad.lock();

        painter_.set_color(railroad->color);

        painter_.draw_line(pointA_pos, pointB_pos, width);
    }


    // --- Line Edit
    if (data_storage_.status.edit_mode == DataStorage::Status::EditMode::AddLine)
    {
        auto railroad        = data_storage_.selection_info.railroad.lock();
        auto pointA          = data_storage_.selection_info.point   .lock();
        auto cursor_position = window_.get_cursor_position();

        auto point_pos = pointA->position - data_storage_.camera.position;

        point_pos.x = int(point_pos.x * data_storage_.camera.scale_modifier_as_pow2);
        point_pos.y = int(point_pos.y * data_storage_.camera.scale_modifier_as_pow2);

        point_pos += brush_shift;


        if (data_storage_.status.overlapped_point.expired())
        {
            if (data_storage_.menus.fields.lines_edit.align_at_creation == true)
            {
                auto new_point_shift = cursor_position - point_pos;

                bool horizontal = utils::is_horizontal_move(new_point_shift);

                new_point_shift.x = (horizontal ? new_point_shift.x : 0);
                new_point_shift.y = (horizontal ? 0 : new_point_shift.y);

                cursor_position = point_pos + new_point_shift;
            }
        }
        else
        {
            auto point = data_storage_.status.overlapped_point.lock();
            auto point_pos = point->position - data_storage_.camera.position;

            point_pos.x = int(point_pos.x * data_storage_.camera.scale_modifier_as_pow2);
            point_pos.y = int(point_pos.y * data_storage_.camera.scale_modifier_as_pow2);

            point_pos += brush_shift;

            cursor_position = point_pos;
        }

        painter_.set_color(railroad->color);

        painter_.draw_line(point_pos, cursor_position, 4.f);
    }
}
void Renderer::draw_points()
{
    if (data_storage_.menus.fields.lines_edit.show_points == false)
        return;


    // --- Preparation
    auto mouse_pos          = window_.get_cursor_position();
    auto screen_size_halved = data_storage_.screen_size / 2;
    auto brush_shift        = screen_size_halved;
    auto screen_radius      = utils::pif(screen_size_halved);

    auto& fields = data_storage_.menus.fields.lines_edit;


    // --- Points
    data_storage_.status.overlapped_point.reset();

    painter_.set_color(sf::Color::White);

    for (auto& point : data_storage_.railroads_data.RR_Points)
    {
        // - Hide if all lines are hidden
        bool hide_point = true;

        if (data_storage_.settings.show_hidden_RRs)
            hide_point = false;
        else
        {
            for (auto& line : point->connected_lines)
            {
                if (line.lock()->zone.lock()->railroad.lock()->hide == false)
                {
                    hide_point = false;
                    break;
                }
            }
        }

        if (hide_point == false)
        {
            hide_point = true;

            for (auto& line : point->connected_lines)
            {
                auto zone = line.lock()->zone.lock();

                if (fields.show_Plans      == true && zone->state == RR_Zone::Plan       ||
                    fields.show_InProgress == true && zone->state == RR_Zone::InProgress ||
                    fields.show_Built      == true && zone->state == RR_Zone::Built      )
                {
                    hide_point = false;
                    break;
                }
            }
        }

        if (point->connected_lines.empty())
            hide_point = false;

        if (hide_point)
            continue;

        // - Calculate point screen position
        auto point_pos = point->position - data_storage_.camera.position;

        point_pos.x = int(point_pos.x * data_storage_.camera.scale_modifier_as_pow2);
        point_pos.y = int(point_pos.y * data_storage_.camera.scale_modifier_as_pow2);

        point_pos += brush_shift;

        // - Shoud we draw it?
        if (utils::pif(screen_size_halved - point_pos) > screen_radius)
            continue;

        // - Setup circle radius
        float radius = 3.f - (data_storage_.camera.scale_modifier_as_pow2 < 0.4f);

        if (!data_storage_.status.mouse_captured)
        {
            if (utils::pif(mouse_pos - point_pos) <= 6.f)
            {
                radius = 6.f;
                data_storage_.status.overlapped_point = point;
            }
        }

        // - Draw
        painter_.draw_point(point_pos, radius);
    }


    // --- Line Edit
    if (data_storage_.status.edit_mode == DataStorage::Status::EditMode::AddLine)
    {
        auto pointA          = data_storage_.selection_info.point.lock();
        auto cursor_position = window_.get_cursor_position();

        auto point_pos = pointA->position - data_storage_.camera.position;

        point_pos.x = int(point_pos.x * data_storage_.camera.scale_modifier_as_pow2);
        point_pos.y = int(point_pos.y * data_storage_.camera.scale_modifier_as_pow2);

        point_pos += brush_shift;


        if (data_storage_.status.overlapped_point.expired())
        {
            if (data_storage_.menus.fields.lines_edit.align_at_creation == true)
            {
                auto new_point_shift = cursor_position - point_pos;

                bool horizontal = utils::is_horizontal_move(new_point_shift);

                new_point_shift.x = (horizontal ? new_point_shift.x : 0);
                new_point_shift.y = (horizontal ? 0 : new_point_shift.y);

                cursor_position = point_pos + new_point_shift;
            }
        }
        else
        {
            auto point     = data_storage_.status.overlapped_point.lock();
            auto point_pos = point->position - data_storage_.camera.position;

            point_pos.x = int(point_pos.x * data_storage_.camera.scale_modifier_as_pow2);
            point_pos.y = int(point_pos.y * data_storage_.camera.scale_modifier_as_pow2);

            point_pos += brush_shift;

            cursor_position = point_pos;
        }

        painter_.draw_cursor_point(cursor_position);
    }
}

sf::Vector2i Renderer::position_to_sector(sf::Vector2i position)
{
    // Shifted relative corner of sprite with 0x0 map coordinates:
    auto shifted_position = position + data_storage_.map_data.sprite_0_0_pos_shift;

    sf::Vector2i sector = {
        data_storage_.map_data.sprite_with_0_0_pos.x + (int)floorf(shifted_position.x / (float)data_storage_.map_data.sprite_size.x),
        data_storage_.map_data.sprite_with_0_0_pos.y + (int)floorf(shifted_position.y / (float)data_storage_.map_data.sprite_size.y)
    };

    return sector;
}
bool Renderer::view_area_is_loaded()
{
    sf::Vector2i &from = data_storage_.map_data.view_from;
    sf::Vector2i &to   = data_storage_.map_data.view_to;

    for (int vertical = 0; vertical < SPRITES_AMOUNT_VERTICAL; ++vertical)
        for (int horizontal = 0; horizontal < SPRITES_AMOUNT_HORIZONTAL; ++horizontal)
            if (utils::in(from.x, horizontal, to.x) && utils::in(from.y, vertical, to.y))
                if (data_storage_.map_data.map_images[vertical * SPRITES_AMOUNT_HORIZONTAL + horizontal].container_state.load() == ImageContainer::EMPTY)
                    return 0;

    return 1;
}

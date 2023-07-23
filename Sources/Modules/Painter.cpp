#include "../../Include/Modules/Painter.h"


Painter::Painter(DataStorage& data_storage, Window& window)
    : data_storage_(data_storage),
      window_      (window)
{
    point_brush.setPointCount(8);

    point_add_cursor.setRadius(4);
    point_add_cursor.setOrigin(4, 4);
    point_add_cursor.setFillColor(sf::Color::Yellow);
    point_add_cursor.setOutlineColor(sf::Color::Red);
    point_add_cursor.setOutlineThickness(2);
}
Painter::~Painter()
{

}


void Painter::draw_point(sf::Vector2i position, float rad)
{
    point_brush.setPosition((float)position.x, (float)position.y);
    point_brush.setRadius(rad);
    point_brush.setOrigin(rad, rad);

    point_brush.setFillColor(brush_color);
    point_brush.setOutlineColor(sf::Color::White);
    point_brush.setOutlineThickness(1);

    window_.get_render_area().draw(point_brush);
}
void Painter::draw_line(sf::Vector2i A, sf::Vector2i B, float width)
{
    auto  diff = B - A;
    float lens = utils::pif(diff);
    float ang  = atan2((float)diff.y, (float)diff.x);

    A.x += lround(width / 2 * sin(ang));
    A.y -= lround(width / 2 * cos(ang));

    line_brush.setSize(sf::Vector2f(lens, width));
    line_brush.setPosition(sf::Vector2f((float)A.x, (float)A.y));
    line_brush.setRotation(float(ang / PI * 180.f));

    line_brush.setFillColor(brush_color);

    window_.get_render_area().draw(line_brush);
}
void Painter::draw_cursor_point(sf::Vector2i position)
{
    point_add_cursor.setPosition((float)position.x, (float)position.y);

    window_.get_render_area().draw(point_add_cursor);
}
void Painter::draw_image(sf::Vector2f position, int index)
{
    
    int   MIP_map_modifier = utils::minmax(1, (int)pow(2, (int)ceil(-data_storage_.camera.scale_modifier)), 32);
    float total_scale      = data_storage_.camera.scale_modifier_as_pow2 * MIP_map_modifier;

    std::shared_ptr<sf::Sprite> target_sprite;

    // --- //

    switch (data_storage_.settings.map_view)
    {
    case DataStorage::Settings::MapView::CURRENT:
        target_sprite = data_storage_.map_data.map_images[index].current_map_spr;
        break;
    case DataStorage::Settings::MapView::DIFFERENCE:
        target_sprite = data_storage_.map_data.map_images[index].difference_map_spr;
        break;
    case DataStorage::Settings::MapView::OLD:
        target_sprite = data_storage_.map_data.map_images[index].old_map_spr;
        break;
    default:
        target_sprite = data_storage_.map_data.map_images[index].current_map_spr;
        break;
    }

    // --- //

    target_sprite->setPosition(position);
    target_sprite->setScale(total_scale, total_scale);

    window_.get_render_area().draw(*target_sprite);
}

void Painter::set_color(sf::Color color)
{
    brush_color = color;
}

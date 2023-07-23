#pragma once
#include "../../Include/DataOperations/DataStorage.h"

#include "Window.h"


class Painter
{
public:
    Painter(DataStorage& data_storage, Window& window);
    Painter() = delete;
    Painter(Painter&) = delete;
    Painter(Painter&&) = delete;
    ~Painter();

    // --- //

    void draw_point(sf::Vector2i position, float rad);
    void draw_line(sf::Vector2i A, sf::Vector2i B, float width = 1.f);
    void draw_cursor_point(sf::Vector2i position);
    void draw_image(sf::Vector2f position, int index);

    void set_color(sf::Color color);

private:
    DataStorage& data_storage_;
    Window&      window_;

    sf::CircleShape point_add_cursor;
    sf::CircleShape point_brush;
    sf::RectangleShape line_brush;

    sf::Color brush_color {255, 255, 255, 255};
};

#pragma once
#include "../../Include/DataOperations/DataStorage.h"
#include "../../Include/DataOperations/DataLoader.h"

#include "Painter.h"


class Renderer
{
public:
    Renderer(DataStorage& data_storage, DataLoader& data_loader, Window& window);
    Renderer() = delete;
    Renderer(Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    ~Renderer();

    // --- //

    void update();
    void draw();

private:
    void draw_map();
    void draw_lines();
    void draw_points();

    sf::Vector2i position_to_sector(sf::Vector2i position);


    DataStorage& data_storage_;
    DataLoader&  data_loader_;
    Window&      window_;

    Painter painter_;
};

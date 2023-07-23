#pragma once
#include "../../Include/Utils/Functions.h"

#include <atomic>


struct ImageContainer
{
    enum ImageState
    {
        EMPTY     = 0,
        LOADING   = 1,
        LOADED    = 2
    };
    std::atomic<ImageState> container_state{ EMPTY };

    std::shared_ptr<sf::Texture> current_map_tex;
    std::shared_ptr<sf::Sprite>  current_map_spr;

    std::shared_ptr<sf::Texture> difference_map_tex;
    std::shared_ptr<sf::Sprite>  difference_map_spr;

    std::shared_ptr<sf::Texture> old_map_tex;
    std::shared_ptr<sf::Sprite>  old_map_spr;
};

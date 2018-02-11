/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/gameview/GameMap.h>
#include <gamebase/impl/graphics/Image.h>

namespace gamebase {

GameMap loadMap(const std::string& fname, const std::map<Color, int>& colorToType)
{
    auto image = impl::loadImageFromFile(fname);
    GameMap result;
    result.w = image->size.width;
    result.h = image->size.height;
    result.map.assign(result.w, std::vector<int>(result.h, 0));
    for (int y = 0; y < result.h; ++y) {
        for (int x = 0; x < result.w; ++x) {
            int offset = (y * result.w + x) * 4;
            Color c(
                image->data[offset],
                image->data[offset + 1],
                image->data[offset + 2],
                image->data[offset + 3]);
            auto it = colorToType.find(c);
            if (it != colorToType.end())
                result[x][result.h - y - 1] = it->second;
        }
    }
    return result;
}

}

/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/math/IntVector.h>
#include <gamebase/impl/graphics/Image.h>
#include <gamebase/graphics/Color.h>
#include <vector>
#include <map>

namespace gamebase {

template <typename ElemType>
struct GameMap {
    GameMap(): width(0), height(0) {}
    GameMap(GameMap<ElemType>&& other)
        : map(std::move(other.map))
        , width(other.width)
        , height(other.height)
    {}

    GameMap<ElemType>& operator=(GameMap<ElemType>&& other)
    {
        map = std::move(other.map);
        width = other.width;
        height = other.height;
        return *this;
    }

    ElemType get(int x, int y)
    {
        return map[x][y];
    }

    ElemType get(const IntVec2& v)
    {
        return map[v.x][v.y];
    }

    void set(int x, int y, const ElemType& elem)
    {
        map[x][y] = elem;
    }

    std::vector<std::vector<ElemType>> map;
    int width;
    int height;
};

template <typename ElemType>
std::vector<std::vector<ElemType>> createMap(int w, int h)
{
    return std::vector<std::vector<ElemType>>(
        w, std::vector<ElemType>(h, static_cast<ElemType>(0)));
}

template <typename ElemType>
GameMap<ElemType> loadMap(const std::string& fname, const std::map<Color, ElemType>& mapToElem)
{
    auto image = impl::loadImageFromFile(fname);
    GameMap<ElemType> result;
    result.width = image->size.width;
    result.height = image->size.height;
    result.map = createMap<ElemType>(result.width, result.height);
    for (int y = 0; y < result.height; ++y) {
        for (int x = 0; x < result.width; ++x) {
            int offset = (y * result.width + x) * 4;
            Color c(
                image->data[offset] / 255.0f,
                image->data[offset + 1] / 255.0f,
                image->data[offset + 2] / 255.0f,
                image->data[offset + 3] / 255.0f);
            auto it = mapToElem.find(c);
            if (it != mapToElem.end())
                result.map[x][result.height - y - 1] = it->second;
        }
    }
    return std::move(result);
}

}

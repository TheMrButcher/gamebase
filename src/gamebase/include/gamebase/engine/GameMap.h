#pragma once

#include <gamebase/graphics/Image.h>
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

    std::vector<std::vector<ElemType>> map;
    int width;
    int height;
};

template <typename ElemType>
GameMap<ElemType> loadMap(const std::string& fname, const std::map<Color, ElemType>& mapToElem)
{
    auto image = loadImageFromFile(fname);
    GameMap<ElemType> result;
    result.width = image->size.width;
    result.height = image->size.height;
    result.map.assign(result.width, std::vector<ElemType>(result.height, static_cast<ElemType>(0)));
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

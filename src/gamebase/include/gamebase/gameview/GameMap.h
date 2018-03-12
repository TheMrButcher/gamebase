/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/math/IntVector.h>
#include <gamebase/graphics/Color.h>
#include <vector>
#include <string>
#include <map>

namespace gamebase {

struct GameMap {
    const std::vector<int>& operator[](int x) const;
    std::vector<int>& operator[](int x);

    int operator[](const IntVec2& v) const;
    int& operator[](const IntVec2& v);

    int get(int x, int y) const;
    int get(const IntVec2& v) const;

    void set(int x, int y, int value);
    void set(const IntVec2& v, int value);

    std::vector<std::vector<int>> map;
    int w;
    int h;

    GameMap();
    GameMap(GameMap&& other);
    GameMap& operator=(GameMap&& other);
};

GAMEBASE_API GameMap createMap(int w, int h);
GAMEBASE_API GameMap loadMap(const std::string& fname, const std::map<Color, int>& colorToType);

/////////////// IMPLEMENTATION ///////////////////

inline const std::vector<int>& GameMap::operator[](int x) const { return map[x]; }
inline std::vector<int>& GameMap::operator[](int x) { return map[x]; }

inline int GameMap::operator[](const IntVec2& v) const { return map[v.x][v.y];  }
inline int& GameMap::operator[](const IntVec2& v) { return map[v.x][v.y];  }

inline int GameMap::get(int x, int y) const { return map[x][y]; }
inline int GameMap::get(const IntVec2& v) const { return get(v.x, v.y); }

inline void GameMap::set(int x, int y, int value) { map[x][y] = value; }
inline void GameMap::set(const IntVec2& v, int value) { set(v.x, v.y, value); }

inline GameMap::GameMap() : w(0), h(0) {}
inline GameMap::GameMap(GameMap&& other)
    : map(std::move(other.map))
    , w(other.w)
    , h(other.h)
{
    other.w = other.h = 0;
}
inline GameMap& GameMap::operator=(GameMap&& other)
{
    map = std::move(other.map);
    w = other.w;
    h = other.h;
    other.w = other.h = 0;
    return *this;
}

}

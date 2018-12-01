/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include "PolygonHelper.h"
#include <mapbox/earcut.hpp>

namespace mapbox { namespace util {

template <>
struct nth<0, gamebase::Vec2> {
	inline static float get(const gamebase::Vec2& v)
	{
		return v.x;
	};
};

template <>
struct nth<1, gamebase::Vec2> {
	inline static float get(const gamebase::Vec2& v)
	{
		return v.y;
	};
};

} }

namespace gamebase { namespace impl {

std::vector<uint16_t> triangulate(const std::vector<std::vector<Vec2>>& vertices)
{
	return mapbox::earcut<uint16_t>(vertices);
}

} }

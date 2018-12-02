/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/drawobj/Polygon.h>

namespace gamebase {

namespace {
std::vector<std::shared_ptr<impl::TexturedPolygonVertex>> convert(
	const std::vector<PolygonVertex>& vertices)
{
	std::vector<std::shared_ptr<impl::TexturedPolygonVertex>> result;
	result.reserve(vertices.size());
	for (const auto& vertex : vertices)
		result.push_back(std::make_shared<impl::TexturedPolygonVertex>(
			vertex.pos, impl::makeGLColor(vertex.color)));
	return result;
}
}

void Polygon::setContour(const std::vector<PolygonVertex>& vertices)
{
	m_impl->setOuterRing(convert(vertices));
}

void Polygon::addHole(const std::vector<PolygonVertex>& vertices)
{
	insertHole(static_cast<int>(m_impl->innerRingsCount()), vertices);
}

void Polygon::insertHole(int index, const std::vector<PolygonVertex>& vertices)
{
	if (index < 0)
		addHole(vertices);
	m_impl->setInnerRing(static_cast<size_t>(index), convert(vertices));
}

}

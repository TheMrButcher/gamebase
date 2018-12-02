/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/drawobj/TexturedPolygonVertex.h>
#include <vector>
#include <memory>

namespace gamebase { namespace impl {

class GAMEBASE_API TexturedPolygonRing : public ISerializable {
public:
	TexturedPolygonRing(std::vector<std::shared_ptr<TexturedPolygonVertex>> vertices)
		: m_vertices(std::move(vertices))
	{}

	const std::vector<std::shared_ptr<TexturedPolygonVertex>>& vertices() const { return m_vertices; }

	virtual void serialize(Serializer& s) const override;

private:
	std::vector<std::shared_ptr<TexturedPolygonVertex>> m_vertices;
};

} }

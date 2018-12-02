/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/drawobj/TexturedPolygonRing.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

void TexturedPolygonRing::serialize(Serializer& s) const
{
	s << "vertices" << m_vertices;
}

std::unique_ptr<IObject> deserializeTexturedPolygonRing(Deserializer& deserializer)
{
	DESERIALIZE(std::vector<std::shared_ptr<TexturedPolygonVertex>>, vertices);
	return std::make_unique<TexturedPolygonRing>(std::move(vertices));
}

REGISTER_CLASS(TexturedPolygonRing);

} }

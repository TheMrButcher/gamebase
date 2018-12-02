/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/drawobj/TexturedPolygonVertex.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

void TexturedPolygonVertex::serialize(Serializer& s) const
{
	s << "pos" << m_pos << "color" << m_color;
}

std::unique_ptr<IObject> deserializeTexturedPolygonVertex(Deserializer& deserializer)
{
	DESERIALIZE(Vec2, pos);
	DESERIALIZE(GLColor, color);
	return std::make_unique<TexturedPolygonVertex>(pos, color);
}

REGISTER_CLASS(TexturedPolygonVertex);

} }

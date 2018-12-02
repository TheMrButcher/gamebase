/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/graphics/GLColor.h>
#include <gamebase/impl/serial/ISerializable.h>
#include <gamebase/math/Vector2.h>

namespace gamebase { namespace impl {

class GAMEBASE_API TexturedPolygonVertex : public ISerializable {
public:
	TexturedPolygonVertex(const Vec2& v, const GLColor& color)
		: m_pos(v)
		, m_color(color)
	{}

	const Vec2& pos() { return m_pos; }
	const GLColor& color() { return m_color; }

	virtual void serialize(Serializer& s) const override;

private:
	Vec2 m_pos;
	GLColor m_color;
};

} }

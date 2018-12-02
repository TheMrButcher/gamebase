/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/drawobj/TexturedPolygon.h>
#include <gamebase/impl/pubhelp/Helpers.h>

namespace gamebase {

struct PolygonVertex {
	PolygonVertex() = default;
	PolygonVertex(float x, float y);
	PolygonVertex(const Vec2& pos);
	PolygonVertex(float x, float y, const Color& color);
	PolygonVertex(const Vec2& pos, const Color& color);

	Vec2 pos;
	Color color;
};

class GAMEBASE_API Polygon {
public:
	void setContour(const std::vector<PolygonVertex>& vertices);
	void addHole(const std::vector<PolygonVertex>& vertices);
	void insertHole(int index, const std::vector<PolygonVertex>& vertices);
	void clear();

	const std::string& imageName() const;
	void setImageName(const std::string& name);
	void setSize(float width, float height);
	void setSize(const Vec2& size);

	bool isVisible() const;
	void setVisible(bool value);
	void show();
	void hide();

	Vec2 pos() const;
	void setPos(float x, float y);
	void setPos(const Vec2& v);
	Box box() const;
	float width() const;
	float height() const;

	operator bool() const;

	GAMEBASE_DEFINE_PIMPL(Polygon, TexturedPolygon);
};

/////////////// IMPLEMENTATION ///////////////////

PolygonVertex::PolygonVertex(float x, float y) : pos(x, y), color(255, 255, 255) {}
PolygonVertex::PolygonVertex(const Vec2& pos) : pos(pos), color(255, 255, 255) {}
PolygonVertex::PolygonVertex(float x, float y, const Color& color) : pos(x, y), color(color) {}
PolygonVertex::PolygonVertex(const Vec2& pos, const Color& color) : pos(pos), color(color) {}

}

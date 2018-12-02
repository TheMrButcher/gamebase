/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/drawobj/TexturedPolygon.h>
#include "src/impl/geom/PolygonHelper.h"
#include "src/impl/graphics/BatchBuilder.h"
#include <gamebase/impl/drawobj/StaticTextureRect.h>
#include <gamebase/impl/graphics/ColoredTextureProgram.h>
#include <gamebase/impl/relbox/FixedBox.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

namespace {
void addVertices(
	std::vector<float>& vertices,
	const BoundingBox& box,
	const std::vector<std::shared_ptr<TexturedPolygonVertex>>& ring)
{
	if (ring.size() < 3)
		return;
	const auto& offset = box.bottomLeft;
	const auto& size = box.size();
	for (const auto& vertex : ring) {
		const auto& pos = vertex->pos();
		BatchBuilder::addVec2(vertices, Vec2(pos.x * size.x, pos.y * size.y) + offset);
		BatchBuilder::addVec2(vertices, Vec2(pos.x, 1.0f - pos.y));
		BatchBuilder::addColor(vertices, vertex->color());
	}
}
} // namespace

void TexturedPolygon::setImageName(const std::string& name)
{
	m_imageName = name;
	m_isTextureDirty = true;
	reload();
}

void TexturedPolygon::clear()
{
	m_outerRing.clear();
	m_innerRings.clear();
	m_isMeshDirty = true;
	reload();
}

void TexturedPolygon::setOuterRing(
	std::vector<std::shared_ptr<TexturedPolygonVertex>> vertices)
{
	m_outerRing = std::move(vertices);
	m_isMeshDirty = true;
	reload();
}

void TexturedPolygon::setInnerRing(
	size_t index, std::vector<std::shared_ptr<TexturedPolygonVertex>> vertices)
{
	if (m_innerRings.size() <= index)
		m_innerRings.resize(index + 1);
	m_innerRings[index] = std::make_shared<TexturedPolygonRing>(std::move(vertices));
	m_isMeshDirty = true;
	reload();
}

void TexturedPolygon::setInnerRing(size_t index, std::shared_ptr<TexturedPolygonRing> ring)
{
	if (m_innerRings.size() <= index)
		m_innerRings.resize(index + 1);
	m_innerRings[index] = std::move(ring);
	m_isMeshDirty = true;
	reload();
}

void TexturedPolygon::setFixedBox(float width, float height)
{
	auto box = std::make_shared<FixedBox>(width, height);
	if (m_box->isValid())
		box->checkInited();
	m_box = box;

	if (!m_parentBox.isValid())
		return;
	setBox(m_parentBox);
	loadResources();
}

void TexturedPolygon::setBox(const BoundingBox& allowedBox)
{
	m_box->setParentBox(allowedBox);
	m_parentBox = allowedBox;
	m_areBuffersDirty = true;
	setPositionBoxes(allowedBox, box());
	reload();
}

void TexturedPolygon::drawAt(const Transform2& position) const
{
	if (m_buffers.empty())
		return;
	const ColoredTextureProgram& program = coloredTextureProgram();
	program.transform = position;
	program.texture = m_texture;
	program.draw(m_buffers.vbo, m_buffers.ibo);
}

void TexturedPolygon::serialize(Serializer& s) const
{
	s << "imageName" << m_imageName << "outerRing" << m_outerRing
		<< "innerRings" << m_innerRings
		<< "box" << m_box << "position" << m_offset;
}

void TexturedPolygon::reload()
{
	if (!m_box->isValid())
		return;
	if (m_isTextureDirty) {
		m_texture = StaticTextureRect::loadTextureImpl(m_imageName);
		m_isTextureDirty = false;
	}
	if (m_isMeshDirty) {
		m_vertices.clear();
		m_indices.clear();
		if (m_outerRing.size() >= 3) {
			size_t ringCount = 1;
			for (const auto& ring : m_innerRings) {
				if (ring->vertices().size() >= 3)
					ringCount++;
			}
			m_vertices.resize(ringCount);
			for (const auto& vertex : m_outerRing)
				m_vertices[0].push_back(vertex->pos());
			for (size_t i = 0; i < m_innerRings.size(); ++i) {
				for (const auto& vertex : m_innerRings[i]->vertices())
					m_vertices[i + 1].push_back(vertex->pos());
			}
			m_indices = triangulate(m_vertices);
		}
		m_isMeshDirty = false;
		m_areBuffersDirty = true;
	}
	if (m_areBuffersDirty) {
		m_buffers = GLBuffers();
		if (!m_vertices.empty() && !m_indices.empty()) {
			static const size_t FLOATS_PER_VERTEX = 8;
			std::vector<float> vertices;
			size_t vertexCount = m_outerRing.size();
			for (const auto& ring : m_innerRings) {
				if (ring->vertices().size() >= 3)
					vertexCount += ring->vertices().size();
			}
			vertices.reserve(FLOATS_PER_VERTEX * vertexCount);
			const auto& box = m_box->get();
			addVertices(vertices, box, m_outerRing);
			for (const auto& ring : m_innerRings)
				addVertices(vertices, box, ring->vertices());
			m_buffers = GLBuffers(VertexBuffer(vertices), IndexBuffer(m_indices));
		}
		m_areBuffersDirty = false;
	}
}

std::unique_ptr<IObject> deserializeTexturedPolygon(Deserializer& deserializer)
{
	DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
	DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
	DESERIALIZE(std::string, imageName);
	DESERIALIZE(std::vector<std::shared_ptr<TexturedPolygonVertex>>, outerRing);
	DESERIALIZE(std::vector<std::shared_ptr<TexturedPolygonRing>>, innerRings);
	auto result = std::make_unique<TexturedPolygon>(box, position);
	result->setImageName(imageName);
	result->setOuterRing(outerRing);
	for (size_t i = 0; i < innerRings.size(); ++i)
		result->setInnerRing(i, innerRings[i]);
	return std::move(result);
}

REGISTER_CLASS(TexturedPolygon);

} }

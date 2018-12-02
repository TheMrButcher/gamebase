/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/drawobj/TexturedPolygon.h>
#include <gamebase/impl/drawobj/StaticTextureRect.h>
#include <gamebase/impl/relbox/FixedBox.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

void TexturedPolygon::setImageName(const std::string& name)
{
	m_imageName = name;
	m_isTextureDirty = true;
	reload();
}

void TexturedPolygon::setOuterRing(
	const std::vector<std::shared_ptr<TexturedPolygonVertex>>& vertices)
{
	m_outerRing = vertices;
	m_isMeshDirty = true;
	reload();
}

void TexturedPolygon::setInnerRing(
	size_t index, const std::vector<std::shared_ptr<TexturedPolygonVertex>>& vertices)
{
	if (m_innerRings.size() <= index)
		m_innerRings.resize(index + 1);
	m_innerRings[index] = vertices;
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
	// ToDo
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
		if (m_outerRing.size() >= 3) {
			size_t ringCount = 1;
			for (const auto& ring : m_innerRings) {
				if (ring.size() >= 3)
					ringCount++;
			}
			m_vertices.resize(ringCount);
			for (const auto& vertex : m_outerRing)
				m_vertices[0].push_back(vertex->pos());
			for (size_t i = 0; i < m_innerRings.size(); ++i) {
				for (const auto& vertex : m_innerRings[i])
					m_vertices[i + 1].push_back(vertex->pos());
			}
		}
		m_isMeshDirty = false;
		m_areBuffersDirty = true;
	}
	if (m_areBuffersDirty) {
		m_buffers = GLBuffers();
		if (!m_vertices.empty()) {
			// ToDo
			//m_buffers = GLBuffers(VertexBuffer(vertices), IndexBuffer(m_indices));
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
	DESERIALIZE(std::vector<std::vector<std::shared_ptr<TexturedPolygonVertex>>>, innerRings);
	auto result = std::make_unique<TexturedPolygon>(box, position);
	result->setImageName(imageName);
	result->setOuterRing(outerRing);
	for (size_t i = 0; i < innerRings.size(); ++i)
		result->setInnerRing(i, innerRings[i]);
	return std::move(result);
}

REGISTER_CLASS(TexturedPolygon);

} }

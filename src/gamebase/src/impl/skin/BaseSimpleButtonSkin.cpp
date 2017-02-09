/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/skin/tools/BaseSimpleButtonSkin.h>
#include <gamebase/impl/geom/IdenticGeometry.h>
#include <gamebase/impl/relbox/OffsettedBox.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

BaseSimpleButtonSkin::BaseSimpleButtonSkin(
	const std::shared_ptr<IRelativeBox>& box)
	: m_box(box)
	, m_geom(std::make_shared<IdenticGeometry>())
	, m_borderWidth(1)
	, m_useGradient(true)
	, m_gradient(std::make_shared<OffsettedBox>())
{
	m_border.setColor(Color(0, 0, 0, 1));
	m_fill.setColor(Color(0.7f, 0.7f, 0.7f, 1));
	m_light.setColor(Color(0, 0, 0, 0));
	m_gradient.setDirection(Direction::Vertical);
	m_gradient.setColor1(Color(1, 1, 1, 0));
	m_gradient.setColor2(Color(1, 1, 1, 0.5f));
}

void BaseSimpleButtonSkin::setSelectionState(SelectionState::Enum state)
{
	m_selectionState = state;
	Color newColor(0, 0, 0, 0);
	switch (state) {
	case SelectionState::MouseOn: newColor = Color(1, 1, 1, 0.3f); break;
	case SelectionState::Pressed: newColor = Color(0, 0, 0, 0.3f); break;
	}
	m_light.setColor(newColor);
}

void BaseSimpleButtonSkin::loadResources()
{
	m_border.loadResources();
	m_fill.loadResources();
	if (m_useGradient)
		m_gradient.loadResources();
	m_light.loadResources();
    loadContent();
}

void BaseSimpleButtonSkin::drawAt(const Transform2& position) const
{
	m_border.draw(position);
	m_fill.draw(position);
    if (m_useGradient && m_selectionState != SelectionState::Pressed)
		m_gradient.draw(position);
	drawContent(position);
	m_light.draw(position);
}

void BaseSimpleButtonSkin::setBox(const BoundingBox& allowedBox)
{
	m_box->setParentBox(allowedBox);
	BoundingBox box = m_box->get();
    m_geom->setBox(box);

	BoundingBox innerBox = extend(box, -m_borderWidth);
	if (!innerBox.isValid())
		innerBox = BoundingBox(box.center());
	if (m_useGradient) {
		auto gradientBox = innerBox;
		gradientBox.bottomLeft.y += 0.5f * gradientBox.height();
		m_gradient.setBox(gradientBox);
	}
	m_border.setBox(box);
	m_fill.setBox(innerBox);
	setContentBox(innerBox);
	m_light.setBox(innerBox);
}

void BaseSimpleButtonSkin::serializeParams(Serializer& s) const
{
	s   << "fillColor" << m_fill.color()
		<< "borderWidth" << m_borderWidth << "borderColor" << m_border.color()
		<< "useGradient" << m_useGradient;
}

void deserializeBaseSimpleButtonSkin(
    BaseSimpleButtonSkin* skin, Deserializer& deserializer)
{
	DESERIALIZE(Color, fillColor);
	DESERIALIZE(float, borderWidth);
	DESERIALIZE(Color, borderColor);
	DESERIALIZE(bool, useGradient);
	skin->setFillColor(fillColor);
	skin->setBorderWidth(borderWidth);
	skin->setBorderColor(borderColor);
	skin->setUseGradient(useGradient);
}

} }

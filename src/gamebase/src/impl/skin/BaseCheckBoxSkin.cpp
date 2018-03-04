/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/skin/tools/BaseCheckBoxSkin.h>
#include <gamebase/impl/geom/IdenticGeometry.h>

namespace gamebase { namespace impl {

BaseCheckBoxSkin::BaseCheckBoxSkin(const std::shared_ptr<IRelativeBox>& box)
    : m_box(box)
    , m_geom(std::make_shared<IdenticGeometry>())
{}

void BaseCheckBoxSkin::drawAt(const Transform2& position) const
{
    drawBackground(position);
    if (m_checked)
        drawCheck(position);
}

void BaseCheckBoxSkin::setBox(const BoundingBox& allowedBox)
{
    m_box->setParentBox(allowedBox);
    BoundingBox box = m_box->get();
    m_geom->setBox(box);
    setContentBox(box);
}

} }

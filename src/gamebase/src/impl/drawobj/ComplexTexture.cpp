/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/drawobj/ComplexTexture.h>
#include "TextureHelpers.h"
#include "src/impl/graphics/BatchBuilder.h"
#include <gamebase/impl/serial/ISerializer.h>

namespace gamebase { namespace impl {

ComplexTexture::ComplexTexture(
    const std::shared_ptr<IRelativeBox>& box,
    const std::shared_ptr<IRelativeOffset>& position)
    : StaticTextureRect(box, position)
    , m_texCenter(0.5f, 0.5f)
    , m_angleSize(0, 0)
{}

void ComplexTexture::serialize(Serializer& s) const
{
    s << "texCenter" << m_texCenter << "angleSizes" << m_angleSize;
    StaticTextureRect::serialize(s);
}

std::unique_ptr<IObject> deserializeComplexTexture(Deserializer& deserializer)
{
    DESERIALIZE(Vec2, texCenter);
    DESERIALIZE(Vec2, angleSizes);
    auto result = deserializeTextureBase<ComplexTexture>(deserializer);
    result->setTexCenter(texCenter);
    result->setAngleSize(angleSizes);
    return std::move(result);
}

REGISTER_CLASS(ComplexTexture);

void ComplexTexture::updateImpl()
{
    BatchBuilder b;
    b.reserveForTextureRects(9);

    auto bbox = box();
    float anglesHeight = static_cast<float>(m_texture.size().height);
    if (m_angleSize.y > 0) {
        float biggestAngleHeight =
            (m_texCenter.y > 0.5f ? m_texCenter.y : (1 - m_texCenter.y)) * anglesHeight;
        float scaleY = m_angleSize.y / biggestAngleHeight;
        anglesHeight *= scaleY;
    }
    bool needHorizontalInner = true;
    float bottomStripeMax = 0;
    float topStripeMin = 1;
    if (anglesHeight > bbox.height()) {
        needHorizontalInner = false;
        bottomStripeMax = bbox.bottomLeft.y + 0.5f * bbox.height();
        topStripeMin = bottomStripeMax;
    } else {
        bottomStripeMax = bbox.bottomLeft.y + 0.5f * anglesHeight;
        topStripeMin = bbox.topRight.y - 0.5f * anglesHeight;
    }

    float anglesWidth = static_cast<float>(m_texture.size().width);
    if (m_angleSize.x > 0) {
        float biggestAngleWidth =
            (m_texCenter.x > 0.5f ? m_texCenter.x : (1 - m_texCenter.x)) * anglesWidth;
        float scaleX = m_angleSize.x / biggestAngleWidth;
        anglesWidth *= scaleX;
    }
    bool needVerticalInner = true;
    float leftStripeMax = 0;
    float rightStripeMin = 1;
    if (anglesWidth > box().width()) {
        needVerticalInner = false;
        leftStripeMax = bbox.bottomLeft.x + 0.5f * bbox.width();
        rightStripeMin = leftStripeMax;
    } else {
        leftStripeMax = bbox.bottomLeft.x + 0.5f * anglesWidth;
        rightStripeMin = bbox.topRight.x - 0.5f * anglesWidth;
    }

    Vec2 texCenter(m_texCenter.x, 1 - m_texCenter.y);
    b.addTextureRect(
        BoundingBox(bbox.bottomLeft, Vec2(leftStripeMax, bottomStripeMax)),
        Vec2(0, 1), texCenter);
    b.addTextureRect(
		BoundingBox(Vec2(bbox.bottomLeft.x, topStripeMin), Vec2(leftStripeMax, bbox.topRight.y)),
        Vec2(0, texCenter.y), Vec2(texCenter.x, 0));
    b.addTextureRect(
		BoundingBox(Vec2(rightStripeMin, topStripeMin), bbox.topRight),
        texCenter, Vec2(1, 0));
    b.addTextureRect(
		BoundingBox(Vec2(rightStripeMin, bbox.bottomLeft.y), Vec2(bbox.topRight.x, bottomStripeMax)),
        Vec2(texCenter.x, 1), Vec2(1, texCenter.y));

    if (needHorizontalInner) {
        b.addTextureRect(
			BoundingBox(Vec2(bbox.bottomLeft.x, bottomStripeMax), Vec2(leftStripeMax, topStripeMin)),
            Vec2(0, texCenter.y), texCenter);
        b.addTextureRect(
			BoundingBox(Vec2(rightStripeMin, bottomStripeMax), Vec2(bbox.topRight.x, topStripeMin)),
            texCenter, Vec2(1, texCenter.y));
    }

    if (needVerticalInner) {
        b.addTextureRect(
			BoundingBox(Vec2(leftStripeMax, bbox.bottomLeft.y), Vec2(rightStripeMin, bottomStripeMax)),
            Vec2(texCenter.x, 1), texCenter);
        b.addTextureRect(
			BoundingBox(Vec2(leftStripeMax, topStripeMin), Vec2(rightStripeMin, bbox.topRight.y)),
            texCenter, Vec2(texCenter.x, 0));
    }

    if (needHorizontalInner && needVerticalInner) {
        b.addTextureRect(
			BoundingBox(Vec2(leftStripeMax, bottomStripeMax), Vec2(rightStripeMin, topStripeMin)),
            texCenter, texCenter);
    }

    m_buffers = GLBuffers(VertexBuffer(b.vertices), IndexBuffer(b.indices));
}

} }

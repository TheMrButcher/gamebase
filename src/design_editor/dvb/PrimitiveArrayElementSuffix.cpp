/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "PrimitiveArrayElementSuffix.h"

namespace gamebase { namespace editor {

PrimitiveArrayElementSuffix::PrimitiveArrayElementSuffix()
{
    using namespace gamebase::impl;
    m_suffixes.resize(static_cast<size_t>(SerializationTag::GLColor) + 1);
    m_suffixes[SerializationTag::Vec2].push_back(".x");
    m_suffixes[SerializationTag::Vec2].push_back(".y");

    m_suffixes[SerializationTag::Matrix2].push_back("[0, 0]");
    m_suffixes[SerializationTag::Matrix2].push_back("[0, 1]");
    m_suffixes[SerializationTag::Matrix2].push_back("[1, 0]");
    m_suffixes[SerializationTag::Matrix2].push_back("[1, 1]");

    m_suffixes[SerializationTag::Transform2].push_back(".m[0, 0]");
    m_suffixes[SerializationTag::Transform2].push_back(".m[0, 1]");
    m_suffixes[SerializationTag::Transform2].push_back(".m[1, 0]");
    m_suffixes[SerializationTag::Transform2].push_back(".m[1, 1]");
    m_suffixes[SerializationTag::Transform2].push_back(".tx");
    m_suffixes[SerializationTag::Transform2].push_back(".ty");

    m_suffixes[SerializationTag::BoundingBox].push_back(".left");
    m_suffixes[SerializationTag::BoundingBox].push_back(".bottom");
    m_suffixes[SerializationTag::BoundingBox].push_back(".right");
    m_suffixes[SerializationTag::BoundingBox].push_back(".top");

    m_suffixes[SerializationTag::GLColor].push_back(".r");
    m_suffixes[SerializationTag::GLColor].push_back(".g");
    m_suffixes[SerializationTag::GLColor].push_back(".b");
    m_suffixes[SerializationTag::GLColor].push_back(".a");
}

} }

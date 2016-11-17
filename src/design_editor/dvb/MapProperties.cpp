/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "MapProperties.h"

namespace gamebase { namespace editor {

MapProperties::MapProperties()
    : currentElem(0)
{}

MapProperties::Element::Element()
    : properties(nullptr)
    , keyNodeID(-1)
{}

MapProperties::Element::Element(
    const std::shared_ptr<Properties>& properties,
    int keyNodeID)
    : properties(properties)
    , keyNodeID(keyNodeID)
{}

} }

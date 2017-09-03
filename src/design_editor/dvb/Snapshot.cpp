/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "Snapshot.h"
#include <DesignViewBuilder.h>

namespace gamebase { namespace editor {

Snapshot::Snapshot(
    DesignViewBuilder& builder,
    const std::shared_ptr<Properties>& properties,
    ObjType::Enum objType)
    : context(builder.m_context)
    , curName(builder.m_curName)
    , modelNodeID(builder.m_curModelNodeID)
    , properties(properties)
    , objType(objType)
    , levelOfHidden(builder.m_levelOfHidden)
{
    if (objType == ObjType::Array)
        arrayType = impl::SerializationTag::Array;
    if (objType == ObjType::Map || objType == ObjType::MapElement)
        arrayType = impl::SerializationTag::Map;
}

} }

/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <dvb/Properties.h>
#include <dvb/ObjType.h>
#include <dvb/SharedContext.h>

namespace gamebase { namespace editor {
class DesignViewBuilder;

struct Snapshot {
    Snapshot(DesignViewBuilder& builder, const Properties& properties, ObjType::Enum objType);

    std::shared_ptr<SharedContext> context;
        
    std::string curName;
    int modelNodeID;
    std::shared_ptr<Properties> properties;
    ObjType::Enum objType;
    boost::optional<impl::SerializationTag::Type> arrayType;
    int levelOfHidden;
};
} }

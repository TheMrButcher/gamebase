/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/serial/ISerializable.h>
#include <string>
#include <map>

namespace gamebase { namespace editor {

class EnumPresentation : public impl::ISerializable {
public:
    virtual void serialize(impl::Serializer& serializer) const override;

    std::string name;
    std::string nameInUI;
    std::map<int, std::string> values;
};

} }

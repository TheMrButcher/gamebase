/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/pubhelp/Deserialize.h>
#include <gamebase/impl/serial/JsonDeserializer.h>

namespace gamebase { namespace impl {

std::shared_ptr<IObject> deserializeObj(const std::string& fileName)
{
    return deserialize<IObject>(fileName);
}

std::shared_ptr<ILayer> deserializeLayer(const std::string& fileName)
{
    return deserialize<ILayer>(fileName);
}

} }

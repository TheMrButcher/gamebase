/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "EnumPresentation.h"
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace editor {

void EnumPresentation::serialize(impl::Serializer& s) const
{
    s << "name" << name << "nameInUI" << nameInUI << "values" << values;
}

std::unique_ptr<impl::IObject> deserializeEnumPresentation(impl::Deserializer& deserializer)
{
    std::unique_ptr<EnumPresentation> result(new EnumPresentation());
    deserializer >> "name" >> result->name >> "nameInUI" >> result->nameInUI
        >> "values" >> result->values;
    return std::move(result);
}

REGISTER_CLASS(EnumPresentation);

} }

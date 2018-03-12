/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/engine/IObject.h>

namespace gamebase { namespace impl {

class Serializer;
class Deserializer;

class ISerializable : public virtual IObject {
public:
    virtual void serialize(Serializer& serializer) const = 0;
};

} }

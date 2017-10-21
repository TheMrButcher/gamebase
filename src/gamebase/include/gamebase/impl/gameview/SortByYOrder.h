/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/gameview/IOrder.h>
#include <gamebase/impl/gameview/GeometryKeyType.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class SortByYOrder : public IOrder, public ISerializable {
public:
    SortByYOrder(GeometryKeyType::Enum keyType = GeometryKeyType::Offset)
        : m_keyType(keyType)
    {}

    virtual void sort(Drawable** begin, Drawable** end) const override;

    virtual void serialize(Serializer& serializer) const override;
    
private:
    GeometryKeyType::Enum m_keyType;
};

} }

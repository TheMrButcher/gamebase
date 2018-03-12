/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/gameview/IOrder.h>
#include <gamebase/impl/gameview/GeometryKeyType.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class SortByIDOrder : public IOrder, public ISerializable {
public:
    SortByIDOrder() : m_objToID(nullptr) {}

    virtual void sort(Drawable** begin, Drawable** end) const override;

    virtual void setObjectToID(const std::unordered_map<IObject*, int>& objToID) { m_objToID = &objToID; }

    virtual void serialize(Serializer& serializer) const override {}
    
    const std::unordered_map<IObject*, int>* m_objToID;
};

} }

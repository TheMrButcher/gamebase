#pragma once

#include <gamebase/engine/IOrder.h>
#include <gamebase/engine/GeometryKeyType.h>
#include <gamebase/serial/ISerializable.h>

namespace gamebase {

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

}

#pragma once

#include <gamebase/engine/IIndex.h>
#include <gamebase/engine/GeometryKeyType.h>
#include <gamebase/serial/ISerializable.h>

namespace gamebase {

class FlatIndex : public IIndex, public ISerializable {
public:
    FlatIndex(GeometryKeyType::Enum keyType = GeometryKeyType::Offset)
        : m_keyType(keyType)
    {}

    virtual void setGameBox(const BoundingBox& box) override {}

    virtual void update() override;
    virtual void insert(int id, Drawable* obj) override;
    virtual void remove(int id) override;
    virtual void clear() override { m_objs.clear(); }

    virtual std::vector<Drawable*> findByBox(const BoundingBox& box) const override;

    virtual void serialize(Serializer& serializer) const override;

private:
    GeometryKeyType::Enum m_keyType;

    struct Node {
        Node() {}
        Node(int id, Drawable* obj) : id(id), obj(obj) {}

        int id;
        Drawable* obj;
        BoundingBox box;
    };

    std::vector<Node> m_objs;
};

}

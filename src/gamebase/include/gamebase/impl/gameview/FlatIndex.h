/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/gameview/IIndex.h>
#include <gamebase/impl/gameview/GeometryKeyType.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class FlatIndex : public IIndex, public ISerializable {
public:
    FlatIndex(GeometryKeyType::Enum keyType = GeometryKeyType::Offset)
        : m_keyType(keyType)
        , m_needFindables(true)
    {}

    virtual void setGameBox(const BoundingBox& box) override {}

    virtual void disableFindablesIndex() override { m_needFindables = false; }
    virtual void update() override;
    virtual void insert(int id, IObject* obj) override;
    virtual void remove(int id) override;
    virtual void clear() override { m_objs.clear(); m_findables.clear(); }

    virtual bool drawablesByBox(
        const BoundingBox& box, std::vector<Drawable*>& drawables) const override;
    virtual bool findablesByBox(
        const BoundingBox& box, std::vector<IFindable*>& findables) const override;

    virtual void serialize(Serializer& serializer) const override;

    template <typename T>
    struct Node {
        Node() {}
        Node(int id, T* obj) : id(id), obj(obj) {}

        int id;
        T* obj;
        BoundingBox box;
    };

private:
    GeometryKeyType::Enum m_keyType;
    std::vector<Node<Drawable>> m_objs;
    bool m_needFindables;
    std::vector<Node<IFindable>> m_findables;
};

} }

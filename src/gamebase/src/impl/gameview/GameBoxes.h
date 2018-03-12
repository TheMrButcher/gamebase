/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/gameview/IGameBox.h>
#include <gamebase/impl/relbox/IRelativeBox.h>
#include <memory>

namespace gamebase { namespace impl {

class InfiniteGameBox : public IGameBox {
public:
    virtual boost::optional<BoundingBox> box() const override { return boost::none; }
    virtual void setViewBox(const BoundingBox& viewBox) override {}
    virtual void serialize(Serializer&) const override {}
};

class RelativeGameBox : public IGameBox {
public:
    enum CoordSystem {
        Center,
        LeftBottom
    };

    RelativeGameBox(
        CoordSystem coordSystem,
        const std::shared_ptr<IRelativeBox>& relativeBox);
    virtual boost::optional<BoundingBox> box() const override;
    virtual void setViewBox(const BoundingBox& viewBox) override;
    virtual void serialize(Serializer& s) const override;

private:
    CoordSystem m_coordSystem;
    std::shared_ptr<IRelativeBox> m_relativeBox;
    boost::optional<BoundingBox> m_box;
};

class FixedGameBox : public IGameBox {
public:
    FixedGameBox(const BoundingBox& box);
    virtual boost::optional<BoundingBox> box() const override;
    virtual void setViewBox(const BoundingBox& viewBox) override {}
    virtual void serialize(Serializer& s) const override;

private:
    BoundingBox m_box;
};

} }

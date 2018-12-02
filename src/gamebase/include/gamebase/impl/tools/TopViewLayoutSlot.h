/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/engine/IDrawable.h>
#include <gamebase/impl/pos/IPositionable.h>
#include <boost/optional.hpp>
#include <memory>

namespace gamebase { namespace impl {

class CanvasLayout;

class GAMEBASE_API TopViewLayoutSlot {
public:
    TopViewLayoutSlot() {}
    ~TopViewLayoutSlot() { reset(); }

    bool isInited() const { return static_cast<bool>(m_id); }

    void init(
        IPositionable* parentPositionable,
        IDrawable* parentDrawable,
        const std::shared_ptr<IObject>& object);
    void reset();

private:
    boost::optional<int> m_id;
	std::weak_ptr<CanvasLayout> m_topViewLayoutCanvas;
};

} }

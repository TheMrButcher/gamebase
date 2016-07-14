/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/skin/base/Skin.h>
#include <gamebase/impl/app/TimeState.h>
#include <gamebase/impl/geom/IRelativeGeometry.h>
#include <gamebase/impl/text/TextGeometry.h>
#include <vector>

namespace gamebase { namespace impl {

class TextBoxSkin : public Skin {
public:
    virtual void setText(const std::string& text) = 0;

    virtual void setSelection(size_t startIndex, size_t endIndex) = 0;

    virtual const std::vector<CharPosition>& textGeometry() const = 0;

    virtual std::shared_ptr<IRelativeGeometry> geometry() const = 0;

    virtual Time shiftPeriod() const = 0;

    virtual void setOffsetX(float x) = 0;

    virtual BoundingBox textAreaBox() const = 0;
};

} }

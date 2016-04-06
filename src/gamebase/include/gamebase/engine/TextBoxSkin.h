#pragma once

#include <gamebase/engine/Skin.h>
#include <gamebase/engine/TimeState.h>
#include <gamebase/geom/IRelativeGeometry.h>
#include <gamebase/text/TextGeometry.h>
#include <vector>

namespace gamebase {

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

}

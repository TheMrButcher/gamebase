#pragma once

#include <gamebase/engine/Skin.h>
#include <gamebase/text/TextGeometry.h>
#include <vector>

namespace gamebase {

class TextEditSkin : public Skin {
public:
    virtual void setText(const std::string& text) = 0;

    virtual void setSelection(size_t startIndex, size_t endIndex) = 0;

    virtual const std::vector<CharPosition>& textGeometry() const = 0;
};

}

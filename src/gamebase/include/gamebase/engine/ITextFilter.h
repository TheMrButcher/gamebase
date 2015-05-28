#pragma once

#include <gamebase/text/Utf8Text.h>

namespace gamebase {

class ITextFilter {
public:
    virtual ~ITextFilter() {}

    virtual Utf8Text filter(const Utf8Text& prevText, const Utf8Text& text) const = 0;
};

}

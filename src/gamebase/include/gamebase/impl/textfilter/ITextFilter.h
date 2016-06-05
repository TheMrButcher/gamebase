#pragma once

#include <gamebase/impl/engine/IObject.h>
#include <gamebase/impl/text/Utf8Text.h>

namespace gamebase { namespace impl {

class ITextFilter : public IObject {
public:
    virtual ~ITextFilter() {}

    virtual Utf8Text filter(const Utf8Text& prevText, const Utf8Text& text) const = 0;
};

} }

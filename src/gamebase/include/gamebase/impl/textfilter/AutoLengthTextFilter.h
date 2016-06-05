#pragma once

#include <gamebase/impl/textfilter/ITextFilter.h>
#include <gamebase/impl/skin/base/TextBoxSkin.h>
#include <gamebase/tools/Exception.h>

namespace gamebase { namespace impl {

class AutoLengthTextFilter : public ITextFilter {
public:
    AutoLengthTextFilter(TextBoxSkin* skin)
        : m_skin(skin)
    {}

    virtual Utf8Text filter(const Utf8Text&, const Utf8Text& text) const override
    {
        m_skin->setText(text.toString());
        size_t length = m_skin->textGeometry().size();
        if (length == 0)
            THROW_EX() << "Broken TextEditSkin: no end empty char";
        return text.substr(0, length - 1);
    }

private:
    TextBoxSkin* m_skin;
};

} }

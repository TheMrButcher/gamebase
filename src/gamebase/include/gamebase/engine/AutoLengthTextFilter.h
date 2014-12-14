#pragma once

#include <gamebase/engine/ITextFilter.h>
#include <gamebase/engine/TextEditSkin.h>
#include <gamebase/utils/Exception.h>

namespace gamebase {

class AutoLengthTextFilter : public ITextFilter {
public:
    AutoLengthTextFilter(TextEditSkin* skin)
        : m_skin(skin)
    {}

    virtual std::string filter(const std::string& text) const override
    {
        m_skin->setText(text);
        size_t length = m_skin->textGeometry().size();
        if (length == 0)
            THROW_EX() << "Broken TextEditSkin: no end empty char";
        return text.substr(0, length - 1);
    }

private:
    TextEditSkin* m_skin;
};

}

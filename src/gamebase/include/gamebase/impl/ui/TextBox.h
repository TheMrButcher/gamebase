/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/skin/base/TextBoxSkin.h>
#include <gamebase/impl/pos/OffsettedPosition.h>
#include <gamebase/impl/findable/FindableGeometry.h>
#include <gamebase/impl/engine/IInputProcessor.h>
#include <gamebase/impl/textfilter/ITextFilter.h>
#include <gamebase/impl/tools/Timer.h>
#include <gamebase/impl/text/Utf8Text.h>
#include <gamebase/impl/serial/ISerializable.h>
#include <gamebase/tools/Delayed.h>

namespace gamebase { namespace impl {

class GAMEBASE_API TextBox : public OffsettedPosition, public FindableGeometry,
    public Selectable, public IInputProcessor, public Drawable, public Registrable, public ISerializable {
public:
    TextBox(
        const std::shared_ptr<TextBoxSkin>& skin,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    const std::string& text() const { return m_text.toString(); }

    void setText(const std::string& text);

    void setTextFilter(const std::shared_ptr<ITextFilter>& textFilter)
    {
        m_textFilter = textFilter;
    }

    void setCallback(const std::function<void()>& callback)
    {
        m_callback = callback;
    }

    virtual void setSelectionState(SelectionState::Enum state) override;

    virtual void loadResources() override
    {
        m_inited = true;
        m_skin->loadResources();
    }

    virtual void drawAt(const Transform2& position) const
    {
        m_skin->draw(position);
    }

    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_skin->setBox(allowedBox);
        setPositionBoxes(allowedBox, box());
    }

    virtual BoundingBox box() const override
    {
        return m_skin->box();
    }

    virtual void processInput(const InputRegister& input) override;

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    
    virtual void serialize(Serializer& s) const override;

private:
    void processKeyboard(const InputRegister& input);
    void processPrintable(const std::string& localStr);
    void processMouse(const InputRegister& input);
    void setCursor(size_t pos);
    void moveCursor(int shift);
    size_t calcCharIndex(float x);
    Utf8Text filterText(const Utf8Text& newText) const;
    void updateSkin();

    std::shared_ptr<TextBoxSkin> m_skin;
    std::shared_ptr<ITextFilter> m_textFilter;
    Utf8Text m_text;
    size_t m_selectionStart;
    size_t m_selectionEnd;
    float m_offsetX;
    size_t m_firstVisibleSymbol;
    bool m_inited;
    std::function<void()> m_callback;
    Timer m_timer;
    Handle m_callbackHandle;
};

} }

/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <dvb/IProperty.h>
#include <gamebase/drawobj/Label.h>
#include <boost/noncopyable.hpp>
#include <functional>

namespace gamebase { namespace editor {
class TypePresentation;
class IPropertyPresentation;
class IIndexablePropertyPresentation;

struct Properties : boost::noncopyable {
    Properties();

    int id;
    std::vector<std::shared_ptr<IProperty>> list;
    std::vector<std::shared_ptr<Properties>> inlined;
    const TypePresentation* type;
    const IPropertyPresentation* presentationFromParent;
    const IIndexablePropertyPresentation* keyPresentationFromParent;
    bool isInline;
    bool isHiddenLevel;

    void attach(Layout layout);
    void sync();
    void detach();

    void setLabel(const Label& label) { m_label = label; }
    const Label& label() const { return m_label; }
    void setLabelText(const std::string& text)
    {
        if (m_label)
            m_label.setText(text);
    }
    void setLabelUpdater(const std::function<void(Label)>& labelUpdater)
    {
        m_labelUpdater = labelUpdater;
    }
    void updateLabel()
    {
        if (m_labelUpdater && m_label)
            m_labelUpdater(m_label);
    }
    std::function<void()> labelUpdater()
    {
        return [this]() { updateLabel(); };
    }

private:
    size_t addSelfProperties(Layout layout);

    static void stub() {}
    Label m_label;
    std::function<void(Label)> m_labelUpdater;
};
} }

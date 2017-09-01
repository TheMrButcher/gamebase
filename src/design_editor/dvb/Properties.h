/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/drawobj/Label.h>
#include <gamebase/ui/Layout.h>
#include <functional>

namespace gamebase { namespace editor {
class TypePresentation;
class IPropertyPresentation;
class IIndexablePropertyPresentation;

struct Properties {
    Properties();

    int id;
    Layout layout;
    const TypePresentation* type;
    const IPropertyPresentation* presentationFromParent;
    const IIndexablePropertyPresentation* keyPresentationFromParent;
    bool isInline;
    bool isHiddenLevel;

    void setLabel(const Label& label) { m_label = label; }
    const Label& label() const { return m_label; }
    void setLabelText(const std::string& text)
    {
        if (m_label)
            m_label.setText(text);
    }
    void setLabelUpdater(const std::function<void()>& labelUpdater)
    {
        if (m_label)
            m_labelUpdater = labelUpdater;
    }
    void updateLabel()
    {
        if (m_labelUpdater)
            m_labelUpdater();
    }
    std::function<void()> labelUpdater()
    {
        if (m_labelUpdater)
            return m_labelUpdater;
        return &Properties::stub;
    }

private:
    static void stub() {}
    Label m_label;
    std::function<void()> m_labelUpdater;
};
} }

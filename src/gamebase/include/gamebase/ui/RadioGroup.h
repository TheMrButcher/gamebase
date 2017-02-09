/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/ui/RadioButton.h>
#include <gamebase/impl/pubhelp/RadioGroupHelpers.h>

namespace gamebase {

class RadioGroup {
public:
    void select(int id);
    void select(RadioButton button);
    bool isAnySelected() const;
    int selected() const;
    RadioButton selectedButton() const;

    void setCallback(const std::function<void()>& callback);

    int add(const RadioButton& button);
    void insert(int id, const RadioButton& button);

    GAMEBASE_DEFINE_RADIO_GROUP_PIMPL();
};

/////////////// IMPLEMENTATION ///////////////////

inline void RadioGroup::select(int id) { m_impl->setSelected(id); }
inline void RadioGroup::select(RadioButton button) { m_impl->setSelected(button.id()); }
inline bool RadioGroup::isAnySelected() const { return m_impl->isAnySelected(); }
inline int RadioGroup::selected() const { return m_impl->isAnySelected() ? m_impl->selected() : -1; }
inline RadioButton RadioGroup::selectedButton() const { return m_impl->isAnySelected() ? impl::wrap<RadioButton>(m_impl->selectedButton()) : RadioButton(); }
inline void RadioGroup::setCallback(const std::function<void()>& callback) { m_impl->setCallback(callback); }
inline int RadioGroup::add(const RadioButton& button) { button.getImpl()->setGroup(m_impl); return button.id(); }
inline void RadioGroup::insert(int id, const RadioButton& button) { button.getImpl()->setIndexInGroup(id); button.getImpl()->setGroup(m_impl); }

}

/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/ui/RadioButtonGroup.h>
#include <gamebase/impl/ui/RadioButton.h>

namespace gamebase { namespace impl {

void RadioButtonGroup::setCallback(const std::function<void()>& callback)
{
	setCallback([callback](int) { callback(); });
}

int RadioButtonGroup::selected() const
{
    if (m_selected == -1)
        THROW_EX() << "Wrong state: no button is selected";
    return m_selected;
}

RadioButton* RadioButtonGroup::selectedButton() const
{
    auto it = m_elements.find(selected());
    if (it == m_elements.end())
        THROW_EX() << "Can't get selected button, no button with id=" << selected();
    return it->second;
}

void RadioButtonGroup::setSelected(int index)
{
    if (m_selected == -1) {
        for (auto it = m_elements.begin(); it != m_elements.end(); ++it)
            it->second->setCheckedImpl(false);
    } else {
        m_elements.at(m_selected)->setCheckedImpl(false);
    }
    m_selected = index;
    m_elements.at(index)->setCheckedImpl(true);
    if (m_callback)
        m_callbackHandle = Handle([this, index]() { m_callback(index); });
}

int RadioButtonGroup::add(RadioButton* button)
{
    int id = m_nextID++;
    m_elements[id] = button;
    button->setIndexInGroup(id);
    return id;
}

void RadioButtonGroup::insert(int id, RadioButton* button)
{
    m_nextID = std::max(id + 1, m_nextID);
    m_elements[id] = button;
    button->setIndexInGroup(id);
}

void RadioButtonGroup::remove(int id)
{
    m_elements.erase(id);
    if (m_selected == id)
        m_selected = -1;
}

} }

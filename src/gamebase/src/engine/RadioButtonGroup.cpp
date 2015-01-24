#include <stdafx.h>
#include <gamebase/engine/RadioButtonGroup.h>
#include <gamebase/engine/RadioButton.h>

namespace gamebase {

size_t RadioButtonGroup::selected() const
{
    if (m_selected == -1)
        THROW_EX() << "Wrong state: no button is selected";
    return m_selected;
}

void RadioButtonGroup::setSelected(size_t index)
{
    if (m_selected == -1) {
        for (auto it = m_elements.begin(); it != m_elements.end(); ++it)
            (*it)->setCheckedImpl(false);
    } else {
        m_elements.at(m_selected)->setCheckedImpl(false);
    }
    if (index >= m_elements.size())
        THROW_EX() << "Wrong index of RadioButton (out of range): " << index;
    m_elements[index]->setCheckedImpl(true);
    if (m_callback)
        m_callback(index);
}

size_t RadioButtonGroup::add(RadioButton* button)
{
    size_t index = m_elements.size();
    m_elements.push_back(button);
    return index;
}

}

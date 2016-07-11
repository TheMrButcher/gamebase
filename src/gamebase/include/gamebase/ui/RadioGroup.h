#pragma once

#include <gamebase/ui/RadioButton.h>
#include <gamebase/impl/pubhelp/RadioGroupHelpers.h>

namespace gamebase {

class RadioGroup {
public:
    void select(int id);
    int selected() const;

    void setCallback(const std::function<void()>& callback);

    int add(const RadioButton& button);
    void insert(int id, const RadioButton& button);

    GAMEBASE_DEFINE_RADIO_GROUP_PIMPL();
};

/////////////// IMPLEMENTATION ///////////////////

inline void RadioGroup::select(int id) { m_impl->setSelected(id); }
inline int RadioGroup::selected() const { return m_impl->isAnySelected() ? m_impl->selected() : -1; }
inline void RadioGroup::setCallback(const std::function<void()>& callback) { m_impl->setCallback(callback); }
inline int RadioGroup::add(const RadioButton& button) { button.getImpl()->setGroup(m_impl); return button.id(); }
inline void RadioGroup::insert(int id, const RadioButton& button) { button.getImpl()->setIndexInGroup(id); button.getImpl()->setGroup(m_impl); }

}

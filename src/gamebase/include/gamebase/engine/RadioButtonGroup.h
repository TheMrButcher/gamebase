#pragma once

#include <gamebase/GameBaseAPI.h>
#include <functional>
#include <vector>

namespace gamebase {

class RadioButton;

class GAMEBASE_API RadioButtonGroup {
public:
    RadioButtonGroup()
        : m_selected(-1)
    {}

    void setCallback(const std::function<void(size_t)>& callback) { m_callback = callback; }

    size_t selected() const;

    void setSelected(size_t index);

    size_t add(RadioButton* button);

private:
    size_t m_selected;
    std::vector<RadioButton*> m_elements;
    std::function<void(size_t)> m_callback;
};

}

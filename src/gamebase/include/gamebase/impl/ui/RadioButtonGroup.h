/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/tools/Delayed.h>
#include <functional>
#include <map>

namespace gamebase { namespace impl {

class RadioButton;

class GAMEBASE_API RadioButtonGroup {
public:
    RadioButtonGroup()
        : m_selected(-1)
        , m_nextID(0)
    {}

    void setCallback(const std::function<void(int)>& callback) { m_callback = callback; }
    void setCallback(const std::function<void()>& callback);

    bool isAnySelected() const { return m_selected != -1; }
    int selected() const;
    RadioButton* selectedButton() const;

    void setSelected(int index);

    int add(RadioButton* button);
    void insert(int id, RadioButton* button);
    void remove(int id);

private:
    int m_selected;
    int m_nextID;
    std::map<int, RadioButton*> m_elements;
    std::function<void(int)> m_callback;
    Handle m_callbackHandle;
};

} }

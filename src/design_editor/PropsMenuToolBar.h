#pragma once

#include <unordered_map>
#include <functional>
#include <string>

namespace gamebase {

class LinearLayout;
class Button;

struct ButtonKey {
    enum Enum {
        Add,
        AddFromFile,
        Remove,
        ReplaceFromFile,
        Down,
        Up
    };
};

namespace editor {

class PropsMenuToolBar {
public:
    PropsMenuToolBar(LinearLayout* bar);

    void clear();

    void addButton(ButtonKey::Enum key, const std::function<void()>& callback);

private:
    LinearLayout* m_bar;
    std::unordered_map<ButtonKey::Enum, std::shared_ptr<Button>> m_buttons;
};

} }

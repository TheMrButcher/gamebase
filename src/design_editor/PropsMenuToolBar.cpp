#include "PropsMenuToolBar.h"
#include <gamebase/engine/LinearLayout.h>
#include <gamebase/engine/Button.h>
#include <gamebase/serial/JsonDeserializer.h>

namespace gamebase { namespace editor {

PropsMenuToolBar::PropsMenuToolBar(LinearLayout* bar)
    : m_bar(bar)
{
    m_buttons[ButtonKey::Add] = deserialize<Button>("ui\\AddButton.json");
    m_buttons[ButtonKey::AddFromFile] = deserialize<Button>("ui\\AddFromFileButton.json");
    m_buttons[ButtonKey::Remove] = deserialize<Button>("ui\\RemoveButton.json");
    m_buttons[ButtonKey::ReplaceFromFile] = deserialize<Button>("ui\\ReplaceFromFileButton.json");
}

void PropsMenuToolBar::clear()
{
    m_bar->clear();
}

void PropsMenuToolBar::addButton(
    ButtonKey::Enum buttonKey, const std::function<void()>& callback)
{
    auto it = m_buttons.find(buttonKey);
    if (it == m_buttons.end())
        return;
    auto button = it->second;
    button->setCallback(callback);
    m_bar->addObject(button);
}

} }

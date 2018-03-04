/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "PropsMenuToolBar.h"
#include <gamebase/serial/LoadObj.h>

namespace gamebase { namespace editor {

PropsMenuToolBar::PropsMenuToolBar(Layout bar)
    : m_bar(bar)
{
    m_buttons[ButtonKey::Add] = loadObj<Button>("ui\\AddButton.json");
    m_buttons[ButtonKey::AddFromFile] = loadObj<Button>("ui\\AddFromFileButton.json");
    m_buttons[ButtonKey::Remove] = loadObj<Button>("ui\\RemoveButton.json");
    m_buttons[ButtonKey::Save] = loadObj<Button>("ui\\SaveButton.json");
    m_buttons[ButtonKey::ReplaceFromFile] = loadObj<Button>("ui\\ReplaceFromFileButton.json");
    m_buttons[ButtonKey::Down] = loadObj<Button>("ui\\DownButton.json");
    m_buttons[ButtonKey::Up] = loadObj<Button>("ui\\UpButton.json");
    m_buttons[ButtonKey::Cut] = loadObj<Button>("ui\\CutButton.json");
    m_buttons[ButtonKey::Copy] = loadObj<Button>("ui\\CopyButton.json");
    m_buttons[ButtonKey::Paste] = loadObj<Button>("ui\\PasteButton.json");
    m_buttons[ButtonKey::AddFromClipboard] = loadObj<Button>("ui\\AddFromClipboardButton.json");
}

void PropsMenuToolBar::clear()
{
    m_bar.clear();
}

void PropsMenuToolBar::addButton(
    ButtonKey::Enum buttonKey, const std::function<void()>& callback)
{
    auto it = m_buttons.find(buttonKey);
    if (it == m_buttons.end())
        return;
    auto button = it->second;
    button.setCallback(callback);
    m_bar.add(button);
}

} }

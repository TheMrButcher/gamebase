#pragma once

#include "ExtFilePathDialog.h"
#include <gamebase/engine/Button.h>
#include <gamebase/engine/TextBox.h>
#include <gamebase/text/TextBank.h>

namespace gamebase { namespace editor {

class ErrorMessageWindow {
public:
    ErrorMessageWindow() : m_panel(0) {}

    ErrorMessageWindow(Panel* panel);

    void showWithMessage(const std::string& prefix, const std::string& message = "");

private:
    Panel* m_panel;
    StaticLabel* m_message;
    ScrollableArea* m_messageArea;
};

void createBackup(const std::string& pathStr, int backupsNum = 1);

ErrorMessageWindow& getErrorMessageWindow();

ExtFilePathDialog& getExtFilePathDialog();

extern std::string g_clipboard;

extern std::shared_ptr<TextBank> g_textBank;

} }

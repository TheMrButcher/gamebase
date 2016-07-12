#pragma once

#include "ExtFilePathDialog.h"

namespace gamebase { namespace editor {

extern std::string g_backupPath;

extern std::string g_clipboard;

extern TextBank g_textBank;

class ErrorMessageWindow {
public:
    ErrorMessageWindow() : m_panel() {}

    ErrorMessageWindow(Panel panel);

    void showWithMessage(const std::string& prefix, const std::string& message = "");

private:
    Panel m_panel;
    Label m_message;
    impl::ScrollableArea* m_messageArea;
};

void createBackup(const std::string& pathStr, int backupsNum = 1);

void createBackupFolder();

ErrorMessageWindow& getErrorMessageWindow();
inline void showError(const std::string& prefix, const std::string& message = "")
{
    getErrorMessageWindow().showWithMessage(prefix, message);
}

ExtFilePathDialog& getExtFilePathDialog();

} }

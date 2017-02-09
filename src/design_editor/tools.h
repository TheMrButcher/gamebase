/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

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
    Layout m_messageArea;
};

void createBackup(const std::string& pathStr, int backupsNum = 1);
void createBackupFolder();

ErrorMessageWindow& getErrorMessageWindow();
inline void showError(const std::string& prefix, const std::string& message = "")
{
    getErrorMessageWindow().showWithMessage(prefix, message);
}

void createFilePathDialog(Panel panel);
ExtFilePathDialog& getDesignPathDialog();
ExtFilePathDialog& getLocalDesignPathDialog();
ExtFilePathDialog& getImagePathDialog();
ExtFilePathDialog& getBackupPathDialog();
void resetDesignFileName();
void resetAllPaths();

} }

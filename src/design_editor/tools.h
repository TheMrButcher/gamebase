/**
 * Copyright (c) 2017 Slavnejshev Filipp
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
    void attachPanel(Panel panel);
    void showWithMessage(const std::string& prefix, const std::string& message = "");

private:
    Panel m_panel;
    Label m_message;
    Layout m_messageArea;
};

class ConfirmationDialog {
public:
    ConfirmationDialog() : m_panel() {}
    void attachPanel(Panel panel);
    void init(const std::string& theme, const std::function<void()>& callback);

private:
    Panel m_panel;
    Label m_title;
    Label m_question;
    Button m_ok;
};

void createBackup(const std::string& pathStr, int backupsNum = 1);
void createBackupFolder();

ErrorMessageWindow& getErrorMessageWindow();
inline void showError(const std::string& prefix, const std::string& message = "")
{
    getErrorMessageWindow().showWithMessage(prefix, message);
}

ConfirmationDialog& getConfirmationDialog();

void createFilePathDialog(Panel panel);
ExtFilePathDialog& getDesignPathDialog();
void initLocalDesignPathDialog(
    const std::function<void(const std::string&)>& okCallback,
    ExtFilePathDialog::Config::Mode mode);
ExtFilePathDialog& getImagePathDialog();
ExtFilePathDialog& getBackupPathDialog();
void resetDesignFileName();
void resetAllPaths();

} }

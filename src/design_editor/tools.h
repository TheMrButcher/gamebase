#pragma once

#include "ExtFilePathDialog.h"
#include <gamebase/engine/AnimatedButtonSkin.h>
#include <gamebase/engine/Button.h>
#include <gamebase/engine/TextBox.h>

namespace gamebase { namespace editor {

void createBackup(const std::string& pathStr, int backupsNum = 1);

ExtFilePathDialog& getExtFilePathDialog();

extern std::string g_clipboard;

} }

#include "tools.h"
#include <gamebase/engine/StaticFilledRect.h>
#include <gamebase/engine/AligningOffset.h>
#include <gamebase/engine/SmoothChange.h>
#include <gamebase/engine/StaticLabel.h>
#include <gamebase/engine/FixedOffset.h>
#include <gamebase/engine/FixedBox.h>
#include <gamebase/engine/RelativeBox.h>
#include <gamebase/utils/FileIO.h>

namespace gamebase { namespace editor {

namespace {
std::string backupExtension(int index)
{
    std::ostringstream ss;
    ss << "backup" << index;
    return ss.str();
}
}

void createBackup(const std::string& pathStr, int backupsNum)
{
    for (int i = backupsNum; i > 0; --i) {
        auto pathToBackup = makePathStr("", pathStr, backupExtension(i));
        if (fileExists(pathToBackup)) {
            if (i == backupsNum)
                removeFile(pathToBackup);
            else
                renameFile(pathToBackup, makePathStr("", pathStr, backupExtension(i + 1)));
        }
    }
    if (fileExists(pathStr))
        renameFile(pathStr, makePathStr("", pathStr, backupExtension(1)));
}

FilePathDialog& getFilePathDialog()
{
    static FilePathDialog dialog;
    return dialog;
}

bool isInterfaceExtended = false;

std::string g_clipboard = "{\"_empty\":true}";

} }

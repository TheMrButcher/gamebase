#include "tools.h"
#include "Settings.h"
#include <gamebase/engine/StaticFilledRect.h>
#include <gamebase/engine/AligningOffset.h>
#include <gamebase/engine/SmoothChange.h>
#include <gamebase/engine/StaticLabel.h>
#include <gamebase/engine/FixedOffset.h>
#include <gamebase/engine/FixedBox.h>
#include <gamebase/engine/RelativeBox.h>
#include <gamebase/engine/ScrollableArea.h>
#include <gamebase/utils/FileIO.h>
#include <gamebase/utils/StringUtils.h>
#include <sstream>

namespace gamebase { namespace editor {

namespace {
std::string backupExtension(int index)
{
    std::ostringstream ss;
    ss << "backup" << index;
    return ss.str();
}
}

std::string g_backupPath;

std::string g_clipboard = "{\"_empty\":true}";

std::shared_ptr<TextBank> g_textBank;

ErrorMessageWindow::ErrorMessageWindow(Panel* panel)
    : m_panel(panel)
    , m_message(panel->getChild<Label>("message"))
    , m_messageArea(panel->getChild<ScrollableArea>("messageArea"))
{
    m_panel->setVisible(false);
    m_panel->getChild<Button>("ok")->setCallback(std::bind(&Panel::close, m_panel));
}

void ErrorMessageWindow::showWithMessage(const std::string& prefix, const std::string& message)
{
    if (!m_panel)
        return;
    std::ostringstream ss;
    if (message.empty()) {
        ss << prefix;
    } else {
        ss << prefix << ". Reason: " << message;
    }
    std::cout << ss.str() << std::endl;
    m_message->setText(ss.str());
    m_messageArea->update();
    m_panel->resetPosition();
    m_panel->setVisible(true);
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

void createBackupFolder()
{
    g_backupPath = "backup";
    int i = 0;
    for (;;) {
        auto fileDesc = fileInfo(g_backupPath);
        if (fileDesc.type == FileDesc::None) {
            createDir(g_backupPath);
            return;
        }
        if (fileDesc.type == FileDesc::Directory)
            return;
        if (i > 255) {
            std::cerr << "Can't create folder for backups, all 256 attempts are failed";
            settings::isBackupEnabled = false;
            g_backupPath = "";
            return;
        }
        g_backupPath = "backup" + toString(++i);
    }
}

ErrorMessageWindow& getErrorMessageWindow()
{
    static ErrorMessageWindow window;
    return window;
}

ExtFilePathDialog& getExtFilePathDialog()
{
    static ExtFilePathDialog dialog;
    return dialog;
}

} }

#include "tools.h"
#include "Settings.h"
#include <gamebase/impl/ui/ScrollableArea.h>

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

std::shared_ptr<impl::TextBank> g_textBank;

ErrorMessageWindow::ErrorMessageWindow(Panel panel)
    : m_panel(panel)
    , m_message(panel.child<Label>("message"))
    , m_messageArea(panel.getImpl()->getChild<impl::ScrollableArea>("messageArea"))
{
    m_panel.hide();
    m_panel.child<Button>("ok").setCallback(std::bind(&Panel::hide, m_panel));
}

void ErrorMessageWindow::showWithMessage(const std::string& prefix, const std::string& message)
{
    std::ostringstream ss;
    if (message.empty()) {
        ss << prefix;
    } else {
        ss << prefix << ". Reason: " << message;
    }
    std::cout << ss.str() << std::endl;
    m_message.setText(ss.str());
    m_messageArea->update();
    m_panel.update();
    m_panel.show();
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

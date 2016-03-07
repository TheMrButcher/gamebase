#include "ExtFilePathDialog.h"
#include <gamebase/engine/LinearLayout.h>
#include <gamebase/engine/ScrollableArea.h>
#include <gamebase/engine/StaticLabel.h>
#include <gamebase/serial/JsonDeserializer.h>
#include <gamebase/utils/FileIO.h>
#include <gamebase/utils/StringUtils.h>

namespace gamebase { namespace editor {

ExtFilePathDialog::ExtFilePathDialog(Panel* panel)
    : m_panel(panel)
    , m_filesList(panel->getChild<LinearLayout>("filesList"))
    , m_filesArea(panel->getChild<ScrollableArea>("filesArea"))
    , m_textBox(panel->getChild<TextBox>("textbox"))
    , m_absPathLabel(panel->getChild<Label>("absPath"))
    , m_ok(panel->getChild<Button>("ok"))
    , m_cancel(panel->getChild<Button>("cancel"))
{
    m_panel->setVisible(false);
}

void ExtFilePathDialog::updateFilesView()
{
    m_filesList->clear();

    auto fullPath = addSlash(m_rootPath) + m_relativePath;
    if (fullPath.empty())
        fullPath = ".";
    auto fullPathLocal = convertToLocal(fullPath);
    auto absPathLocal = normalizePath(absolutePath(addSlash(fullPathLocal)));
    m_absPathLabel->setText(convertToUtf8(absPathLocal));
    if (absPathLocal.length() > 3) {
        auto button = loadObj<Button>("ui\\DirButton.json");
        button->getChild<Label>("label")->setText("..");
        button->setCallback(std::bind(&ExtFilePathDialog::goUp, this));
        m_filesList->addObject(button);
    }

    auto files = listFilesInDirectory(fullPathLocal);
    for (auto it = files.begin(); it != files.end(); ++it) {
        const auto& desc = *it;
        if (desc.type == FileDesc::Directory)
        {
            auto dirNameUtf8 = convertToUtf8(desc.fileName);
            auto button = loadObj<Button>("ui\\DirButton.json");
            button->getChild<Label>("label")->setText(dirNameUtf8);
            button->setCallback(std::bind(&ExtFilePathDialog::goInto, this, dirNameUtf8));
            m_filesList->addObject(button);
        }
    }
        
    for (auto it = files.begin(); it != files.end(); ++it) {
        const auto& desc = *it;
        if (desc.type == FileDesc::File)
        {
            auto fullFileNameUtf8 = convertToUtf8(desc.fullFileName());
            auto button = loadObj<Button>("ui\\FileButton.json");
            button->getChild<Label>("label")->setText(fullFileNameUtf8);
            button->setCallback(std::bind(&ExtFilePathDialog::selectFile, this, fullFileNameUtf8));
            m_filesList->addObject(button);
        }
    }

    m_filesArea->update();
}

void ExtFilePathDialog::goUp()
{
    m_relativePath = addSlash(m_relativePath) + "..";
    updateFilesView();
}

void ExtFilePathDialog::goInto(const std::string& dirName)
{
    m_relativePath = addSlash(m_relativePath) + dirName;
    updateFilesView();
}

void ExtFilePathDialog::selectFile(const std::string& fileName)
{
    m_textBox->setText(fileName);
}

void ExtFilePathDialog::adaptCall(
    const std::function<void(const std::string&)>& okCallback,
    const std::string& path,
    const std::string& fileName)
{
    if (okCallback) {
        auto fullPath = addSlash(m_rootPath);
        if (fileName.empty())
            fullPath += path;
        else
            fullPath += addSlash(path) + fileName;
        okCallback(fullPath);
    }
}

void ExtFilePathDialog::processResult(
    const std::function<void(const std::string&, const std::string&)>& callback)
{
    auto localRelativePath = convertToLocal(m_relativePath);
    auto localFileName = convertToLocal(fileName());
    if (callback)
        callback(localRelativePath, localFileName);
    m_panel->setVisible(false);
}

} }

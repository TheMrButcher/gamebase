/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "tools.h"
#include "Settings.h"

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

TextBank g_textBank;

void ErrorMessageWindow::attachPanel(Panel panel)
{
	m_panel = panel;
	m_message = panel.child<Label>("message");
	m_messageArea = panel.child<Layout>("messageArea");
    m_panel.hide();
	m_panel.child<Button>("ok").setCallback([this]() { m_panel.hide(); });
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
    m_messageArea.update();
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

ExtFilePathDialog& getFilePathDialog()
{
    static ExtFilePathDialog dialog;
    return dialog;
}

namespace {
struct PathDesc {
	enum Enum {
		Design,
		LocalDesign,
		Image,
		Backup,
		MaxValue
	};

	std::string relativePath;
	std::string fileName;

	void saveCurrent()
	{
		auto& dialog = getFilePathDialog();
		relativePath = dialog.relativePath();
		fileName = dialog.fileName();
	}
};

std::vector<PathDesc> pathDescriptors;
PathDesc::Enum lastPathDescriptor;

ExtFilePathDialog& switchFilePathDialog(
	PathDesc::Enum newPathDescriptor,
	const std::string& rootPath)
{
	auto& dialog = getFilePathDialog();
	if (lastPathDescriptor == newPathDescriptor)
		return dialog;
	pathDescriptors[lastPathDescriptor].saveCurrent();
	lastPathDescriptor = newPathDescriptor;
	dialog.setRootPath(rootPath);
	dialog.setRelativePath(pathDescriptors[newPathDescriptor].relativePath);
	dialog.setFileName(pathDescriptors[newPathDescriptor].fileName);
	return dialog;
}
}

void createFilePathDialog(Panel panel)
{
	getFilePathDialog().attachPanel(panel);
	pathDescriptors.resize(PathDesc::MaxValue);
	resetAllPaths();
	lastPathDescriptor = PathDesc::Design;
}

ExtFilePathDialog& getDesignPathDialog()
{
	return switchFilePathDialog(PathDesc::Design, settings::workDir);
}

void initLocalDesignPathDialog(
    const std::function<void(const std::string&)>& okCallback)
{
    auto& dialog = switchFilePathDialog(PathDesc::LocalDesign, settings::workDir);
    dialog.init(okCallback);
}

ExtFilePathDialog& getImagePathDialog()
{
	return switchFilePathDialog(PathDesc::Image, settings::imagesDir);
}

ExtFilePathDialog& getBackupPathDialog()
{
	return switchFilePathDialog(PathDesc::Backup, g_backupPath);
}

void resetDesignFileName()
{
	pathDescriptors[PathDesc::Design].fileName = "Unnamed.json";
}

void resetAllPaths()
{
    auto& dialog = getFilePathDialog();
    switch (lastPathDescriptor) {
    case PathDesc::Image: dialog.setRootPath(settings::imagesDir); break;
    case PathDesc::Backup: dialog.setRootPath(g_backupPath); break;
    default: dialog.setRootPath(settings::workDir); break;
    }

	pathDescriptors[PathDesc::Design].relativePath = "";
	pathDescriptors[PathDesc::Design].fileName = "Unnamed.json";

	pathDescriptors[PathDesc::LocalDesign].relativePath = "";
	pathDescriptors[PathDesc::LocalDesign].fileName = "Unnamed.json";

	pathDescriptors[PathDesc::Image].relativePath = "";
	pathDescriptors[PathDesc::Image].fileName = "";

	pathDescriptors[PathDesc::Backup].relativePath = "";
	pathDescriptors[PathDesc::Backup].fileName = "Last.json";
}

} }

/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "ExtFilePathDialog.h"
#include "tools.h"
#include <gamebase/impl/ui/ScrollableArea.h>

namespace gamebase { namespace editor {

ExtFilePathDialog::ExtFilePathDialog(Panel panel)
    : m_panel(panel)
    , m_filesList(panel.child<Layout>("filesList"))
    , m_filesArea(panel.child<Layout>("filesArea"))
    , m_textBox(panel.child<TextBox>("textbox"))
    , m_absPathLabel(panel.child<Label>("absPath"))
    , m_ok(panel.child<Button>("ok"))
    , m_cancel(panel.child<Button>("cancel"))
{
    m_panel.hide();
}

void ExtFilePathDialog::setFilePath(const std::string& value)
{
	if (value.empty())
		return;
	setRelativePath(pathToDir(value));
	setFileName(gamebase::fileName(value));
}

void ExtFilePathDialog::updateFilesView()
{
    m_filesList.clear();
	try {
		auto fullPath = addSlash(m_rootPath) + m_relativePath;
		if (fullPath.empty())
			fullPath = ".";
		auto fullPathLocal = toLocal(fullPath);
		auto absPathLocal = normalizePath(absolutePath(addSlash(fullPathLocal)));
		m_absPathLabel.setText(toUnicode(absPathLocal));
		if (absPathLocal.length() > 3) {
			auto button = loadObj<Button>("ui\\DirButton.json");
			button.child<Label>("label").setText("..");
			button.setCallback(std::bind(&ExtFilePathDialog::goUp, this));
			m_filesList.add(button);
		}

		auto files = listFilesInDirectory(fullPathLocal);
		for (auto it = files.begin(); it != files.end(); ++it) {
			const auto& desc = *it;
			if (desc.type == FileDesc::Directory)
			{
				auto dirNameUtf8 = toUnicode(desc.fileName);
				auto button = loadObj<Button>("ui\\DirButton.json");
				button.child<Label>("label").setText(dirNameUtf8);
				button.setCallback(std::bind(&ExtFilePathDialog::goInto, this, dirNameUtf8));
				m_filesList.add(button);
			}
		}
        
		for (auto it = files.begin(); it != files.end(); ++it) {
			const auto& desc = *it;
			if (desc.type == FileDesc::File)
			{
				auto fullFileNameUtf8 = toUnicode(desc.fullFileName());
				auto button = loadObj<Button>("ui\\FileButton.json");
				button.child<Label>("label").setText(fullFileNameUtf8);
				button.setCallback(std::bind(&ExtFilePathDialog::selectFile, this, fullFileNameUtf8));
				m_filesList.add(button);
			}
		}

		m_filesArea.update();
	} catch (Exception& ex) {
		if (m_relativePath.empty()) {
			m_panel.hide();
			showError("Error while showing file dialog", ex.what());
			return;
		}

		m_relativePath = "";
		updateFilesView();
	}
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
    m_textBox.setText(fileName);
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
    auto localRelativePath = toLocal(m_relativePath);
    auto localFileName = toLocal(fileName());
    if (callback)
        callback(localRelativePath, localFileName);
    m_panel.hide();
}

} }

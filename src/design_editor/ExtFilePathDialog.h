/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/ui/Panel.h>
#include <gamebase/ui/Layout.h>
#include <gamebase/ui/TextBox.h>
#include <gamebase/ui/Button.h>
#include <gamebase/drawobj/Label.h>

namespace gamebase { namespace editor {

class ExtFilePathDialog {
public:
    ExtFilePathDialog()
        : m_config({ Config::Open, boost::none })
    {}

    void attachPanel(Panel panel);

    struct Config {
        enum Mode {
            Save,
            Open,
            LoadImage,
            LoadSound
        };

        Mode mode;
        boost::optional<std::string> curFilePathLocal;
    };
    void setConfig(const Config& config) { m_config = config; }

    const std::string& rootPath() const { return m_rootPath; }
    void setRootPath(const std::string& value) { m_rootPath = value; }

    const std::string& relativePath() const { return m_relativePath; }
    void setRelativePath(const std::string& value) { m_relativePath = value; }

    const std::string& fileName() const { return m_textBox.text(); }
    void setFileName(const std::string& value) { m_textBox.setText(value); }

	void setFilePath(const std::string& value);

    void setCallbacks(
        const std::function<void(const std::string&, const std::string&)>& okCallback,
        const std::function<void()>& cancelCallback = nullptr)
    {
        m_panel.setCallback(cancelCallback);
		m_cancel.setCallback([this]() { m_panel.hide(); });
		m_ok.setCallback([this, okCallback]() { processResult(okCallback); });
    }

    void init()
    {
        updateFilesView();
        m_panel.update();
        m_panel.show();
    }

    void init(const std::function<void(const std::string&)>& okCallback)
    {
        std::function<void(const std::string&, const std::string&)> adapted =
			[this, okCallback](const std::string& path, const std::string& fileName)
		{
			adaptCall(okCallback, path, fileName);
		};
        setCallbacks(adapted);
        init();
    }

private:
    void updateFilesView();
    void goUp();
    void goInto(const std::string& dirName);
    void selectFile(const std::string& fileName);

    void adaptCall(
        const std::function<void(const std::string&)>& okCallback,
        const std::string& path,
        const std::string& fileName);
    void processResult(const std::function<void(const std::string&, const std::string&)>& callback);

    Panel m_panel;

    Layout m_filesList;
    Layout m_filesArea;
    TextBox m_textBox;
    Label m_absPathLabel;
    Button m_ok;
    Button m_cancel;
    std::string m_rootPath;
    std::string m_relativePath;
    Config m_config;
};

} }

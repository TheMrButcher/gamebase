/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "SettingsView.h"
#include "Settings.h"
#include "tools.h"
#include "Version.h"
#include <gamebase/impl/app/Config.h>
#include <fstream>

namespace gamebase { namespace editor {

void SettingsView::init(Layout layout)
{
    m_layout = layout;
    layout.child<TextBox>("workdir").setText(settings::workDir);
    layout.child<TextBox>("imgdir").setText(settings::imagesDir);
    layout.child<TextBox>("sounddir").setText(settings::soundsDir);
    layout.child<TextBox>("musicdir").setText(settings::musicDir);
    layout.child<ComboBox>("dimension").setText(
        impl::getValueFromConfig("width") + "x" + impl::getValueFromConfig("height"));
    layout.child<CheckBox>("windowed").setChecked(
        impl::app->mode() == impl::GraphicsMode::Window);
    layout.child<CheckBox>("backup").setChecked(settings::isBackupEnabled);
    if (g_backupPath.empty())
        layout.child<CheckBox>("backup").disable();
    layout.child<CheckBox>("complexBoxMode").setChecked(settings::isComplexBoxMode);
    layout.child<CheckBox>("complexLayerMode").setChecked(settings::isComplexLayerMode());
    layout.child<CheckBox>("complexAnimationMode").setChecked(settings::isComplexAnimationMode());
	layout.child<Button>("ok").setCallback([this]() { apply(); });
    layout.child<Label>("versionLabel").setText(version());
}

void SettingsView::apply()
{
    settings::workDir = m_layout.child<TextBox>("workdir").text();
    settings::imagesDir = m_layout.child<TextBox>("imgdir").text();
    settings::soundsDir = m_layout.child<TextBox>("sounddir").text();
    settings::musicDir = m_layout.child<TextBox>("musicdir").text();
    settings::isBackupEnabled = m_layout.child<CheckBox>("backup").isChecked();
    settings::isComplexBoxMode = m_layout.child<CheckBox>("complexBoxMode").isChecked();
    settings::setComplexLayerMode(m_layout.child<CheckBox>("complexLayerMode").isChecked());
    settings::setComplexAnimationMode(m_layout.child<CheckBox>("complexAnimationMode").isChecked());

    std::string dimensionStr = m_layout.child<ComboBox>("dimension").text();
    int xPos = dimensionStr.find("x");
    int width = toInt(dimensionStr.substr(0, xPos));
    int height = toInt(dimensionStr.substr(xPos + 1));
    impl::GraphicsMode::Enum mode = m_layout.child<CheckBox>("windowed").isChecked()
        ? impl::GraphicsMode::Window : impl::GraphicsMode::Fullscreen;

    settings::formMainConfig(width, height, mode);
    settings::formDesignedObjConfig();

    std::ofstream confFile(impl::app->configName());
    confFile << settings::mainConf;

	resetAllPaths();
}

} }

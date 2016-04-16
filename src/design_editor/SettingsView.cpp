#include "SettingsView.h"
#include "Settings.h"
#include "tools.h"
#include <gamebase/core/Core.h>
#include <gamebase/engine/Application.h>
#include <gamebase/engine/CanvasLayout.h>
#include <gamebase/engine/CheckBox.h>
#include <gamebase/engine/ComboBox.h>
#include <gamebase/utils/StringUtils.h>
#include <fstream>

namespace gamebase { namespace editor {

void SettingsView::init(CanvasLayout* layout)
{
    m_layout = layout;
    layout->getChild<TextBox>("#workdir")->setText(settings::workDir);
    layout->getChild<TextBox>("#imgdir")->setText(settings::imagesDir);
    layout->getChild<ComboBox>("#dimension")->setText(
        getValueFromConfig("width") + "x" + getValueFromConfig("height"));
    layout->getChild<CheckBox>("#windowed")->setChecked(
        app->mode() == GraphicsMode::Window);
    layout->getChild<CheckBox>("#backup")->setChecked(settings::isBackupEnabled);
    if (g_backupPath.empty())
        layout->getChild<CheckBox>("#backup")->disable();
    layout->getChild<Button>("#ok")->setCallback(std::bind(&SettingsView::apply, this));
}

void SettingsView::apply()
{
    settings::workDir = m_layout->getChild<TextBox>("#workdir")->text();
    getExtFilePathDialog().setRootPath(settings::workDir);
    settings::imagesDir = m_layout->getChild<TextBox>("#imgdir")->text();
    settings::isBackupEnabled = m_layout->getChild<CheckBox>("#backup")->isChecked();

    std::string dimensionStr = m_layout->getChild<ComboBox>("#dimension")->text();
    int xPos = dimensionStr.find("x");
    int width = toInt(dimensionStr.substr(0, xPos));
    int height = toInt(dimensionStr.substr(xPos + 1));
    GraphicsMode::Enum mode = m_layout->getChild<CheckBox>("#windowed")->isChecked()
        ? GraphicsMode::Window : GraphicsMode::Fullscreen;

    settings::formMainConfig(width, height, mode);
    settings::formDesignedObjConfig();

    std::ofstream confFile(app->configName());
    confFile << settings::mainConf;
}

} }

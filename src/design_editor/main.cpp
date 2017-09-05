/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#define GAMEBASE_DONT_DECLARE_APP_TYPE

namespace gamebase { namespace editor {
class MainApp;
} }
typedef gamebase::editor::MainApp MyApp;
#define GAMEBASE_APP_TYPE MyApp

#include "TreeView.h"
#include "DesignViewBuilder.h"
#include "Presentation.h"
#include "tools.h"
#include "NewObjDialog.h"
#include "SettingsView.h"
#include "Settings.h"
#include "SimpleTreeViewSkin.h"
#include <reg/RegisterSwitcher.h>
#include <dvb/ColorDialog.h>
#include <gamebase/impl/relbox/RelativeBox.h>
#include <gamebase/impl/relpos/AligningOffset.h>
#include <gamebase/impl/app/Config.h>
#include <gamebase/impl/serial/JsonSerializer.h>
#include <fstream>

namespace gamebase { namespace editor {

class MainApp : public App {
public:
    static const int DESIGN_VIEW = 0;
    static const int PRESENTATION_VIEW = 1;
    static const int SETTINGS_VIEW = 2;

    static const int MAIN_VIEW = 0;
    static const int FULLSCREEN_VIEW = 1;

    MainApp()
    {
        setDesign("ui\\Design.json");
    }

    virtual void load() override
    {
        RegisterSwitcher::init();
        settings::init();
        createBackupFolder();

        std::cout << "Generating default patterns for presentation view..." << std::endl;
        presentationForPresentationView()->serializeAllDefaultPatterns();
        presentationForDesignView()->serializeAllDefaultPatterns();

        std::cout << "Loading text bank..." << std::endl;
        g_textBank = loadObj<TextBank>("texts\\TextBank.json");

        std::cout << "Creating editor's views..." << std::endl;

        createFilePathDialog(design.child<Panel>("filePathDialog"));

        auto viewSelector = makeRaw(m_viewSelector);
        connect0(design.child<Button>("exit"), showClosingDialog);
        connect1(design.child<Button>("settings"), selectView, SETTINGS_VIEW);
        connect1(design.child<Button>("design"), selectView, DESIGN_VIEW);

        if (settings::isInterfaceExtended)
            connect1(design.child<Button>("scheme"), selectView, PRESENTATION_VIEW);
        else
            design.child<Button>("scheme").hide();

        m_designViewLayout.child<Button>("new").setCallback(
			[newObjDialog = &m_newObjDialog]() { newObjDialog->run(); });

		{
			std::function<void(const std::string&, const std::string&)> pathProcessor =
				[this](const std::string& localRelativePath, const std::string& localFileName)
			{
				saveDesign(localRelativePath, localFileName);
			};
            connect1(m_designViewLayout.child<Button>("save"), initFilePathDialog, pathProcessor);
        }
        {
			std::function<void(const std::string&, const std::string&)> pathProcessor =
				[this](const std::string& localRelativePath, const std::string& localFileName)
			{
				loadDesign(localRelativePath, localFileName);
			};
            connect1(m_designViewLayout.child<Button>("load"), initFilePathDialog, pathProcessor);
        }
        connect0(m_designViewLayout.child<Button>("update"), updateDesignByModel);
        connect0(m_designViewLayout.child<Button>("fullscreen"), enterFullScreen);
		connect0(m_designViewLayout.child<Button>("restore"), initOpenBackupDialog);
        if (settings::isInterfaceExtended)
            connect0(m_designViewLayout.child<Button>("rebuild"), setDesignFromCurrentObject);
        else
            m_designViewLayout.child<Button>("rebuild").hide();

        {
            auto treeLayout = m_designViewLayout.child<Layout>("designTreeView");
            auto skin = std::make_shared<SimpleTreeViewSkin>(
                std::make_shared<impl::RelativeBox>(impl::RelativeValue(), impl::RelativeValue()));
            auto treeView = std::make_shared<TreeView>(nullptr, skin);
            treeLayout.getImpl()->addObject(treeView);
            m_designTreeView = treeView.get();
        }

        m_designPropsMenuToolBar = std::make_shared<PropsMenuToolBar>(
            m_designViewLayout.child<Layout>("m_designPropsMenuToolBar"));

        if (settings::isInterfaceExtended) {
            connect0(design.child<Button>("save_scheme"), savePresentation);
            connect0(design.child<Button>("update_templates"), savePatterns);

            auto treeLayout = design.child<Layout>("presTreeView");
            auto skin = std::make_shared<SimpleTreeViewSkin>(
                std::make_shared<impl::RelativeBox>(impl::RelativeValue(), impl::RelativeValue()));
            auto treeView = std::make_shared<TreeView>(nullptr, skin);
            treeLayout.getImpl()->addObject(treeView);

            DesignViewBuilder builder(
                *treeView,
                design.child<Selector>("presPropertiesMenu"),
                m_presentationModel,
                presentationForPresentationView(),
                std::make_shared<PropsMenuToolBar>(
                    design.child<Layout>("presPropsMenuToolBar")),
                design.child<Layout>("presPropsMenuArea"));
            impl::Serializer serializer(&builder, impl::SerializationMode::ForcedFull);
            serializer << "" << presentationForDesignView();
        }

        m_settingsView.init(m_viewSelector.get<Layout>(SETTINGS_VIEW));
        m_viewSelector.select(DESIGN_VIEW);
        m_mainSelector.select(MAIN_VIEW);

        {
            std::function<void(const std::string&)> pathProcessor =
				[this](const std::string& path) { createObject(path); };
            m_newObjDialog.init(design.child<Panel>("newObjDialog"), pathProcessor);
        }

        m_runAnimationDialog.child<Button>("ok").setCallback(
			[this]() { runAnimation(); });
        m_runAnimationDialog.child<Button>("cancel").setCallback(
			[this]() { m_runAnimationDialog.hide(); });
        m_designViewLayout.child<Button>("animation").setCallback(
			[this]() { m_runAnimationDialog.show(); });
		m_closingDialog.child<Button>("ok").setCallback([this]()
		{
			m_isCloseConfirmed = true;
			close();
		});
		m_closingDialog.child<Button>("cancel").setCallback([this]() { m_closingDialog.hide(); });

        getErrorMessageWindow().attachPanel(design.child<Panel>("errorMessageWindow"));
		getColorDialog().attachPanel(design.child<Panel>("colorDialog"));

        setDesignFromCurrentObject();
        updateDesign(serializeModel());
    }

    void process(Input input) override
    {
        if (m_mainSelector.selected() == FULLSCREEN_VIEW) {
            if (input.justPressed(InputKey::Escape))
                m_mainSelector.select(MAIN_VIEW);
        }

        if (input.wheel() != 0) {
            if (m_mainSelector.selected() == MAIN_VIEW
                && m_viewSelector.selected() == DESIGN_VIEW
                && impl::isMouseOn(m_designTreeView)) {
                m_designTreeView->getChild<impl::ScrollBar>("vertScrollBar")->move(input.wheel());
            }
        }
    }

	bool onClose() override
	{
		if (m_isCloseConfirmed)
			return true;
		showClosingDialog();
		return false;
	}

private:
	void showClosingDialog()
	{
		m_closingDialog.show();
	}

    void selectView(int index)
    {
        m_viewSelector.select(index);
    }

    void setDesignFromCurrentObject()
    {
        std::cout << "Initing design view..." << std::endl;
        m_designTreeView->clear();
        m_designPropertiesMenu.clear();
        m_designModel.clear();
        m_designPropsMenuToolBar->clear();
        std::cout << "Creating design by object..." << std::endl;
        try {
            DesignViewBuilder builder(*m_designTreeView, makeRaw(m_designPropertiesMenu),
                m_designModel, presentationForDesignView(),
                m_designPropsMenuToolBar, m_designPropsMenuArea);
            impl::Serializer serializer(&builder, impl::SerializationMode::ForcedFull);
            serializer << "" << m_currentObjectForDesign;
        } catch (std::exception& ex) {
            showError("Error while building design view for current object", ex.what());
            return;
        }

        std::cout << "Loading resources..." << std::endl;
        m_designTreeView->countBoxes();
        m_designTreeView->loadResources();
        m_designPropsMenuArea.update();

        std::cout << "Done updating design by object" << std::endl;
    }

    void updateDesignByModel()
    {
        auto designStr = serializeModel();
        if (settings::isBackupEnabled) {
            auto newBackupFileName = addSlash(g_backupPath) + "Last.json";
            createBackup(newBackupFileName, 4);
            std::ofstream newBackupFile(newBackupFileName);
            newBackupFile << designStr;
        }
        updateDesign(designStr);
    }

    bool updateDesign(const std::string& designStr, bool allowErrors = false)
    {
        if (designStr.empty())
            return false;
        std::shared_ptr<impl::IObject> designedObj;
        std::cout << "Building object by design..." << std::endl;
        try {
			RegisterSwitcher switcher;
            deserializeFromJson(designStr, designedObj);
        } catch (std::exception& ex) {
            showError("Error while building object by design", ex.what());
            return false;
        }

        std::cout << "Adding object to canvas..." << std::endl;
        try {
            impl::configurateFromString(settings::designedObjConf, false);
            m_canvas.getImpl()->insertObject(0, designedObj);
            impl::configurateFromString(settings::mainConf, false);
        } catch (std::exception& ex)
        {
            showError("Error while trying to place object on canvas", ex.what());
            if (allowErrors) {
                m_canvas.remove(0);
            } else {
                impl::configurateFromString(settings::designedObjConf, false);
                m_canvas.getImpl()->insertObject(0, m_currentObjectForDesign);
                impl::configurateFromString(settings::mainConf, false);
                return false;
            }
        }
        m_currentObjectForDesign = designedObj;
        m_canvasArea.update();
        std::cout << "Done updating design" << std::endl;
        return true;
    }

    void savePresentation()
    {
        std::cout << "Serializing presentation..." << std::endl;
        auto presentationStr = m_presentationModel.toString(impl::JsonFormat::Styled);
        std::cout << "Building presentation by design..." << std::endl;
        std::shared_ptr<Presentation> designedPresentation;
        try {
            deserializeFromJson(presentationStr, designedPresentation);
        } catch (std::exception& ex) {
            showError("Error while building presentation by design", ex.what());
            return;
        }
        std::cout << "Setting new presentation..." << std::endl;
        setPresentationForDesignView(designedPresentation);
        std::cout << "Done saving presentation" << std::endl;
    }

    void savePatterns()
    {
        std::cout << "Saving patterns..." << std::endl;
        auto presentation = presentationForDesignView();
        presentation->serializeAllDefaultPatterns();
        std::cout << "Done saving patterns" << std::endl;
    }

    void saveDesign(const std::string& relativePathLocal, const std::string& fileNameLocal)
    {
        std::cout << "Started saving design to file..." << std::endl;
        auto designStr = serializeModel(impl::SerializationMode::Default);
        if (designStr.empty())
            return;
        if (!updateDesign(designStr))
            return;
        if (!m_currentObjectForDesign)
            return;
        
        auto fullName = 
            addSlash(toLocal(settings::workDir)) + addSlash(relativePathLocal) + fileNameLocal;
        std::cout << "Saving design in file with name: " << fullName << std::endl;
		std::ofstream file(fullName);
		file << designStr;
        std::cout << "Done saving design" << std::endl;
    }

    void loadDesignInternal(const std::string& fileNameLocal)
    {
        std::cout << "Started loading design from file..." << std::endl;
        std::string designStr;
        try {
            designStr = loadTextFile(fileNameLocal);
        } catch (std::exception& ex) {
            showError("Error while loading design", ex.what());
            return;
        }
        if (!updateDesign(designStr, true))
            return;
        setDesignFromCurrentObject();
        std::cout << "Done loading design" << std::endl;
    }

    void createObject(const std::string& fileNameLocal)
    {
        loadDesignInternal(fileNameLocal);
		resetDesignFileName();
    }

    void loadDesign(const std::string& relativePathLocal, const std::string& fileNameLocal)
    {
        loadDesignInternal(
            addSlash(toLocal(settings::workDir)) + addSlash(relativePathLocal) + fileNameLocal);
    }

    void enterFullScreen()
    {
        std::cout << "Starting building fullscreen design..." << std::endl;
        auto designStr = serializeModel();
        if (designStr.empty())
            return;
        std::shared_ptr<impl::IObject> designedObj;
        std::cout << "Building object by design..." << std::endl;
        try {
            deserializeFromJson(designStr, designedObj);
        } catch (std::exception& ex) {
            showError("Error while building object by design", ex.what());
            return;
        }
        
        std::cout << "Adding object to canvas..." << std::endl;
        try {
            impl::configurateFromString(settings::designedObjConf, false);
            m_fullscreenCanvas.getImpl()->insertObject(0, designedObj);
            impl::configurateFromString(settings::mainConf, false);
        } catch (std::exception& ex)
        {
            showError("Error while placing object on canvas", ex.what());
            return;
        }
        m_mainSelector.select(FULLSCREEN_VIEW);
        try {
            m_fullscreenCanvas.update();
        } catch (std::exception& ex)
        {
            showError("Error while updating canvas", ex.what());
            m_mainSelector.select(MAIN_VIEW);
            return;
        }
        std::cout << "Done building fullscreen design" << std::endl;
    }

    void initFilePathDialog(const std::function<void(const std::string&, const std::string&)>& callback)
    {
        auto& dialog = getDesignPathDialog();
        dialog.setCallbacks(callback);
        dialog.init();
    }

	void initOpenBackupDialog()
	{
		auto& dialog = getBackupPathDialog();
		dialog.init([this](const std::string& path) { loadDesignInternal(path); });
	}

    std::string serializeModel(impl::SerializationMode mode = impl::SerializationMode::Compressed)
    {
        std::cout << "Serializing model..." << std::endl;
        std::string designStr;
        try {
            designStr = m_designModel.toString(impl::JsonFormat::Fast);
        } catch (std::exception& ex) {
            showError("Error while serializing model", ex.what());
            return std::string();
        }

		try {
			std::shared_ptr<impl::IObject> obj;
			impl::deserializeFromJson(designStr, obj);

			designStr = impl::serializeToJson(obj, mode);
		} catch (std::exception& ex) {
            showError("Error while translating design", ex.what());
            return std::string();
		}

        return std::move(designStr);
    }

    void runAnimation()
    {
        std::cout << "Starting running animation..." << std::endl;
        auto objName = m_runAnimationDialog.child<TextBox>("objname").text();

        GameObj obj;
        try {
            if (objName.empty()) {
                if (auto optObj = impl::tryWrap<GameObj>(m_currentObjectForDesign.get())) {
                    obj = *optObj;
                } else {
                    showError("Error while casting root object to GameObj");
                    return;
                }
            } else {
                obj = m_canvas.child<GameObj>(objName);
            }
        } catch (std::exception& ex) {
            getErrorMessageWindow().showWithMessage("Error while searching for object", ex.what());
            return;
        }

        if (!dynamic_cast<impl::AnimatedObjectConstruct*>(obj.getImpl()->getInternalObj().get())) {
            showError("Object '" + objName + "' is GameObj, but has no animations");
            return;
        }
        
        std::string animName =  m_runAnimationDialog.child<TextBox>("animname").text();
        try {
            obj.anim.run(animName);
        } catch (std::exception& ex) {
            showError("Error while starting animation", ex.what());
            return;
        }

        m_runAnimationDialog.hide();

        std::cout << "Done running animation" << std::endl;
    }

    std::shared_ptr<impl::IObject> m_currentObjectForDesign;

    FromDesign(Layout, m_designViewLayout);
    DesignModel m_designModel;
    TreeView* m_designTreeView;
    FromDesign(Selector, m_designPropertiesMenu);
    FromDesign(Layout, m_designViewPropertiesLayout);
    std::shared_ptr<PropsMenuToolBar> m_designPropsMenuToolBar;
    FromDesign(Layout, m_designPropsMenuArea);
    FromDesign(Layout, m_canvas);
    FromDesign(Layout, m_canvasArea);
    FromDesign(Selector, m_mainSelector);
    FromDesign(Selector, m_viewSelector);
    FromDesign(Layout, m_fullscreenCanvas);

    DesignModel m_presentationModel;

    NewObjDialog m_newObjDialog;
	ColorDialog m_colorDialog;
    FromDesign(Panel, m_runAnimationDialog);
	FromDesign(Panel, m_closingDialog);
    SettingsView m_settingsView;

	bool m_isCloseConfirmed = false;
};

} }

int main(int argc, char** argv)
{
    using namespace gamebase::editor;
    MainApp app;
    app.setConfig("..\\Editor\\design_editor_config.json");
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}

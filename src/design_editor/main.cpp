#include "TreeView.h"
#include "DesignViewBuilder.h"
#include "Presentation.h"
#include "tools.h"
#include "NewObjDialog.h"
#include "SettingsView.h"
#include "Settings.h"
#include "SimpleTreeViewSkin.h"
#include <gamebase/impl/relbox/RelativeBox.h>
#include <gamebase/impl/relpos/AligningOffset.h>
#include <gamebase/impl/app/Config.h>
#include <fstream>

namespace gamebase { namespace editor {

class MainApp : public App {
public:
    static const int DESIGN_VIEW = 0;
    static const int PRESENTATION_VIEW = 1;
    static const int SETTINGS_VIEW = 2;

    static const int MAIN_VIEW = 0;
    static const int FULLSCREEN_VIEW = 1;

    MainApp() : m_fileName("Unnamed.json")
    {
        setDesign("ui\\VertLayout.json");
    }

    virtual void load() override
    {
        settings::init();
        createBackupFolder();

        std::cout << "Generating default patterns for presentation view..." << std::endl;
        presentationForPresentationView()->serializeAllDefaultPatterns();
        presentationForDesignView()->serializeAllDefaultPatterns();

        std::cout << "Loading text bank..." << std::endl;
        g_textBank = loadObj<TextBank>("texts\\TextBank.json");

        std::cout << "Creating editor's views..." << std::endl;
        m_mainSelector = Selector(impl::SmartPointer<impl::SelectingWidget>(std::make_shared<impl::SelectingWidget>(
            std::make_shared<impl::RelativeBox>(impl::RelativeValue(), impl::RelativeValue()))));

        auto mainLayout = loadObj<Layout>("ui\\VertLayout.json");

        m_viewSelector = Selector(impl::SmartPointer<impl::SelectingWidget>(std::make_shared<impl::SelectingWidget>(
            std::make_shared<impl::RelativeBox>(impl::RelativeValue(), impl::RelativeValue()))));

        {
            Layout topPanelLayout = loadObj<Layout>(
                settings::isInterfaceExtended
                ? std::string("ui\\TopLayoutExt.json")
                : std::string("ui\\TopLayout.json"));

            auto viewSelector = makeRaw(m_viewSelector);
            topPanelLayout.child<Button>("exit").setCallback(std::bind(&App::stop, this));
            topPanelLayout.child<Button>("settings").setCallback(
                std::bind(&Selector::select, viewSelector, SETTINGS_VIEW));
            topPanelLayout.child<Button>("design").setCallback(
                std::bind(&Selector::select, viewSelector, DESIGN_VIEW));

            if (settings::isInterfaceExtended)
                topPanelLayout.child<Button>("scheme").setCallback(
                    std::bind(&Selector::select, viewSelector, PRESENTATION_VIEW));

            mainLayout.add(topPanelLayout);
        }

        mainLayout.add(loadObj<FilledRect>("ui\\VertDelim.json"));

        m_designViewLayout = loadObj<Layout>("ui\\VertLayout.json");

        {
            auto designViewControlPanel = loadObj<Layout>(
                settings::isInterfaceExtended
                ? std::string("ui\\DesignTopLayoutExt.json")
                : std::string("ui\\DesignTopLayout.json"));
            
            designViewControlPanel.child<Button>("new").setCallback(
                std::bind(&NewObjDialog::run, &m_newObjDialog));

            {
                std::function<void(const std::string&, const std::string&)> pathProcessor =
                    std::bind(&MainApp::saveDesign, this, std::placeholders::_1, std::placeholders::_2);
                designViewControlPanel.child<Button>("save").setCallback(
                    std::bind(&MainApp::initFilePathDialog, this, pathProcessor));
            }
            {
                std::function<void(const std::string&, const std::string&)> pathProcessor =
                    std::bind(&MainApp::loadDesign, this, std::placeholders::_1, std::placeholders::_2);
                designViewControlPanel.child<Button>("load").setCallback(
                    std::bind(&MainApp::initFilePathDialog, this, pathProcessor));
            }
            designViewControlPanel.child<Button>("update").setCallback(std::bind(&MainApp::updateDesign, this));
            designViewControlPanel.child<Button>("copy").setCallback(std::bind(&MainApp::copyDesign, this));
            designViewControlPanel.child<Button>("paste").setCallback(std::bind(&MainApp::pasteDesign, this));
            designViewControlPanel.child<Button>("fullscreen").setCallback(std::bind(&MainApp::enterFullScreen, this));
            if (settings::isInterfaceExtended)
                designViewControlPanel.child<Button>("rebuild").setCallback(std::bind(&MainApp::setDesignFromCurrentObject, this));
            
            m_designViewLayout.add(designViewControlPanel);
        }

        m_designViewLayout.add(loadObj<FilledRect>("ui\\VertDelim.json"));

        {
            m_designViewPropertiesLayout = loadObj<Layout>("ui\\DesignViewPropsLayout.json");

            auto skin = std::make_shared<SimpleTreeViewSkin>(
                std::make_shared<impl::RelativeBox>(
                    impl::RelativeValue(impl::RelType::Ratio, 0.390625f), impl::RelativeValue()));
            auto treeView = std::make_shared<TreeView>(nullptr, skin);
            m_designViewPropertiesLayout.getImpl()->addObject(treeView);
            m_designTreeView = treeView.get();

            m_designViewPropertiesLayout.add(loadObj<FilledRect>("ui\\HorDelim.json"));

            auto propertiesMenuLayout = loadObj<Layout>("ui\\PropertiesMenuLayout.json");
            m_designViewPropertiesLayout.add(propertiesMenuLayout);
            
            m_designPropsMenuArea = dynamic_cast<impl::ScrollableArea*>(
                propertiesMenuLayout.getImpl()->getAbstractChild("area"));
            if (!m_designPropsMenuArea)
                THROW_EX() << "Can't find area for properties in design";
            m_designPropsMenuToolBar = std::make_shared<PropsMenuToolBar>(
                propertiesMenuLayout.child<Layout>("toolbar"));
            m_designPropertiesMenu = propertiesMenuLayout.child<Selector>("menu");

            m_designViewLayout.add(m_designViewPropertiesLayout);
        }

        m_designViewLayout.add(loadObj<FilledRect>("ui\\VertDelim.json"));

        {
            auto canvas = std::make_shared<impl::CanvasLayout>(
                std::make_shared<impl::RelativeBox>(impl::RelativeValue(), impl::RelativeValue()),
                std::make_shared<impl::AligningOffset>(impl::HorAlign::Center, impl::VertAlign::Center));
            canvas->setAdjustment(impl::Adjustment::ToFitContentAndArea);
            m_canvas = impl::wrap<Layout>(canvas.get());

            auto area = std::make_shared<impl::ScrollableArea>(
                impl::deserialize<impl::ScrollableAreaSkin>("ui\\ScrollableAreaSkin.json"));
            area->objects().addObject(canvas);
            m_canvasArea = area.get();
            m_designViewLayout.getImpl()->addObject(area);
        }
        
        m_viewSelector.insert(DESIGN_VIEW, m_designViewLayout);

        if (settings::isInterfaceExtended) {
            auto presentationViewLayout = loadObj<Layout>("ui\\VertLayout.json");
            
            {
                auto presentationViewControlPanel = loadObj<Layout>("ui\\PresTopLayout.json");

                presentationViewControlPanel.child<Button>("save_scheme").setCallback(
                    std::bind(&MainApp::savePresentation, this));
                presentationViewControlPanel.child<Button>("update_templates").setCallback(
                    std::bind(&MainApp::savePatterns, this));

                presentationViewLayout.add(presentationViewControlPanel);
            }

            presentationViewLayout.add(loadObj<FilledRect>("ui\\VertDelim.json"));

            {
                auto presentationViewPropertiesLayout = loadObj<Layout>("ui\\HorLayout.json");

                auto skin = std::make_shared<SimpleTreeViewSkin>(
                    std::make_shared<impl::RelativeBox>(
                        impl::RelativeValue(impl::RelType::Ratio, 0.390625f), impl::RelativeValue()));
                auto treeView = std::make_shared<TreeView>(nullptr, skin);
                presentationViewPropertiesLayout.getImpl()->addObject(treeView);

                presentationViewPropertiesLayout.add(loadObj<FilledRect>("ui\\HorDelim.json"));

                auto propertiesMenuLayout = loadObj<Layout>("ui\\PropertiesMenuLayout.json");
                presentationViewPropertiesLayout.add(propertiesMenuLayout);

                presentationViewLayout.add(presentationViewPropertiesLayout);

                {
                    auto* area = dynamic_cast<impl::ScrollableArea*>(
                        propertiesMenuLayout.getImpl()->getAbstractChild("area"));
                    if (!area)
                        THROW_EX() << "Can't find area for properties in presentation";
                    DesignViewBuilder builder(
                        *treeView,
                        propertiesMenuLayout.child<Selector>("menu"),
                        m_presentationModel,
                        presentationForPresentationView(),
                        std::make_shared<PropsMenuToolBar>(
                            propertiesMenuLayout.child<Layout>("toolbar")),
                        area);
                    impl::Serializer serializer(&builder);
                    serializer << "" << presentationForDesignView();
                }
            }

            m_viewSelector.insert(PRESENTATION_VIEW, presentationViewLayout);
        }

        {
            auto settingsLayout = loadObj<Layout>("ui\\SettingsLayout.json");
            m_settingsView.init(makeRaw(settingsLayout));
            m_viewSelector.insert(SETTINGS_VIEW, settingsLayout);
        }

        m_viewSelector.select(DESIGN_VIEW);
        mainLayout.add(m_viewSelector);

        m_mainSelector.insert(MAIN_VIEW, mainLayout);

        {
            auto canvas = std::make_shared<impl::CanvasLayout>(
                std::make_shared<impl::RelativeBox>(impl::RelativeValue(), impl::RelativeValue()),
                std::make_shared<impl::AligningOffset>(impl::HorAlign::Center, impl::VertAlign::Center));
            m_fullscreenCanvas = impl::wrap<Layout>(canvas);
            m_mainSelector.insert(FULLSCREEN_VIEW, m_fullscreenCanvas);
        }

        m_mainSelector.select(MAIN_VIEW);
        
        auto mainCanvas = impl::wrap<Layout>(
            std::make_shared<impl::CanvasLayout>(
                std::make_shared<impl::RelativeBox>(impl::RelativeValue(), impl::RelativeValue())));
        mainCanvas.add(m_mainSelector);
        design.add(mainCanvas);

        {
            auto panel = loadObj<Panel>("ui\\NewObjDialog.json");
            std::function<void(const std::string&)> pathProcessor =
                std::bind(&MainApp::createObject, this, std::placeholders::_1);
            m_newObjDialog.init(makeRaw(panel), pathProcessor);
            mainCanvas.add(panel);
        }

        {
            auto panel = loadObj<Panel>("ui\\ExtFilePathDialog.json");
            getExtFilePathDialog() = ExtFilePathDialog(makeRaw(panel));
            mainCanvas.add(panel);
            getExtFilePathDialog().setRootPath(settings::workDir);
        }

        {
            auto panel = loadObj<Panel>("ui\\RunAnimationDialog.json");
            panel.child<Button>("ok").setCallback(std::bind(&MainApp::runAnimation, this));
            panel.child<Button>("cancel").setCallback(std::bind(&Panel::hide, makeRaw(panel)));
            panel.hide();
            m_runAnimationDialog = makeRaw(panel);
            m_designViewLayout.child<Button>("animation").setCallback(
                std::bind(&Panel::setVisible, m_runAnimationDialog, true));
            mainCanvas.add(panel);
        }
        
        {
            auto panel = loadObj<Panel>("ui\\ErrorMessage.json");
            getErrorMessageWindow() = ErrorMessageWindow(makeRaw(panel));
            mainCanvas.add(panel);
        }

        m_isObjectDrawable = true;

        m_drawableObjPropsBox = m_designViewPropertiesLayout.box();
        m_notDrawableObjPropsBox = BoundingBox(
            m_drawableObjPropsBox.width(), m_drawableObjPropsBox.height() + m_canvasArea->height());

        setDesignFromCurrentObject();
        updateDesign(serializeModel());
    }

    void processInput()
    {
        if (m_mainSelector.selected() == FULLSCREEN_VIEW) {
            if (input.escapeJustPressed())
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

private:
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
            impl::Serializer serializer(&builder);
            serializer << "" << m_currentObjectForDesign;
        } catch (std::exception& ex) {
            showError("Error while building design view for current object", ex.what());
            return;
        }

        std::cout << "Loading resources..." << std::endl;
        m_designTreeView->countBoxes();
        m_designTreeView->loadResources();
        m_designPropsMenuArea->update();

        std::cout << "Done updating design by object" << std::endl;
    }

    void updateDesign()
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
            deserializeFromJson(designStr, designedObj);
        } catch (std::exception& ex) {
            showError("Error while building object by design", ex.what());
            return false;
        }
        
        bool isObjectDrawable = dynamic_cast<impl::IDrawable*>(designedObj.get()) != nullptr;
        if (isObjectDrawable != m_isObjectDrawable) {
            auto box = isObjectDrawable ? m_drawableObjPropsBox : m_notDrawableObjPropsBox;
            m_designViewPropertiesLayout.setSizes(box.width(), box.height());
            m_designViewLayout.update();
            m_isObjectDrawable = isObjectDrawable;
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
        m_canvasArea->update();
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
        auto designStr = serializeModel(impl::JsonFormat::Styled);
        if (designStr.empty())
            return;
        auto fullName = 
            addSlash(toLocal(settings::workDir)) + addSlash(relativePathLocal) + fileNameLocal;
        std::cout << "Saving design in file with name: " << fullName << std::endl;
        std::ofstream outputFile(fullName);
        outputFile << designStr;
        std::cout << "Done saving design" << std::endl;
        
        m_relativePath = relativePathLocal;
        m_fileName = fileNameLocal;
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
        m_fileName = "Unnamed.json";
    }

    void loadDesign(const std::string& relativePathLocal, const std::string& fileNameLocal)
    {
        loadDesignInternal(
            addSlash(toLocal(settings::workDir)) + addSlash(relativePathLocal) + fileNameLocal);
        m_relativePath = relativePathLocal;
        m_fileName = fileNameLocal;
    }

    void copyDesign()
    {
        std::cout << "Started copying design to clipboard..." << std::endl;
        auto designStr = serializeModel();
        if (designStr.empty())
            return;
        g_clipboard = designStr;
        std::cout << "Done copying design" << std::endl;
    }

    void pasteDesign()
    {
        std::cout << "Started pasting design..." << std::endl;
        if (!updateDesign(g_clipboard))
            return;
        setDesignFromCurrentObject();
        std::cout << "Done pasting design" << std::endl;
    }

    void enterFullScreen()
    {
        if (!m_isObjectDrawable) {
            std::cout << "Object is not drawable. Cancelling fullscreen mode..." << std::endl;
            return;
        }

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
        auto& dialog = getExtFilePathDialog();
        dialog.setRootPath(settings::workDir);
        dialog.setRelativePath(toUnicode(normalizePath(m_relativePath)));
        dialog.setFileName(toUnicode(m_fileName));
        dialog.setCallbacks(callback);
        dialog.init();
    }

    std::string serializeModel(impl::JsonFormat::Enum format = impl::JsonFormat::Fast)
    {
        std::cout << "Serializing model..." << std::endl;
        std::string designStr;
        try {
            designStr = m_designModel.toString(format);
        } catch (std::exception& ex) {
            showError("Error while serializing model", ex.what());
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
    bool m_isObjectDrawable;
    BoundingBox m_drawableObjPropsBox;
    BoundingBox m_notDrawableObjPropsBox;

    Layout m_designViewLayout;
    DesignModel m_designModel;
    TreeView* m_designTreeView;
    Selector m_designPropertiesMenu;
    Layout m_designViewPropertiesLayout;
    std::shared_ptr<PropsMenuToolBar> m_designPropsMenuToolBar;
    impl::ScrollableArea* m_designPropsMenuArea;
    Layout m_canvas;
    impl::ScrollableArea* m_canvasArea;
    Selector m_mainSelector;
    Selector m_viewSelector;
    Layout m_fullscreenCanvas;

    DesignModel m_presentationModel;

    std::string m_relativePath;
    std::string m_fileName;

    NewObjDialog m_newObjDialog;
    Panel m_runAnimationDialog;
    SettingsView m_settingsView;
};

} }

int main(int argc, char** argv)
{
    using namespace gamebase::editor;
    MainApp app;
    app.setConfig("design_editor_config.json");
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}

#include "TreeView.h"
#include "DesignViewBuilder.h"
#include "Presentation.h"
#include "tools.h"
#include "NewObjDialog.h"
#include "SettingsView.h"
#include "Settings.h"
#include "SimpleTreeViewSkin.h"
#include <gamebase/core/Core.h>
#include <gamebase/engine/Application.h>
#include <gamebase/engine/FullscreenPanelSkin.h>
#include <gamebase/engine/FixedOffset.h>
#include <gamebase/engine/FixedBox.h>
#include <gamebase/engine/RelativeBox.h>
#include <gamebase/engine/AligningOffset.h>
#include <gamebase/engine/SelectingWidget.h>
#include <gamebase/engine/CanvasLayout.h>
#include <gamebase/engine/StaticFilledRect.h>
#include <gamebase/engine/ButtonList.h>
#include <gamebase/engine/AnimatedObjectConstruct.h>
#include <gamebase/serial/JsonSerializer.h>
#include <gamebase/serial/JsonDeserializer.h>
#include <gamebase/text/TextBank.h>
#include <gamebase/text/Conversion.h>
#include <gamebase/utils/StringUtils.h>
#include <gamebase/utils/FileIO.h>

namespace gamebase { namespace editor {

class MainApp : public Application {
public:
    static const int DESIGN_VIEW = 0;
    static const int PRESENTATION_VIEW = 1;
    static const int SETTINGS_VIEW = 2;

    static const int MAIN_VIEW = 0;
    static const int FULLSCREEN_VIEW = 1;

    MainApp() : m_fileName("Unnamed.json") {}

    virtual void load() override
    {
        settings::init();

        std::cout << "Generating default patterns for presentation view..." << std::endl;
        presentationForPresentationView()->serializeAllDefaultPatterns();
        presentationForDesignView()->serializeAllDefaultPatterns();

        std::cout << "Loading text bank..." << std::endl;
        g_textBank = loadObj<TextBank>("texts\\TextBank.json");

        std::cout << "Creating editor's views..." << std::endl;
        m_view = std::make_shared<Panel>(
            std::make_shared<FixedOffset>(),
            std::make_shared<FullscreenPanelSkin>(Color(1.0f, 1.0f, 1.0f)));

        auto mainSelector = std::make_shared<SelectingWidget>(
            std::make_shared<RelativeBox>(RelativeValue(), RelativeValue()));
        m_mainSelector = mainSelector.get();

        auto mainLayout = deserialize<LinearLayout>("ui\\VertLayout.json");

        auto viewsSelector = std::make_shared<SelectingWidget>(
            std::make_shared<RelativeBox>(RelativeValue(), RelativeValue()));

        {
            std::shared_ptr<LinearLayout> topPanelLayout = deserialize<LinearLayout>(
                settings::isInterfaceExtended
                ? std::string("ui\\TopLayoutExt.json")
                : std::string("ui\\TopLayout.json"));

            topPanelLayout->getChild<Button>("#exit")->setCallback(std::bind(&Application::stop, this));
            topPanelLayout->getChild<Button>("#settings")->setCallback(
                std::bind(&SelectingWidget::select, viewsSelector.get(), SETTINGS_VIEW));
            topPanelLayout->getChild<Button>("#design")->setCallback(
                std::bind(&SelectingWidget::select, viewsSelector.get(), DESIGN_VIEW));

            if (settings::isInterfaceExtended)
                topPanelLayout->getChild<Button>("#scheme")->setCallback(
                    std::bind(&SelectingWidget::select, viewsSelector.get(), PRESENTATION_VIEW));

            mainLayout->addObject(topPanelLayout);
        }

        mainLayout->addObject(deserialize<StaticFilledRect>("ui\\VertDelim.json"));

        auto designViewLayout = deserialize<LinearLayout>("ui\\VertLayout.json");
        m_designViewLayout = designViewLayout.get();

        {
            auto designViewControlPanel = deserialize<LinearLayout>(
                settings::isInterfaceExtended
                ? std::string("ui\\DesignTopLayoutExt.json")
                : std::string("ui\\DesignTopLayout.json"));
            
            designViewControlPanel->getChild<Button>("#new")->setCallback(
                std::bind(&NewObjDialog::run, &m_newObjDialog));

            {
                std::function<void(const std::string&, const std::string&)> pathProcessor =
                    std::bind(&MainApp::saveDesign, this, std::placeholders::_1, std::placeholders::_2);
                designViewControlPanel->getChild<Button>("#save")->setCallback(
                    std::bind(&MainApp::initFilePathDialog, this, pathProcessor));
            }
            {
                std::function<void(const std::string&, const std::string&)> pathProcessor =
                    std::bind(&MainApp::loadDesign, this, std::placeholders::_1, std::placeholders::_2);
                designViewControlPanel->getChild<Button>("#load")->setCallback(
                    std::bind(&MainApp::initFilePathDialog, this, pathProcessor));
            }
            designViewControlPanel->getChild<Button>("#update")->setCallback(std::bind(&MainApp::updateDesign, this));
            designViewControlPanel->getChild<Button>("#copy")->setCallback(std::bind(&MainApp::copyDesign, this));
            designViewControlPanel->getChild<Button>("#paste")->setCallback(std::bind(&MainApp::pasteDesign, this));
            designViewControlPanel->getChild<Button>("#fullscreen")->setCallback(std::bind(&MainApp::enterFullScreen, this));
            if (settings::isInterfaceExtended)
                designViewControlPanel->getChild<Button>("#rebuild")->setCallback(std::bind(&MainApp::setDesignFromCurrentObject, this));
            
            designViewLayout->addObject(designViewControlPanel);
        }

        designViewLayout->addObject(deserialize<StaticFilledRect>("ui\\VertDelim.json"));

        {
            auto designViewPropertiesLayout = deserialize<LinearLayout>("ui\\DesignViewPropsLayout.json");
            m_designViewPropertiesLayout = designViewPropertiesLayout.get();

            auto skin = std::make_shared<SimpleTreeViewSkin>(
                std::make_shared<RelativeBox>(
                    RelativeValue(RelType::Ratio, 0.390625f), RelativeValue()));
            auto treeView = std::make_shared<TreeView>(nullptr, skin);
            m_designViewPropertiesLayout->addObject(treeView);
            m_designTreeView = treeView.get();

            m_designViewPropertiesLayout->addObject(deserialize<StaticFilledRect>("ui\\HorDelim.json"));

            auto propertiesMenuLayout = deserialize<LinearLayout>("ui\\PropertiesMenuLayout.json");
            m_designViewPropertiesLayout->addObject(propertiesMenuLayout);
            
            m_designPropsMenuArea = propertiesMenuLayout->getChild<ScrollableArea>("#area");
            m_designPropsMenuToolBar = std::make_shared<PropsMenuToolBar>(
                propertiesMenuLayout->getChild<LinearLayout>("#toolbar"));
            m_designPropertiesMenu = propertiesMenuLayout->getChild<SelectingWidget>("#menu");

            setDesignFromCurrentObject();

            designViewLayout->addObject(designViewPropertiesLayout);
        }

        designViewLayout->addObject(deserialize<StaticFilledRect>("ui\\VertDelim.json"));

        {
            auto canvas = std::make_shared<CanvasLayout>(
                std::make_shared<RelativeBox>(RelativeValue(), RelativeValue()),
                std::make_shared<AligningOffset>(HorAlign::Center, VertAlign::Center));
            canvas->setAdjustment(Adjustment::ToFitContentAndArea);
            m_canvas = canvas.get();

            auto area = std::make_shared<ScrollableArea>(
                deserialize<ScrollableAreaSkin>("ui\\ScrollableAreaSkin.json"));
            area->objects().addObject(canvas);
            m_canvasArea = area.get();
            designViewLayout->addObject(area);
        }
        
        viewsSelector->addObject(DESIGN_VIEW, designViewLayout);

        if (settings::isInterfaceExtended) {
            auto presentationViewLayout = deserialize<LinearLayout>("ui\\VertLayout.json");
            
            {
                std::shared_ptr<LinearLayout> presentationViewControlPanel =
                    deserialize<LinearLayout>("ui\\PresTopLayout.json");

                presentationViewControlPanel->getChild<Button>("#save_scheme")->setCallback(
                    std::bind(&MainApp::savePresentation, this));
                presentationViewControlPanel->getChild<Button>("#update_templates")->setCallback(
                    std::bind(&MainApp::savePatterns, this));

                presentationViewLayout->addObject(presentationViewControlPanel);
            }

            presentationViewLayout->addObject(deserialize<StaticFilledRect>("ui\\VertDelim.json"));

            {
                auto presentationViewPropertiesLayout = deserialize<LinearLayout>("ui\\HorLayout.json");

                auto skin = std::make_shared<SimpleTreeViewSkin>(
                    std::make_shared<RelativeBox>(
                        RelativeValue(RelType::Ratio, 0.390625f), RelativeValue()));
                auto treeView = std::make_shared<TreeView>(nullptr, skin);
                presentationViewPropertiesLayout->addObject(treeView);

                presentationViewPropertiesLayout->addObject(deserialize<StaticFilledRect>("ui\\HorDelim.json"));

                auto propertiesMenuLayout = deserialize<LinearLayout>("ui\\PropertiesMenuLayout.json");
                presentationViewPropertiesLayout->addObject(propertiesMenuLayout);

                presentationViewLayout->addObject(presentationViewPropertiesLayout);

                {
                    DesignViewBuilder builder(
                        *treeView,
                        *propertiesMenuLayout->getChild<SelectingWidget>("#menu"),
                        m_presentationModel,
                        presentationForPresentationView(),
                        std::make_shared<PropsMenuToolBar>(
                            propertiesMenuLayout->getChild<LinearLayout>("#toolbar")),
                        propertiesMenuLayout->getChild<ScrollableArea>("#area"));
                    Serializer serializer(&builder);
                    serializer << "" << presentationForDesignView();
                }
            }

            viewsSelector->addObject(PRESENTATION_VIEW, presentationViewLayout);
        }

        {
            auto settingsLayout = deserialize<CanvasLayout>("ui\\SettingsLayout.json");
            m_settingsView.init(settingsLayout.get());
            viewsSelector->addObject(SETTINGS_VIEW, settingsLayout);
        }

        viewsSelector->select(DESIGN_VIEW);
        mainLayout->addObject(viewsSelector);

        mainSelector->addObject(MAIN_VIEW, mainLayout);

        {
            auto canvas = std::make_shared<CanvasLayout>(
                std::make_shared<RelativeBox>(RelativeValue(), RelativeValue()),
                std::make_shared<AligningOffset>(HorAlign::Center, VertAlign::Center));
            m_fullscreenCanvas = canvas.get();
            mainSelector->addObject(FULLSCREEN_VIEW, canvas);
        }

        mainSelector->select(MAIN_VIEW);
        
        m_view->addObject(mainSelector);
        activateController(this);

        {
            auto panel = deserialize<Panel>("ui\\NewObjDialog.json");
            std::function<void(const std::string&)> pathProcessor =
                std::bind(&MainApp::createObject, this, std::placeholders::_1);
            m_newObjDialog.init(panel.get(), pathProcessor);
            m_view->addObject(panel);
        }

        {
            auto panel = deserialize<Panel>("ui\\ExtFilePathDialog.json");
            getExtFilePathDialog() = ExtFilePathDialog(panel.get());
            m_view->addObject(panel);
            getExtFilePathDialog().setRootPath(settings::workDir);
        }

        {
            auto panel = deserialize<Panel>("ui\\RunAnimationDialog.json");
            panel->getChild<Button>("#ok")->setCallback(std::bind(&MainApp::runAnimation, this));
            panel->getChild<Button>("#cancel")->setCallback(std::bind(&Panel::close, panel.get()));
            panel->setVisible(false);
            m_runAnimationDialog = panel.get();
            designViewLayout->getChild<Button>("#animation")->setCallback(
                std::bind(&Panel::setVisible, m_runAnimationDialog, true));
            m_view->addObject(panel);
        }
        
        {
            auto panel = deserialize<Panel>("ui\\ErrorMessage.json");
            getErrorMessageWindow() = ErrorMessageWindow(panel.get());
            m_view->addObject(panel);
        }

        m_isObjectDrawable = true;
    }

    void postload()
    {
        m_drawableObjPropsBox = m_designViewPropertiesLayout->box();
        m_notDrawableObjPropsBox = BoundingBox(
            m_drawableObjPropsBox.width(), m_drawableObjPropsBox.height() + m_canvasArea->height());
        updateDesign();
    }

    void processInput(const InputRegister& inputRegister)
    {
        if (m_mainSelector->selected() == FULLSCREEN_VIEW) {
            if (inputRegister.keys.isJustPressed(27))
                m_mainSelector->select(MAIN_VIEW);
        }
    }

private:
    void setDesignFromCurrentObject()
    {
        std::cout << "Initing design view..." << std::endl;
        m_designTreeView->clear();
        m_designPropertiesMenu->clear();
        m_designModel.clear();
        m_designPropsMenuToolBar->clear();
        std::cout << "Creating design by object..." << std::endl;
        try {
            DesignViewBuilder builder(*m_designTreeView, *m_designPropertiesMenu,
                m_designModel, presentationForDesignView(),
                m_designPropsMenuToolBar, m_designPropsMenuArea);
            Serializer serializer(&builder);
            serializer << "" << m_currentObjectForDesign;
        } catch (std::exception& ex) {
            if (m_inited)
                getErrorMessageWindow().showWithMessage("Error while building design view for current object", ex.what());
            return;
        }

        if (m_inited) {
            std::cout << "Loading resources..." << std::endl;
            m_designTreeView->countBoxes();
            m_designTreeView->loadResources();
            m_designPropsMenuArea->update();
        }
        std::cout << "Done updating design by object" << std::endl;
    }

    void updateDesign()
    {
        updateDesign(serializeModel());
    }

    bool updateDesign(const std::string& designStr, bool allowErrors = false)
    {
        if (designStr.empty())
            return false;
        std::shared_ptr<IObject> designedObj;
        std::cout << "Building object by design..." << std::endl;
        try {
            deserializeFromJson(designStr, designedObj);
        } catch (std::exception& ex) {
            getErrorMessageWindow().showWithMessage("Error while building object by design", ex.what());
            return false;
        }
        
        bool isObjectDrawable = dynamic_cast<IDrawable*>(designedObj.get()) != nullptr;
        if (isObjectDrawable != m_isObjectDrawable) {
            auto box = isObjectDrawable ? m_drawableObjPropsBox : m_notDrawableObjPropsBox;
            m_designViewPropertiesLayout->setFixedBox(box.width(), box.height());
            m_designViewLayout->update();
            m_isObjectDrawable = isObjectDrawable;
        }

        std::cout << "Adding object to canvas..." << std::endl;
        try {
            configurateFromString(settings::designedObjConf, false);
            m_canvas->insertObject(0, designedObj);
            configurateFromString(settings::mainConf, false);
        } catch (std::exception& ex)
        {
            getErrorMessageWindow().showWithMessage("Error while trying to place object on canvas", ex.what());
            if (allowErrors) {
                m_canvas->removeObject(0);
            } else {
                configurateFromString(settings::designedObjConf, false);
                m_canvas->insertObject(0, m_currentObjectForDesign);
                configurateFromString(settings::mainConf, false);
                return false;
            }
        }
        m_currentObjectForDesign = designedObj;
        if (m_inited)
            m_canvasArea->update();
        std::cout << "Done updating design" << std::endl;
        return true;
    }

    void savePresentation()
    {
        std::cout << "Serializing presentation..." << std::endl;
        auto presentationStr = m_presentationModel.toString(JsonFormat::Styled);
        std::cout << "Building presentation by design..." << std::endl;
        std::shared_ptr<Presentation> designedPresentation;
        try {
            deserializeFromJson(presentationStr, designedPresentation);
        } catch (std::exception& ex) {
            getErrorMessageWindow().showWithMessage("Error while building presentation by design", ex.what());
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
        auto designStr = serializeModel();
        if (designStr.empty())
            return;
        auto fullName = 
            addSlash(convertToLocal(settings::workDir)) + addSlash(relativePathLocal) + fileNameLocal;
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
            getErrorMessageWindow().showWithMessage("Error while loading design", ex.what());
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
            addSlash(convertToLocal(settings::workDir)) + addSlash(relativePathLocal) + fileNameLocal);
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
        std::shared_ptr<IObject> designedObj;
        std::cout << "Building object by design..." << std::endl;
        try {
            deserializeFromJson(designStr, designedObj);
        } catch (std::exception& ex) {
            getErrorMessageWindow().showWithMessage("Error while building object by design", ex.what());
            return;
        }
        
        std::cout << "Adding object to canvas..." << std::endl;
        try {
            configurateFromString(settings::designedObjConf, false);
            m_fullscreenCanvas->insertObject(0, designedObj);
            configurateFromString(settings::mainConf, false);
        } catch (std::exception& ex)
        {
            getErrorMessageWindow().showWithMessage("Error while placing object on canvas", ex.what());
            return;
        }
        m_mainSelector->select(FULLSCREEN_VIEW);
        try {
            m_fullscreenCanvas->update();
        } catch (std::exception& ex)
        {
            getErrorMessageWindow().showWithMessage("Error while updating canvas", ex.what());
            m_mainSelector->select(MAIN_VIEW);
            return;
        }
        std::cout << "Done building fullscreen design" << std::endl;
    }

    void initFilePathDialog(const std::function<void(const std::string&, const std::string&)>& callback)
    {
        auto& dialog = getExtFilePathDialog();
        dialog.setRootPath(settings::workDir);
        dialog.setRelativePath(convertToUtf8(normalizePath(m_relativePath)));
        dialog.setFileName(convertToUtf8(m_fileName));
        dialog.setCallbacks(callback);
        dialog.init();
    }

    std::string serializeModel()
    {
        std::cout << "Serializing model..." << std::endl;
        std::string designStr;
        try {
            designStr = m_designModel.toString(JsonFormat::Styled);
        } catch (std::exception& ex) {
            getErrorMessageWindow().showWithMessage("Error while serializing model", ex.what());
            return std::string();
        }
        return std::move(designStr);
    }

    void runAnimation()
    {
        std::cout << "Starting running animation..." << std::endl;
        auto objName = m_runAnimationDialog->getChild<TextBox>("#objname")->text();

        AnimatedObjectConstruct* obj = nullptr;
        try {
            if (objName.empty()) {
                obj = dynamic_cast<AnimatedObjectConstruct*>(m_currentObjectForDesign.get());
                if (!obj) {
                    getErrorMessageWindow().showWithMessage("Error while casting root object to AnimatedObjectConstruct");
                    return;
                }
            } else {
                obj = m_canvas->getChild<AnimatedObjectConstruct>(objName);
            }
        } catch (std::exception& ex) {
            getErrorMessageWindow().showWithMessage("Error while searching for object", ex.what());
            return;
        }
        
        std::string animName =  m_runAnimationDialog->getChild<TextBox>("#animname")->text();
        try {
            obj->runAnimation(animName);
        } catch (std::exception& ex) {
            getErrorMessageWindow().showWithMessage("Error while stating animation", ex.what());
            return;
        }

        m_runAnimationDialog->setVisible(false);

        std::cout << "Done running animation" << std::endl;
    }

    std::shared_ptr<IObject> m_currentObjectForDesign;
    bool m_isObjectDrawable;
    BoundingBox m_drawableObjPropsBox;
    BoundingBox m_notDrawableObjPropsBox;

    LinearLayout* m_designViewLayout;
    DesignModel m_designModel;
    TreeView* m_designTreeView;
    SelectingWidget* m_designPropertiesMenu;
    LinearLayout* m_designViewPropertiesLayout;
    std::shared_ptr<PropsMenuToolBar> m_designPropsMenuToolBar;
    ScrollableArea* m_designPropsMenuArea;
    CanvasLayout* m_canvas;
    ScrollableArea* m_canvasArea;
    SelectingWidget* m_mainSelector;
    CanvasLayout* m_fullscreenCanvas;

    DesignModel m_presentationModel;

    std::string m_relativePath;
    std::string m_fileName;

    NewObjDialog m_newObjDialog;
    Panel* m_runAnimationDialog;
    SettingsView m_settingsView;
};

} }

int main(int argc, char** argv)
{
    using namespace gamebase::editor;
    MainApp app;
    app.setConfigName("design_editor_config.json");
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}

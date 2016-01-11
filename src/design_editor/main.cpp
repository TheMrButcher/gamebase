#include "TreeView.h"
#include "DesignViewBuilder.h"
#include "Presentation.h"
#include "tools.h"
#include "FilePathDialog.h"
#include <gamebase/core/Core.h>
#include <gamebase/engine/Application.h>
#include <gamebase/engine/FullscreenPanelSkin.h>
#include <gamebase/engine/OffsettedBox.h>
#include <gamebase/engine/FixedOffset.h>
#include <gamebase/engine/FixedBox.h>
#include <gamebase/engine/RelativeBox.h>
#include <gamebase/engine/FixedOffset.h>
#include <gamebase/engine/AligningOffset.h>
#include <gamebase/engine/FilledRect.h>
#include <gamebase/engine/TransparentLinearLayoutSkin.h>
#include <gamebase/engine/SelectingWidget.h>
#include <gamebase/engine/CanvasLayout.h>
#include <gamebase/engine/CommonScrollableAreaSkin.h>
#include <gamebase/engine/CommonScrollBarSkin.h>
#include <gamebase/engine/CommonPanelSkin.h>
#include <gamebase/engine/StaticFilledRect.h>
#include <gamebase/engine/ButtonList.h>
#include <gamebase/engine/CommonButtonListSkin.h>
#include <gamebase/serial/JsonSerializer.h>
#include <gamebase/serial/JsonDeserializer.h>
#include <gamebase/text/Conversion.h>

namespace gamebase { namespace editor {

namespace {
std::shared_ptr<ScrollBarSkin> createScrollBarSkin(
    Direction::Enum direction)
{
    RelativeValue width;
    RelativeValue height(RelType::Pixels, 20.0f);
    RelativeValue dragBoxWidth(RelType::ValueMinusPixels, 40.0f);
    RelativeValue dragBoxHeight;

    if (direction == Direction::Vertical) {
        std::swap(width, height);
        std::swap(dragBoxWidth, dragBoxHeight);
    }

    auto skin = std::make_shared<CommonScrollBarSkin>(
        std::make_shared<RelativeBox>(width, height),
        std::make_shared<RelativeBox>(dragBoxWidth, dragBoxHeight),
        direction);
    skin->setAlwaysShow(false);
    skin->setDecButtonSkin(createButtonSkin(20.0f, 20.0f, "D"));
    skin->setIncButtonSkin(createButtonSkin(20.0f, 20.0f, "I"));
    skin->setDragBarSkin(createButtonSkin(
        std::make_shared<RelativeBox>(RelativeValue(), RelativeValue()), ""));

    auto fill = std::make_shared<StaticFilledRect>(std::make_shared<OffsettedBox>());
    fill->setColor(Color(0.8f, 0.8f, 0.8f));
    skin->addElement(fill);

    return skin;
}

class SimpleTreeViewSkin : public TreeViewSkin {
public:
    SimpleTreeViewSkin(
        const std::shared_ptr<IRelativeBox>& box)
        : m_box(box)
        , m_treeBox(std::make_shared<RelativeBox>(
            RelativeValue(RelType::ValuePlusPixels, 20.0f), RelativeValue(),
            std::make_shared<FixedOffset>(-10.0f, 0.0f)))
    {}

    virtual BoundingBox treeBox() const override
    {
        return m_treeBox->get();
    }

    virtual std::shared_ptr<ScrollableArea> createTreeArea() const override
    {
        auto skin = deserialize<CommonScrollableAreaSkin>("ui\\ScrollableAreaSkin.json");
        return std::make_shared<ScrollableArea>(std::make_shared<FixedOffset>(), skin);
    }

    virtual std::shared_ptr<ToggleButton> createOpenButton() const override
    {
        auto skin = deserialize<AnimatedButtonSkin>("ui\\ArrowButton.json");
        auto button = std::make_shared<ToggleButton>(
            std::make_shared<AligningOffset>(HorAlign::Center, VertAlign::Center), skin);
        button->setUnpressOnFocusLost(false);
        return button;
    }

    virtual std::shared_ptr<IRelativeBox> createSubtreeBox() const override
    {
        return std::make_shared<OffsettedBox>(std::make_shared<FixedOffset>(20.0f, -2.0f));
    }

    virtual std::shared_ptr<IRelativeOffset> createOffset() const override
    {
        return std::make_shared<AligningOffset>(
            HorAlign::Left, VertAlign::Top,
            RelativeValue(RelType::Pixels, 22.0f), RelativeValue(RelType::Pixels, 0.0f));
    }

    virtual void loadResources() override {}
    virtual void drawAt(const Transform2& position) const override {}

    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_box->setParentBox(allowedBox);
        m_treeBox->setParentBox(m_box->get());
    }

    virtual BoundingBox box() const override
    {
        return m_box->get();
    }

private:
    std::shared_ptr<IRelativeBox> m_box;
    std::shared_ptr<IRelativeBox> m_treeBox;
};
}

class MainApp : public Application {
public:
    static const int DESIGN_VIEW = 0;
    static const int PRESENTATION_VIEW = 1;

    MainApp() : m_designedObjID(-1), m_fileName("Default.json") {}

    virtual void load() override
    {
        {
            const auto& config = configAsDictionary();
            auto it = config.find("interface");
            if (it != config.end() && it->second == "extended")
                isInterfaceExtended = true;
        }

        std::cout << "Loading default object..." << std::endl;
        try {
            m_currentObjectForDesign = deserialize<Button>("Default.json");
        } catch (std::exception& ex) {
            std::cerr << "Error! Using simple StaticFilledRect as default object. Reason: " << ex.what() << std::endl;
            auto filledRect = std::make_shared<StaticFilledRect>(
                std::make_shared<FixedBox>(BoundingBox(32, 32)));
            filledRect->setColor(Color(0, 0, 0));
            m_currentObjectForDesign = filledRect;
        }

        std::cout << "Generating default patterns for presentation view..." << std::endl;
        presentationForPresentationView()->serializeAllDefaultPatterns();

        std::cout << "Creating editor's views..." << std::endl;
        m_view = std::make_shared<Panel>(
            std::make_shared<FixedOffset>(),
            std::make_shared<FullscreenPanelSkin>(Color(1.0f, 1.0f, 1.0f)));

        auto mainLayout = deserialize<LinearLayout>("ui\\VertLayout.json");

        auto viewsSelector = std::make_shared<SelectingWidget>(
            std::make_shared<RelativeBox>(RelativeValue(), RelativeValue()));

        {
            std::shared_ptr<LinearLayout> topPanelLayout = deserialize<LinearLayout>(
                isInterfaceExtended
                ? std::string("ui\\TopLayoutExt.json")
                : std::string("ui\\TopLayout.json"));

            topPanelLayout->getChild<Button>("#exit")->setCallback(std::bind(&Application::stop, this));
            topPanelLayout->getChild<Button>("#design")->setCallback(
                std::bind(&SelectingWidget::select, viewsSelector.get(), DESIGN_VIEW));

            if (isInterfaceExtended)
                topPanelLayout->getChild<Button>("#scheme")->setCallback(
                    std::bind(&SelectingWidget::select, viewsSelector.get(), PRESENTATION_VIEW));

            mainLayout->addObject(topPanelLayout);
        }

        mainLayout->addObject(deserialize<StaticFilledRect>("ui\\VertDelim.json"));

        auto designViewLayout = deserialize<LinearLayout>("ui\\VertLayout.json");

        std::shared_ptr<LinearLayout> designViewControlPanel;
        {
            designViewControlPanel = deserialize<LinearLayout>(
                isInterfaceExtended
                ? std::string("ui\\DesignTopLayoutExt.json")
                : std::string("ui\\DesignTopLayout.json"));

            {
                std::function<void(const std::string&)> pathProcessor =
                    std::bind(&MainApp::saveDesign, this, std::placeholders::_1);
                designViewControlPanel->getChild<Button>("#save")->setCallback(
                    std::bind(&MainApp::initFilePathDialog, this, pathProcessor));
            }
            {
                std::function<void(const std::string&)> pathProcessor =
                    std::bind(&MainApp::loadDesign, this, std::placeholders::_1);
                designViewControlPanel->getChild<Button>("#load")->setCallback(
                    std::bind(&MainApp::initFilePathDialog, this, pathProcessor));
            }
            designViewControlPanel->getChild<Button>("#update")->setCallback(std::bind(&MainApp::updateDesign, this));
            if (isInterfaceExtended)
                designViewControlPanel->getChild<Button>("#rebuild")->setCallback(std::bind(&MainApp::setDesignFromCurrentObject, this));
            
            designViewLayout->addObject(designViewControlPanel);
        }

        designViewLayout->addObject(deserialize<StaticFilledRect>("ui\\VertDelim.json"));

        {
            auto designViewPropertiesLayout = deserialize<LinearLayout>("ui\\DesignViewPropsLayout.json");

            auto skin = std::make_shared<SimpleTreeViewSkin>(
                std::make_shared<RelativeBox>(
                    RelativeValue(RelType::Ratio, 0.5f), RelativeValue()));
            auto treeView = std::make_shared<TreeView>(nullptr, skin);
            designViewPropertiesLayout->addObject(treeView);
            m_designTreeView = treeView.get();

            designViewPropertiesLayout->addObject(deserialize<StaticFilledRect>("ui\\HorDelim.json"));

            auto propertiesMenu = std::make_shared<SelectingWidget>(
                std::make_shared<OffsettedBox>());
            designViewPropertiesLayout->addObject(propertiesMenu);
            m_designPropertiesMenu = propertiesMenu.get();

            setDesignFromCurrentObject();

            designViewLayout->addObject(designViewPropertiesLayout);
        }

        designViewLayout->addObject(deserialize<StaticFilledRect>("ui\\VertDelim.json"));

        {
            auto canvas = std::make_shared<CanvasLayout>(
                std::make_shared<RelativeBox>(RelativeValue(), RelativeValue()));
            m_canvas = canvas.get();
            designViewLayout->addObject(canvas);
        }
        
        viewsSelector->addObject(DESIGN_VIEW, designViewLayout);

        if (isInterfaceExtended) {
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
                        RelativeValue(RelType::Ratio, 0.5f), RelativeValue()));
                auto treeView = std::make_shared<TreeView>(nullptr, skin);
                presentationViewPropertiesLayout->addObject(treeView);

                presentationViewPropertiesLayout->addObject(deserialize<StaticFilledRect>("ui\\HorDelim.json"));

                auto propertiesMenu = std::make_shared<SelectingWidget>(
                    std::make_shared<OffsettedBox>());
                presentationViewPropertiesLayout->addObject(propertiesMenu);
                presentationViewLayout->addObject(presentationViewPropertiesLayout);

                {
                    DesignViewBuilder builder(*treeView, *propertiesMenu, m_presentationModel, presentationForPresentationView());
                    Serializer serializer(&builder);
                    serializer << "" << presentationForDesignView();
                }
            }

            viewsSelector->addObject(PRESENTATION_VIEW, presentationViewLayout);
        }

        viewsSelector->select(DESIGN_VIEW);
        mainLayout->addObject(viewsSelector);
        
        m_view->addObject(mainLayout);
        activateController(this);

        {
            auto panel = createObjectTypeListPanel();
            panel->setVisible(false);
            m_view->addObject(panel);
            designViewControlPanel->getChild<Button>("#new")->setCallback(std::bind(&Panel::setVisible, panel.get(), true));
        }

        {
            auto panel = deserialize<Panel>("ui\\FilePathDialog.json");
            getFilePathDialog() = FilePathDialog(panel.get());
            m_view->addObject(panel);
        }

        updateDesign();
    }

private:
    void setDesignFromCurrentObject()
    {
        std::cout << "Initing design view..." << std::endl;
        m_designTreeView->clear();
        m_designPropertiesMenu->clear();
        m_designModel.clear();
        std::cout << "Creating design by object..." << std::endl;
        {
            DesignViewBuilder builder(*m_designTreeView, *m_designPropertiesMenu,
                m_designModel, presentationForDesignView());
            Serializer serializer(&builder);
            serializer << "" << m_currentObjectForDesign;
        }

        if (m_inited) {
            std::cout << "Loading resources..." << std::endl;
            m_designTreeView->countBoxes();
            m_designTreeView->loadResources();
        }
        std::cout << "Done updating design by object" << std::endl;
    }

    void updateDesign()
    {
        updateDesign(serializeModel());
    }

    bool updateDesign(const std::string& designStr)
    {
        if (designStr.empty())
            return false;
        std::shared_ptr<IObject> designedObj;
        std::cout << "Building object by design..." << std::endl;
        try {
            deserializeFromJson(designStr, designedObj);
        } catch (std::exception& ex) {
            std::cout << "Error while building object by design. Reason: " << ex.what() << std::endl;
            return false;
        }
        
        std::cout << "Adding object to canvas..." << std::endl;
        if (m_designedObjID == -1)
            m_designedObjID = m_canvas->addObject(designedObj);
        else
        {
            try {
                m_canvas->replaceObject(m_designedObjID, designedObj);
            } catch (std::exception& ex)
            {
                std::cout << "Error while trying to place object on canvas. Reason: " << ex.what() << std::endl;
                m_canvas->replaceObject(m_designedObjID, m_currentObjectForDesign);
                return false;
            }
        }
        m_currentObjectForDesign = designedObj;
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
            std::cout << "Error while building presentation by design. Reason: " << ex.what() << std::endl;
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

    void saveDesign(const std::string& fileNameLocal)
    {
        std::cout << "Started saving design to file..." << std::endl;
        auto designStr = serializeModel();
        if (designStr.empty())
            return;
        std::cout << "Saving design in file with name: " << fileNameLocal << std::endl;
        std::ofstream outputFile(fileNameLocal);
        outputFile << designStr;
        std::cout << "Done saving design" << std::endl;

        m_fileName = fileNameLocal;
    }

    void loadDesignInternal(const std::string& fileNameLocal)
    {
        std::cout << "Started loading design from file..." << std::endl;
        std::string designStr;
        try {
            designStr = loadTextFile(fileNameLocal);
        } catch (std::exception& ex) {
            std::cout << "Error while loading design. Reason: " << ex.what() << std::endl;
            return;
        }
        if (!updateDesign(designStr))
            return;
        setDesignFromCurrentObject();
        std::cout << "Done loading design" << std::endl;
    }

    void loadDesign(const std::string& fileNameLocal)
    {
        loadDesignInternal(fileNameLocal);
        m_fileName = fileNameLocal;
    }

    void initFilePathDialog(const std::function<void(const std::string&)>& callback)
    {
        getFilePathDialog().initWithText(m_fileName, callback);
    }

    std::string serializeModel()
    {
        std::cout << "Serializing model..." << std::endl;
        std::string designStr;
        try {
            designStr = m_designModel.toString(JsonFormat::Styled);
        } catch (std::exception& ex) {
            std::cout << "Error while serializing model. Reason: " << ex.what() << std::endl;
            return std::string();
        }
        return std::move(designStr);
    }

    std::shared_ptr<Panel> createObjectTypeListPanel()
    {
        auto skin = std::make_shared<CommonPanelSkin>(
            std::make_shared<FixedBox>(550.0f, 500.0f),
            std::make_shared<RelativeBox>(
                RelativeValue(RelType::ValueMinusPixels, 4), RelativeValue(RelType::ValueMinusPixels, 20),
                std::make_shared<AligningOffset>(HorAlign::Center, VertAlign::Bottom)));
        {
            auto border = std::make_shared<StaticFilledRect>(
                std::make_shared<RelativeBox>(RelativeValue(), RelativeValue()));
            border->setColor(Color(0, 0, 0));
            skin->addElement(border);
            auto fill = std::make_shared<StaticFilledRect>(
                std::make_shared<RelativeBox>(
                    RelativeValue(RelType::ValueMinusPixels, 4.0f),
                    RelativeValue(RelType::ValueMinusPixels, 4.0f),
                    std::make_shared<AligningOffset>(HorAlign::Center, VertAlign::Center)));
            fill->setColor(Color(0.6f, 0.6f, 0.65f));
            skin->addElement(fill);
        }

        {
            auto dragBarSkin = std::make_shared<AnimatedButtonSkin>(
                std::make_shared<RelativeBox>(
                    RelativeValue(RelType::ValueMinusPixels, 20.0f),
                    RelativeValue(RelType::Pixels, 20.0f)));
            auto fill = std::make_shared<StaticFilledRect>(
                std::make_shared<RelativeBox>(RelativeValue(), RelativeValue()));
            fill->setColor(Color(0.2f, 0.2f, 0.5f));
            dragBarSkin->addElement(fill);
            skin->setDragBarSkin(dragBarSkin);
        }

        {
            auto closeButtonSkin = createButtonSkin(20.0f, 20.0f, "X");
            skin->setCloseButtonSkin(closeButtonSkin);
        }

        auto panel = std::make_shared<Panel>(
            std::make_shared<FixedOffset>(), skin);
        std::shared_ptr<LinearLayout> vertLayout;
        {
            auto skin = std::make_shared<TransparentLinearLayoutSkin>(
                std::make_shared<RelativeBox>(RelativeValue(), RelativeValue()),
                Direction::Vertical);
            skin->setPadding(2.0f);
            skin->setAdjustSize(false);
            vertLayout = std::make_shared<LinearLayout>(
                std::make_shared<AligningOffset>(HorAlign::Center, VertAlign::Bottom), skin);
        }

        {
            std::shared_ptr<ButtonList> objTypesList;
            {
                auto skin = std::make_shared<CommonButtonListSkin>(
                    std::make_shared<RelativeBox>(
                        RelativeValue(RelType::ValueMinusPixels, 20),
                        RelativeValue(RelType::ValueMinusPixels, 40),
                        std::make_shared<AligningOffset>(HorAlign::Left, VertAlign::Center)),
                    std::make_shared<RelativeBox>(
                        RelativeValue(RelType::ValuePlusPixels, 20),
                        RelativeValue()),
                    Direction::Vertical);
                skin->setScrollBarSkin(createScrollBarSkin(Direction::Vertical));
                objTypesList = std::make_shared<ButtonList>(nullptr, skin);
            }
            auto presentation = presentationForDesignView();
            auto allObjects = presentation->derivedTypesByBaseTypeName("IObject");
            std::map<std::string, std::string> types;
            for (auto it = allObjects.begin(); it != allObjects.end(); ++it)
            {
                if (!SerializableRegister::instance().isRegistered((*it)->name))
                    continue;
                types[(*it)->name] = (*it)->nameInUI + " (" + (*it)->name + ")";
            }
            for (auto it = types.begin(); it != types.end(); ++it)
            {
                auto button = std::make_shared<Button>(
                    nullptr,
                    createButtonSkin(
                        std::make_shared<RelativeBox>(
                            RelativeValue(),
                            RelativeValue(RelType::Pixels, 20.0f)),
                        it->second));
                button->setCallback(std::bind(&MainApp::createObject, this,
                    panel.get(), presentation->pathToPattern(it->first)));
                objTypesList->addButton(button);
            }
            vertLayout->addObject(objTypesList);
        }

        auto bottomLayout = std::make_shared<CanvasLayout>(
            std::make_shared<RelativeBox>(
                RelativeValue(), RelativeValue(RelType::Pixels, 30.0f)));
        {
            auto cancelButton = createButton(100.0f, 30.0f, convertToUtf8("������"), nullptr);
            cancelButton->setCallback(std::bind(&Panel::setVisible, panel.get(), false));
            bottomLayout->addObject(cancelButton);
        }

        vertLayout->addObject(bottomLayout);
        panel->addObject(vertLayout);
        return panel;
    }

    void createObject(Panel* eventSource, const std::string& name)
    {
        eventSource->setVisible(false);
        loadDesignInternal(name);
    }

    std::shared_ptr<IObject> m_currentObjectForDesign;
    int m_designedObjID;

    DesignModel m_designModel;
    TreeView* m_designTreeView;
    SelectingWidget* m_designPropertiesMenu;
    CanvasLayout* m_canvas;

    DesignModel m_presentationModel;

    std::string m_fileName;
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

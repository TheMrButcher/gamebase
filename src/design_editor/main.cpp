#include "TreeView.h"
#include "DesignViewBuilder.h"
#include "Presentation.h"
#include "tools.h"
#include <gamebase/engine/Application.h>
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
#include <gamebase/engine/SimpleScrollableAreaSkin.h>
#include <gamebase/engine/SimpleScrollBarSkin.h>
#include <gamebase/engine/StaticFilledRect.h>
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

    auto skin = std::make_shared<SimpleScrollBarSkin>(
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
    {
        m_border.setColor(Color(0.65f, 0.65f, 0.75f));
    }

    virtual BoundingBox treeBox() const override
    {
        return m_treeBox->get();
    }

    virtual std::shared_ptr<ScrollableArea> createTreeArea() const override
    {
        auto skin = std::make_shared<SimpleScrollableAreaSkin>(
            std::make_shared<RelativeBox>(RelativeValue(), RelativeValue()),
            std::make_shared<RelativeBox>(
                RelativeValue(RelType::ValueMinusPixels, 20.0f),
                RelativeValue(RelType::ValueMinusPixels, 20.0f),
                std::make_shared<AligningOffset>(HorAlign::Left, VertAlign::Top)));
        skin->setAreaWithoutScrollBarsBox(std::make_shared<OffsettedBox>());
        skin->setScrollBarSkin(createScrollBarSkin(Direction::Horizontal), Direction::Horizontal);
        skin->setScrollBarSkin(createScrollBarSkin(Direction::Vertical), Direction::Vertical);
        return std::make_shared<ScrollableArea>(std::make_shared<FixedOffset>(), skin);
    }

    virtual std::shared_ptr<PressableButton> createOpenButton() const override
    {
        auto button = std::make_shared<PressableButton>(
            std::make_shared<AligningOffset>(HorAlign::Center, VertAlign::Center),
            createButtonSkin(16, 16, "+"));
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
            RelativeValue(RelType::Pixels, 20.0f), RelativeValue(RelType::Pixels, 0.0f));
    }

    virtual void loadResources() override
    {
        m_border.loadResources();
    }

    virtual void drawAt(const Transform2& position) const override
    {
        m_border.draw(position);
    }

    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_box->setParentBox(allowedBox);
        m_treeBox->setParentBox(m_box->get());
        m_border.setBox(m_box->get());
    }

    virtual BoundingBox box() const override
    {
        return m_box->get();
    }

private:
    std::shared_ptr<IRelativeBox> m_box;
    std::shared_ptr<IRelativeBox> m_treeBox;
    FilledRect m_border;
};

class MainPanelSkin : public PanelSkin, public ISerializable {
public:
    MainPanelSkin() { m_bg.setColor(Color(0.6f, 0.6f, 0.65f)); }

    virtual std::shared_ptr<Button> createCloseButton() const { return nullptr; }
    virtual std::shared_ptr<ScrollDragBar> createDragBar() const { return nullptr; }
    virtual BoundingBox panelBox() const { return m_box; }
    virtual void loadResources() override { m_bg.loadResources(); }
    virtual void drawAt(const Transform2& position) const override { m_bg.draw(position); }
    virtual void setBox(const BoundingBox& allowedBox) { m_box = allowedBox; m_bg.setBox(m_box); }
    virtual BoundingBox box() const { return m_box; } 
    virtual void registerObject(PropertiesRegisterBuilder* builder) override
    {
        m_bg.registerProperties("background", builder);
    }

    virtual void serialize(Serializer& s) const override {}

private:
    BoundingBox m_box;
    FilledRect m_bg;
};

}

class MainApp : public Application {
public:
    static const int DESIGN_VIEW = 0;
    static const int PRESENTATION_VIEW = 1;

    MainApp() : m_designedObjID(-1) {}

    virtual void load() override
    {
        //generatePresentationPatternsForPresentationView();
        std::cout << "Generating default patterns for presentation view..." << std::endl;
        presentationForPresentationView()->serializeAllDefaultPatterns();

        std::cout << "Creating editor's views..." << std::endl;
        m_view = std::make_shared<Panel>(std::make_shared<FixedOffset>(), std::make_shared<MainPanelSkin>());

        std::shared_ptr<LinearLayout> mainLayout;

        {
            auto skin = std::make_shared<TransparentLinearLayoutSkin>(
                std::make_shared<OffsettedBox>(), Direction::Vertical);
            skin->setPadding(0.0f);
            skin->setAdjustSize(false);
            mainLayout = std::make_shared<LinearLayout>(
                std::make_shared<AligningOffset>(HorAlign::Center, VertAlign::Center), skin);
        }

        m_currentObjectForDesign = createButton(100.0f, 30.0f, convertToUtf8("Выход"), nullptr);
        auto viewsSelector = std::make_shared<SelectingWidget>(
            std::make_shared<OffsettedBox>());
            //std::make_shared<RelativeBox>(RelativeValue(), RelativeValue()));

        {
            std::shared_ptr<LinearLayout> topPanelLayout;
            {
                auto skin = std::make_shared<TransparentLinearLayoutSkin>(
                    std::make_shared<OffsettedBox>(), Direction::Horizontal);
                skin->setPadding(10.0f);
                skin->setAdjustSize(true);
                topPanelLayout = std::make_shared<LinearLayout>(nullptr, skin);
            }

            auto exitButton = createButton(100.0f, 30.0f, convertToUtf8("Выход"), nullptr);
            /*serializeToJsonFile(exitButton, JsonFormat::Styled, "button.json");
            exitButton.reset();
            deserializeFromJsonFile("button.json", exitButton);*/
            exitButton->setCallback(std::bind(&Application::stop, this));
            topPanelLayout->addObject(exitButton);

            auto updateButton = createButton(100.0f, 30.0f, convertToUtf8("Обновить"), nullptr);
            updateButton->setCallback(std::bind(&MainApp::updateDesign, this));
            topPanelLayout->addObject(updateButton);

            auto recreateButton = createButton(200.0f, 30.0f, convertToUtf8("Перестроить дизайн"), nullptr);
            recreateButton->setCallback(std::bind(&MainApp::setDesignFromCurrentObject, this));
            topPanelLayout->addObject(recreateButton);

            auto savePresentationButton = createButton(200.0f, 30.0f, convertToUtf8("Сохранить схему типов"), nullptr);
            savePresentationButton->setCallback(std::bind(&MainApp::savePresentation, this));
            topPanelLayout->addObject(savePresentationButton);

            auto savePatternsButton = createButton(200.0f, 30.0f, convertToUtf8("Сохранить паттерны"), nullptr);
            savePatternsButton->setCallback(std::bind(&MainApp::savePatterns, this));
            topPanelLayout->addObject(savePatternsButton);
            
            auto selectDesignButton = createButton(100.0f, 30.0f, convertToUtf8("Дизайн"), nullptr);
            selectDesignButton->setCallback(std::bind(&SelectingWidget::select, viewsSelector.get(), DESIGN_VIEW));
            topPanelLayout->addObject(selectDesignButton);

            auto selectPresentationButton = createButton(100.0f, 30.0f, convertToUtf8("Схема типов"), nullptr);
            selectPresentationButton->setCallback(std::bind(&SelectingWidget::select, viewsSelector.get(), PRESENTATION_VIEW));
            topPanelLayout->addObject(selectPresentationButton);

            mainLayout->addObject(topPanelLayout);
        }

        std::shared_ptr<LinearLayout> designViewLayout;
        {
            auto skin = std::make_shared<TransparentLinearLayoutSkin>(
                std::make_shared<OffsettedBox>(), Direction::Vertical);
            skin->setPadding(0.0f);
            skin->setAdjustSize(false);
            designViewLayout = std::make_shared<LinearLayout>(nullptr, skin);
        }

        {
            std::shared_ptr<LinearLayout> designViewPropertiesLayout;
            {
                auto skin = std::make_shared<TransparentLinearLayoutSkin>(
                    std::make_shared<RelativeBox>(
                        RelativeValue(), RelativeValue(RelType::Ratio, 0.5f)),
                    Direction::Horizontal);
                skin->setPadding(0.0f);
                skin->setAdjustSize(false);
                designViewPropertiesLayout = std::make_shared<LinearLayout>(nullptr, skin);
            }

            auto skin = std::make_shared<SimpleTreeViewSkin>(
                std::make_shared<RelativeBox>(
                    RelativeValue(RelType::Ratio, 0.5f), RelativeValue()));
            auto treeView = std::make_shared<TreeView>(nullptr, skin);
            designViewPropertiesLayout->addObject(treeView);
            m_designTreeView = treeView.get();

            auto propertiesMenu = std::make_shared<SelectingWidget>(
                std::make_shared<OffsettedBox>());
            designViewPropertiesLayout->addObject(propertiesMenu);
            m_designPropertiesMenu = propertiesMenu.get();

            setDesignFromCurrentObject();

            designViewLayout->addObject(designViewPropertiesLayout);
        }

        {
            auto canvas = std::make_shared<CanvasLayout>(
                std::make_shared<RelativeBox>(RelativeValue(), RelativeValue()));
            m_canvas = canvas.get();
            designViewLayout->addObject(canvas);
        }
        
        viewsSelector->addObject(DESIGN_VIEW, designViewLayout);

        {
            std::shared_ptr<LinearLayout> presentationViewLayout;
            {
                auto skin = std::make_shared<TransparentLinearLayoutSkin>(
                    std::make_shared<OffsettedBox>(), Direction::Horizontal);
                skin->setPadding(0.0f);
                skin->setAdjustSize(false);
                presentationViewLayout = std::make_shared<LinearLayout>(nullptr, skin);
            }

            auto skin = std::make_shared<SimpleTreeViewSkin>(
                std::make_shared<RelativeBox>(
                    RelativeValue(RelType::Ratio, 0.5f), RelativeValue()));
            auto treeView = std::make_shared<TreeView>(nullptr, skin);
            presentationViewLayout->addObject(treeView);

            auto propertiesMenu = std::make_shared<SelectingWidget>(
                std::make_shared<OffsettedBox>());
            presentationViewLayout->addObject(propertiesMenu);

            {
                DesignViewBuilder builder(*treeView, *propertiesMenu, m_presentationModel, presentationForPresentationView());
                Serializer serializer(&builder);
                serializer << "" << presentationForDesignView();
            }

            viewsSelector->addObject(PRESENTATION_VIEW, presentationViewLayout);
        }

        viewsSelector->select(DESIGN_VIEW);
        mainLayout->addObject(viewsSelector);
        
        m_view->addObject(mainLayout);
        activateController(this);

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
        std::cout << "Serializing model..." << std::endl;
        auto designStr = m_designModel.toString(JsonFormat::Styled);
        //std::cout << designStr;
        std::shared_ptr<IObject> designedObj;
        std::cout << "Building object by design..." << std::endl;
        try {
            deserializeFromJson(designStr, designedObj);
        } catch (std::exception& ex) {
            std::cout << "Error while building object by design. Reason: " << ex.what() << std::endl;
            return;
        }
        
        std::cout << "Adding object to canvas..." << std::endl;
        if (m_designedObjID == -1)
            m_designedObjID = m_canvas->addObject(designedObj);
        else
            m_canvas->replaceObject(m_designedObjID, designedObj);
        m_currentObjectForDesign = designedObj;
        std::cout << "Done updating design" << std::endl;
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

    std::shared_ptr<IObject> m_currentObjectForDesign;
    DesignModel m_designModel;
    TreeView* m_designTreeView;
    SelectingWidget* m_designPropertiesMenu;
    CanvasLayout* m_canvas;
    int m_designedObjID;

    DesignModel m_presentationModel;
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

#include "TreeView.h"
#include "DesignViewBuilder.h"
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
#include <gamebase/serial/JsonSerializer.h>
#include <gamebase/serial/JsonDeserializer.h>
#include <gamebase/text/Conversion.h>

namespace gamebase { namespace editor {

namespace {
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
        //m_border.draw(position);
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
    MainApp() : m_designedObjID(-1) {}

    virtual void load() override
    {
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

        std::shared_ptr<Button> button = createButton(100.0f, 30.0f, convertToUtf8("Выход"), nullptr);
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

            mainLayout->addObject(topPanelLayout);
        }

        {
            std::shared_ptr<LinearLayout> designViewLayout;
            {
                auto skin = std::make_shared<TransparentLinearLayoutSkin>(
                    std::make_shared<RelativeBox>(
                        RelativeValue(), RelativeValue(RelType::Ratio, 0.5f)),
                    Direction::Horizontal);
                skin->setPadding(0.0f);
                skin->setAdjustSize(false);
                designViewLayout = std::make_shared<LinearLayout>(nullptr, skin);
            }

            auto skin = std::make_shared<SimpleTreeViewSkin>(
                std::make_shared<RelativeBox>(
                    RelativeValue(RelType::Ratio, 0.5f), RelativeValue()));
            auto treeView = std::make_shared<TreeView>(nullptr, skin);
            designViewLayout->addObject(treeView);

            auto propertiesMenu = std::make_shared<SelectingWidget>(
                std::make_shared<OffsettedBox>());
            designViewLayout->addObject(propertiesMenu);

            {
                DesignViewBuilder builder(*treeView, *propertiesMenu, m_designModel);
                Serializer serializer(&builder);
                serializer << "" << button;
            }

            mainLayout->addObject(designViewLayout);
        }

        {
            auto canvas = std::make_shared<CanvasLayout>(
                std::make_shared<RelativeBox>(RelativeValue(), RelativeValue()));
            m_canvas = canvas.get();
            mainLayout->addObject(canvas);
        }
        
        m_view->addObject(mainLayout);
        activateController(this);

        updateDesign();
    }

private:
    void updateDesign()
    {
        std::cout << "Updating model..." << std::endl;
        m_designModel.update();
        std::cout << "Serializing model..." << std::endl;
        auto designStr = m_designModel.toString(JsonFormat::Styled);
        //std::cout << designStr;
        std::shared_ptr<IObject> designedObj;
        std::cout << "Building object..." << std::endl;
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
        std::cout << "Done updating design" << std::endl;
    }

    DesignModel m_designModel;
    CanvasLayout* m_canvas;
    int m_designedObjID;
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

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
#include <gamebase/serial/JsonSerializer.h>
#include <gamebase/serial/JsonDeserializer.h>

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

        {
            auto button = createButton(100.0f, 30.0f, "Exit", nullptr);
            button->setCallback(std::bind(&Application::stop, this));
            mainLayout->addObject(button);

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
                DesignViewBuilder builder(*treeView, *propertiesMenu);
                Serializer serializer(&builder);
                serializer << "" << button;
            }

            mainLayout->addObject(designViewLayout);
        }
        
        m_view->addObject(mainLayout);
        activateController(this);
    }
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

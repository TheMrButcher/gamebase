#include <gamebase/engine/Application.h>
#include <gamebase/engine/Button.h>
#include <gamebase/engine/TextEdit.h>
#include <gamebase/engine/ButtonList.h>
#include <gamebase/engine/CheckBox.h>
#include <gamebase/engine/RadioButton.h>
#include <gamebase/engine/FilledRect.h>
#include <gamebase/engine/StaticLabel.h>
#include <gamebase/engine/EditableLabel.h>
#include <gamebase/engine/TextEditCursor.h>
#include <gamebase/engine/AligningOffset.h>
#include <gamebase/engine/FixedBox.h>
#include <gamebase/engine/RelativeBox.h>
#include <gamebase/geom/IdenticGeometry.h>
#include <iostream>
#include <map>

using namespace gamebase;

class SimpleButtonSkin : public ButtonSkin {
public:
    SimpleButtonSkin(const std::shared_ptr<IRelativeBox>& box)
        : m_box(box)
        , m_geom(std::make_shared<IdenticGeometry>())
    {
        m_borderWidth = 2;
        m_border.setColor(Color(1, 1, 1));
        m_colors[SelectionState::None] = Color(0.7f, 0.7f, 0.7f);
        m_colors[SelectionState::MouseOn] = Color(0.9f, 0.9f, 0.9f);
        m_colors[SelectionState::Pressed] = Color(0.5f, 0.5f, 0.5f);
        m_fill.setColor(m_colors[SelectionState::None]);

        AlignProperties alignProps;
        alignProps.horAlign = HorAlign::Center;
        alignProps.vertAlign = VertAlign::Center;
        m_text.setAlignProperties(alignProps);
    }

    void setText(const std::string& text)
    {
        m_text.setText(text);
    }

    virtual void setSelectionState(SelectionState::Enum state) override
    {
        m_fill.setColor(m_colors.at(state));
    }

    virtual void loadResources() override
    {
        m_border.loadResources();
        m_fill.loadResources();
        m_text.loadResources();
    }

    virtual void draw(const Transform2& globalPosition) const override
    {
        m_border.draw(globalPosition);
        m_fill.draw(globalPosition);
        m_text.draw(globalPosition);
    }
    
    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_box->setParentBox(allowedBox);
        auto box = m_box->get();
        m_geom->setBox(box);
        m_border.setBox(box);
        
        BoundingBox fillRect = box.extension(-m_borderWidth);
        m_fill.setBox(fillRect);
        m_text.setBox(fillRect);
    }

    virtual BoundingBox box() const override
    {
        return m_box->get();
    }

    virtual std::shared_ptr<IRelativeGeometry> geometry() const override
    {
        return m_geom;
    }

private:
    std::shared_ptr<IRelativeBox> m_box;
    std::shared_ptr<IdenticGeometry> m_geom;

    FilledRect m_border;
    FilledRect m_fill;
    StaticLabel m_text;

    float m_borderWidth;
    std::map<SelectionState::Enum, Color> m_colors;
};

class SimpleTextEditSkin : public TextEditSkin {
public:
    SimpleTextEditSkin(const std::shared_ptr<IRelativeBox>& box)
        : m_box(box)
        , m_geom(std::make_shared<IdenticGeometry>())
    {
        m_borderWidth = 2.0f;
        m_margin = 2.0f;
        m_border.setColor(Color(255, 0, 0));
        m_colors[SelectionState::Disabled] = Color(0.0f, 0.0f, 0.7f);
        m_colors[SelectionState::None] = Color(0.55f, 0.55f, 0.55f);
        m_colors[SelectionState::MouseOn] = Color(0.7f, 0.7f, 0.7f);
        m_colors[SelectionState::Selected] = Color(1.0f, 1.0f, 1.0f);
        m_fill.setColor(m_colors[SelectionState::None]);

        m_cursorPos = 0;
        m_drawCursor = false;
    }

    virtual void setText(const std::string& text) override
    {
        m_text.setText(text);
    }

    virtual void setSelection(size_t startIndex, size_t endIndex) override
    {
        if (startIndex == endIndex) {
            m_cursorPos = startIndex;
            m_drawCursor = true;
        } else {
            m_drawCursor = false;
        }
        m_text.setSelection(startIndex, endIndex);
    }

    virtual const std::vector<CharPosition>& textGeometry() const override
    {
        return m_text.textGeometry();
    }

    virtual std::shared_ptr<IRelativeGeometry> geometry() const override
    {
        return m_geom;
    }

    virtual void setSelectionState(SelectionState::Enum state) override
    {
        m_selectionState = state;
        m_fill.setColor(m_colors.at(state));
    }

    virtual void loadResources() override
    {
        m_border.loadResources();
        m_fill.loadResources();
        m_text.loadResources();

        BoundingBox charBox = m_text.textGeometry().at(m_cursorPos).position;
        m_cursor.setX(charBox.bottomLeft.x);
        m_cursor.setYRange(charBox.bottomLeft.y, charBox.topRight.y);
        m_cursor.loadResources();
    }

    virtual void draw(const Transform2& globalPosition) const override
    {
        m_border.draw(globalPosition);
        m_fill.draw(globalPosition);
        m_text.draw(globalPosition);
        if (m_selectionState == SelectionState::Selected && m_drawCursor)
            m_cursor.draw(globalPosition);
    }
    
    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_box->setParentBox(allowedBox);
        auto box = m_box->get();
        m_geom->setBox(box);
        m_border.setBox(box);
        
        BoundingBox fillRect = box.extension(-m_borderWidth);
        m_fill.setBox(fillRect);
        m_text.setBox(fillRect.extension(-m_margin));
    }

    virtual BoundingBox box() const override
    {
        return m_box->get();
    }

private:
    std::shared_ptr<IRelativeBox> m_box;
    std::shared_ptr<IdenticGeometry> m_geom;

    FilledRect m_border;
    FilledRect m_fill;
    EditableLabel m_text;
    TextEditCursor m_cursor;

    float m_borderWidth;
    float m_margin;
    std::map<SelectionState::Enum, Color> m_colors;

    size_t m_cursorPos;
    bool m_drawCursor;
};

class SimpleDragBarSkin : public ScrollDragBarSkin {
public:
    SimpleDragBarSkin(
        const std::shared_ptr<IRelativeBox>& box,
        Direction::Enum direction)
        : m_box(box)
        , m_direction(direction)
        , m_geom(std::make_shared<IdenticGeometry>())
    {
        m_borderWidth = 2;
        m_border.setColor(Color(1, 1, 1));
        m_colors[SelectionState::None] = Color(0.7f, 0.7f, 0.7f);
        m_colors[SelectionState::MouseOn] = Color(0.9f, 0.9f, 0.9f);
        m_colors[SelectionState::Pressed] = Color(0.5f, 0.5f, 0.5f);
        m_fill.setColor(m_colors[SelectionState::None]);
    }

    virtual std::shared_ptr<IRelativeGeometry> geometry() const override
    {
        return m_geom;
    }

    virtual Direction::Enum direction() const override
    {
        return m_direction;
    }

    virtual void setSelectionState(SelectionState::Enum state) override
    {
        m_selectionState = state;
        if (m_selectionState != SelectionState::Disabled)
            m_fill.setColor(m_colors.at(state));
    }

    virtual void loadResources() override
    {
        m_border.loadResources();
        m_fill.loadResources();
    }

    virtual void draw(const Transform2& globalPosition) const override
    {
        if (m_selectionState != SelectionState::Disabled) {
            m_border.draw(globalPosition);
            m_fill.draw(globalPosition);
        }
    }
    
    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_box->setParentBox(allowedBox);
        auto box = m_box->get();
        m_geom->setBox(box);
        m_border.setBox(box);
        
        BoundingBox fillRect = box.extension(-m_borderWidth);
        m_fill.setBox(fillRect);
    }

    virtual BoundingBox box() const override
    {
        return m_box->get();
    }

private:
    std::shared_ptr<IRelativeBox> m_box;
    Direction::Enum m_direction;
    std::shared_ptr<IdenticGeometry> m_geom;

    FilledRect m_border;
    FilledRect m_fill;

    float m_borderWidth;
    std::map<SelectionState::Enum, Color> m_colors;
};

class SimpleScrollBarSkin : public ScrollBarSkin {
public:
    SimpleScrollBarSkin(const std::shared_ptr<IRelativeBox>& box, Direction::Enum direction)
        : m_box(box)
        , m_direction(direction)
    {
        m_fill.setColor(Color(0.8f, 0.8f, 0.8f));
        RelativeValue horValue;
        RelativeValue vertValue(RelType::ValueMinusPixels, 40.0f);
        if (m_direction == Direction::Horizontal)
            std::swap(horValue, vertValue);
        m_dragBox = std::make_shared<RelativeBox>(
            horValue, vertValue,
            std::make_shared<AligningOffset>(HorAlign::Center, VertAlign::Center));
    }

    virtual std::shared_ptr<Button> createDecButton() const override
    {
        auto skin = std::make_shared<SimpleButtonSkin>(
            std::make_shared<RelativeBox>(
                RelativeValue(RelType::Pixels, 20.0f),
                RelativeValue(RelType::Pixels, 20.0f)));
        skin->setText("D");
        return std::make_shared<Button>(
            std::make_shared<AligningOffset>(
                m_direction == Direction::Horizontal ? HorAlign::Left : HorAlign::Center,
                m_direction == Direction::Horizontal ? VertAlign::Center : VertAlign::Bottom),
            skin);
    }

    virtual std::shared_ptr<Button> createIncButton() const override
    {
        auto skin = std::make_shared<SimpleButtonSkin>(
            std::make_shared<RelativeBox>(
                RelativeValue(RelType::Pixels, 20.0f),
                RelativeValue(RelType::Pixels, 20.0f)));
        skin->setText("I");
        return std::make_shared<Button>(
            std::make_shared<AligningOffset>(
                m_direction == Direction::Horizontal ? HorAlign::Right : HorAlign::Center,
                m_direction == Direction::Horizontal ? VertAlign::Center : VertAlign::Top),
            skin);
    }

    virtual std::shared_ptr<ScrollDragBar> createDragBar(
        const std::shared_ptr<FixedOffset>& position,
        const std::shared_ptr<FloatValue>& controlledValue) const override
    {
        auto skin = std::make_shared<SimpleDragBarSkin>(
            std::make_shared<RelativeBox>(RelativeValue(), RelativeValue()),
            m_direction);
        return std::make_shared<ScrollDragBar>(position, skin, controlledValue);
    }

    virtual BoundingBox dragBox() const override
    {
        return m_dragBox->get();
    }

    virtual Direction::Enum direction() const override
    {
        return m_direction;
    }

    virtual void loadResources() override
    {
        m_fill.loadResources();
    }

    virtual void draw(const Transform2& globalPosition) const override
    {
        m_fill.draw(globalPosition);
    }
    
    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_box->setParentBox(allowedBox);
        auto box = m_box->get();
        m_fill.setBox(box);
        m_dragBox->setParentBox(box);
    }

    virtual BoundingBox box() const override
    {
        return m_box->get();
    }

private:
    std::shared_ptr<IRelativeBox> m_box;
    Direction::Enum m_direction;
    std::shared_ptr<IRelativeBox> m_dragBox;

    FilledRect m_fill;
};

class SimpleButtonListSkin : public ButtonListSkin {
public:
    SimpleButtonListSkin(
        const std::shared_ptr<IRelativeBox>& box,
        Direction::Enum direction)
        : m_box(box)
        , m_direction(direction)
    {
        m_border.setColor(Color(1.0f, 0.0f, 0.0f));
        RelativeValue horValue(RelType::ValueMinusPixels, 20.0f);
        RelativeValue vertValue;
        if (m_direction == Direction::Horizontal)
            std::swap(horValue, vertValue);
        m_listBox = std::make_shared<RelativeBox>(
            horValue, vertValue,
            std::make_shared<AligningOffset>(
                m_direction == Direction::Horizontal ? HorAlign::Center : HorAlign::Left,
                m_direction == Direction::Horizontal ? VertAlign::Top : VertAlign::Center));
    }

    virtual std::shared_ptr<ScrollBar> createScrollBar(
        const std::shared_ptr<FloatValue>& controlledValue) const override
    {
        RelativeValue horValue(RelType::Pixels, 20.0f);
        RelativeValue vertValue;
        if (m_direction == Direction::Horizontal)
            std::swap(horValue, vertValue);
        auto skin = std::make_shared<SimpleScrollBarSkin>(
            std::make_shared<RelativeBox>(horValue, vertValue), m_direction);
        auto result = std::make_shared<ScrollBar>(
            std::make_shared<AligningOffset>(
                m_direction == Direction::Horizontal ? HorAlign::Center : HorAlign::Right,
                m_direction == Direction::Horizontal ? VertAlign::Bottom : VertAlign::Center),
            skin, controlledValue);
        result->setStepSize(5.0f);
        return result;
    }

    virtual BoundingBox listBox() const override
    {
        return m_listBox->get();
    }

    virtual Direction::Enum direction() const override
    {
        return m_direction;
    }

    virtual std::shared_ptr<IRelativeOffset> createButtonOffset(size_t) const override
    {
        return m_direction == Direction::Horizontal
            ? std::make_shared<AligningOffset>(HorAlign::Left, VertAlign::Center)
            : std::make_shared<AligningOffset>(HorAlign::Center, VertAlign::Top);
    }

    virtual void loadResources() override
    {
        m_border.loadResources();
    }

    virtual void draw(const Transform2& globalPosition) const override
    {
        m_border.draw(globalPosition);
    }
    
    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_box->setParentBox(allowedBox);
        auto box = m_box->get();
        m_listBox->setParentBox(box);
        //m_geom->setBox(box);
        m_border.setBox(box);
    }

    virtual BoundingBox box() const override
    {
        return m_box->get();
    }

private:
    std::shared_ptr<IRelativeBox> m_box;
    Direction::Enum m_direction;
    std::shared_ptr<IRelativeBox> m_listBox;

    FilledRect m_border;
};

class SimpleCheckBoxSkin : public CheckBoxSkin {
public:
    SimpleCheckBoxSkin(const std::shared_ptr<IRelativeBox>& box)
        : m_box(box)
        , m_geom(std::make_shared<IdenticGeometry>())
        , m_checked(false)
    {
        m_borderWidth = 2;
        m_checkMarginWidth = 2;
        m_border.setColor(Color(0.7f, 0.7f, 0.7f));
        m_fill.setColor(Color(1.0f, 1.0f, 1.0f));
        m_check.setColor(Color(0.0f, 0.0f, 0.0f));
    }

    virtual void setSelectionState(SelectionState::Enum) override {}

    virtual std::shared_ptr<IRelativeGeometry> geometry() const override
    {
        return m_geom;
    }

    virtual void setChecked(bool status) override
    {
        m_checked = status;
    }

    virtual void loadResources() override
    {
        m_border.loadResources();
        m_fill.loadResources();
        m_check.loadResources();
    }

    virtual void draw(const Transform2& globalPosition) const override
    {
        m_border.draw(globalPosition);
        m_fill.draw(globalPosition);
        if (m_checked)
            m_check.draw(globalPosition);
    }
    
    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_box->setParentBox(allowedBox);
        auto box = m_box->get();
        m_geom->setBox(box);
        m_border.setBox(box);
        
        BoundingBox fillRect = box.extension(-m_borderWidth);
        m_fill.setBox(fillRect);
        m_check.setBox(fillRect.extension(-m_checkMarginWidth));
    }

    virtual BoundingBox box() const override
    {
        return m_box->get();
    }

private:
    std::shared_ptr<IRelativeBox> m_box;
    std::shared_ptr<IdenticGeometry> m_geom;

    bool m_checked;

    FilledRect m_border;
    FilledRect m_fill;
    FilledRect m_check;

    float m_borderWidth;
    float m_checkMarginWidth;
};

void sayHello()
{
    std::cout << "Hello!" << std::endl;
}

void printText(const std::string& str)
{
    std::cout << "Text: " << str << std::endl;
}

void checkBoxCallback(bool status)
{
    std::cout << "New check box status: "
        << (status ? std::string("Checked") : std::string("Unchecked")) << std::endl;
}

void radioButtonCallback(size_t index)
{
    std::cout << "Checked RadioButton #" << index << std::endl;
}

class MyApplication : public Application {
public:
    virtual void load() override
    {
        {
            auto skin = std::make_shared<SimpleButtonSkin>(
                std::make_shared<FixedBox>(200.0f, 40.0f));
            skin->setText("Button");
            m_rootObject.addChild(std::shared_ptr<Button>(new Button(
                std::make_shared<FixedOffset>(-200.0f, -100.0f), skin, sayHello)));
        }

        {
            auto skin = std::make_shared<SimpleTextEditSkin>(
                std::make_shared<FixedBox>(200.0f, 40.0f));
            auto textEdit = std::shared_ptr<TextEdit>(new TextEdit(
                std::make_shared<FixedOffset>(300.0f, 200.0f), skin));
            //textEdit->setSelectionState(SelectionState::Disabled);
            m_rootObject.addChild(textEdit);
        }

        {
            auto listSkin = std::make_shared<SimpleButtonListSkin>(
                std::make_shared<FixedBox>(200.0f, 300.0f), Direction::Vertical);
            std::shared_ptr<ButtonList> buttonList(new ButtonList(
                std::make_shared<FixedOffset>(-300.0f, 150.0f), listSkin));

            const char* BUTTON_TEXTS[] = {
                "A", "B", "C", "D", "E", "F", "G", "H", "I", "J"
            };
            for (size_t i = 0; i < 10; ++i) {
                auto skin = std::make_shared<SimpleButtonSkin>(
                    std::make_shared<FixedBox>(150.0f, 40.0f));
                skin->setText(BUTTON_TEXTS[i]);
                buttonList->addButton(std::shared_ptr<Button>(new Button(
                    nullptr, skin, std::bind(printText, BUTTON_TEXTS[i]))));
            }

            m_rootObject.addChild(buttonList);
        }

        {
            auto listSkin = std::make_shared<SimpleButtonListSkin>(
                std::make_shared<FixedBox>(200.0f, 60.0f), Direction::Horizontal);
            std::shared_ptr<ButtonList> buttonList(new ButtonList(
                std::make_shared<FixedOffset>(300.0f, -200.0f), listSkin));

            const char* BUTTON_TEXTS[] = {
                "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"
            };
            for (size_t i = 0; i < 3; ++i) {
                auto skin = std::make_shared<SimpleButtonSkin>(
                    std::make_shared<FixedBox>(40.0f, 40.0f));
                skin->setText(BUTTON_TEXTS[i]);
                buttonList->addButton(std::shared_ptr<Button>(new Button(
                    nullptr, skin, std::bind(printText, BUTTON_TEXTS[i]))));
            }

            m_rootObject.addChild(buttonList);
        }

        {
            auto skin = std::make_shared<SimpleCheckBoxSkin>(
                std::make_shared<FixedBox>(20.0f, 20.0f));
            auto checkBox = std::make_shared<CheckBox>(
                std::make_shared<FixedOffset>(200.0f, 0.0f), skin);
            checkBox->setCallback(&checkBoxCallback);
            m_rootObject.addChild(checkBox);
        }

        {
            auto radioButtonGroup = std::make_shared<RadioButtonGroup>();
            radioButtonGroup->setCallback(radioButtonCallback);
            for (size_t i = 0; i < 3; ++i) {
                auto skin = std::make_shared<SimpleCheckBoxSkin>(
                    std::make_shared<FixedBox>(20.0f, 20.0f));
                auto radioButton = std::make_shared<RadioButton>(
                    std::make_shared<FixedOffset>(400.0f, 30.0f * i), skin, radioButtonGroup);
                m_rootObject.addChild(radioButton);
            }
            radioButtonGroup->setSelected(0);
        }
    }

    virtual void processKeyDown(unsigned char key) override
    {
        if (key == 27)
            stop();
    }
};

MyApplication app;

int main(int argc, char** argv)
{
    if (!app.init(&argc, argv, Application::Window, 1024, 768))
        return 1;
    app.run();
    return 0;
}

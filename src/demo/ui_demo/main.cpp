#include <gamebase/engine/Application.h>
#include <gamebase/engine/Button.h>
#include <gamebase/engine/TextEdit.h>
#include <gamebase/engine/ButtonList.h>
#include <gamebase/engine/CheckBox.h>
#include <gamebase/engine/RadioButton.h>
#include <gamebase/engine/TextList.h>
#include <gamebase/engine/Panel.h>
#include <gamebase/engine/FilledRect.h>
#include <gamebase/engine/StaticLabel.h>
#include <gamebase/engine/EditableLabel.h>
#include <gamebase/engine/TextEditCursor.h>
#include <gamebase/engine/AligningOffset.h>
#include <gamebase/engine/FixedBox.h>
#include <gamebase/engine/RelativeBox.h>
#include <gamebase/engine/AnimationManager.h>
#include <gamebase/engine/SmoothChange.h>
#include <gamebase/serial/JsonDeserializer.h>
#include <gamebase/serial/JsonSerializer.h>
#include <gamebase/geom/IdenticGeometry.h>
#include <gamebase/geom/PointGeometry.h>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <map>

using namespace gamebase;

std::shared_ptr<IAnimation> createSmoothChange(
    float startValue, float newValue)
{
    auto anim = std::make_shared<SmoothChange<Color>>(
        "fillColor", Color(startValue, startValue, startValue),
        Color(newValue, newValue, newValue), TypedTime(TimeState::Real, 500),
        ChangeFunc::Linear);
    anim->setMoveToStart(false);
    return anim;
}

class SimpleButtonSkin : public ButtonSkin {
public:
    SimpleButtonSkin(const std::shared_ptr<IRelativeBox>& box)
        : m_box(box)
        , m_geom(std::make_shared<IdenticGeometry>())
    {
        m_borderWidth = 2;
        m_border.setColor(Color(1, 1, 1));
        m_animations[SelectionState::None] = createSmoothChange(0.9f, 0.7f);
        m_animations[SelectionState::MouseOn] = createSmoothChange(0.7f, 0.9f);
        m_animations[SelectionState::Pressed] = createSmoothChange(0.9f, 0.5f);
        m_fill.setColor(Color(0.7f, 0.7f, 0.7f));

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
        m_animManager.reset();
        m_animManager.addAnimation(m_animations.at(state));
    }

    virtual void loadResources() override
    {
        m_border.loadResources();
        m_fill.loadResources();
        m_text.loadResources();
        for (auto it = m_animations.begin(); it != m_animations.end(); ++it)
            it->second->load(m_register);
    }

    virtual void drawAt(const Transform2& position) const override
    {
        m_animManager.step();
        m_border.draw(position);
        m_fill.draw(position);
        m_text.draw(position);
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

    virtual void registerObject(PropertiesRegisterBuilder* builder) override
    {
        m_border.registerProperties("border", builder);
        m_fill.registerProperties("fill", builder);
    }

private:
    std::shared_ptr<IRelativeBox> m_box;
    std::shared_ptr<IdenticGeometry> m_geom;

    FilledRect m_border;
    FilledRect m_fill;
    StaticLabel m_text;

    float m_borderWidth;
    std::map<SelectionState::Enum, std::shared_ptr<IAnimation>> m_animations;
    AnimationManager m_animManager;
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

    virtual void drawAt(const Transform2& position) const override
    {
        m_border.draw(position);
        m_fill.draw(position);
        m_text.draw(position);
        if (m_selectionState == SelectionState::Selected && m_drawCursor)
            m_cursor.draw(position);
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

    virtual void registerObject(PropertiesRegisterBuilder* builder) override
    {
        m_border.registerProperties("border", builder);
        m_fill.registerProperties("fill", builder);
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
        const std::shared_ptr<IRelativeBox>& box)
        : m_box(box)
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

    virtual void drawAt(const Transform2& position) const override
    {
        if (m_selectionState != SelectionState::Disabled) {
            m_border.draw(position);
            m_fill.draw(position);
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

    virtual void registerObject(PropertiesRegisterBuilder* builder) override
    {
        m_border.registerProperties("border", builder);
        m_fill.registerProperties("fill", builder);
    }

private:
    std::shared_ptr<IRelativeBox> m_box;
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
        const std::shared_ptr<FixedOffset>& position) const override
    {
        auto skin = std::make_shared<SimpleDragBarSkin>(
            std::make_shared<RelativeBox>(RelativeValue(), RelativeValue()));
        return std::make_shared<ScrollDragBar>(position, skin);
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

    virtual void drawAt(const Transform2& position) const override
    {
        m_fill.draw(position);
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

    virtual void registerObject(PropertiesRegisterBuilder* builder) override
    {
        m_fill.registerProperties("fill", builder);
    }

private:
    std::shared_ptr<IRelativeBox> m_box;
    Direction::Enum m_direction;
    std::shared_ptr<IRelativeBox> m_dragBox;

    FilledRect m_fill;
};

std::shared_ptr<IRelativeBox> createBox(
    RelativeValue horValue, RelativeValue vertValue, Direction::Enum direction,
    const std::shared_ptr<IRelativeOffset>& offset = nullptr)
{
    if (direction == Direction::Horizontal)
        std::swap(horValue, vertValue);
    return std::make_shared<RelativeBox>(
        horValue, vertValue, offset);
}

std::shared_ptr<IRelativeOffset> createOffset(
    Direction::Enum direction,
    HorAlign::Enum horAlignForHorizontal, HorAlign::Enum horAlignForVertical,
    VertAlign::Enum vertAlignForHorizontal, VertAlign::Enum vertAlignForVertical)
{
    return std::make_shared<AligningOffset>(
        direction == Direction::Horizontal ? horAlignForHorizontal : horAlignForVertical,
        direction == Direction::Horizontal ? vertAlignForHorizontal : vertAlignForVertical);
}

class SimpleButtonListSkin : public ButtonListSkin {
public:
    SimpleButtonListSkin(
        const std::shared_ptr<IRelativeBox>& box,
        Direction::Enum direction,
        bool adjust = false)
        : m_box(box)
        , m_direction(direction)
        , m_adjust(adjust)
        , m_scrollIsVisible(true)
    {
        m_border.setColor(Color(1.0f, 0.0f, 0.0f));
        m_listBox = createBox(RelativeValue(RelType::ValueMinusPixels, 24.0f), RelativeValue(RelType::ValueMinusPixels, 4.0f), m_direction,
            createOffset(direction, HorAlign::Center, HorAlign::Left, VertAlign::Top, VertAlign::Center));
        m_listWithScrollBox = createBox(RelativeValue(RelType::ValuePlusPixels, 20.0f), RelativeValue(), m_direction);
        m_borderBox = std::make_shared<RelativeBox>(RelativeValue(RelType::ValuePlusPixels, 4.0f), RelativeValue(RelType::ValuePlusPixels, 4.0f));
    }

    virtual std::shared_ptr<ScrollBar> createScrollBar(
        const std::shared_ptr<FloatValue>& controlledValue) const override
    {
        auto skin = std::make_shared<SimpleScrollBarSkin>(
            createBox(RelativeValue(RelType::Pixels, 20.0f), RelativeValue(), m_direction), m_direction);
        auto result = std::make_shared<ScrollBar>(
            createOffset(m_direction, HorAlign::Center, HorAlign::Right, VertAlign::Bottom, VertAlign::Center),
            skin, controlledValue);
        result->setStepSize(5.0f);
        return result;
    }

    virtual BoundingBox listBox() const override
    {
        if (m_scrollIsVisible)
            return m_listBox->get();
        else
            return m_listWithScrollBox->get();
    }

    virtual Direction::Enum direction() const override
    {
        return m_direction;
    }

    virtual std::shared_ptr<IRelativeOffset> createButtonOffset(size_t) const override
    {
        return createOffset(m_direction, HorAlign::Left, HorAlign::Center, VertAlign::Center, VertAlign::Top);
    }

    virtual void setMaxSize(float size) override
    {
        auto curBox = m_listBox->get();
        float curSize = m_direction == Direction::Horizontal
            ? curBox.width() : curBox.height();
        if (curSize >= size) {
            m_scrollIsVisible = false;
            if (m_adjust) {
                if (m_direction == Direction::Horizontal)
                    curBox.topRight.x = curBox.bottomLeft.x + size;
                else
                    curBox.bottomLeft.y = curBox.topRight.y - size;
                m_listWithScrollBox->setParentBox(curBox);
                m_borderBox->setParentBox(m_listWithScrollBox->get());
                m_border.setBox(m_borderBox->get());
            }

        } else {
            m_scrollIsVisible = true;
        }
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
        auto box = m_box->get();
        m_listBox->setParentBox(box);
        m_listWithScrollBox->setParentBox(m_listBox->get());
        m_borderBox->setParentBox(m_listWithScrollBox->get());
        m_border.setBox(m_borderBox->get());
    }

    virtual BoundingBox box() const override
    {
        return m_borderBox->get();
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override
    {
        m_border.registerProperties("border", builder);
    }

private:
    std::shared_ptr<IRelativeBox> m_box;
    Direction::Enum m_direction;
    bool m_adjust;
    std::shared_ptr<IRelativeBox> m_listBox;
    std::shared_ptr<IRelativeBox> m_listWithScrollBox;
    std::shared_ptr<IRelativeBox> m_borderBox;
    FilledRect m_border;
    bool m_scrollIsVisible;
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

    virtual void drawAt(const Transform2& position) const override
    {
        m_border.draw(position);
        m_fill.draw(position);
        if (m_checked)
            m_check.draw(position);
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

    virtual void registerObject(PropertiesRegisterBuilder* builder) override
    {
        m_border.registerProperties("border", builder);
        m_fill.registerProperties("fill", builder);
        m_check.registerProperties("check", builder);
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

class SimpleTextListSkin : public TextListSkin {
public:
    SimpleTextListSkin(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<IRelativeBox>& listBox)
        : m_box(box)
        , m_listBox(listBox)
    {}

    virtual std::shared_ptr<PressableButton> createOpenButton() const override
    {
        auto skin = std::make_shared<SimpleButtonSkin>(
            std::make_shared<FixedBox>(20.0f, 20.0f));
        skin->setText("O");
        return std::make_shared<PressableButton>(
            std::make_shared<AligningOffset>(HorAlign::Right, VertAlign::Center), skin);
    }

    virtual std::shared_ptr<TextEdit> createTextEdit() const override
    {
        auto skin = std::make_shared<SimpleTextEditSkin>(
            std::make_shared<RelativeBox>(
                RelativeValue(RelType::ValueMinusPixels, 20.0f),
                RelativeValue(RelType::Pixels, 20.0f)));
        return std::make_shared<TextEdit>(
            std::make_shared<AligningOffset>(HorAlign::Left, VertAlign::Center), skin);
    }

    virtual std::shared_ptr<ButtonList> createList() const override
    {
        auto skin = std::make_shared<SimpleButtonListSkin>(
            std::make_shared<RelativeBox>(RelativeValue(), RelativeValue()),
            Direction::Vertical, true);
        return std::make_shared<ButtonList>(
            std::make_shared<AligningOffset>(HorAlign::Left, VertAlign::Top), skin);
    }

    virtual BoundingBox listBox() const override
    {
        return m_listBox->get();
    }

    virtual void loadResources() override {}
    virtual void drawAt(const Transform2&) const override {}
    
    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_box->setParentBox(allowedBox);
        m_listBox->setParentBox(allowedBox);
    }

    virtual BoundingBox box() const override
    {
        return m_box->get();
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override {}

private:
    std::shared_ptr<IRelativeBox> m_box;
    std::shared_ptr<IRelativeBox> m_listBox;
};

class SimplePanelSkin : public PanelSkin {
public:
    SimplePanelSkin(
        const std::shared_ptr<IRelativeBox>& box)
        : m_box(box)
    {
        m_borderWidth = 2;
        m_border.setColor(Color(1.0f, 0.0f, 0.0f));
        m_fill.setColor(Color(0.9f, 0.9f, 0.9f));
        m_panelBox = std::make_shared<RelativeBox>(RelativeValue(RelType::ValueMinusPixels, 4.0f), RelativeValue(RelType::ValueMinusPixels, 4.0f));
    }

    virtual std::shared_ptr<Button> createCloseButton() const override
    {
        auto skin = std::make_shared<SimpleButtonSkin>(
            std::make_shared<RelativeBox>(
                RelativeValue(RelType::Pixels, 20.0f),
                RelativeValue(RelType::Pixels, 20.0f)));
        skin->setText("X");
        return std::make_shared<Button>(
            std::make_shared<AligningOffset>(HorAlign::Right, VertAlign::Top), skin);
    }

    virtual std::shared_ptr<ScrollDragBar> createDragBar() const override
    {
        auto skin = std::make_shared<SimpleDragBarSkin>(
            std::make_shared<RelativeBox>(
                RelativeValue(RelType::ValueMinusPixels, 20.0f),
                RelativeValue(RelType::Pixels, 20.0f)));
        return std::make_shared<ScrollDragBar>(
            std::make_shared<AligningOffset>(HorAlign::Left, VertAlign::Top), skin);
    }

    virtual BoundingBox panelBox() const override
    {
        return m_panelBox->get();
    }

    virtual void loadResources() override
    {
        m_border.loadResources();
        m_fill.loadResources();
    }

    virtual void drawAt(const Transform2& position) const override
    {
        m_border.draw(position);
        m_fill.draw(position);
    }
    
    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_box->setParentBox(allowedBox);
        auto box = m_box->get();
        m_border.setBox(box);
        m_panelBox->setParentBox(box);
        
        BoundingBox fillRect = box.extension(-m_borderWidth);
        m_fill.setBox(fillRect);
    }

    virtual BoundingBox box() const override
    {
        return m_box->get();
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override
    {
        m_border.registerProperties("border", builder);
        m_fill.registerProperties("fill", builder);
    }

private:
    std::shared_ptr<IRelativeBox> m_box;
    std::shared_ptr<IRelativeBox> m_panelBox;

    float m_borderWidth;
    FilledRect m_border;
    FilledRect m_fill;
    
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

void openWindow(Panel* window)
{
    window->setVisible(true);
    window->resetPosition();
}

class SmallWindow : public ViewController {
public:
    SmallWindow(const std::string& id)
        : ViewController(id)
    {
        m_zIndex = 10;
    }

    virtual void load() override
    {
        auto panelSkin = std::make_shared<SimplePanelSkin>(
            std::make_shared<FixedBox>(200.0f, 300.0f));
        m_view = std::make_shared<Panel>(
            std::make_shared<FixedOffset>(0.0f, 0.0f), panelSkin);
        m_view->setCloseCallback(std::bind(&ViewController::deactivate, this));

        {
            auto skin = std::make_shared<SimpleButtonSkin>(
                std::make_shared<FixedBox>(150.0f, 40.0f));
            skin->setText("Button in window");
            m_view->addObject(std::shared_ptr<Button>(new Button(
                std::make_shared<FixedOffset>(0.0f, -80.0f), skin, sayHello)));
        }

        {
            auto skin = std::make_shared<SimpleTextEditSkin>(
                std::make_shared<FixedBox>(150.0f, 40.0f));
            auto textEdit = std::shared_ptr<TextEdit>(new TextEdit(
                std::make_shared<FixedOffset>(0.0f, 80.0f), skin));
            m_view->addObject(textEdit);
        }
    }

    virtual void focused() override
    {
        ViewController::focused();
        std::cout << m_id << " get focus" << std::endl;
    }
};

class MainPanelSkin : public PanelSkin {
public:
    MainPanelSkin() { m_bg.setColor(Color(0.6f, 0.6f, 0.6f)); }

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

private:
    BoundingBox m_box;
    FilledRect m_bg;
};

void findObject(IRegistrable* registrable)
{
    auto& props = registrable->properties();
    std::string objToFind = props.getProperty<std::string>("/appView/objects/objToFind/text")->get();
    auto searchResultType = props.getProperty<std::string>("/appView/objects/searchResultType/text");
    auto searchValue = props.getProperty<std::string>("/appView/objects/searchValue/text");
    if (props.hasProperty(objToFind)) {
        auto prop = props.getAbstractProperty(objToFind);
        if (auto realProp = std::dynamic_pointer_cast<Value<float>>(prop)) {
            searchResultType->set("Float property");
            searchValue->set(boost::lexical_cast<std::string>(realProp->get()));
            return;
        }

        searchValue->set("");
        searchResultType->set("Property (unknown type)");
        return;
    }
    if (props.hasObject(objToFind)) {
        searchValue->set(typeid(*props.getAbstractObject(objToFind)).name());
        searchResultType->set("Object");
        return;
    }
    searchResultType->set("Can't find object");
}

class SmallSerializationTest : public ISerializable {
public:
    SmallSerializationTest()
        : v(10, 5)
        , f(20)
    {}

    virtual void serialize(ISerializer* serializer) const override
    {
        Serializer(serializer) << "v" << v << "f" << f;
    }

    Vec2 v;
    float f;
};

class SerializationTest : public ISerializable {
public:
    SerializationTest()
        : subobj(std::make_shared<SmallSerializationTest>())
        , v(124, 546)
        , c(0.5f, 0.25f, 0.75f, 1.0f)
        , b(Vec2(10, 20), Vec2(110, 120))
        , t(Matrix2(10, 20, 20, 10), Vec2(100, 200))
        , vb(10, true)
    {
        m[10] = 25.5;
        m[45] = 12.4;
    }

    virtual void serialize(ISerializer* serializer) const override
    {
        Serializer(serializer) << "v" << v << "c" << c << "b" << b << "t" << t << "vb" << vb
            << "subobj" << subobj << "m" << m;
    }

    std::shared_ptr<IObject> subobj;
    Vec2 v;
    Color c;
    BoundingBox b;
    Transform2 t;
    std::vector<bool> vb;
    std::map<int, double> m;
};

IObject* deserializeSmallTest(IDeserializer* deserializer)
{
    SmallSerializationTest* t = new SmallSerializationTest;
    Deserializer(deserializer) >> "v" >> t->v >> "f" >> t->f;
    return t;
}

IObject* deserializeTest(IDeserializer* deserializer)
{
    SerializationTest* t = new SerializationTest;
    Deserializer(deserializer) >> "v" >> t->v >> "c" >> t->c >> "b" >> t->b >> "t" >> t->t >> "vb" >> t->vb
        >> "subobj" >> t->subobj >> "m" >> t->m;
    return t;
}

class MyApplication : public Application {
public:
    virtual void load() override
    {
        SerializableRegister::instance().registerType<SmallSerializationTest>("SmallSerializationTest", &deserializeSmallTest);
        SerializableRegister::instance().registerType<SerializationTest>("SerializationTest", &deserializeTest);

        SerializationTest serializationTest;
        serializationTest.v = Vec2(1000, 2000);
        std::dynamic_pointer_cast<SmallSerializationTest>(serializationTest.subobj)->f = -1090.35f;
        serializeToJsonFile(serializationTest, JsonSerializer::Styled, "test.json");

        SerializationTest serializationTest2;
        deserializeFromJsonFile("test.json", serializationTest2);
        std::cout << serializeToJson(serializationTest2, JsonSerializer::Styled);

        m_view = std::make_shared<Panel>(std::make_shared<FixedOffset>(), std::make_shared<MainPanelSkin>());
        
        {
            auto skin = std::make_shared<SimpleTextEditSkin>(
                std::make_shared<FixedBox>(300.0f, 30.0f));
            auto textEdit = std::shared_ptr<TextEdit>(new TextEdit(
                std::make_shared<FixedOffset>(300.0f, 300.0f), skin));
            textEdit->setName("objToFind");
            m_view->addObject(textEdit);
        }

        {
            auto skin = std::make_shared<SimpleButtonSkin>(
                std::make_shared<FixedBox>(200.0f, 30.0f));
            skin->setText("Find");
            auto button = std::shared_ptr<Button>(new Button(
                std::make_shared<FixedOffset>(250.0f, 250.0f), skin));
            button->setCallback(std::bind(&findObject, button.get()));
            m_view->addObject(button);
        }
        
        {
            auto skin = std::make_shared<SimpleTextEditSkin>(
                std::make_shared<FixedBox>(300.0f, 30.0f));
            auto textEdit = std::shared_ptr<TextEdit>(new TextEdit(
                std::make_shared<FixedOffset>(300.0f, 200.0f), skin));
            textEdit->setName("searchResultType");
            m_view->addObject(textEdit);
        }

        {
            auto skin = std::make_shared<SimpleTextEditSkin>(
                std::make_shared<FixedBox>(300.0f, 30.0f));
            auto textEdit = std::shared_ptr<TextEdit>(new TextEdit(
                std::make_shared<FixedOffset>(300.0f, 150.0f), skin));
            textEdit->setName("searchValue");
            m_view->addObject(textEdit);
        }

        {
            auto skin = std::make_shared<SimpleButtonSkin>(
                std::make_shared<FixedBox>(200.0f, 40.0f));
            skin->setText("Pressable button");
            auto button = std::make_shared<PressableButton>(
                std::make_shared<FixedOffset>(-300.0f, -200.0f), skin);
            button->setCallback(std::bind(&printText, "Pressed"));
            button->setUnpressCallback(std::bind(&printText, "Unpressed"));
            m_view->addObject(button);
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

            m_view->addObject(buttonList);
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

            m_view->addObject(buttonList);
        }

        {
            auto skin = std::make_shared<SimpleCheckBoxSkin>(
                std::make_shared<FixedBox>(20.0f, 20.0f));
            auto checkBox = std::make_shared<CheckBox>(
                std::make_shared<FixedOffset>(200.0f, 0.0f), skin);
            checkBox->setCallback(&checkBoxCallback);
            m_view->addObject(checkBox);
        }

        {
            auto radioButtonGroup = std::make_shared<RadioButtonGroup>();
            radioButtonGroup->setCallback(radioButtonCallback);
            for (size_t i = 0; i < 3; ++i) {
                auto skin = std::make_shared<SimpleCheckBoxSkin>(
                    std::make_shared<FixedBox>(20.0f, 20.0f));
                auto radioButton = std::make_shared<RadioButton>(
                    std::make_shared<FixedOffset>(400.0f, 30.0f * i), skin, radioButtonGroup);
                m_view->addObject(radioButton);
            }
            radioButtonGroup->setSelected(0);
        }

        {
            auto listSkin = std::make_shared<SimpleTextListSkin>(
                std::make_shared<FixedBox>(200.0f, 20.0f),
                std::make_shared<FixedBox>(BoundingBox(Vec2(-100.0f, -210.0f), Vec2(100.0f, -10.0f))));
            auto textList = std::make_shared<TextList>(
                std::make_shared<FixedOffset>(0.0f, 0.0f), listSkin);

            const char* BUTTON_TEXTS[] = {
                "Abracadabra", "Some text", "Third variant"
            };
            for (size_t i = 0; i < 3; ++i) {
                auto skin = std::make_shared<SimpleButtonSkin>(
                    std::make_shared<RelativeBox>(RelativeValue(), RelativeValue(RelType::Pixels, 30.0f)));
                skin->setText(BUTTON_TEXTS[i]);
                textList->addButton(
                    BUTTON_TEXTS[i],
                    std::shared_ptr<Button>(new Button(nullptr, skin)));
            }

            m_view->addObject(textList);
        }

        {
            auto skin = std::make_shared<SimpleButtonSkin>(
                std::make_shared<FixedBox>(200.0f, 40.0f));
            skin->setText("Open window 1");
            m_view->addObject(std::shared_ptr<Button>(new Button(
                std::make_shared<FixedOffset>(-300.0f, -280.0f), skin,
                std::bind(&MyApplication::activateControllerByName, this, "window1"))));
        }
        
        {
            auto skin = std::make_shared<SimpleButtonSkin>(
                std::make_shared<FixedBox>(200.0f, 40.0f));
            skin->setText("Open window 2");
            m_view->addObject(std::shared_ptr<Button>(new Button(
                std::make_shared<FixedOffset>(-300.0f, -340.0f), skin,
                std::bind(&MyApplication::activateControllerByName, this, "window2"))));
        }
        
        registerController(std::make_shared<SmallWindow>("window1"));
        registerController(std::make_shared<SmallWindow>("window2"));
        activateController(this);
    }

    virtual void processKeyDown(unsigned char key) override
    {
        if (key == 27)
            stop();
    }
};

int main(int argc, char** argv)
{
    MyApplication app;
    if (!app.init(&argc, argv, Application::Window, 1024, 768))
        return 1;
    app.run();
    return 0;
}

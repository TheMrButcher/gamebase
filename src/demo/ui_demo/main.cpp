#include <gamebase/engine/Application.h>
#include <gamebase/engine/Button.h>
#include <gamebase/engine/TextEdit.h>
#include <gamebase/engine/FilledRect.h>
#include <gamebase/engine/StaticLabel.h>
#include <gamebase/engine/EditableLabel.h>
#include <gamebase/engine/TextEditCursor.h>
#include <gamebase/geom/RectGeometry.h>
#include <iostream>
#include <map>

using namespace gamebase;

class SimpleButtonSkin : public Skin {
public:
    SimpleButtonSkin()
    {
        m_border.setColor(Color(1, 1, 1));
        m_colors.push_back(Color(0.7f, 0.7f, 0.7f));
        m_colors.push_back(Color(0.9f, 0.9f, 0.9f));
        m_colors.push_back(Color(0.5f, 0.5f, 0.5f));
        m_fill.setColor(m_colors[SelectionState::None]);

        AlignProperties alignProps;
        alignProps.horAlign = HorAlign::Center;
        alignProps.vertAlign = VertAlign::Center;
        m_text.setAlignProperties(alignProps);
    }

    void setBorder(const BoundingBox& rect, float borderWidth)
    {
        m_border.setRect(rect);
        
        BoundingBox fillRect = rect.extension(-borderWidth);
        m_fill.setRect(fillRect);
        m_text.setRect(fillRect);
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

private:
    FilledRect m_border;
    FilledRect m_fill;
    StaticLabel m_text;

    std::vector<Color> m_colors;
};

class SimpleTextEditSkin : public TextEditSkin {
public:
    SimpleTextEditSkin()
    {
        m_borderWidth = 2.0f;
        m_margin = 2.0f;
        m_border.setColor(Color(255, 0, 0));
        m_colors[SelectionState::None] = Color(0.55f, 0.55f, 0.55f);
        m_colors[SelectionState::MouseOn] = Color(0.7f, 0.7f, 0.7f);
        m_colors[SelectionState::Selected] = Color(1.0f, 1.0f, 1.0f);
        m_fill.setColor(m_colors[SelectionState::None]);

        m_cursorPos = 0;
        m_drawCursor = false;
    }

    void setRect(const BoundingBox& rect)
    {
        m_border.setRect(rect);
        
        BoundingBox fillRect = rect.extension(-m_borderWidth);
        m_fill.setRect(fillRect);
        m_text.setRect(fillRect.extension(-m_margin));
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

private:
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

void sayHello()
{
    std::cout << "Hello!" << std::endl;
}

class MyApplication : public Application {
public:
    virtual void load() override
    {
        {
            BoundingBox rect(Vec2(-100, -20), Vec2(100, 20));
            auto rectGeom = std::make_shared<RectGeometry>(rect);
            auto skin = std::make_shared<SimpleButtonSkin>();
            skin->setBorder(rect, 3);
            skin->setText("Button");
            m_rootObject.addChild(std::shared_ptr<Button>(new Button(
                Vec2(-200, -100), rectGeom, skin, sayHello)));
        }

        {
            BoundingBox rect(Vec2(-100, -20), Vec2(100, 20));
            auto rectGeom = std::make_shared<RectGeometry>(rect);
            auto skin = std::make_shared<SimpleTextEditSkin>();
            skin->setRect(rect);
            m_rootObject.addChild(std::shared_ptr<TextEdit>(new TextEdit(
                Vec2(300, 200), rectGeom, skin)));
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

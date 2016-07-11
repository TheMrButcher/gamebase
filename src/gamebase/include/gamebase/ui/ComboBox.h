#pragma once

#include <gamebase/ui/Button.h>
#include <gamebase/impl/ui/ComboBox.h>
#include <gamebase/impl/pubhelp/Helpers.h>

namespace gamebase {

class ComboBox {
public:
    int add(const std::string& text, const Button& button);
    void insert(int id, const std::string& text, const Button& button);

    const std::string& text() const;
    void setText(const std::string& text);

    int selected() const;
    std::vector<std::string> variants() const;

    void setCallback(const std::function<void()>& callback);

    bool isVisible() const;
    void setVisible(bool value);
    void show();
    void hide();

    Vec2 pos() const;
    void setPos(float x, float y);
    void setPos(const Vec2& v);
    BoundingBox box() const;
    BoundingBox movedBox() const;
    float width() const;
    float height() const;

    operator bool() const;

    GAMEBASE_DEFINE_PIMPL(ComboBox, ComboBox);
};

/////////////// IMPLEMENTATION ///////////////////

inline int ComboBox::add(const std::string& text, const Button& button) { return m_impl->addButton(text, button.getImpl().getShared()); }
inline void ComboBox::insert(int id, const std::string& text, const Button& button) { m_impl->addButton(text, button.getImpl().getShared(), static_cast<unsigned int>(id)); }
inline const std::string& ComboBox::text() const { return m_impl->text(); }
inline void ComboBox::setText(const std::string& text) { m_impl->setText(text); }
inline int ComboBox::selected() const { return m_impl->currentVariantID(); }
inline std::vector<std::string> ComboBox::variants() const { return m_impl->textVariants(); }
inline void ComboBox::setCallback(const std::function<void()>& callback) { m_impl->setCallback(callback); }
GAMEBASE_DEFINE_UI_PASSIVE_ELEMENT_METHODS(ComboBox);

}

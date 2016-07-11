#pragma once

#include <gamebase/impl/skin/base/ComboBoxSkin.h>

namespace gamebase { namespace impl {

class GAMEBASE_API ComboBox : public OffsettedPosition, public IFindable,
    public Drawable, public Registrable, public ISerializable {
public:
    ComboBox(
        const std::shared_ptr<ComboBoxSkin>& skin,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    const std::string& text() const { return m_textBox->text(); }
    void setText(const std::string& text) { m_textBox->setText(text); }

    int currentVariantID() const;

    std::vector<std::string> textVariants() const;
    size_t variantsNum() const { return m_idToText.size(); }

    void setCallback(const std::function<void()>& callback)
    {
        m_callback = callback;
    }

    int addButton(const std::string& text, const std::shared_ptr<Button>& button);
    void addButton(const std::string& text, const std::shared_ptr<Button>& button, unsigned int id);
    void update();

    virtual bool isSelectableByPoint(const Vec2& point) const override { return false; }
    virtual std::shared_ptr<IObject> findChildByPoint(const Vec2& point) const override;

    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;
    virtual BoundingBox box() const override;

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    
    virtual void serialize(Serializer& s) const override;

private:
    void changeState(bool isOpened);
    void changeStateCallback();
    void setTextFromVariant(int id);

    std::shared_ptr<ComboBoxSkin> m_skin;
    std::shared_ptr<ToggleButton> m_openButton;
    std::shared_ptr<TextBox> m_textBox;
    std::function<void()> m_callback;
    std::map<int, std::string> m_idToText;
    std::unordered_map<std::string, int> m_textToID;
    int m_nextID;
    std::shared_ptr<ButtonList> m_list;
    
    bool m_isListOpened;
    int m_buttonListID;
};

} }

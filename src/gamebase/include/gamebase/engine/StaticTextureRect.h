#pragma once

#include <gamebase/engine/TextureRect.h>
#include <gamebase/engine/OffsettedPosition.h>
#include <gamebase/engine/IRelativeBox.h>
#include <gamebase/engine/Registrable.h>
#include <gamebase/serial/ISerializable.h>

namespace gamebase {

class GAMEBASE_API StaticTextureRect : public TextureRect, public OffsettedPosition,
    public Registrable, public ISerializable {
public:
    StaticTextureRect(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<IRelativeOffset>& position = nullptr)
        : TextureRect(this)
        , OffsettedPosition(position)
        , m_box(box)
    {}
    
    const std::string& imageName() const { return m_imageName; }
    void setImageName(const std::string& name) { m_imageName = name; }

    void setFixedBox(float width, float height);

    virtual void loadResources() override;

    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_box->setParentBox(allowedBox);
        TextureRect::setBox(m_box->get());
        setPositionBoxes(allowedBox, box());
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override
    {
        builder->registerProperty("color", &m_color);
        builder->registerProperty("colorA", &m_color.a);
        builder->registerProperty("r", &m_color.r);
        builder->registerProperty("g", &m_color.g);
        builder->registerProperty("b", &m_color.b);
        builder->registerProperty("a", &m_color.a);
    }
    
    virtual void serialize(Serializer& s) const override;

protected:
    void loadTextureImpl();

private:
    virtual void setTexture(const GLTexture& texture)
    {
        THROW_EX() << "Call to StaticTextureRect::setTexture is restricted";
    }

    std::shared_ptr<IRelativeBox> m_box;
    std::string m_imageName;
};

typedef StaticTextureRect Texture;

}

#pragma once

#include <gamebase/engine/TextureRect.h>
#include <gamebase/engine/OffsettedPosition.h>
#include <gamebase/engine/IRelativeBox.h>
#include <gamebase/engine/Registrable.h>
#include <gamebase/serial/ISerializable.h>

namespace gamebase {

class GAMEBASE_API StaticTextureRect : public OffsettedPosition, public TextureRect,
    public Registrable, public ISerializable {
public:
    StaticTextureRect(const std::shared_ptr<IRelativeBox>& box)
        : TextureRect(this)
        , m_box(box)
    {}
    
    const std::string& imageName() const { return m_imageName; }
    void setImageName(const std::string& name) { m_imageName = name; }

    virtual void loadResources() override;

    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_box->setParentBox(allowedBox);
        TextureRect::setBox(m_box->get());
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override
    {
        builder->registerColor("color", &m_color);
    }
    
    virtual void serialize(Serializer& s) const override;

private:
    virtual void setTexture(const Texture& texture)
    {
        THROW_EX() << "Call to StaticTextureRect::setTexture is restricted";
    }

    std::shared_ptr<IRelativeBox> m_box;
    std::string m_imageName;
};

}

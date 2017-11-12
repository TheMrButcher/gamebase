/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/drawobj/TextureRect.h>
#include <gamebase/impl/pos/OffsettedPosition.h>
#include <gamebase/impl/relbox/IRelativeBox.h>
#include <gamebase/impl/relbox/IResizable.h>
#include <gamebase/impl/reg/Registrable.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class GAMEBASE_API StaticTextureRect : public TextureRect, public OffsettedPosition,
    public Registrable, public ISerializable, public IResizable {
public:
    StaticTextureRect(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<IRelativeOffset>& position = nullptr)
        : TextureRect(this)
        , OffsettedPosition(position)
        , m_box(box)
    {}
    
    const std::string& imageName() const { return m_imageName; }
    void setImageName(const std::string& name) { m_imageName = name; reload(); }

    const std::shared_ptr<IRelativeBox>& relativeBox() const { return m_box; }
    virtual void setFixedBox(float width, float height) override;

    static GLTexture loadTextureImpl(const std::string& imageName);

    virtual void loadResources() override { reload(); }

    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_box->setParentBox(allowedBox);
        m_parentBox = allowedBox;
        TextureRect::setBox(m_box->get());
        setPositionBoxes(allowedBox, box());
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override
    {
        builder->registerProperty("color", &m_color);
        builder->registerProperty("r", &m_color.r);
        builder->registerProperty("g", &m_color.g);
        builder->registerProperty("b", &m_color.b);
        builder->registerProperty("a", &m_color.a);
    }
    
    virtual void serialize(Serializer& s) const override;

protected:
    void loadTextureImpl();

    void reload()
    {
        if (m_box->isValid()) {
            reloadImpl();
            updateImpl();
        }
    }

    virtual void reloadImpl()
    {
        loadTextureImpl();
    }

    void update()
    {
        if (isTextureLoaded())
            updateImpl();
    }

    virtual void updateImpl()
    {
        TextureRect::loadResources();
    }

private:
    virtual void setTexture(const GLTexture& texture)
    {
        THROW_EX() << "Call to StaticTextureRect::setTexture is restricted";
    }

    std::shared_ptr<IRelativeBox> m_box;
    std::string m_imageName;
    BoundingBox m_parentBox;
};

typedef StaticTextureRect Texture;

} }

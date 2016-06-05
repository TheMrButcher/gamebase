#pragma once

#include <gamebase/impl/engine/Direction.h>
#include <gamebase/impl/engine/Drawable.h>
#include <gamebase/impl/pos/IPositionable.h>
#include <gamebase/impl/reg/Registrable.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class GAMEBASE_API Line : public Drawable, public IPositionable,
    public Registrable, public ISerializable {
public:
    Line(const std::shared_ptr<Drawable>& skin);

    const Vec2& p0() const { return m_p0; }
    void setP0(const Vec2& v) { m_p0 = v; update(); }
    const Vec2& start() const { return m_p0; }
    void setStart(const Vec2& v) { setP0(v); }

    const Vec2& p1() const { return m_p1; }
    void setP1(const Vec2& v) { m_p1 = v; update(); }
    const Vec2& end() const { return m_p1; }
    void setEnd(const Vec2& v) { setP1(v); }

    float width() const { return m_width; }
    void setWidth(float value) { m_width = value; update(); }

    void set(const Vec2& p0, const Vec2& p1, float width);
    void set(const Vec2& p0, const Vec2& p1) { set(p0, p1, m_width); }

    Direction::Enum stretchDir() const { return m_stretchDir; }
    void setStretchDir(Direction::Enum value) { m_stretchDir = value; }

    virtual Transform2 position() const override { return m_transform; }
    
    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override
    {
        if (m_skin)
            m_skin->draw(position);
    }

    virtual void setBox(const BoundingBox& allowedBox) override;
    virtual BoundingBox box() const override
    {
        return m_box;
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    virtual void serialize(Serializer& s) const override;

private:
    void update();
    void updateBox();

    std::shared_ptr<Drawable> m_skin;
    Vec2 m_p0;
    Vec2 m_p1;
    float m_width;
    Direction::Enum m_stretchDir;
    
    bool m_inited;
    Box m_box;
    Transform2 m_transform;
};

} }

/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/skin/base/LinearLayoutSkin.h>
#include <gamebase/impl/tools/ObjectsCollection.h>
#include <gamebase/impl/pos/OffsettedPosition.h>
#include <gamebase/impl/relbox/IResizable.h>

namespace gamebase { namespace impl {

class GAMEBASE_API LinearLayout : public OffsettedPosition, public Drawable,
    public Registrable, public IFindable, public ISerializable, public IResizable {
public:
    LinearLayout(
        const std::shared_ptr<LinearLayoutSkin>& skin,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    LinearLayoutSkin* skin() const { return m_skin.get(); }
	void setSkipInvisibleElements(bool value) { m_skipInvisibleElements = value; }
	bool skipInvisibleElements() const { return m_skipInvisibleElements; }
    int addObject(const std::shared_ptr<IObject>& obj);
    void insertObject(int id, const std::shared_ptr<IObject>& obj);
    void removeObject(int id);
    void removeObject(IObject* obj);
    const std::vector<std::shared_ptr<IObject>>& objects() const { return m_list.objects(); }
    void clear() { m_list.clear(); }
    Direction::Enum direction() const { return m_skin->direction(); }

    virtual void setFixedBox(float width, float height) override;
    void update();

    void setAssociatedSelectable(ISelectable* selectable);

    virtual bool isSelectableByPoint(const Vec2& point) const override { return false; }
    virtual std::shared_ptr<IObject> findChildByPoint(const Vec2& point) const override;
    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;

    virtual BoundingBox box() const override
    {
        return m_skin->box();
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    
    virtual void serialize(Serializer& s) const override;

private:
    std::shared_ptr<LinearLayoutSkin> m_skin;
	bool m_skipInvisibleElements;
    ObjectsCollection m_list;
	BoundingBox m_allowedBox;
};

} }


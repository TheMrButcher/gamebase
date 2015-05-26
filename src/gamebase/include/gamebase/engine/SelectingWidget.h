#pragma once

#include <gamebase/engine/ObjectsSelector.h>
#include <gamebase/engine/OffsettedPosition.h>
#include <gamebase/engine/IRelativeBox.h>

namespace gamebase {

class GAMEBASE_API SelectingWidget : public ObjectsSelector, public OffsettedPosition {
public:
    SelectingWidget(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    virtual void addObject(int id, const std::shared_ptr<IObject>& object) override;

    virtual void setBox(const BoundingBox& allowedBox) override;
    virtual BoundingBox box() const override;

    virtual void serialize(Serializer& s) const override;

private:
    std::shared_ptr<IRelativeBox> m_box;
};

}

#include <stdafx.h>
#include <gamebase/engine/SelectingWidget.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

SelectingWidget::SelectingWidget(
    const std::shared_ptr<IRelativeBox>& box,
    const std::shared_ptr<IRelativeOffset>& position)
    : ObjectsSelector(this)
    , OffsettedPosition(position)
    , m_box(box)
{}

void SelectingWidget::addObject(int id, const std::shared_ptr<IObject>& object)
{
    if (auto positionable = dynamic_cast<IPositionable*>(object.get()))
        positionable->setParentPosition(this);
    ObjectsSelector::addObject(id, object);
}

void SelectingWidget::setBox(const BoundingBox& allowedBox)
{
    m_box->setParentBox(allowedBox);
    ObjectsSelector::setBox(allowedBox);
    setPositionBoxes(allowedBox, box());
}

BoundingBox SelectingWidget::box() const
{
    return m_box->get();
}

void SelectingWidget::serialize(Serializer& s) const
{
    ObjectsSelector::serialize(s);
    s << "box" << m_box << "position" << m_offset;
}

std::unique_ptr<IObject> deserializeSelectingWidget(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    std::unique_ptr<SelectingWidget> result(new SelectingWidget(box, position));
    deserializeObjectsSelectorElements(deserializer, result.get());
    return std::move(result);
}

REGISTER_CLASS(SelectingWidget);

}

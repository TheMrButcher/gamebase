#include <stdafx.h>
#include <gamebase/impl/ui/SelectingWidget.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

SelectingWidget::SelectingWidget(
    const std::shared_ptr<IRelativeBox>& box,
    const std::shared_ptr<IRelativeOffset>& position)
    : ObjectsSelector(this)
    , OffsettedPosition(position)
    , m_box(box)
    , m_adjustment(Adjustment::None)
{}

void SelectingWidget::addObject(int id, const std::shared_ptr<IObject>& object)
{
    if (auto positionable = dynamic_cast<IPositionable*>(object.get()))
        positionable->setParentPosition(this);
    ObjectsSelector::addObject(id, object);
}

void SelectingWidget::removeObject(int id)
{
    ObjectsSelector::removeObject(id);
    m_initedObjects.erase(id);
}

void SelectingWidget::clear()
{
    ObjectsSelector::clear();
    m_initedObjects.clear();
}

void SelectingWidget::select(int id)
{
    ObjectsSelector::select(id);
    if (m_initedObjects.count(id) == 0 && m_box->isValid()) {
        setBox(m_parentBox);
        loadResources();
    }
}

void SelectingWidget::setBox(const BoundingBox& allowedBox)
{
    m_parentBox = allowedBox;
    m_box->setParentBox(allowedBox);
    auto it = m_objDescs.find(m_currentObjectID);
    if (it != m_objDescs.end() && it->second.drawable) {
        it->second.drawable->setBox(m_box->get());
        m_initedObjects.erase(m_currentObjectID);
    }

    if (m_adjustment == Adjustment::None || it == m_objDescs.end() || !it->second.drawable) {
        m_curBox = m_box->get();
    } else {
        m_curBox = it->second.drawable->box();
        if (it->second.positionable)
            m_curBox = m_curBox.transformed(it->second.positionable->position());
        if (m_adjustment == Adjustment::ToFitContentAndArea)
            m_curBox.enlarge(m_box->get());
    }

    setPositionBoxes(allowedBox, box());
}

void SelectingWidget::loadResources()
{
    if (m_initedObjects.count(m_currentObjectID) == 0) {
        auto it = m_objDescs.find(m_currentObjectID);
        if (it != m_objDescs.end()) {
            it->second.drawable->loadResources();
            m_initedObjects.insert(m_currentObjectID);
        }
    }
}

void SelectingWidget::serialize(Serializer& s) const
{
    ObjectsSelector::serialize(s);
    s << "box" << m_box << "position" << m_offset << "adjustment" << m_adjustment;
}

std::unique_ptr<IObject> deserializeSelectingWidget(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE_OPT(Adjustment::Enum, adjustment, Adjustment::None);
    std::unique_ptr<SelectingWidget> result(new SelectingWidget(box, position));
    result->setAdjustment(adjustment);
    deserializeObjectsSelectorElements(deserializer, result.get());
    return std::move(result);
}

REGISTER_CLASS(SelectingWidget);

} }

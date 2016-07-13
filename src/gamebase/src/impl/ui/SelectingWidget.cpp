#include <stdafx.h>
#include <gamebase/impl/ui/SelectingWidget.h>
#include <gamebase/impl/relbox/FixedBox.h>
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
    , m_nextID(0)
{}

int SelectingWidget::addObject(const std::shared_ptr<IObject>& object)
{
    int id = m_nextID;
    insertObject(id, object);
    return id;
}

void SelectingWidget::insertObject(int id, const std::shared_ptr<IObject>& object)
{
    m_nextID = std::max(id + 1, m_nextID);
    if (auto positionable = dynamic_cast<IPositionable*>(object.get()))
        positionable->setParentPosition(this);
    markNeedReload(id);
    ObjectsSelector::insertObject(id, object);
    if (m_currentObjectID == id)
        select(id);
    m_cachedObjs.clear();
}

void SelectingWidget::removeObject(int id)
{
    ObjectsSelector::removeObject(id);
    m_initedObjects.erase(id);
    m_cachedObjs.clear();
}

void SelectingWidget::removeObject(IObject* obj)
{
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
        if (it->second.get() == obj) {
            removeObject(it->first);
            return;
        }
}

bool SelectingWidget::hasObject(IObject* obj) const
{
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
        if (it->second.get() == obj)
            return true;
    return false;
}

IObject* SelectingWidget::getIObject(int id) const
{
    auto it = m_objects.find(id);
    if (it == m_objects.end())
        THROW_EX() << "Can't find object with id=" << id << " in Selector";
    return it->second.get();
}

void SelectingWidget::clear()
{
    ObjectsSelector::clear();
    m_initedObjects.clear();
    m_cachedObjs.clear();
}

void SelectingWidget::select(int id)
{
    ObjectsSelector::select(id);
    if (m_initedObjects.count(id) == 0 && m_box->isValid()) {
        setBox(m_parentBox);
        loadResources();
    }
}

void SelectingWidget::setFixedBox(float width, float height)
{
    auto box = std::make_shared<FixedBox>(width, height);
    if (m_box->isValid())
        box->checkInited();
    m_box = box;
    if (m_parentBox.isValid()) {
        markAllNeedReload();
        select(selected());
    }
}

const std::vector<std::shared_ptr<IObject>>& SelectingWidget::objectsAsList() const
{
    if (m_cachedObjs.empty()) {
        m_cachedObjs.reserve(m_objects.size());
        for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
            m_cachedObjs.push_back(it->second);
    }
    return m_cachedObjs;
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
            m_curBox.transform(it->second.positionable->position());
        if (m_adjustment == Adjustment::ToFitContentAndArea)
            m_curBox.add(m_box->get());
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

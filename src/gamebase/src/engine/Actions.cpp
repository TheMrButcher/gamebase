#include <stdafx.h>
#include <gamebase/engine/RemoveAction.h>
#include "src/core/GlobalTemporary.h"
#include <gamebase/engine/MoveAction.h>
#include <gamebase/engine/GameView.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

namespace {
void removeObjectFunc(ILayer* layer, IObject* obj)
{
    layer->removeObject(obj);
}

void moveObjectFunc(ILayer* srcLayer, ILayer* dstLayer, std::shared_ptr<IObject> obj)
{
    srcLayer->removeObject(obj);
    dstLayer->addObject(obj);
}
}

void RemoveAction::load(const PropertiesRegister& props)
{
    m_obj = props.holder();
}

void RemoveAction::exec()
{
    auto* layer = m_obj->properties().findParentOfType<ILayer>();
    g_temp.delayedTasks.push_back(std::bind(removeObjectFunc, layer, m_obj));
}

void RemoveAction::serialize(Serializer& s) const {}

std::unique_ptr<IObject> deserializeRemoveAction(Deserializer& deserializer)
{
    return std::unique_ptr<IObject>(new RemoveAction());
}

void MoveAction::load(const PropertiesRegister& props)
{
    m_obj = props.holder();
}

void MoveAction::exec()
{
    auto* srcLayer = m_obj->properties().findParentOfType<ILayer>();
    auto* gameView = srcLayer->properties().findParentOfType<GameView>();
    auto* dstLayer = gameView->getLayer<ILayer>(m_dstLayerID);
    auto objSPtr = srcLayer->getIObjectSPtr(m_obj);
    g_temp.delayedTasks.push_back(std::bind(moveObjectFunc, srcLayer, dstLayer, objSPtr));
}

void MoveAction::serialize(Serializer& s) const
{
    s << "dstLayerID" << m_dstLayerID;
}

std::unique_ptr<IObject> deserializeMoveAction(Deserializer& deserializer)
{
    DESERIALIZE(int, dstLayerID);
    return std::unique_ptr<IObject>(new MoveAction(dstLayerID));
}

REGISTER_CLASS(RemoveAction);
REGISTER_CLASS(MoveAction);

}

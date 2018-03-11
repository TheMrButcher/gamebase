/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/anim/RemoveAction.h>
#include "src/impl/global/GlobalTemporary.h"
#include <gamebase/impl/anim/MoveAction.h>
#include <gamebase/impl/gameview/GameView.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>
#include <iostream>

namespace gamebase { namespace impl {

void RemoveAction::load(const PropertiesRegister& props)
{
    m_obj = props.holder();
}

void RemoveAction::exec()
{
    auto* layer = m_obj->properties().findParentOfType<ILayer>();
    auto obj = layer->getIObjectSPtr(m_obj);
    g_temp.delayedTasks.push_back([obj, regObj = m_obj]()
    {
        auto* layer = regObj->properties().findParentOfType<ILayer>();
        auto* gameView = layer->properties().findParentOfType<GameView>();
        if (layer->hasObject(obj))
            layer->removeObject(obj);
    });
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
    if (!m_dstLayerName) {
        std::cout << "Layer ID in MoveAction is not supported anymore. Please set name of layer" << std::endl;
        return;
    }

    auto* layer = m_obj->properties().findParentOfType<ILayer>();
    auto obj = layer->getIObjectSPtr(m_obj);
    g_temp.delayedTasks.push_back([obj, regObj = m_obj, dstLayerName = *m_dstLayerName]()
    {
        auto* srcLayer = regObj->properties().findParentOfType<ILayer>();
        auto* gameView = srcLayer->properties().findParentOfType<GameView>();
        auto* dstLayer = gameView->getLayer(dstLayerName);
        if (srcLayer == dstLayer)
            return;
        if (srcLayer->hasObject(obj)) {
            srcLayer->removeObject(obj);
            dstLayer->addObject(obj);
        }
    });
}

void MoveAction::serialize(Serializer& s) const
{
    if (s.mode() == SerializationMode::ForcedFull || m_dstLayerName)
        s << "dstLayerName" << m_dstLayerName.get_value_or("");
}

std::unique_ptr<IObject> deserializeMoveAction(Deserializer& deserializer)
{
    if (deserializer.hasMember("dstLayerName")) {
        DESERIALIZE(std::string, dstLayerName);
        return std::make_unique<MoveAction>(dstLayerName);
    }
    return std::make_unique<MoveAction>();
}

REGISTER_CLASS(RemoveAction);
REGISTER_CLASS(MoveAction);

} }

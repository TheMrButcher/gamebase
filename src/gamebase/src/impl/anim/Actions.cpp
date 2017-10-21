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

namespace gamebase { namespace impl {

void RemoveAction::load(const PropertiesRegister& props)
{
    m_obj = props.holder();
}

void RemoveAction::exec()
{
    auto* layer = m_obj->properties().findParentOfType<ILayer>();
	g_temp.delayedTasks.push_back([layer, obj = m_obj]()
	{
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
    auto* srcLayer = m_obj->properties().findParentOfType<ILayer>();
    auto* gameView = srcLayer->properties().findParentOfType<GameView>();
    auto* dstLayer = gameView->getLayer<ILayer>(m_dstLayerID);
    auto obj = srcLayer->getIObjectSPtr(m_obj);
	g_temp.delayedTasks.push_back([srcLayer, dstLayer, obj]()
	{
		if (srcLayer->hasObject(obj))
			srcLayer->removeObject(obj);
		if (!dstLayer->hasObject(obj))
			dstLayer->addObject(obj);
	});
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

} }

/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/pubhelp/ToImpl.h>
#include <gamebase/ui/Layout.h>
#include <gamebase/drawobj/DrawObj.h>
#include <gamebase/gameobj/GameObj.h>

namespace gamebase { namespace impl {
    
IObject* ToImpl<Layout>::castRaw(const Layout& obj)
{
    return obj.getImpl()->getInternalObj().get();
}

std::shared_ptr<IObject> ToImpl<Layout>::castShared(const Layout& obj)
{
    return obj.getImpl()->getInternalObj().getShared();
}

SmartPointer<IObject> ToImpl<Layout>::castSmart(const Layout& obj)
{
    return obj.getImpl()->getInternalObj();
}

IObject* ToImpl<DrawObj>::castRaw(const DrawObj& obj)
{
    return obj.getImpl()->getInternalObj().get();
}

std::shared_ptr<IObject> ToImpl<DrawObj>::castShared(const DrawObj& obj)
{
    return obj.getImpl()->getInternalObj().getShared();
}

SmartPointer<IObject> ToImpl<DrawObj>::castSmart(const DrawObj& obj)
{
    return obj.getImpl()->getInternalObj();
}

IObject* ToImpl<GameObj>::castRaw(const GameObj& obj)
{
    return obj.getImpl()->getInternalObj().get();
}

std::shared_ptr<IObject> ToImpl<GameObj>::castShared(const GameObj& obj)
{
    return obj.getImpl()->getInternalObj().getShared();
}

SmartPointer<IObject> ToImpl<GameObj>::castSmart(const GameObj& obj)
{
    return obj.getImpl()->getInternalObj();
}

} }

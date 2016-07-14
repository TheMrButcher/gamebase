/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/pubhelp/SelfRegistrar.h>
#include <gamebase/gameview/Layer.h>

#define FromDesign(ClassName, MemberName) GAMEBASE_DEFINE_SELF_REGISTRAR_WITH_NAME(ClassName, MemberName, #MemberName)
#define FromDesign2(ClassName, MemberName, NameInDesign) GAMEBASE_DEFINE_SELF_REGISTRAR_WITH_NAME(ClassName, MemberName, NameInDesign)
#define LayerFromDesign(DataTypeName, MemberName) GAMEBASE_DEFINE_SELF_REGISTRAR_WITH_NAME(Layer<DataTypeName>, MemberName, #MemberName)
#define LayerFromDesign2(DataTypeName, MemberName, NameInDesign) GAMEBASE_DEFINE_SELF_REGISTRAR_WITH_NAME(Layer<DataTypeName>, MemberName, NameInDesign)

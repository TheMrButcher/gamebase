#pragma once

#include <gamebase/impl/pubhelp/SelfRegistrar.h>

#define FromDesign(ClassName, MemberName) GAMEBASE_DEFINE_SELF_REGISTRAR_WITH_NAME(ClassName, MemberName, #MemberName)
#define FromDesignWithName(ClassName, MemberName, NameInDesign) GAMEBASE_DEFINE_SELF_REGISTRAR_WITH_NAME(ClassName, MemberName, NameInDesign)

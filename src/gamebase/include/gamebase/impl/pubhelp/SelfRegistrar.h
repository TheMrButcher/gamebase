/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/app/AppType.h>
#include <gamebase/app/App.h>
#include <gamebase/impl/pubhelp/AppImpl.h>
#include <gamebase/impl/pubhelp/Wrapping.h>

namespace gamebase { namespace impl {

template <typename T>
void registerObject(T* obj, const std::string& name, IRegistrable* reg)
{
    IObject* objImpl = reg->getAbstractChild(name);
    *obj = wrap<T>(objImpl);
}


#define GAMEBASE_DEFINE_SELF_REGISTRAR_WITH_NAME(ClassName, MemberName, NameInDesign) \
    ClassName MemberName; \
    struct __Gamebase__##MemberName##SelfRegistrarClass { \
        __Gamebase__##MemberName##SelfRegistrarClass() \
        { \
            gamebase::impl::g_appImpl->addRegistrar( \
                std::function<void(App*, gamebase::impl::IRegistrable*)>(*this)); \
        } \
        void operator()(App* pubAppPtr, gamebase::impl::IRegistrable* reg) \
        { \
            auto realPubAllPtr = dynamic_cast<GAMEBASE_APP_TYPE*>(pubAppPtr); \
            registerObject(&(realPubAllPtr->MemberName), NameInDesign, reg); \
        } \
    } __Gamebase__##MemberName##SelfRegistrar

} }

#pragma once

#include <gamebase/app/AppType.h>
#include <gamebase/app/App.h>
#include <gamebase/impl/pubhelp/AppImpl.h>
#include <gamebase/impl/pubhelp/Wrapping.h>

namespace gamebase { namespace impl {

template <typename T>
bool registerObject(T* obj, const std::string& name, IRegistrable* reg, bool throwOnError)
{
    IObject* objImpl = nullptr;
    if (throwOnError) {
        objImpl = reg->getAbstractChild(name);
    } else {
        objImpl = reg->tryGetAbstractChild(name);
        if (!objImpl)
            return false;
    }

    try {
        *obj = wrap<T>(objImpl);
    } catch (Exception&) {
        if (throwOnError) {
            throw;
        } else {
            return false;
        }
    }
    return true;
}


#define GAMEBASE_DEFINE_SELF_REGISTRAR_WITH_NAME(ClassName, MemberName, NameInDesign) \
    ClassName MemberName; \
    struct __Gamebase__##MemberName##SelfRegistrarClass { \
        __Gamebase__##MemberName##SelfRegistrarClass() \
        { \
            gamebase::impl::g_appImpl->addRegistrar( \
                std::function<bool(App*, gamebase::impl::IRegistrable*, bool)>(*this)); \
        } \
        bool operator()(App* pubAppPtr, gamebase::impl::IRegistrable* reg, bool throwOnError) \
        { \
            auto realPubAllPtr = dynamic_cast<GAMEBASE_APP_TYPE*>(pubAppPtr); \
            return registerObject(&(realPubAllPtr->MemberName), NameInDesign, reg, throwOnError); \
        } \
    } __Gamebase__##MemberName##SelfRegistrar

} }

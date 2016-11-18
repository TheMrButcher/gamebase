/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "RegisterSwitcher.h"
#include <gamebase/impl/relpos/AligningOffset.h>
#include <gamebase/impl/relpos/FixedOffset.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>
#include <gamebase/impl/serial/SerializableRegister.h>

namespace gamebase { namespace editor {
namespace {
typedef std::function<void(const impl::IObject*, impl::Serializer&)> SFunc;
typedef std::function<std::unique_ptr<impl::IObject>(impl::Deserializer&)> DFunc;

RegisterSwitcher::RegType currentRegType;

template <typename T>
struct ClassRegistrar {
    ClassRegistrar(
        const std::string& typeName,
        const DFunc& deserialize,
        const SFunc& serialize = nullptr)
        : typeName(typeName)
        , deserialize(deserialize)
        , serialize(serialize ? serialize : &impl::internal::defaultSerialize)
    {}

    void operator()() const
    {
        impl::SerializableRegister::instance().registerType<T>(typeName, deserialize, serialize);
    }

    std::string typeName;
    DFunc deserialize;
    SFunc serialize;
};

class RegisterSnapshot {
public:
    RegisterSnapshot(RegisterSwitcher::RegType targetRegType)
        : targetRegType_(targetRegType)
    {}

    template <typename T>
    void add()
    {
        add<T>(impl::SerializableRegister::instance().typeTraits<T>().deserialize);
    }

    template <typename T>
    void add(const DFunc& deserialize)
    {
        auto typeName = impl::SerializableRegister::instance().typeName<T>();
        m_registrars.push_back(ClassRegistrar<T>(typeName, deserialize));
    }

    template <typename T>
    void add(const DFunc& deserialize, const SFunc& serialize)
    {
        auto typeName = impl::SerializableRegister::instance().typeName<T>();
        add<T>(typeName, deserialize, serialize);
    }

    template <typename T>
    void add(const std::string& typeName, const DFunc& deserialize, const SFunc& serialize)
    {
        m_registrars.push_back(ClassRegistrar<T>(typeName, deserialize, serialize));
    }

    void apply() const
    {
        currentRegType = targetRegType_;
        for (auto it = m_registrars.begin(); it != m_registrars.end(); ++it) {
            (*it)();
        }
    }

protected:
    std::vector<std::function<void()>> m_registrars;
    RegisterSwitcher::RegType targetRegType_;
};

void serializeFixedOffsetAsAligning(
    const impl::IObject* obj, impl::Serializer& s)
{
    if (auto offset = dynamic_cast<const impl::FixedOffset*>(obj)) {
        Vec2 v = offset->get();
        s   << "horAlign" << impl::HorAlign::None
            << "vertAlign" << impl::VertAlign::None
            << "horOffset" << impl::RelativeValue(impl::RelType::Pixels, v.x)
            << "vertOffset" << impl::RelativeValue(impl::RelType::Pixels, v.y);
    } else {
        THROW_EX() << "Serializer and object don't match";
    }
}

std::unique_ptr<impl::IObject> deserializeOffset(impl::Deserializer& deserializer)
{
    using namespace impl;
    DESERIALIZE(HorAlign::Enum, horAlign);
    DESERIALIZE(VertAlign::Enum, vertAlign);
    DESERIALIZE(RelativeValue, horOffset);
    DESERIALIZE(RelativeValue, vertOffset);
    std::unique_ptr<IObject> result;
    if (horAlign == HorAlign::None && vertAlign == VertAlign::None
        && horOffset.type() == RelType::Pixels && vertOffset.type() == RelType::Pixels) {
        result.reset(new FixedOffset(horOffset.value(), vertOffset.value()));
    } else {
        result.reset(new AligningOffset(
            static_cast<HorAlign::Enum>(horAlign), 
            static_cast<VertAlign::Enum>(vertAlign),
            horOffset, vertOffset));
    }
    return std::move(result);
}

class ResultBuilderRegisterSnapshot : public RegisterSnapshot {
public:
    static const RegisterSnapshot& instance()
    {
        static ResultBuilderRegisterSnapshot SNAPSHOT;
        return SNAPSHOT;
    }

private:
    ResultBuilderRegisterSnapshot()
        : RegisterSnapshot(RegisterSwitcher::ResultReg)
    {
        add<impl::FixedOffset>();
        add<impl::AligningOffset>(&deserializeOffset);
    }
};

class DesignViewBuilderRegisterSnapshot : public RegisterSnapshot {
public:
    static const RegisterSnapshot& instance()
    {
        static DesignViewBuilderRegisterSnapshot SNAPSHOT;
        return SNAPSHOT;
    }

private:
    DesignViewBuilderRegisterSnapshot()
        : RegisterSnapshot(RegisterSwitcher::DVBReg)
    {
        add<impl::FixedOffset>("AligningOffset", &deserializeOffset, &serializeFixedOffsetAsAligning);
        add<impl::AligningOffset>(&deserializeOffset);
    }
};
}

RegisterSwitcher::RegisterSwitcher()
    : startingReg_(current())
{
    if (startingReg_ != DVBReg)
        DesignViewBuilderRegisterSnapshot::instance().apply();
}

RegisterSwitcher::~RegisterSwitcher()
{
    if (startingReg_ == ResultReg)
        ResultBuilderRegisterSnapshot::instance().apply();
}

void RegisterSwitcher::init()
{
    ResultBuilderRegisterSnapshot::instance().apply();
}

RegisterSwitcher::RegType RegisterSwitcher::current()
{
    return currentRegType;
}
} }

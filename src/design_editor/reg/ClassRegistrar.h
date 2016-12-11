/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/engine/IObject.h>
#include <gamebase/impl/serial/IDeserializer.h>
#include <gamebase/impl/serial/ISerializer.h>

namespace gamebase { namespace editor {

typedef std::function<void(const impl::IObject*, impl::Serializer&)> SFunc;
typedef std::function<std::unique_ptr<impl::IObject>(impl::Deserializer&)> DFunc;

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

} }

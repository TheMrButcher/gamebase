/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <reg/RegisterSwitcher.h>
#include <reg/ClassRegistrar.h>
#include <reg/CurrentRegType.h>

namespace gamebase { namespace editor {

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
        g_currentRegType = targetRegType_;
        for (auto it = m_registrars.begin(); it != m_registrars.end(); ++it) {
            (*it)();
        }
    }

protected:
    std::vector<std::function<void()>> m_registrars;
    RegisterSwitcher::RegType targetRegType_;
};

} }

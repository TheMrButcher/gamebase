/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/pubhelp/Helpers.h>
#include <gamebase/impl/ui/RadioButtonGroup.h>

#define GAMEBASE_DEFINE_RADIO_GROUP_PIMPL() \
public: \
    typedef impl::RadioButtonGroup Impl; \
    RadioGroup() : m_impl(std::make_shared<Impl>()) {} \
    const std::shared_ptr<Impl>& getImpl() const { return m_impl; } \
private: \
    std::shared_ptr<Impl> m_impl

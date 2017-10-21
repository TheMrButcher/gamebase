/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/text/TextBank.h>
#include <gamebase/impl/pubhelp/Helpers.h>

namespace gamebase {

class TextBank {
public:
    void insert(const std::string& key, const std::string& value);
	std::string operator[](const std::string& key) const;
    std::string get(const std::string& key) const;
    std::string get(const std::string& key, int index) const;

    operator bool() const;

    GAMEBASE_DEFINE_PIMPL(TextBank, TextBank);
};

/////////////// IMPLEMENTATION ///////////////////

inline void TextBank::insert(const std::string& key, const std::string& value) { m_impl->insert(key, value); }
inline std::string TextBank::operator[](const std::string& key) const { return m_impl->get(key); }
inline std::string TextBank::get(const std::string& key) const { return m_impl->get(key); }
inline std::string TextBank::get(const std::string& key, int index) const { return m_impl->get(key, index); }
inline TextBank::operator bool() const { return m_impl; }

}

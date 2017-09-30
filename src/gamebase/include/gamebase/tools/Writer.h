/**
* Copyright (c) 2016 Slavnejshev Filipp
* This file is licensed under the terms of the MIT license.
*/

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/tools/Exception.h>
#include <gamebase/tools/Movable.h>
#include <functional>
#include <sstream>

namespace gamebase {

class Writer {
public:
	MOVABLE(Writer);

	typedef std::function<void(const std::string&)> Flusher;

	template <typename ObjType>
	Writer(ObjType& obj);

	Writer(std::ostringstream&& stream, Flusher&& flush);

	template <typename T>
	Writer operator<<(const T& value);

	~Writer();

private:
	std::ostringstream m_stream;
	Flusher m_flush;
};

/////////////// IMPLEMENTATION ///////////////////

template <typename ObjType> Writer::Writer(ObjType& obj)
	: m_flush([o = obj](const auto& str) mutable { o.setText(str); })
{}
Writer::Writer(std::ostringstream&& stream, Flusher&& flush)
	: m_stream(std::move(stream)), m_flush(std::move(flush))
{}
Writer::~Writer()
{
	if (m_flush)
		m_flush(m_stream.str());
}

template <typename T>
Writer Writer::operator<<(const T& value)
{
	m_stream << value;
	Flusher flush(std::move(m_flush));
	m_flush = nullptr;
	return Writer(std::move(m_stream), std::move(flush));
}

}

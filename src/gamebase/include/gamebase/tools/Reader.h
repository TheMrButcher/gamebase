/**
* Copyright (c) 2016 Slavnejshev Filipp
* This file is licensed under the terms of the MIT license.
*/

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/tools/Exception.h>
#include <gamebase/tools/Movable.h>
#include <sstream>

namespace gamebase {

class Reader {
public:
	MOVABLE(Reader);

	template <typename ObjType>
	Reader(const ObjType& obj);

	Reader(std::istringstream&& stream);

	template <typename T>
	Reader operator>>(T& dst);

private:
	std::istringstream m_stream;
};

/////////////// IMPLEMENTATION ///////////////////

template <typename ObjType> Reader::Reader(const ObjType& obj) : m_stream(obj.text()) {}
Reader::Reader(std::istringstream&& stream) : m_stream(std::move(stream)) {}

template <typename T>
Reader Reader::operator>>(T& dst)
{
	m_stream >> dst;
	if (m_stream.bad() || m_stream.fail())
		THROW_EX() << "Failed to read value of type: " << typeid(T).name();
	return Reader(std::move(m_stream));
}

}

/**
* Copyright (c) 2016 Slavnejshev Filipp
* This file is licensed under the terms of the MIT license.
*/

#include <stdafx.h>
#include <gamebase/impl/graphics/Window.h>
#include "InitInternal.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>

namespace gamebase { namespace impl {

Window::Window()
    : m_mode(GraphicsMode::Window)
    , m_size(800, 600)
{}

Window::Window(Window&& other)
    : m_title(std::move(other.title()))
    , m_mode(other.mode())
    , m_size(other.size())
    , m_windowImpl(std::move(other.m_windowImpl))
{}

Window::~Window() {}

std::string Window::title() const
{
    return m_title;
}

void Window::setTitle(const std::string& title)
{
    m_title = title;
    if (isInited())
        m_windowImpl->setTitle(sf::String::fromUtf8(title.begin(), title.end()));
}

Size Window::size() const
{
    return m_size;
}

void Window::setSize(unsigned int w, unsigned int h)
{
    m_size = Size(w, h);
	if (isInited()) {
		auto windowImplSize = m_windowImpl->getSize();
		if (windowImplSize.x != w || windowImplSize.y != h)
			m_windowImpl->setSize(sf::Vector2u(w, h));
		m_windowImpl->setView(sf::View(
			sf::FloatRect(0.f, 0.f, static_cast<float>(w), static_cast<float>(h))));
	}
}

GraphicsMode::Enum Window::mode() const
{
    return m_mode;
}

void Window::setMode(GraphicsMode::Enum mode)
{
    if (isInited()) {
        std::cerr << "Warning: can't set mode, already inited";
        return;
    }
    m_mode = mode;
}

void Window::init(int* argc, char** argv)
{
    if (m_title.empty())
       setTitle("Gamebase Application");
    m_windowImpl = initWindowImpl(
        argc, argv,
        static_cast<int>(m_size.width),
        static_cast<int>(m_size.height),
        m_title, m_mode);
}

void Window::destroy()
{
    m_windowImpl.reset();
}

sf::RenderWindow* Window::getImpl()
{
    return m_windowImpl.get();
}

bool Window::isInited() const
{
    return m_windowImpl != nullptr;
}

} }

/**
* Copyright (c) 2017 Slavnejshev Filipp
* This file is licensed under the terms of the MIT license.
*/

#pragma once

#include <gamebase/tools/Size.h>
#include <gamebase/impl/graphics/GraphicsMode.h>
#include <boost/optional.hpp>
#include <memory>
#include <string>

namespace sf {
class RenderWindow;
}

namespace gamebase { namespace impl {

class Window {
public:
    Window();
    Window(Window&& other);
    ~Window();

    std::string title() const;
    void setTitle(const std::string& title);
    Size size() const;
    void setSize(unsigned int w, unsigned int h);
    void setMinSize(unsigned int w, unsigned int h);
    void setMaxSize(unsigned int w, unsigned int h);
    GraphicsMode::Enum mode() const;
    void setMode(GraphicsMode::Enum mode);
    void init(int* argc, char** argv);
    void destroy();

    sf::RenderWindow* getImpl() const;
    bool isInited() const;

private:
    std::string m_title;
    GraphicsMode::Enum m_mode;
    Size m_size;
    boost::optional<Size> m_minSize;
    boost::optional<Size> m_maxSize;
    std::unique_ptr<sf::RenderWindow> m_windowImpl;
};

} }

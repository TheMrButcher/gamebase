/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/app/ViewController.h>
#include "src/impl/graphics/State.h"

namespace gamebase { namespace impl {

ViewController::ViewController(const std::string& id)
    : m_id(id)
    , m_viewState(Invisible)
    , m_zIndex(0)
{}

ViewController::~ViewController() {}

void ViewController::initView()
{
    load();
}

void ViewController::loadViewResources()
{
    if (m_view) {
        const auto& graphicsState = gamebase::impl::state();
        m_view->setBox(BoundingBox(
            Vec2(-0.5f * graphicsState.width, -0.5f * graphicsState.height),
            Vec2(0.5f * graphicsState.width, 0.5f * graphicsState.height)));
        m_view->loadResources();
    }
}

void ViewController::renderView()
{
    if (m_view)
        m_view->draw(projectionTransform());
}

void ViewController::moveView()
{
    move();
}
} }

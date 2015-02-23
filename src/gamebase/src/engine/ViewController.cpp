#include <stdafx.h>
#include <gamebase/engine/ViewController.h>
#include "src/graphics/State.h"

namespace gamebase {

ViewController::ViewController(const std::string& id)
    : m_id(id)
    , m_viewState(Invisible)
    , m_zIndex(0)
    , m_moveTime(1.0f / 60)
{}

ViewController::~ViewController() {}

void ViewController::initView()
{
    load();
    if (m_view) {
        const auto& graphicsState = gamebase::state();
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
    if (m_view)
        m_view->objects().move(m_moveTime);
}
}

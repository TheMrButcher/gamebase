#pragma once

#include <gamebase/engine/AnimatedButtonSkin.h>
#include <gamebase/engine/Button.h>

namespace gamebase { namespace editor {

std::shared_ptr<AnimatedButtonSkin> createButtonSkin(
    float width, float height, const std::string& textStr);

std::shared_ptr<Button> createButton(
    float width, float height, const std::string& textStr,
    const std::shared_ptr<IRelativeOffset>& offset = nullptr);

} }

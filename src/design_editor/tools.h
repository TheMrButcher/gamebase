#pragma once

#include <gamebase/engine/AnimatedButtonSkin.h>
#include <gamebase/engine/Button.h>

namespace gamebase { namespace editor {

std::shared_ptr<AnimatedButtonSkin> createButtonSkin(
    const std::shared_ptr<IRelativeBox>& box, const std::string& textStr);

std::shared_ptr<AnimatedButtonSkin> createButtonSkin(
    float width, float height, const std::string& textStr);

std::shared_ptr<Button> createButton(
    float width, float height, const std::string& textStr,
    const std::shared_ptr<IRelativeOffset>& offset = nullptr);

void createBackup(const std::string& pathStr, int backupsNum = 1);

} }

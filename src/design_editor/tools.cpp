#include "tools.h"
#include <gamebase/engine/StaticFilledRect.h>
#include <gamebase/engine/AligningOffset.h>
#include <gamebase/engine/SmoothChange.h>
#include <gamebase/engine/StaticLabel.h>
#include <gamebase/engine/FixedOffset.h>
#include <gamebase/engine/FixedBox.h>
#include <gamebase/engine/RelativeBox.h>
#include <gamebase/utils/FileIO.h>

namespace gamebase { namespace editor {

namespace {
std::shared_ptr<IAnimation> createSmoothChange(
    float startValue, float newValue)
{
    auto anim = std::make_shared<SmoothChange<Color>>(
        "#fill/color", Color(startValue, startValue, startValue),
        Color(newValue, newValue, newValue), TypedTime(TimeState::Real, 100),
        ChangeFunc::Linear);
    anim->setMoveToStart(false);
    return anim;
}

std::string backupExtension(int index)
{
    std::ostringstream ss;
    ss << "backup" << index;
    return ss.str();
}
}
std::shared_ptr<AnimatedButtonSkin> createButtonSkin(
    const std::shared_ptr<IRelativeBox>& box, const std::string& textStr)
{
    auto skin = std::make_shared<AnimatedButtonSkin>(box);

    auto border = std::make_shared<StaticFilledRect>(
        std::make_shared<RelativeBox>(RelativeValue(), RelativeValue()));
    border->setColor(Color(0, 0, 0));
    border->setName("border");
    skin->addElement(border);

    auto fill = std::make_shared<StaticFilledRect>(
        std::make_shared<RelativeBox>(
            RelativeValue(RelType::ValueMinusPixels, 4.0f),
            RelativeValue(RelType::ValueMinusPixels, 4.0f),
            std::make_shared<AligningOffset>(HorAlign::Center, VertAlign::Center)));
    fill->setColor(Color(0.7f, 0.7f, 0.7f));
    fill->setName("fill");
    skin->addElement(fill);

    if (!textStr.empty()) {
        auto text = std::make_shared<StaticLabel>(
            std::make_shared<RelativeBox>(RelativeValue(), RelativeValue()));
        AlignProperties properties;
        properties.horAlign = HorAlign::Center;
        properties.vertAlign = VertAlign::Center;
        properties.enableStacking = false;
        text->setAlignProperties(properties);
        text->setText(textStr);
        text->setAdjustSize(false);
        text->setName("text");
        skin->addElement(text);
    }

    skin->setTransitAnimation(SelectionState::None, createSmoothChange(0.9f, 0.7f));
    skin->setTransitAnimation(SelectionState::MouseOn, createSmoothChange(0.7f, 0.9f));
    skin->setTransitAnimation(SelectionState::Pressed, createSmoothChange(0.9f, 0.5f));

    return skin;
}


std::shared_ptr<AnimatedButtonSkin> createButtonSkin(
    float width, float height, const std::string& textStr)
{
    return createButtonSkin(std::make_shared<FixedBox>(width, height), textStr);
}

std::shared_ptr<Button> createButton(
    float width, float height, const std::string& textStr,
    const std::shared_ptr<IRelativeOffset>& offset)
{
    auto skin = createButtonSkin(width, height, textStr);
    return std::shared_ptr<Button>(new Button(
        offset ? offset : std::make_shared<FixedOffset>(0.0f, 0.0f), skin));
}

void createBackup(const std::string& pathStr, int backupsNum)
{
    for (int i = backupsNum; i > 0; --i) {
        auto pathToBackup = makePathStr("", pathStr, backupExtension(i));
        if (fileExists(pathToBackup)) {
            if (i == backupsNum)
                removeFile(pathToBackup);
            else
                renameFile(pathToBackup, makePathStr("", pathStr, backupExtension(i + 1)));
        }
    }
    if (fileExists(pathStr))
        renameFile(pathStr, makePathStr("", pathStr, backupExtension(1)));
}

FilePathDialog& getFilePathDialog()
{
    static FilePathDialog dialog;
    return dialog;
}

} }

/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "Settings.h"
#include "Presentation.h"
#include <gamebase/impl/app/Config.h>
#include <json/reader.h>
#include <json/writer.h>

namespace gamebase { namespace editor { namespace settings {

bool isInterfaceExtended = false;
std::string workDir;
std::string imagesDir;
std::string mainConf;
std::string designedObjConf;
bool isBackupEnabled;
bool isComplexBoxMode;

namespace {
bool complexLayerMode;
bool complexAnimationMode;
}

bool isComplexLayerMode()
{
    return complexLayerMode;
}

void setComplexLayerMode(bool value)
{
    complexLayerMode = value;
    if (complexLayerMode) {
        presentationForDesignView()->setPropertyBaseType(
            "ImmobileLayer", "objects", "");
        presentationForDesignView()->setPropertyBaseType(
            "GameView", "layers", "");
    } else {
        presentationForDesignView()->setPropertyBaseType(
            "ImmobileLayer", "objects", "ObjectConstruct");
        presentationForDesignView()->setPropertyBaseType(
            "GameView", "layers", "Layer");
    }
}

bool isComplexAnimationMode()
{
    return complexAnimationMode;
}

void setComplexAnimationMode(bool value)
{
    complexAnimationMode = value;
    if (complexAnimationMode) {
        presentationForDesignView()->setPropertyBaseType(
            "AnimatedObject", "animations", "");
        presentationForDesignView()->setPropertyBaseType(
            "CompositeAnimation", "animations", "");
        presentationForDesignView()->setPropertyBaseType(
            "ParallelAnimation", "animations", "");
        presentationForDesignView()->setPropertyBaseType(
            "RepeatingAnimation", "animation", "");
        presentationForDesignView()->setPropertyBaseType(
            "AnimatedCheckBoxSkin", "checkAnimation", "");
        presentationForDesignView()->setPropertyBaseType(
            "AnimatedCheckBoxSkin", "uncheckAnimation", "");
        presentationForDesignView()->setPropertyBaseType(
            "AnimatedObjectConstruct", "animations", "");
    }
    else {
        presentationForDesignView()->setPropertyBaseType(
            "AnimatedObject", "animations", "BasicAnimation");
        presentationForDesignView()->setPropertyBaseType(
            "CompositeAnimation", "animations", "BasicAnimation");
        presentationForDesignView()->setPropertyBaseType(
            "ParallelAnimation", "animations", "BasicAnimation");
        presentationForDesignView()->setPropertyBaseType(
            "RepeatingAnimation", "animation", "BasicAnimation");
        presentationForDesignView()->setPropertyBaseType(
            "AnimatedCheckBoxSkin", "checkAnimation", "BasicAnimation");
        presentationForDesignView()->setPropertyBaseType(
            "AnimatedCheckBoxSkin", "uncheckAnimation", "BasicAnimation");
        presentationForDesignView()->setPropertyBaseType(
            "AnimatedObjectConstruct", "animations", "BasicAnimation");
    }
}

void init()
{
    isInterfaceExtended = impl::getValueFromConfig("interface", "basic") == "extended";
    workDir = impl::getValueFromConfig("workingPath", ".");
    imagesDir = impl::getValueFromConfig("designedObjectImagesPath", impl::getValueFromConfig("imagesPath"));
    isBackupEnabled = impl::getValueFromConfig("isBackupEnabled", "true") == "true";
    isComplexBoxMode = impl::getValueFromConfig("isComplexBoxMode", "false") == "true";
    setComplexLayerMode(impl::getValueFromConfig("isComplexLayerMode", "false") == "true");
    setComplexAnimationMode(impl::getValueFromConfig("isComplexAnimationMode", "false") == "true");

    mainConf = impl::configAsString();
    formDesignedObjConfig();
}

void formMainConfig(int width, int height, impl::GraphicsMode::Enum mode)
{
    Json::Value conf;
    Json::Reader r;
    r.parse(mainConf, conf);

    conf["workingPath"] = workDir;
    conf["designedObjectImagesPath"] = imagesDir;
    conf["width"] = width;
    conf["height"] = height;
    conf["mode"] = mode == impl::GraphicsMode::Window ? std::string("window") : std::string("fullscreen");
    conf["isBackupEnabled"] = isBackupEnabled;
    conf["isComplexBoxMode"] = isComplexBoxMode;
    conf["isComplexLayerMode"] = isComplexLayerMode();
    conf["isComplexAnimationMode"] = isComplexAnimationMode();

    Json::StyledWriter w;
    mainConf = w.write(conf);
}

void formDesignedObjConfig()
{
    Json::Value conf;
    Json::Reader r;
    r.parse(mainConf, conf);

    conf["imagesPath"] = imagesDir;

    Json::FastWriter w;
    designedObjConf = w.write(conf);
}

} } }

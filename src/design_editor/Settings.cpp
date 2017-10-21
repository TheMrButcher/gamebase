/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "Settings.h"
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

void init()
{
    isInterfaceExtended = impl::getValueFromConfig("interface", "basic") == "extended";
    workDir = impl::getValueFromConfig("workingPath", ".");
    imagesDir = impl::getValueFromConfig("designedObjectImagesPath", impl::getValueFromConfig("imagesPath"));
    isBackupEnabled = impl::getValueFromConfig("isBackupEnabled", "true") == "true";
    isComplexBoxMode = impl::getValueFromConfig("isComplexBoxMode", "false") == "true";

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

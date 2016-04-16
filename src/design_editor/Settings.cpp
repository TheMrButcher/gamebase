#include "Settings.h"
#include <gamebase/core/Core.h>
#include <gamebase/utils/StringUtils.h>
#include <json/reader.h>
#include <json/writer.h>

namespace gamebase { namespace editor { namespace settings {

bool isInterfaceExtended = false;
std::string workDir;
std::string imagesDir;
std::string mainConf;
std::string designedObjConf;
bool isBackupEnabled;

void init()
{
    isInterfaceExtended = getValueFromConfig("interface", "basic") == "extended";
    workDir = getValueFromConfig("workingPath", ".");
    imagesDir = getValueFromConfig("designedObjectImagesPath", getValueFromConfig("imagesPath"));
    isBackupEnabled = getValueFromConfig("isBackupEnabled", "true") == "true";

    mainConf = configAsString();
    formDesignedObjConfig();
}

void formMainConfig(int width, int height, GraphicsMode::Enum mode)
{
    Json::Value conf;
    Json::Reader r;
    r.parse(mainConf, conf);

    conf["workingPath"] = workDir;
    conf["designedObjectImagesPath"] = imagesDir;
    conf["width"] = width;
    conf["height"] = height;
    conf["mode"] = mode == GraphicsMode::Window ? std::string("window") : std::string("fullscreen");
    conf["isBackupEnabled"] = isBackupEnabled;

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

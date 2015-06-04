#include "TypePresentation.h"
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/JsonDeserializer.h>
#include <gamebase/utils/FileIO.h>

namespace gamebase { namespace editor {

std::shared_ptr<IObject> TypePresentation::loadPatternValue()
{
    if (isAbstract)
        return nullptr;
    std::string path;
    if (pathToPatternValue.empty()) {
        path = pathToDesign(std::string("pattern_values\\") + name + ".json");
    } else {
        if (isAbsolute(pathToPatternValue))
            path = pathToPatternValue;
        else
            pathToDesign(pathToPatternValue);
    }
    std::shared_ptr<IObject> result;
    try {
        deserializeFromJsonFile(path, result);
    } catch (std::exception& ex) {
        std::cout << "Error while deserializing object of type " << name << " from file " << path
            << ". Reason: " << ex.what() << std::endl;
        result.reset();
    }
    return result;
}

void TypePresentation::serialize(Serializer& s) const
{
    s << "name" << name << "nameInUI" << nameInUI << "isAbstract" << isAbstract
        << "parents" << parents << "properties" << properties
        << "pathToPatternValue" << pathToPatternValue;
}

std::unique_ptr<IObject> deserializeTypePresentation(Deserializer& deserializer)
{
    std::unique_ptr<TypePresentation> result(new TypePresentation());
    deserializer >> "name" >> result->name >> "nameInUI" >> result->nameInUI
        >> "isAbstract" >> result->isAbstract  >> "parents" >> result->parents
        >> "properties" >> result->properties >> "pathToPatternValue" >> result->pathToPatternValue;
    return std::move(result);
}

REGISTER_CLASS(TypePresentation);

} }

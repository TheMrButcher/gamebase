#pragma once

#include <gamebase/serial/ISerializable.h>
#include <string>
#include <map>

namespace gamebase { namespace editor {

class EnumPresentation : public ISerializable {
public:
    virtual void serialize(Serializer& serializer) const override;

    std::string name;
    std::string nameInUI;
    std::map<int, std::string> values;
};

} }

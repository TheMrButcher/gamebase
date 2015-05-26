#pragma once

#include "TreeView.h"
#include <gamebase/engine/ObjectsSelector.h>
#include <gamebase/engine/LinearLayout.h>
#include <gamebase/serial/ISerializer.h>

namespace gamebase { namespace editor {

class DesignViewBuilder : public ISerializer {
public:
    DesignViewBuilder(
        TreeView& treeView,
        ObjectsSelector& propertiesMenu,
        int rootID = TreeView::ROOT_ID);

    virtual void writeFloat(const std::string& name, float f) override;
    virtual void writeDouble(const std::string& name, double d) override;
    virtual void writeInt(const std::string& name, int i) override;
    virtual void writeBool(const std::string& name, bool b) override;
    virtual void writeString(const std::string& name, const std::string& value) override;
    virtual void startObject(const std::string& name) override;
    virtual void finishObject() override;
    virtual void startArray(const std::string& name, SerializationTag::Type tag) override;
    virtual void finishArray() override;

private:
    void addProperty(const std::string& name, const std::string& initialValue);

    struct ObjType {
        enum Enum {
            Unknown,
            Primitive,
            PrimitiveArray,
            Object,
            Array,
            Map
        };
    };

    TreeView& m_treeView;
    ObjectsSelector& m_propertiesMenu;
    std::vector<ObjType::Enum> m_objTypes;
    std::vector<SerializationTag::Type> m_arrayTypes;
    std::vector<int> m_nodeIDs;
    std::vector<LinearLayout*> m_properties;
    std::string m_curName;
    size_t m_primitiveElementIndex;
};

} }

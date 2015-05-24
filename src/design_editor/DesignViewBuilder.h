#pragma once

#include "TreeView.h"
#include <gamebase/serial/ISerializer.h>

namespace gamebase { namespace editor {

class DesignViewBuilder : public ISerializer {
public:
    DesignViewBuilder(TreeView& treeView, int rootID = TreeView::ROOT_ID);

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
    struct ObjType {
        enum Enum {
            Unknown,
            Primitive,
            Object,
            Array,
            Map
        };
    };

    TreeView& m_treeView;
    std::vector<ObjType::Enum> m_objTypes;
    std::vector<int> m_nodeIDs;
    std::string m_curName;
};

} }

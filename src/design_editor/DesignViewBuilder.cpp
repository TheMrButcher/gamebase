#include "DesignViewBuilder.h"
#include "tools.h"

namespace gamebase { namespace editor {

DesignViewBuilder::DesignViewBuilder(TreeView& treeView, int rootID)
    : m_treeView(treeView)
{
    m_nodeIDs.push_back(rootID);
}

void DesignViewBuilder::writeFloat(const std::string& name, float f)
{
}

void DesignViewBuilder::writeDouble(const std::string& name, double d)
{
}

void DesignViewBuilder::writeInt(const std::string& name, int i)
{
}

void DesignViewBuilder::writeBool(const std::string& name, bool b)
{
}

void DesignViewBuilder::writeString(const std::string& name, const std::string& value)
{
    if (name == TYPE_NAME_TAG) {
        m_objTypes.back() = ObjType::Object;
        std::string objName =
            (m_curName.empty() ? std::string("") : (m_curName + " : ")) + value;
        m_nodeIDs.push_back(m_treeView.addObject(
            m_nodeIDs.back(),
            createButton(200.f, 20.f, objName)));
    }
}

void DesignViewBuilder::startObject(const std::string& name)
{
    m_objTypes.push_back(ObjType::Unknown);
    m_curName = name;
}

void DesignViewBuilder::finishObject()
{
    if (m_objTypes.back() == ObjType::Object)
        m_nodeIDs.pop_back();
    m_objTypes.pop_back();
}

void DesignViewBuilder::startArray(const std::string& name, SerializationTag::Type tag)
{
}

void DesignViewBuilder::finishArray()
{
}

} }

/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <dvb/Snapshot.h>
#include <dvb/TypesList.h>
#include <reg/RegisterSwitcher.h>
#include <gamebase/Gamebase.h>
#include <gamebase/impl/serial/ISerializer.h>

namespace gamebase { namespace editor {

class DesignViewBuilder : public impl::ISerializer {
public:
    DesignViewBuilder(
        TreeView& treeView,
        Selector propertiesMenu,
        DesignModel& model,
        const std::shared_ptr<Presentation>& presentation,
        const std::shared_ptr<PropsMenuToolBar>& toolBar,
        Layout propertiesMenuArea,
        int rootID = TreeView::ROOT_ID);

    DesignViewBuilder(Snapshot& snapshot);

    ~DesignViewBuilder();

    virtual void writeFloat(const std::string& name, float f) override;
    virtual void writeDouble(const std::string& name, double d) override;
    virtual void writeInt(const std::string& name, int i) override;
    virtual void writeUInt(const std::string& name, unsigned int i) override;
    virtual void writeInt64(const std::string& name, int64_t i) override;
    virtual void writeUInt64(const std::string& name, uint64_t i) override;
    virtual void writeBool(const std::string& name, bool b) override;
    virtual void writeString(const std::string& name, const std::string& value) override;
    virtual void startObject(const std::string& name) override;
    virtual void finishObject() override;
    virtual void startArray(const std::string& name, impl::SerializationTag::Type tag) override;
    virtual void finishArray() override;

private:
    friend struct Snapshot;

    void addProperty(
        const std::string& name,
        const std::string& typeName,
        const std::string& initialValue,
        const std::function<void(TextBox, std::string, Json::Value*)>& updater);
    void addProperty(
        const std::string& name,
        const std::string& initialValue,
        const std::function<void(TextBox, std::string, Json::Value*)>& updater,
        Properties* properties);
    const IPropertyPresentation* presentationFromParent(const std::string& name) const;

    std::shared_ptr<Properties> createPropertiesImpl(int parentID, bool isInline = false);
    std::shared_ptr<Properties> createProperties(const std::string& name, const std::string& typeName);
    std::shared_ptr<Properties> currentPropertiesForPrimitive(const std::string& typeName);
    ObjType::Enum parentObjType() const;
    void createObjectCallbacks(int propsID);
    std::string propertyName(const std::string& nameFromSerializer);
    std::shared_ptr<Properties> currentProperties();
    void finishCurrentProperties();
    std::string propertyNameFromPresentation(const std::string& name);
    int addFictiveNode(
        const std::string& name,
        const IPropertyPresentation* propertyPresentation);
    TypesList createTypesList(
        const std::string& label,
        const IPropertyPresentation* propertyPresentation,
        const std::string& variantIfNoPresentation = std::string("No presentation"));
    void createObjectReplaceCallbacks(TypesList& typesList);
    void addStaticTypeLabel(Layout propertiesLayout, const std::string& typeName);
    bool isHidden() const { return m_levelOfHidden > 0; }

    std::shared_ptr<SharedContext> m_context;
    std::vector<ObjType::Enum> m_objTypes;
    std::vector<impl::SerializationTag::Type> m_arrayTypes;
    std::vector<std::shared_ptr<Properties>> m_properties;
    std::string m_curName;
    size_t m_primitiveElementIndex;
    int m_curModelNodeID;
    int m_levelOfHidden;
    std::vector<std::shared_ptr<MapProperties>> m_mapProperties;
    RegisterSwitcher regSwitch;
};

} }

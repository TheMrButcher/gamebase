/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <dvb/Snapshot.h>
#include <reg/RegisterSwitcher.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/drawobj/FilledRect.h>

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
        const std::shared_ptr<IProperty>& prop);
    void addProperty(
        Properties* properties,
        const std::string& name,
        const std::shared_ptr<IProperty>& prop);
    const IPropertyPresentation* presentationFromParent(const std::string& name) const;

    std::shared_ptr<Properties> createPropertiesImpl(int parentID, bool isInline = false);
    std::shared_ptr<Properties> createProperties(const std::string& name, const std::string& typeName);
    std::shared_ptr<Properties> currentPropertiesForPrimitive(
        const std::string& name, const std::string& typeName);
    ObjType::Enum parentObjType() const;
    void createObjectCallbacks(int propsID);
    std::string propertyName(const std::string& nameFromSerializer);
    std::shared_ptr<Properties> currentProperties();
    void finishCurrentProperties();
    std::string propertyNameFromPresentation(const std::string& name);
    IProperty* addFictiveProperty(
        const std::string& name,
        const IPropertyPresentation* propertyPresentation);
    void addStaticTypeLabel(Properties* props, const std::string& typeName);
    bool isHidden() const { return m_levelOfHidden > 0; }
	void chooseColor(FilledRect colorRect);
    void setupProperty(
        Properties* props, const std::string& name, const std::shared_ptr<IProperty>& prop);
    void setupClassNameProperty(
        const std::shared_ptr<Properties>& props,
        const std::shared_ptr<IClassNameProperty>& classNameProperty);
    void createObjectReplaceCallbacks(
        const std::shared_ptr<Properties>& props,
        IClassNameProperty* classNameProperty);

    std::shared_ptr<SharedContext> m_context;
    std::vector<ObjType::Enum> m_objTypes;
    std::vector<impl::SerializationTag::Type> m_arrayTypes;
    std::vector<std::shared_ptr<Properties>> m_properties;
    std::string m_curName;
    size_t m_primitiveElementIndex;
    int m_curModelNodeID;
    int m_levelOfHidden;
    RegisterSwitcher regSwitch;
};

} }

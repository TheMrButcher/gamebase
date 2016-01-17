#pragma once

#include "TreeView.h"
#include "DesignModel.h"
#include "Presentation.h"
#include "PropsMenuToolBar.h"
#include <gamebase/engine/ObjectsSelector.h>
#include <gamebase/engine/LinearLayout.h>
#include <gamebase/serial/ISerializer.h>

namespace gamebase {

class TextBox;
class RadioButtonGroup;
class StaticLabel;
class ComboBox;
class ClickableTextCheckBoxSkin;
class ScrollableArea;
    
namespace editor {

class DesignViewBuilder : public ISerializer {
public:
    DesignViewBuilder(
        TreeView& treeView,
        ObjectsSelector& propertiesMenu,
        DesignModel& model,
        const std::shared_ptr<Presentation>& presentation,
        const std::shared_ptr<PropsMenuToolBar>& toolBar,
        ScrollableArea* propertiesMenuArea,
        int rootID = TreeView::ROOT_ID);

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
    virtual void startArray(const std::string& name, SerializationTag::Type tag) override;
    virtual void finishArray() override;

public:
    struct Properties {
        Properties()
            : id(-1)
            , switchButtonSkin(nullptr)
            , layout(nullptr)
            , type(nullptr)
            , presentationFromParent(nullptr)
            , keyPresentationFromParent(nullptr)
        {}

        int id;
        ClickableTextCheckBoxSkin* switchButtonSkin;
        LinearLayout* layout;
        const TypePresentation* type;
        const IPropertyPresentation* presentationFromParent;
        const IIndexablePropertyPresentation* keyPresentationFromParent;
        std::function<void()> buttonTextUpdater;
        std::shared_ptr<int> collectionSize;
    };

    struct ObjType {
        enum Enum {
            Unknown,
            PrimitiveArray,
            Object,
            Array,
            Map,
            FictiveObject
        };
    };

    struct MapProperties {
        MapProperties() : currentElem(0) {}

        struct Element {
            Element() : properties(nullptr), keyNodeID(-1) {}

            Element(
                const std::shared_ptr<Properties>& properties,
                int keyNodeID)
                : properties(properties)
                , keyNodeID(keyNodeID)
            {}

            std::shared_ptr<Properties> properties;
            int keyNodeID;
        };
        std::vector<Element> elements;
        size_t currentElem;
        int keysArrayNodeID;
    };

    struct Node {
        std::map<ButtonKey::Enum, std::function<void()>> callbacks;
    };

    struct SharedContext {
        SharedContext(
            TreeView& treeView,
            ObjectsSelector& propertiesMenu,
            DesignModel& model)
            : treeView(treeView)
            , propertiesMenu(propertiesMenu)
            , model(model)
        {}

        void select(int id);

        TreeView& treeView;
        ObjectsSelector& propertiesMenu;
        DesignModel& model;
        std::shared_ptr<Presentation> presentation;
        std::shared_ptr<RadioButtonGroup> switchsGroup;
        std::shared_ptr<PropsMenuToolBar> toolBar;
        ScrollableArea* propertiesMenuArea;
        std::unordered_map<int, Node> nodes;
    };

    struct Snapshot {
        Snapshot(DesignViewBuilder& builder, const Properties& properties, ObjType::Enum objType);

        std::shared_ptr<SharedContext> context;
        
        std::string curName;
        int modelNodeID;
        std::shared_ptr<Properties> properties;
        ObjType::Enum objType;
        boost::optional<SerializationTag::Type> arrayType;
        std::shared_ptr<MapProperties> mapProperties;
    };

    struct TypesList {
        ComboBox* comboBox;
        std::vector<const TypePresentation*> types;
        size_t indexInLayout;
    };

    DesignViewBuilder(Snapshot& snapshot);

private:
    void addProperty(
        const std::string& name,
        const std::string& typeName,
        const std::string& initialValue,
        const std::function<void(TextBox*, std::string, Json::Value*)>& updater);

    void addProperty(
        const std::string& name,
        const std::string& initialValue,
        const std::function<void(TextBox*, std::string, Json::Value*)>& updater,
        Properties* properties);
    
    std::shared_ptr<Properties> createPropertiesImpl(int parentID);
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

    void createObjectReplaceCallbacks(const TypesList& typesList);

    void addStaticTypeLabel(LinearLayout* propertiesLayout, const std::string& typeName);

    std::shared_ptr<SharedContext> m_context;

    std::vector<ObjType::Enum> m_objTypes;
    std::vector<SerializationTag::Type> m_arrayTypes;
    std::vector<std::shared_ptr<Properties>> m_properties;
    std::string m_curName;
    size_t m_primitiveElementIndex;
    int m_curModelNodeID;
    std::vector<std::shared_ptr<MapProperties>> m_mapProperties;
};

} }

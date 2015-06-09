#pragma once

#include "TreeView.h"
#include "DesignModel.h"
#include "Presentation.h"
#include <gamebase/engine/ObjectsSelector.h>
#include <gamebase/engine/LinearLayout.h>
#include <gamebase/serial/ISerializer.h>

namespace gamebase {

class TextEdit;
class RadioButtonGroup;
class StaticLabel;
class TextList;
    
namespace editor {

class DesignViewBuilder : public ISerializer {
public:
    DesignViewBuilder(
        TreeView& treeView,
        ObjectsSelector& propertiesMenu,
        DesignModel& model,
        const std::shared_ptr<Presentation>& presentation,
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
            , switchButtonLabel(nullptr)
            , layout(nullptr)
            , type(nullptr)
            , presentationFromParent(nullptr)
            , keyPresentationFromParent(nullptr)
        {}

        ~Properties()
        {
            if (buttonTextUpdater)
                buttonTextUpdater();
        }

        int id;
        StaticLabel* switchButtonLabel;
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

        std::vector<std::shared_ptr<Properties>> elements;
        size_t currentElem;
        int keysArrayNodeID;
    };

    struct Snapshot {
        Snapshot(DesignViewBuilder& builder, const Properties& properties, ObjType::Enum objType);

        TreeView& treeView;
        ObjectsSelector& propertiesMenu;
        DesignModel& model;
        std::shared_ptr<Presentation> presentation;
        std::string curName;
        int modelNodeID;
        std::shared_ptr<RadioButtonGroup> switchsGroup;

        std::shared_ptr<Properties> properties;
        ObjType::Enum objType;
        boost::optional<SerializationTag::Type> arrayType;
        std::shared_ptr<MapProperties> mapProperties;
    };

    struct TypesList {
        TextList* textList;
        std::vector<const TypePresentation*> types;
        size_t indexInLayout;
    };

    DesignViewBuilder(Snapshot& snapshot);

private:
    void addProperty(
        const std::string& name,
        const std::string& typeName,
        const std::string& initialValue,
        const std::function<void(TextEdit*, std::string, Json::Value*)>& updater);

    void addProperty(
        const std::string& name,
        const std::string& initialValue,
        const std::function<void(TextEdit*, std::string, Json::Value*)>& updater,
        LinearLayout* propertiesLayout);
    
    std::shared_ptr<Properties> createPropertiesImpl(int parentID);
    std::shared_ptr<Properties> createProperties(const std::string& name, const std::string& typeName);
    std::shared_ptr<Properties> currentPropertiesForPrimitive(const std::string& typeName);
    ObjType::Enum parentObjType() const;
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

    TreeView& m_treeView;
    ObjectsSelector& m_propertiesMenu;
    DesignModel& m_model;
    std::shared_ptr<Presentation> m_presentation;
    std::vector<ObjType::Enum> m_objTypes;
    std::vector<SerializationTag::Type> m_arrayTypes;
    std::vector<std::shared_ptr<Properties>> m_properties;
    std::string m_curName;
    size_t m_primitiveElementIndex;
    int m_curModelNodeID;

    std::vector<std::shared_ptr<MapProperties>> m_mapProperties;
    std::shared_ptr<RadioButtonGroup> m_switchsGroup;
};

} }

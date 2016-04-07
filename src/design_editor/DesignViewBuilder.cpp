#include "DesignViewBuilder.h"
#include "tools.h"
#include "Settings.h"
#include <gamebase/engine/RadioButton.h>
#include <gamebase/engine/ClickableTextCheckBoxSkin.h>
#include <gamebase/engine/StaticLabel.h>
#include <gamebase/engine/ComboBox.h>
#include <gamebase/engine/ScrollableArea.h>
#include <gamebase/serial/JsonDeserializer.h>
#include <gamebase/utils/StringUtils.h>
#include <json/value.h>
#include <boost/lexical_cast.hpp>
#include <fstream>

namespace gamebase { namespace editor {

namespace {
std::shared_ptr<LinearLayout> createPropertyLayout()
{
    return deserialize<LinearLayout>("ui\\PropertyLayout.json");
}

std::shared_ptr<StaticLabel> createLabel(const std::string& text)
{
    auto label = deserialize<StaticLabel>("ui\\PropertyLabel.json");
    label->setText(text + ": ");
    return label;
}

std::shared_ptr<StaticLabel> createRightLabel(const std::string& text)
{
    auto label = deserialize<StaticLabel>("ui\\PropertyRightLabel.json");
    label->setText(text);
    return label;
}

std::shared_ptr<TextBox> createTextBox()
{
    return deserialize<TextBox>("ui\\PropertyTextBox.json");
}

std::shared_ptr<ComboBox> createComboBox(
    const std::vector<std::string>& variants,
    const std::vector<int>& indices = std::vector<int>())
{
    auto comboBox = deserialize<ComboBox>("ui\\PropertyComboBox.json");
    auto itIndex = indices.begin();
    for (auto itVariant = variants.begin(); itVariant != variants.end(); ++itVariant) {
        const auto& text = *itVariant;
        auto button = deserialize<Button>("ui\\ComboBoxButton.json");
        button->getChild<StaticLabel>("#label")->setText(text);
        if (itIndex == indices.end())
            comboBox->addButton(text, button);
        else
            comboBox->addButton(text, button, *itIndex++);
    }
    return comboBox;
}

class PrimitiveArrayElementSuffix {
public:
    PrimitiveArrayElementSuffix()
    {
        m_suffixes[SerializationTag::Vec2].push_back(".x");
        m_suffixes[SerializationTag::Vec2].push_back(".y");

        m_suffixes[SerializationTag::Matrix2].push_back("[0, 0]");
        m_suffixes[SerializationTag::Matrix2].push_back("[0, 1]");
        m_suffixes[SerializationTag::Matrix2].push_back("[1, 0]");
        m_suffixes[SerializationTag::Matrix2].push_back("[1, 1]");

        m_suffixes[SerializationTag::Transform2].push_back(".m[0, 0]");
        m_suffixes[SerializationTag::Transform2].push_back(".m[0, 1]");
        m_suffixes[SerializationTag::Transform2].push_back(".m[1, 0]");
        m_suffixes[SerializationTag::Transform2].push_back(".m[1, 1]");
        m_suffixes[SerializationTag::Transform2].push_back(".tx");
        m_suffixes[SerializationTag::Transform2].push_back(".ty");

        m_suffixes[SerializationTag::BoundingBox].push_back(".left");
        m_suffixes[SerializationTag::BoundingBox].push_back(".bottom");
        m_suffixes[SerializationTag::BoundingBox].push_back(".right");
        m_suffixes[SerializationTag::BoundingBox].push_back(".top");

        m_suffixes[SerializationTag::Color].push_back(".r");
        m_suffixes[SerializationTag::Color].push_back(".g");
        m_suffixes[SerializationTag::Color].push_back(".b");
        m_suffixes[SerializationTag::Color].push_back(".a");
    }

    const std::string& get(SerializationTag::Type tag, size_t index) const
    {
        return m_suffixes.at(tag).at(index);
    }

private:
    std::map<SerializationTag::Type, std::vector<std::string>> m_suffixes;
};

const PrimitiveArrayElementSuffix PRIMITIVE_ARRAY_ELEMENT_SUFFIX;

std::string mergeStrings(const std::string& str1, const std::string& str2)
{
    bool bothNotEmpty = !str1.empty() && !str2.empty();
    return str1 + (bothNotEmpty ? std::string(" : ") : "") + str2;
}

template <typename T>
void setData(Json::Value* data, std::string name, const T& value)
{
    if (data->isArray())
        data->append(Json::Value(value));
    else
        (*data)[name] = Json::Value(value);
}

template <typename T>
void setDataFromString(Json::Value* data, std::string name, const std::string& valueStr)
{
    T value;
    try {
        value = boost::lexical_cast<T>(valueStr);
    } catch (boost::bad_lexical_cast& ex) {
        THROW_EX() << "Can't cast " << valueStr << " to type " << typeid(T).name()
            << ". Reason: " << ex.what();
    }
    setData(data, name, value);
}

template <typename T>
void updateProperty(TextBox* textBox, std::string name, Json::Value* data)
{
    setDataFromString<T>(data, name, textBox->text());
}

template <typename T>
void constUpdater(const std::string& name, const T& t, Json::Value* data)
{
    (*data)[name] = Json::Value(t);
}

template <typename T>
DesignModel::UpdateModelFunc createConstUpdater(
    const std::string& name, const T& t)
{
    DesignModel::UpdateModelFunc result =
        std::bind(&constUpdater<T>, name, t, std::placeholders::_1);
    return result;
}

void collectionSizeUpdater(std::shared_ptr<int> sharedSize, Json::Value* data)
{
    (*data)[COLLECTION_SIZE_TAG] = Json::Value(*sharedSize);
}

std::string extractText(LinearLayout* propertiesLayout, size_t index)
{
    if (propertiesLayout->objects().size() <= index)
        return std::string();
    LinearLayout* layout = dynamic_cast<LinearLayout*>(propertiesLayout->objects()[index].get());
    if (!layout || layout->objects().size() < 2)
        return std::string();
    IObject* sourceObj = layout->objects()[1].get();
    if (TextBox* textBox = dynamic_cast<TextBox*>(sourceObj))
        return textBox->text();
    if (ComboBox* comboBox = dynamic_cast<ComboBox*>(sourceObj))
        return comboBox->text();
    if (StaticLabel* label = dynamic_cast<StaticLabel*>(sourceObj))
        return label->text();
    return std::string();
}

void nameForPresentationSetter(ClickableTextCheckBoxSkin* skin, LinearLayout* propertiesLayout)
{
    if (propertiesLayout->objects().size() < 3)
        return;
    auto text = extractText(propertiesLayout, 2);
    if (text.empty())
        text = extractText(propertiesLayout, 1);
    skin->setText(text);
}

void nameFromPropertiesSetter(
    ClickableTextCheckBoxSkin* skin, LinearLayout* propertiesLayout,
    const std::string& prefix, size_t sourceIndex)
{
    if (propertiesLayout->objects().size() < sourceIndex + 1)
        return;
    skin->setText(mergeStrings(
        prefix, extractText(propertiesLayout, sourceIndex)));
}

void mapElementNameFromPropertiesSetter(ClickableTextCheckBoxSkin* skin, LinearLayout* propertiesLayout)
{
    if (propertiesLayout->objects().size() < 2)
        return;
    skin->setText(extractText(propertiesLayout, 0) + " => " + extractText(propertiesLayout, 1));
}

void updateBoolProperty(ComboBox* comboBox, std::string name, Json::Value* data)
{
    setData(data, name, comboBox->text() == "true" ? true : false);
}

void updateEnumProperty(ComboBox* comboBox, std::string name, Json::Value* data)
{
    setData(data, name, comboBox->currentVariantID());
}

void updateTypeTag(const DesignViewBuilder::TypesList& typesList, Json::Value* data)
{
    auto id = typesList.comboBox->currentVariantID();
    if (id < static_cast<int>(typesList.types.size()) && (id >= 0 || typesList.comboBox->name() != "None")) {
        if (id < 0) {
            (*data)[TYPE_NAME_TAG] = Json::Value(typesList.comboBox->text());
        } else {
            (*data)[TYPE_NAME_TAG] = Json::Value(typesList.types[id]->name);
        }
    } else {
        if (data->isMember(TYPE_NAME_TAG))
            data->removeMember(TYPE_NAME_TAG);
    }
}

void updateEmptyTag(const DesignViewBuilder::TypesList& typesList, Json::Value* data)
{
    auto id = typesList.comboBox->currentVariantID();
    (*data)[EMPTY_TAG] = Json::Value(
        id >= static_cast<int>(typesList.types.size()) || (id < 0 && typesList.comboBox->name() == "None"));
}
                
void serializeDefaultValue(Serializer& serializer, const std::string& name,
    const std::shared_ptr<Presentation>& presentation,
    const IPropertyPresentation* propertyPresentation)
{
    if (propertyPresentation->presentationType() == PropertyPresentation::Primitive) {
        switch (dynamic_cast<const PrimitivePropertyPresentation*>(propertyPresentation)->type) {
            case PrimitiveType::Float: serializer << name << 0.0f; break;
            case PrimitiveType::Double: serializer << name << 0.0; break;
            case PrimitiveType::Int: serializer << name << int(0); break;
            case PrimitiveType::UInt: serializer << name << unsigned int(0); break;
            case PrimitiveType::Int64: serializer << name << int64_t(0); break;
            case PrimitiveType::UInt64: serializer << name << uint64_t(0); break;
            case PrimitiveType::Bool: serializer << name << false; break;
            case PrimitiveType::String: serializer << name << std::string(); break;
        }
        return;
    }
    if (propertyPresentation->presentationType() == PropertyPresentation::Enum) {
        auto enumPropertyPresentation = dynamic_cast<const EnumPropertyPresentation*>(propertyPresentation);
        if (auto enumPresentation = presentation->enumByName(enumPropertyPresentation->type)) {
            if (enumPresentation->values.empty())
                return;
            serializer << name << enumPresentation->values.begin()->first;
        }
    }
}

void addPrimitiveValueFromSource(
    int sourceID, const std::string& name, const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot,
    const IIndexablePropertyPresentation* presentation)
{
    DesignViewBuilder builder(*snapshot);
    Serializer serializer(&builder);
    auto fictiveData = snapshot->context->model.toJsonValue(sourceID);
    if (!fictiveData->isMember(name))
        return;
    const auto& sourceData = (*fictiveData)[name];
    if (presentation) {
        PrimitiveType::Enum type = PrimitiveType::Int;
        if (auto primitivePropertyPresentation = dynamic_cast<const PrimitivePropertyPresentation*>(presentation))
            type = primitivePropertyPresentation->type;
        switch (type) {
        case PrimitiveType::Float:
        case PrimitiveType::Double:
            if (!sourceData.isDouble())
                THROW_EX() << "Wrong type of key, expected double";
            serializer << "" << sourceData.asDouble();
            break;

        case PrimitiveType::Int:
            if (!sourceData.isInt())
                THROW_EX() << "Wrong type of key, expected integer";
            serializer << "" << sourceData.asInt();
            break;

        case PrimitiveType::UInt:
            if (!sourceData.isUInt())
                THROW_EX() << "Wrong type of key, expected unsigned integer";
            serializer << "" << sourceData.asUInt();
            break;

        case PrimitiveType::Int64:
            if (!sourceData.isInt64())
                THROW_EX() << "Wrong type of key, expected integer64";
            serializer << "" << sourceData.asInt64();
            break;

        case PrimitiveType::UInt64:
            if (!sourceData.isUInt64())
                THROW_EX() << "Wrong type of key, expected unsigned integer64";
            serializer << "" << sourceData.asUInt64();
            break;

        case PrimitiveType::Bool:
            if (!sourceData.isBool())
                THROW_EX() << "Wrong type of key, expected boolean";
            serializer << "" << sourceData.asBool();
            break;

        case PrimitiveType::String:
            serializer << "" << sourceData.asString();
            break;
        }
    } else {
        if (sourceData.isDouble())
            serializer << "" << sourceData.asDouble();
        else if (sourceData.isInt())
            serializer << "" << sourceData.asInt();
        else if (sourceData.isUInt())
            serializer << "" << sourceData.asUInt();
        else if (sourceData.isInt64())
            serializer << "" << sourceData.asInt64();
        else if (sourceData.isUInt64())
            serializer << "" << sourceData.asUInt64();
        else if (sourceData.isBool())
            serializer << "" << sourceData.asBool();
        else
            serializer << "" << sourceData.asString();
    }
}

void addObject(
    const std::shared_ptr<IObject>& obj,
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot)
{
    DesignViewBuilder builder(*snapshot);
    Serializer serializer(&builder);
    serializer << "" << obj;
}

void addObjectFromPattern(
    ComboBox* comboBox,
    const std::vector<const TypePresentation*>& types,
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot)
{
    auto id = comboBox->currentVariantID();
    std::shared_ptr<IObject> obj;
    if (id < 0 || id >= static_cast<int>(types.size())) {
        std::cerr << "Wrong index of type: " << id << ". Adding empty object";
    } else {
        obj = snapshot->context->presentation->loadPattern(types[id]->name);
    }
    addObject(obj, snapshot);
}

void addObjectFromFile(
    const std::string& pathToFile,
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot)
{
    std::shared_ptr<IObject> obj;
    deserializeFromJsonFile(addSlash(settings::workDir) + pathToFile, obj);
    addObject(obj, snapshot);
}

void updateView(TreeView* view)
{
    view->countBoxes();
    view->loadResources();
}

void updateView(const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot)
{
    updateView(&snapshot->context->treeView);
}

void updateView(
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot,
    int propsID)
{
    updateView(snapshot);
    auto& switchsGroup = *snapshot->context->switchsGroup;
    if (!switchsGroup.isAnySelected() || switchsGroup.selected() != propsID)
        dynamic_cast<RadioButton*>(snapshot->context->treeView.getObject(propsID).get())->setChecked();
    else
        snapshot->context->select(propsID);
}

void addPrimitiveValueToArray(
    int sourceID, const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot)
{
    const IIndexablePropertyPresentation* elementPresentation = 0;
    if (auto arrayPresentation = dynamic_cast<const ArrayPresentation*>(snapshot->properties->presentationFromParent))
        elementPresentation = dynamic_cast<const IIndexablePropertyPresentation*>(arrayPresentation->elementType.get());
    addPrimitiveValueFromSource(sourceID, "newElement", snapshot, elementPresentation);
    updateView(snapshot);
}

void addObjectToArray(
    ComboBox* comboBox,
    const std::vector<const TypePresentation*>& types,
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot)
{
    addObjectFromPattern(comboBox, types, snapshot);
    updateView(snapshot);
}

void addObjectFromFileToArray(
    const std::string& pathToFile,
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot)
{
    addObjectFromFile(pathToFile, snapshot);
    updateView(snapshot);
}

void addObjectFromClipboardToArray(
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot)
{
    std::shared_ptr<IObject> obj;
    deserializeFromJson(g_clipboard, obj);
    addObject(obj, snapshot);
    updateView(snapshot);
}

void addElementToMap(
    int keySourceID, int keysArrayNodeID, int valuesArrayNodeID,
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot,
    const std::function<void()>& addValueFunc)
{
    snapshot->mapProperties = std::make_shared<DesignViewBuilder::MapProperties>();
    snapshot->mapProperties->elements.resize(*snapshot->properties->collectionSize);
    snapshot->arrayType = SerializationTag::Keys;
    snapshot->modelNodeID = keysArrayNodeID;

    const IIndexablePropertyPresentation* keyPresentation = 0;
    if (auto mapPresentation = dynamic_cast<const MapPresentation*>(snapshot->properties->presentationFromParent))
        keyPresentation = dynamic_cast<const IIndexablePropertyPresentation*>(mapPresentation->keyType.get());
    addPrimitiveValueFromSource(keySourceID, "newKey", snapshot, keyPresentation);
    
    snapshot->mapProperties->currentElem = *snapshot->properties->collectionSize - 1;
    snapshot->arrayType = SerializationTag::Values;
    snapshot->modelNodeID = valuesArrayNodeID;
    addValueFunc();
    
    snapshot->mapProperties.reset();
    updateView(snapshot);
}

void addPrimitiveElementToMap(
    int keySourceID, int valueSourceID,
    int keysArrayNodeID, int valuesArrayNodeID,
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot)
{
    const IIndexablePropertyPresentation* valuePresentation = 0;
    if (auto mapPresentation = dynamic_cast<const MapPresentation*>(snapshot->properties->presentationFromParent))
        valuePresentation = dynamic_cast<const IIndexablePropertyPresentation*>(mapPresentation->valueType.get());
    addElementToMap(keySourceID, keysArrayNodeID, valuesArrayNodeID, snapshot,
        std::bind(addPrimitiveValueFromSource, valueSourceID, "newValue", snapshot, valuePresentation));
}

void addObjectToMap(
    int keySourceID,  int keysArrayNodeID, int valuesArrayNodeID,
    ComboBox* comboBox,
    const std::vector<const TypePresentation*>& types,
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot)
{
    addElementToMap(keySourceID, keysArrayNodeID, valuesArrayNodeID, snapshot,
        std::bind(addObjectFromPattern, comboBox, types, snapshot));
}

void addObjectFromFileToMap(
    int keySourceID,  int keysArrayNodeID, int valuesArrayNodeID,
    const std::string& pathToFile,
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot)
{
    std::shared_ptr<IObject> obj;
    deserializeFromJsonFile(addSlash(settings::workDir) + pathToFile, obj);
    addElementToMap(keySourceID, keysArrayNodeID, valuesArrayNodeID, snapshot,
        std::bind(addObject, obj, snapshot));
}

void addObjectFromClipboardToMap(
    int keySourceID,  int keysArrayNodeID, int valuesArrayNodeID,
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot)
{
    std::shared_ptr<IObject> obj;
    deserializeFromJson(g_clipboard, obj);
    addElementToMap(keySourceID, keysArrayNodeID, valuesArrayNodeID, snapshot,
        std::bind(addObject, obj, snapshot));
}

void replaceObjectWith(
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot,
    size_t updatersToSaveNum,
    size_t fieldsInLayoutToSave,
    const std::shared_ptr<IObject>& obj)
{
    auto& context = *snapshot->context;
    auto& node = context.model.get(snapshot->modelNodeID);
    auto children = node.children();
    std::vector<DesignModel::UpdateModelFunc> updatersToSave;
    for (auto it = children.begin(); updatersToSave.size() < updatersToSaveNum && it != children.end(); ++it) {
        if (it->type == DesignModel::Node::Element::Updater)
            updatersToSave.push_back(it->updater);
    }
    context.model.clearNode(snapshot->modelNodeID);
    for (auto it = updatersToSave.begin(); it != updatersToSave.end(); ++it)
        context.model.addUpdater(snapshot->modelNodeID, *it);

    const auto& objects = snapshot->properties->layout->objects();
    std::vector<std::shared_ptr<IObject>> objectsToSave(
        objects.begin(), objects.begin() + std::min(objects.size(), fieldsInLayoutToSave));
    snapshot->properties->layout->clear();
    for (auto it = objectsToSave.begin(); it != objectsToSave.end(); ++it)
        snapshot->properties->layout->addObject(*it);
    context.treeView.removeChildren(snapshot->properties->id);

    if (obj) {
        DesignViewBuilder builder(*snapshot);
        try {
            Serializer objectSerializer(&builder);
            if (const ISerializable* serObj = dynamic_cast<const ISerializable*>(obj.get())) {
                serObj->serialize(objectSerializer);
            } else {
                THROW_EX() << "Object with type_index=" << typeid(*obj).name() << " is not serializable";
            }
            if (const IRegistrable* regObj = dynamic_cast<const IRegistrable*>(obj.get())) {
                objectSerializer << REG_NAME_TAG << regObj->name();
            }
            if (const IDrawable* drawObj = dynamic_cast<const IDrawable*>(obj.get())) {
                objectSerializer << VISIBLE_TAG << drawObj->isVisible();
            }
        } catch (std::exception& ex) {
            std::cout << "Error while replacing (serialization step) object. Reason: " << ex.what() << std::endl;
        }
    }
    snapshot->properties->buttonTextUpdater();
    updateView(snapshot, snapshot->properties->id);
}

void replaceObjectWithPattern(
    const DesignViewBuilder::TypesList& typesList,
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot,
    size_t updatersToSaveNum,
    ComboBox* comboBox)
{
    const auto& typeName = comboBox->text();
    int variantID = comboBox->currentVariantID();

    std::shared_ptr<IObject> obj;
    if (variantID >= 0 && variantID < static_cast<int>(typesList.types.size())) {
        obj = snapshot->context->presentation->loadPattern(typesList.types[variantID]->name);
        snapshot->properties->type = typesList.types[variantID];
    } else {
        snapshot->properties->type = nullptr;
    }

    replaceObjectWith(snapshot, updatersToSaveNum, typesList.indexInLayout + 1, obj);
}

void removeArrayElement(const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot)
{
    auto& context = *snapshot->context;
    auto& props = *snapshot->properties;
    context.model.remove(snapshot->modelNodeID);
    context.treeView.removeSubtree(props.id);
    context.propertiesMenu.removeObject(props.id);
    context.nodes.erase(props.id);
    context.toolBar->clear();
    --(*props.collectionSize);
    updateView(snapshot);
    snapshot->context->propertiesMenuArea->update();
}

void removeMapElement(
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot,
    int keyNodeID)
{
    snapshot->context->model.remove(keyNodeID);
    removeArrayElement(snapshot);
}

void replaceMember(
    const std::shared_ptr<IObject>& obj,
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot,
    int oldNodeID,
    int oldPropsID)
{
    auto& context = *snapshot->context;
    auto& oldNode = context.model.get(oldNodeID);
    auto parentNodeID = context.model.get(oldNodeID).parentID;
    auto nameInParent = oldNode.nameInParent;
    auto newPropertiesID = context.treeView.nextID();

    DesignViewBuilder builder(*snapshot);
    Serializer serializer(&builder);
    serializer << nameInParent << obj;

    context.model.remove(oldNodeID);
    context.treeView.swapInParents(oldPropsID, newPropertiesID);
    context.treeView.removeSubtree(oldPropsID);
    context.propertiesMenu.removeObject(oldPropsID);
    context.nodes.erase(oldPropsID);

    updateView(snapshot, newPropertiesID);
}

void replaceMemberFromFile(
    const std::string& fileName,
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot,
    int oldNodeID,
    int oldPropsID)
{
    std::shared_ptr<IObject> obj;
    deserializeFromJsonFile(addSlash(settings::workDir) + fileName, obj);
    replaceMember(obj, snapshot, oldNodeID, oldPropsID);
}

void pasteMember(
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot,
    int oldNodeID,
    int oldPropsID)
{
    std::shared_ptr<IObject> obj;
    deserializeFromJson(g_clipboard, obj);
    replaceMember(obj, snapshot, oldNodeID, oldPropsID);
}

void replaceArrayElement(
    const std::shared_ptr<IObject>& obj,
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot,
    int oldNodeID,
    int oldPropsID)
{
    auto& context = *snapshot->context;
    auto& callbacks = context.nodes[oldPropsID].callbacks;
    auto it = callbacks.find(ButtonKey::Remove);
    if (it == callbacks.end())
        THROW_EX() << "Can't replace element, it's not removable";
    auto remover = it->second;

    int newNodeID = context.model.nextID();
    int newPropsID = context.treeView.nextID();

    addObject(obj, snapshot);

    auto& parent = context.model.get(snapshot->modelNodeID);
    parent.swap(oldNodeID, newNodeID);
    context.treeView.swapInParents(oldPropsID, newPropsID);
    remover();

    updateView(snapshot, newPropsID);
}

void replaceArrayElementFromFile(
    const std::string& fileName,
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot,
    int oldNodeID,
    int oldPropsID)
{
    std::shared_ptr<IObject> obj;
    deserializeFromJsonFile(addSlash(settings::workDir) + fileName, obj);
    replaceArrayElement(obj, snapshot, oldNodeID, oldPropsID);
}

void pasteArrayElement(
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot,
    int oldNodeID,
    int oldPropsID)
{
    std::shared_ptr<IObject> obj;
    deserializeFromJson(g_clipboard, obj);
    replaceArrayElement(obj, snapshot, oldNodeID, oldPropsID);
}

void replaceMapElement(
    const std::shared_ptr<IObject>& obj,
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot,
    int oldNodeID)
{
    auto& context = *snapshot->context;
    
    auto& props = *snapshot->mapProperties->elements.front().properties;
    context.treeView.removeChildren(props.id);
    auto keyLayout = props.layout->objects().front();
    props.layout->clear();
    props.layout->addObject(keyLayout);

    int newNodeID = context.model.nextID();
    snapshot->mapProperties->currentElem = 0;
    addObject(obj, snapshot);

    auto& parent = context.model.get(snapshot->modelNodeID);
    parent.swap(oldNodeID, newNodeID);
    parent.remove(oldNodeID);

    updateView(snapshot, props.id);
}

void replaceMapElementFromFile(
    const std::string& fileName,
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot,
    int oldNodeID)
{
    std::shared_ptr<IObject> obj;
    deserializeFromJsonFile(addSlash(settings::workDir) + fileName, obj);
    replaceMapElement(obj, snapshot, oldNodeID);
}

void pasteMapElement(
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot,
    int oldNodeID)
{
    std::shared_ptr<IObject> obj;
    deserializeFromJson(g_clipboard, obj);
    replaceMapElement(obj, snapshot, oldNodeID);
}

void moveArrayElementUp(
    DesignModel* model,
    TreeView* treeView,
    int nodeID,
    int propsID)
{
    auto& parentNode = model->get(model->get(nodeID).parentID);
    int index = parentNode.position(nodeID);
    if (index - 1 < 0)
        return;
    parentNode.swap(nodeID, parentNode.children()[index - 1].id);

    int treeParentID = treeView->parentID(propsID);
    const auto& treeChildren = treeView->children(treeParentID);
    if (treeChildren.size() != parentNode.children().size() || treeChildren[index] != propsID)
        THROW_EX() << "Detected inconsistent states of Model and View";
    treeView->swapInParents(propsID, treeChildren[index - 1]);
    updateView(treeView);
}

void moveArrayElementDown(
    DesignModel* model,
    TreeView* treeView,
    int nodeID,
    int propsID)
{
    auto& parentNode = model->get(model->get(nodeID).parentID);
    int index = parentNode.position(nodeID);
    if (index < 0)
        return;
    if (static_cast<size_t>(index + 1) >= parentNode.children().size())
        return;
    parentNode.swap(nodeID, parentNode.children()[index + 1].id);

    int treeParentID = treeView->parentID(propsID);
    const auto& treeChildren = treeView->children(treeParentID);
    if (treeChildren.size() != parentNode.children().size() || treeChildren[index] != propsID)
        THROW_EX() << "Detected inconsistent states of Model and View";
    treeView->swapInParents(propsID, treeChildren[index + 1]);
    updateView(treeView);
}

void saveNode(
    DesignModel* model,
    int nodeID,
    const std::string& fileName)
{
    auto jsonStr = model->toString(nodeID, JsonFormat::Styled);
    std::ofstream outputFile(addSlash(settings::workDir) + fileName);
    outputFile << jsonStr;
}

void copyNode(
    DesignModel* model,
    int nodeID)
{
    g_clipboard = model->toString(nodeID, JsonFormat::Fast);
}
}

void DesignViewBuilder::SharedContext::select(int id)
{
    propertiesMenu.select(id);
    toolBar->clear();
    auto it = nodes.find(id);
    if (it != nodes.end()) {
        const auto& buttonCallbacks = it->second.callbacks;
        for (auto itButton = buttonCallbacks.begin(); itButton != buttonCallbacks.end(); ++itButton)
            toolBar->addButton(itButton->first, itButton->second);
    }
    propertiesMenuArea->update();
}

DesignViewBuilder::DesignViewBuilder(
    TreeView& treeView,
    ObjectsSelector& propertiesMenu,
    DesignModel& model,
    const std::shared_ptr<Presentation>& presentation,
    const std::shared_ptr<PropsMenuToolBar>& toolBar,
    ScrollableArea* propertiesMenuArea,
    int rootID)
    : m_context(std::make_shared<SharedContext>(treeView, propertiesMenu, model))
{
    m_context->presentation = presentation;
    m_context->toolBar = toolBar;
    m_context->propertiesMenuArea = propertiesMenuArea;

    auto props = std::make_shared<Properties>();
    props->id = rootID;
    m_properties.push_back(props);
    m_curModelNodeID = DesignModel::ROOT_ID;

    m_context->switchsGroup = std::make_shared<RadioButtonGroup>();
    m_context->switchsGroup->setCallback(
        std::bind(&SharedContext::select, m_context.get(), std::placeholders::_1));
}

DesignViewBuilder::Snapshot::Snapshot(
    DesignViewBuilder& builder,
    const Properties& properties,
    ObjType::Enum objType)
    : context(builder.m_context)
    , curName(builder.m_curName)
    , modelNodeID(builder.m_curModelNodeID)
    , properties(std::make_shared<Properties>(properties))
    , objType(objType)
{
    if (objType == ObjType::Array)
        arrayType = SerializationTag::Array;
}

DesignViewBuilder::DesignViewBuilder(Snapshot& snapshot)
    : m_context(snapshot.context)
    , m_curName(snapshot.curName)
    , m_curModelNodeID(snapshot.modelNodeID)
{
    m_objTypes.push_back(snapshot.objType);
    if (snapshot.arrayType)
        m_arrayTypes.push_back(*snapshot.arrayType);
    m_properties.push_back(snapshot.properties);
    if (snapshot.mapProperties)
        m_mapProperties.push_back(snapshot.mapProperties);
}

DesignViewBuilder::~DesignViewBuilder() {}

void DesignViewBuilder::writeFloat(const std::string& name, float f)
{
    writeDouble(name, static_cast<double>(f));
}

void DesignViewBuilder::writeDouble(const std::string& name, double d)
{
    std::string fullName = name;
    if (m_objTypes.back() == ObjType::PrimitiveArray) {
        fullName = propertyNameFromPresentation(propertyName(name)) + PRIMITIVE_ARRAY_ELEMENT_SUFFIX.get(
            m_arrayTypes.back(), m_primitiveElementIndex++);
    }
    addProperty(fullName, "double", boost::lexical_cast<std::string>(d),
        &updateProperty<double>);
}

void DesignViewBuilder::writeInt(const std::string& name, int i)
{
    if (name == COLLECTION_SIZE_TAG)
        return;

    bool isObject = parentObjType() == ObjType::Object;
    bool isMainPresentation = !(parentObjType() == ObjType::Map && m_arrayTypes.back() == SerializationTag::Keys);
    auto properties = currentPropertiesForPrimitive("int");
    const IPropertyPresentation* propertyPresentation = nullptr;
    if (isObject) {
        if (properties->type)
            propertyPresentation = m_context->presentation->propertyByName(properties->type->name, name);
    } else {
        propertyPresentation = isMainPresentation
            ? properties->presentationFromParent : properties->keyPresentationFromParent;
    }
    if (propertyPresentation) {
        if (propertyPresentation->presentationType() == PropertyPresentation::Enum) {
            auto enumPropertyPresentation = dynamic_cast<const EnumPropertyPresentation*>(propertyPresentation);
            if (auto enumPresentation = m_context->presentation->enumByName(enumPropertyPresentation->type)) {
                std::vector<std::string> enumValuesNames;
                std::vector<int> enumValues;
                for (auto it = enumPresentation->values.begin(); it != enumPresentation->values.end(); ++it) {
                    enumValues.push_back(it->first);
                    enumValuesNames.push_back(it->second);
                }
                auto layout = createPropertyLayout();
                layout->addObject(createLabel(propertyNameFromPresentation(propertyName(name))));
                auto comboBox = createComboBox(enumValuesNames, enumValues);
                comboBox->setText(enumPresentation->values.at(i));
                layout->addObject(comboBox);
                properties->layout->addObject(layout);

                DesignModel::UpdateModelFunc modelUpdater = std::bind(
                    updateEnumProperty, comboBox.get(), name, std::placeholders::_1);
                m_context->model.addUpdater(m_curModelNodeID, modelUpdater);
                comboBox->setCallback(properties->buttonTextUpdater);
                if (name.empty())
                    properties->buttonTextUpdater();
                return;
            }
        }
    }
    addProperty(propertyName(name), boost::lexical_cast<std::string>(i),
        &updateProperty<int>, properties.get());
    if (name.empty())
        properties->buttonTextUpdater();
}

void DesignViewBuilder::writeUInt(const std::string& name, unsigned int i)
{
    addProperty(name, "unsigned int", boost::lexical_cast<std::string>(i),
        &updateProperty<unsigned int>);
}

void DesignViewBuilder::writeInt64(const std::string& name, int64_t i)
{
    addProperty(name, "int64", boost::lexical_cast<std::string>(i),
        &updateProperty<int64_t>);
}

void DesignViewBuilder::writeUInt64(const std::string& name, uint64_t i)
{
    addProperty(name, "unsigned int64", boost::lexical_cast<std::string>(i),
        &updateProperty<uint64_t>);
}

void DesignViewBuilder::writeBool(const std::string& name, bool b)
{
    static const char* TEXT_VARIANTS[] = { "false", "true" };
    static const std::vector<std::string> TEXT_VARIANTS_VEC(TEXT_VARIANTS, TEXT_VARIANTS + 2);

    if (name == EMPTY_TAG) {
        if (m_objTypes.back() != ObjType::Unknown)
            return;

        m_properties.push_back(createProperties(m_curName, EMPTY_TYPE_NAME));
        m_objTypes.back() = ObjType::Object;

        auto presentationFromParent = m_properties.back()->presentationFromParent;
        auto typesList = createTypesList("type", presentationFromParent, "None");
        auto& comboBox = *typesList.comboBox;
        comboBox.setText("None");
        if (comboBox.variantsNum() > 1
            || (comboBox.variantsNum() == 1 && comboBox.textVariants().front() != "None")) {
            createObjectReplaceCallbacks(typesList);
        } else {
            m_context->model.addUpdater(m_curModelNodeID, createConstUpdater(EMPTY_TAG, true));
        }
        return;
    }

    auto layout = createPropertyLayout();
    layout->addObject(createLabel(propertyNameFromPresentation(propertyName(name))));
    auto comboBox = createComboBox(TEXT_VARIANTS_VEC);
    comboBox->setText(b ? TEXT_VARIANTS[1] : TEXT_VARIANTS[0]);
    layout->addObject(comboBox);
    auto properties = currentPropertiesForPrimitive("bool");
    properties->layout->addObject(layout);

    DesignModel::UpdateModelFunc modelUpdater = std::bind(
        updateBoolProperty, comboBox.get(), name, std::placeholders::_1);
    m_context->model.addUpdater(m_curModelNodeID, modelUpdater);
    comboBox->setCallback(properties->buttonTextUpdater);
    if (name.empty())
        properties->buttonTextUpdater();
}

void DesignViewBuilder::writeString(const std::string& name, const std::string& value)
{
    if (name == TYPE_NAME_TAG) {
        m_properties.push_back(createProperties(m_curName, value));
        m_objTypes.back() = ObjType::Object;

        auto typePresentation = m_context->presentation->typeByName(value);
        auto presentationFromParent = m_properties.back()->presentationFromParent;
        auto typesList = createTypesList("type", presentationFromParent, value);
        auto thisTypeName = value;
        if (typePresentation) {
            const auto& typeNameVariants = typesList.comboBox->textVariants();
            auto it = std::find(typeNameVariants.begin(), typeNameVariants.end(),
                typePresentation->nameInUI);
            if (it != typeNameVariants.end())
                thisTypeName = typePresentation->nameInUI;
        }
        auto& comboBox = *typesList.comboBox;
        comboBox.setText(thisTypeName);
        if (comboBox.variantsNum() > 1
            || (comboBox.variantsNum() == 1 && comboBox.textVariants().front() != thisTypeName)) {
            createObjectReplaceCallbacks(typesList);
        } else {
            m_context->model.addUpdater(m_curModelNodeID, createConstUpdater(name, value));
            m_context->model.addUpdater(m_curModelNodeID, createConstUpdater(EMPTY_TAG, false));
        }
        return;
    }

    addProperty(name, "string", value, &updateProperty<std::string>);
}

void DesignViewBuilder::startObject(const std::string& name)
{
    m_objTypes.push_back(ObjType::Unknown);
    m_curName = name;
    m_curModelNodeID = m_context->model.add(m_curModelNodeID, DesignModel::Node::Object, name).id;
}

void DesignViewBuilder::finishObject()
{
    if (m_objTypes.back() == ObjType::Object
        || m_objTypes.back() == ObjType::Array
        || m_objTypes.back() == ObjType::Map)
        finishCurrentProperties();
    if (m_objTypes.back() == ObjType::Map)
        m_mapProperties.pop_back();
    m_objTypes.pop_back();
    m_curModelNodeID = m_context->model.get(m_curModelNodeID).parentID;
}

void DesignViewBuilder::startArray(const std::string& name, SerializationTag::Type tag)
{
    auto prevModeNodeID = m_curModelNodeID;
    m_curModelNodeID = m_context->model.add(m_curModelNodeID, DesignModel::Node::Array, name).id;
    if (tag == SerializationTag::Array) {
        auto props = createProperties(m_curName, "array");
        addStaticTypeLabel(props->layout, "array");
        props->collectionSize.reset(new int(0));
        m_context->model.addUpdater(prevModeNodeID, std::bind(
            collectionSizeUpdater, props->collectionSize, std::placeholders::_1));
        m_properties.push_back(props);
        if (auto arrayPresentation = dynamic_cast<const ArrayPresentation*>(props->presentationFromParent)) {
            auto snapshot = std::make_shared<Snapshot>(*this, *props, ObjType::Array);
            auto* elementPresentation = arrayPresentation->elementType.get();
            if (elementPresentation->presentationType() == PropertyPresentation::Primitive
                || elementPresentation->presentationType() == PropertyPresentation::Enum) {
                auto fictiveNodeID = addFictiveNode("newElement", elementPresentation);
                m_context->nodes[props->id].callbacks[ButtonKey::Add] = std::bind(
                    addPrimitiveValueToArray, fictiveNodeID, snapshot);
            }

            if (elementPresentation->presentationType() == PropertyPresentation::Object) {
                auto typesList = createTypesList("newElement", elementPresentation);
                if (!typesList.types.empty()) {
                    m_context->nodes[props->id].callbacks[ButtonKey::Add] = std::bind(
                        addObjectToArray, typesList.comboBox, typesList.types, snapshot);
                }

                std::function<void(const std::string&)> pathProcessor = std::bind(
                    addObjectFromFileToArray, std::placeholders::_1, snapshot);
                m_context->nodes[props->id].callbacks[ButtonKey::AddFromFile] = std::bind(
                    &ExtFilePathDialog::init, &getExtFilePathDialog(), pathProcessor);
                m_context->nodes[props->id].callbacks[ButtonKey::AddFromClipboard] = std::bind(
                    addObjectFromClipboardToArray, snapshot);
            }
        }
        m_objTypes.back() = ObjType::Array;
        m_arrayTypes.push_back(tag);
        return;
    }
    if (tag == SerializationTag::Keys) {
        auto props = createProperties(m_curName, "map");
        addStaticTypeLabel(props->layout, "map");
        m_properties.push_back(props);
        props->collectionSize.reset(new int(0));
        m_context->model.addUpdater(prevModeNodeID, std::bind(
            collectionSizeUpdater, props->collectionSize, std::placeholders::_1));

        m_objTypes.back() = ObjType::Map;
        m_arrayTypes.push_back(tag);
        auto mapProperties = std::make_shared<MapProperties>();
        mapProperties->keysArrayNodeID = m_curModelNodeID;
        m_mapProperties.push_back(mapProperties);
        return;
    }

    if (tag == SerializationTag::Values) {
        auto& props = m_properties.back();
        if (auto mapPresentation = dynamic_cast<const MapPresentation*>(props->presentationFromParent)) {
            auto snapshot = std::make_shared<Snapshot>(*this, *props, ObjType::Map);
            auto valuePresentation = mapPresentation->valueType;
            auto fictiveKeyNodeID = addFictiveNode("newKey", mapPresentation->keyType.get());

            if (valuePresentation->presentationType() == PropertyPresentation::Primitive
                || valuePresentation->presentationType() == PropertyPresentation::Enum) {
                auto fictiveValueNodeID = addFictiveNode("newValue", valuePresentation.get());
                m_context->nodes[props->id].callbacks[ButtonKey::Add] = std::bind(
                    addPrimitiveElementToMap,
                    fictiveKeyNodeID, fictiveValueNodeID,
                    m_mapProperties.back()->keysArrayNodeID, m_curModelNodeID, snapshot);
            }

            if (valuePresentation->presentationType() == PropertyPresentation::Object) {
                auto typesList = createTypesList("newValue", valuePresentation.get());
                if (!typesList.types.empty()) {
                    m_context->nodes[props->id].callbacks[ButtonKey::Add] = std::bind(
                        addObjectToMap,
                        fictiveKeyNodeID, m_mapProperties.back()->keysArrayNodeID,
                        m_curModelNodeID, typesList.comboBox, typesList.types, snapshot);
                }

                std::function<void(const std::string&)> pathProcessor = std::bind(
                    addObjectFromFileToMap,
                    fictiveKeyNodeID, m_mapProperties.back()->keysArrayNodeID,
                    m_curModelNodeID, std::placeholders::_1, snapshot);
                m_context->nodes[props->id].callbacks[ButtonKey::AddFromFile] = std::bind(
                    &ExtFilePathDialog::init, &getExtFilePathDialog(), pathProcessor);
                m_context->nodes[props->id].callbacks[ButtonKey::AddFromClipboard] = std::bind(
                    addObjectFromClipboardToMap,
                    fictiveKeyNodeID, m_mapProperties.back()->keysArrayNodeID,
                    m_curModelNodeID, snapshot);
            }
        }

        m_objTypes.back() = ObjType::Map;
        m_arrayTypes.push_back(tag);
        return;
    }

    m_curName = name;
    m_objTypes.push_back(ObjType::PrimitiveArray);
    auto parent = parentObjType();
    if (parent == ObjType::Array || parent == ObjType::Map)
        m_properties.push_back(createProperties("", "primitive array"));
    m_primitiveElementIndex = 0;
    m_arrayTypes.push_back(tag);
}

void DesignViewBuilder::finishArray()
{
    if (m_objTypes.back() == ObjType::PrimitiveArray) {
        m_objTypes.pop_back();
        if (m_objTypes.back() == ObjType::Array || m_objTypes.back() == ObjType::Map)
            finishCurrentProperties();
    }
    m_arrayTypes.pop_back();
    m_curModelNodeID = m_context->model.get(m_curModelNodeID).parentID;
}

void DesignViewBuilder::addProperty(
    const std::string& name,
    const std::string& typeName,
    const std::string& initialValue,
    const std::function<void(TextBox*, std::string, Json::Value*)>& updater)
{
    auto properties = currentPropertiesForPrimitive(typeName);
    addProperty(propertyName(name), initialValue, updater, properties.get());
    if (name.empty())
        properties->buttonTextUpdater();
}

void DesignViewBuilder::addProperty(
    const std::string& name,
    const std::string& initialValue,
    const std::function<void(TextBox*, std::string, Json::Value*)>& updater,
    Properties* properties)
{
    auto layout = createPropertyLayout();
    layout->addObject(createLabel(propertyNameFromPresentation(name)));
    auto textBox = createTextBox();
    textBox->setName("value");
    textBox->setText(initialValue);
    textBox->setCallback(properties->buttonTextUpdater);
    layout->addObject(textBox);
    properties->layout->addObject(layout);

    DesignModel::UpdateModelFunc modelUpdater = std::bind(
        updater, textBox.get(), name, std::placeholders::_1);
    m_context->model.addUpdater(m_curModelNodeID, modelUpdater);
}

std::shared_ptr<DesignViewBuilder::Properties> DesignViewBuilder::createPropertiesImpl(int parentID)
{
    auto buttonSkin = deserialize<ClickableTextCheckBoxSkin>("ui\\SwitchButtonSkin.json");
    auto button = std::make_shared<RadioButton>(buttonSkin);
    auto props = std::make_shared<Properties>();
    props->id = m_context->treeView.addObject(parentID, button);
    props->switchButtonSkin = buttonSkin.get();
    auto layout = deserialize<LinearLayout>("ui\\PropertiesLayout.json");
    props->layout = layout.get();
    m_context->propertiesMenu.addObject(props->id, layout);
    button->setIndexInGroup(props->id);
    button->setGroup(m_context->switchsGroup);
    return props;
}

std::shared_ptr<DesignViewBuilder::Properties> DesignViewBuilder::createProperties(
    const std::string& name, const std::string& typeName)
{
    int parentID = m_properties.back()->id;
    ObjType::Enum parentObj = parentObjType();

    auto curCollectionSize = m_properties.back()->collectionSize;
    if (parentObj == ObjType::Array) {
        (*curCollectionSize)++;
        auto props = createPropertiesImpl(parentID);

        {
            auto snapshot = std::make_shared<Snapshot>(*this, *props, ObjType::Array);
            snapshot->properties->collectionSize = curCollectionSize;
            if (m_objTypes.back() != ObjType::Unknown && m_objTypes.back() != ObjType::PrimitiveArray)
                snapshot->modelNodeID = m_context->model.nextID();
            m_context->nodes[props->id].callbacks[ButtonKey::Remove] = std::bind(
                std::bind(removeArrayElement, snapshot));
            m_context->nodes[props->id].callbacks[ButtonKey::Down] = std::bind(
                std::bind(moveArrayElementDown, &m_context->model, &m_context->treeView, snapshot->modelNodeID, props->id));
            m_context->nodes[props->id].callbacks[ButtonKey::Up] = std::bind(
                std::bind(moveArrayElementUp, &m_context->model, &m_context->treeView, snapshot->modelNodeID, props->id));
        }

        if (typeName == "TypePresentation" || typeName == "EnumPresentation") {
            props->type = m_context->presentation->typeByName(typeName);
            props->buttonTextUpdater = std::bind(
                nameForPresentationSetter, props->switchButtonSkin, props->layout);
            return props;
        }

        PropertyPresentation::Type propertyType = PropertyPresentation::Object;
        if (auto parentPresentation = dynamic_cast<const ArrayPresentation*>(m_properties.back()->presentationFromParent)) {
            auto thisPresentation = parentPresentation->elementType.get();
            propertyType = thisPresentation->presentationType();
            props->presentationFromParent = thisPresentation;
        }
        if (propertyType == PropertyPresentation::Object) {
            props->type = m_context->presentation->typeByName(typeName);

            auto snapshot = std::make_shared<Snapshot>(*this, *m_properties.back(), ObjType::Array);
            snapshot->modelNodeID = m_context->model.get(m_curModelNodeID).parentID;

            std::function<void(const std::string&)> pathProcessor = std::bind(
                replaceArrayElementFromFile, std::placeholders::_1, snapshot, m_curModelNodeID, props->id);
            m_context->nodes[props->id].callbacks[ButtonKey::ReplaceFromFile] = std::bind(
                &ExtFilePathDialog::init, &getExtFilePathDialog(), pathProcessor);
            m_context->nodes[props->id].callbacks[ButtonKey::Paste] = std::bind(
                pasteArrayElement, snapshot, m_curModelNodeID, props->id);

            createObjectCallbacks(props->id);
        }
        props->buttonTextUpdater = std::bind(
            &nameFromPropertiesSetter, props->switchButtonSkin, props->layout, "element", 0);
        return props;
    }
    
    if (parentObj == ObjType::Map) {
        if (m_arrayTypes.back() == SerializationTag::Keys) {
            (*curCollectionSize)++;
            auto props = createPropertiesImpl(parentID);
            props->buttonTextUpdater = std::bind(
                &mapElementNameFromPropertiesSetter, props->switchButtonSkin, props->layout);
            if (auto parentPresentation = dynamic_cast<const MapPresentation*>(m_properties.back()->presentationFromParent)) {
                props->presentationFromParent = parentPresentation->valueType.get();
                props->keyPresentationFromParent = parentPresentation->keyType.get();
            }

            m_mapProperties.back()->elements.push_back(MapProperties::Element(
                props, m_context->model.nextID()));
            return props;
        }

        auto& mapProperties = m_mapProperties.back();
        auto& mapElement = mapProperties->elements.at(mapProperties->currentElem++);
        const auto& props = mapElement.properties;
        {
            auto snapshot = std::make_shared<Snapshot>(*this, *props, ObjType::Map);
            snapshot->properties->collectionSize = curCollectionSize;
            if (m_objTypes.back() != ObjType::Unknown && m_objTypes.back() != ObjType::PrimitiveArray)
                snapshot->modelNodeID = m_context->model.nextID();
            m_context->nodes[props->id].callbacks[ButtonKey::Remove] = std::bind(
                removeMapElement, snapshot, mapElement.keyNodeID);
        }
        PropertyPresentation::Type propertyType = props->presentationFromParent
            ? props->presentationFromParent->presentationType()
            : PropertyPresentation::Object;
        if (propertyType == PropertyPresentation::Object) {
            props->type = m_context->presentation->typeByName(typeName);

            auto snapshot = std::make_shared<Snapshot>(*this, *m_properties.back(), ObjType::Map);
            snapshot->arrayType = SerializationTag::Values;
            snapshot->modelNodeID = m_context->model.get(m_curModelNodeID).parentID;
            snapshot->mapProperties = std::make_shared<MapProperties>();
            snapshot->mapProperties->keysArrayNodeID = mapProperties->keysArrayNodeID;

            MapProperties::Element mapElementSnapshot(
                std::make_shared<Properties>(*props), mapElement.keyNodeID);
            snapshot->mapProperties->elements.push_back(mapElementSnapshot);

            std::function<void(const std::string&)> pathProcessor = std::bind(
                replaceMapElementFromFile, std::placeholders::_1, snapshot, m_curModelNodeID);
            m_context->nodes[props->id].callbacks[ButtonKey::ReplaceFromFile] = std::bind(
                &ExtFilePathDialog::init, &getExtFilePathDialog(), pathProcessor);
            m_context->nodes[props->id].callbacks[ButtonKey::Paste] = std::bind(
                pasteMapElement, snapshot, m_curModelNodeID);

            createObjectCallbacks(props->id);
        }
        return props;
    }
    auto props = createPropertiesImpl(parentID);
    props->type = m_context->presentation->typeByName(typeName);
    if (!name.empty() && !m_properties.empty() && m_properties.back()->type) {
        props->presentationFromParent = m_context->presentation->propertyByName(
            m_properties.back()->type->name, name);
    }
    props->buttonTextUpdater = std::bind(
        &nameFromPropertiesSetter, props->switchButtonSkin, props->layout,
        propertyNameFromPresentation(name), 0);

    if (typeName != "array" && typeName != "map" && !m_properties.empty()) {
        auto snapshot = std::make_shared<Snapshot>(*this, *m_properties.back(), ObjType::Object);
        snapshot->modelNodeID = m_context->model.get(m_curModelNodeID).parentID;

        std::function<void(const std::string&)> pathProcessor = std::bind(
            replaceMemberFromFile, std::placeholders::_1, snapshot, m_curModelNodeID, props->id);
        m_context->nodes[props->id].callbacks[ButtonKey::ReplaceFromFile] = std::bind(
            &ExtFilePathDialog::init, &getExtFilePathDialog(), pathProcessor);
        m_context->nodes[props->id].callbacks[ButtonKey::Paste] = std::bind(
            pasteMember, snapshot, m_curModelNodeID, props->id);

        createObjectCallbacks(props->id);
    }

    return props;
}

std::string DesignViewBuilder::propertyName(const std::string& nameFromSerializer)
{
    if (nameFromSerializer.empty()) {
        ObjType::Enum parentObj = parentObjType();
        if (parentObj == ObjType::Array)
            return "element";
        if (parentObj == ObjType::Map) {
            SerializationTag::Type arrayType = m_arrayTypes.back();
            if (m_objTypes.back() == ObjType::PrimitiveArray)
                arrayType = *(m_arrayTypes.rbegin() + 1);
            if (arrayType == SerializationTag::Keys)
                return "key";
            if (arrayType == SerializationTag::Values)
                return "value";
            THROW_EX() << "Bad map's serialialization tag: " << arrayType;
        }
        return m_curName;
    }
    return nameFromSerializer;
}

std::shared_ptr<DesignViewBuilder::Properties> DesignViewBuilder::currentPropertiesForPrimitive(
    const std::string& typeName)
{
    auto parent = parentObjType();
    if (parent == ObjType::Array || parent == ObjType::Map)
        return createProperties("", typeName);
    return m_properties.back();
}

DesignViewBuilder::ObjType::Enum DesignViewBuilder::parentObjType() const
{
    for (auto it = m_objTypes.rbegin(); it != m_objTypes.rend(); ++it)
        if (*it != ObjType::Unknown && *it != ObjType::PrimitiveArray)
            return *it;
    return ObjType::Unknown;
}

void DesignViewBuilder::createObjectCallbacks(int propsID)
{
    std::function<void(const std::string&)> pathProcessor = std::bind(
        saveNode, &m_context->model, m_curModelNodeID, std::placeholders::_1);
    m_context->nodes[propsID].callbacks[ButtonKey::Save] = std::bind(
        &ExtFilePathDialog::init, &getExtFilePathDialog(), pathProcessor);
    m_context->nodes[propsID].callbacks[ButtonKey::Copy] = std::bind(
        copyNode, &m_context->model, m_curModelNodeID);
}

std::shared_ptr<DesignViewBuilder::Properties> DesignViewBuilder::currentProperties()
{
    return m_properties.back();
}

void DesignViewBuilder::finishCurrentProperties()
{
    if (m_properties.back()->buttonTextUpdater)
        m_properties.back()->buttonTextUpdater();
    m_properties.pop_back();
}

std::string DesignViewBuilder::propertyNameFromPresentation(const std::string& name)
{
    if (!m_properties.empty()) {
        if (auto type = m_properties.back()->type) {
            if (auto* propertyPresentation = m_context->presentation->propertyByName(type->name, name))
                return propertyPresentation->nameInUI;
        }
    }
    return name;
}

int DesignViewBuilder::addFictiveNode(
    const std::string& name,
    const IPropertyPresentation* elementPresentation)
{
    auto& props = m_properties.back();
    auto modelNodeID = m_curModelNodeID;
    auto presentationFromParent = props->presentationFromParent;
                
    // add fictive node and prepare to adding input UI
    auto fictiveNodeID = m_context->model.addFictiveNode().id;
    m_curModelNodeID = fictiveNodeID;
    props->presentationFromParent = elementPresentation;
    m_objTypes.back() = ObjType::FictiveObject;

    // add input UI connected to fictive data node
    Serializer serializer(this);
    serializeDefaultValue(serializer, name, m_context->presentation, elementPresentation);
                
    // restore state
    props->presentationFromParent = presentationFromParent;
    m_curModelNodeID = modelNodeID;
    return fictiveNodeID;
}


DesignViewBuilder::TypesList DesignViewBuilder::createTypesList(
    const std::string& label,
    const IPropertyPresentation* propertyPresentation,
    const std::string& variantIfNoPresentation)
{
    DesignViewBuilder::TypesList result;
    std::vector<std::string> typesNames;
    auto objectPresentation = dynamic_cast<const ObjectPresentation*>(propertyPresentation);
    if (objectPresentation) {
        result.types = m_context->presentation->derivedTypesByBaseTypeName(objectPresentation->baseType);
        for (auto it = result.types.begin(); it != result.types.end(); ++it)
            typesNames.push_back((*it)->nameInUI);
        if (objectPresentation->canBeEmpty)
            typesNames.push_back("None");
    }

    if (typesNames.empty())
        typesNames.push_back(variantIfNoPresentation);

    auto layout = createPropertyLayout();
    layout->addObject(createLabel(label));
    auto comboBox = createComboBox(typesNames);
    comboBox->setText(typesNames[0]);
    layout->addObject(comboBox);
    auto propertiesLayout = m_properties.back()->layout;
    result.indexInLayout = propertiesLayout->objects().size();
    propertiesLayout->addObject(layout);
    result.comboBox = comboBox.get();
    return result;
}

void DesignViewBuilder::createObjectReplaceCallbacks(const TypesList& typesList)
{
    const auto& props = *m_properties.back();
    m_context->model.addUpdater(m_curModelNodeID, std::bind(updateTypeTag, typesList, std::placeholders::_1));
    m_context->model.addUpdater(m_curModelNodeID, std::bind(updateEmptyTag, typesList, std::placeholders::_1));
    auto snapshot = std::make_shared<Snapshot>(*this, props, ObjType::Object);
    typesList.comboBox->setCallback(std::bind(
        replaceObjectWithPattern, typesList, snapshot,
        m_context->model.get(m_curModelNodeID).updatersNum(),
        typesList.comboBox));
}

void DesignViewBuilder::addStaticTypeLabel(
    LinearLayout* propertiesLayout, const std::string& typeName)
{
    auto layout = createPropertyLayout();
    layout->addObject(createLabel("type"));
    layout->addObject(createRightLabel(typeName));
    propertiesLayout->addObject(layout);
}

} }

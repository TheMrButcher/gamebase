#include "DesignViewBuilder.h"
#include "tools.h"
#include <gamebase/engine/TransparentLinearLayoutSkin.h>
#include <gamebase/engine/OffsettedBox.h>
#include <gamebase/engine/FixedBox.h>
#include <gamebase/engine/RadioButton.h>
#include <gamebase/engine/InstantChange.h>
#include <gamebase/engine/AnimatedCheckBoxSkin.h>
#include <gamebase/engine/RelativeBox.h>
#include <gamebase/engine/AligningOffset.h>
#include <gamebase/engine/StaticLabel.h>
#include <gamebase/engine/StaticFilledRect.h>
#include <gamebase/engine/AnimatedTextBoxSkin.h>
#include <gamebase/engine/CommonButtonListSkin.h>
#include <gamebase/engine/CommonComboBoxSkin.h>
#include <gamebase/engine/ComboBox.h>
#include <gamebase/serial/JsonDeserializer.h>
#include <json/value.h>
#include <boost/lexical_cast.hpp>

namespace gamebase { namespace editor {

namespace {
std::shared_ptr<LinearLayout> createPropertiesListLayout()
{
    auto skin = std::make_shared<TransparentLinearLayoutSkin>(
        std::make_shared<OffsettedBox>(), Direction::Vertical);
    skin->setPadding(2.0f);
    skin->setAdjustSize(false);
    return std::make_shared<LinearLayout>(
        std::make_shared<AligningOffset>(HorAlign::Center, VertAlign::Center), skin);
}

std::shared_ptr<LinearLayout> createPropertyLayout()
{
    auto skin = std::make_shared<TransparentLinearLayoutSkin>(
        std::make_shared<OffsettedBox>(), Direction::Horizontal);
    skin->setPadding(5.0f);
    skin->setAdjustSize(true);
    return std::make_shared<LinearLayout>(nullptr, skin);
}

std::shared_ptr<AnimatedCheckBoxSkin> createSwitchButtonSkin()
{
    auto skin = std::make_shared<AnimatedCheckBoxSkin>(
        std::make_shared<FixedBox>(250.f, 20.f));

    auto fill = std::make_shared<StaticFilledRect>(
        std::make_shared<RelativeBox>(
            RelativeValue(RelType::ValueMinusPixels, 4.0f),
            RelativeValue(RelType::ValueMinusPixels, 4.0f),
            std::make_shared<AligningOffset>(HorAlign::Center, VertAlign::Center)));
    fill->setColor(Color(0.8f, 0.8f, 1.0f, 0.2f));
    fill->setName("fill");
    skin->addElement(fill);

    skin->setCheckAnimation(std::make_shared<InstantChange<float>>(
        "elements/fill/colorA", 1.0f));
    skin->setUncheckAnimation(std::make_shared<InstantChange<float>>(
        "elements/fill/colorA", 0.2f));

    return skin;
}

std::shared_ptr<StaticLabel> createLabel(
    const std::shared_ptr<IRelativeBox>& box,
    const std::string& text,
    HorAlign::Enum horAlign)
{
    auto label = std::make_shared<StaticLabel>(box);
    AlignProperties alignProperties;
    alignProperties.horAlign = horAlign;
    alignProperties.vertAlign = VertAlign::Center;
    alignProperties.enableStacking = false;
    label->setAlignProperties(alignProperties);
    label->setAdjustSize(false);
    label->setText(text);
    return label;
}

std::shared_ptr<StaticLabel> createSwitchButtonLabel()
{
    return createLabel(
        std::make_shared<RelativeBox>(RelativeValue(), RelativeValue()),
        "", HorAlign::Center);
}

std::shared_ptr<StaticLabel> createLabel(const std::string& text)
{
    return createLabel(
        std::make_shared<FixedBox>(200.0f, 20.0f), text + ":", HorAlign::Right);
}

std::shared_ptr<StaticLabel> createConstValueLabel(const std::string& text)
{
    return createLabel(
        std::make_shared<FixedBox>(200.0f, 20.0f), text, HorAlign::Left);
}

std::shared_ptr<AnimatedTextBoxSkin> createTextBoxSkin(
    const std::shared_ptr<IRelativeBox>& box = std::make_shared<FixedBox>(300.0f, 20.0f))
{
    auto skin = std::make_shared<AnimatedTextBoxSkin>(box);

    skin->label().setRelativeBox(std::make_shared<RelativeBox>(
        RelativeValue(RelType::ValueMinusPixels, 8.0f),
        RelativeValue(RelType::ValueMinusPixels, 8.0f),
        std::make_shared<AligningOffset>(HorAlign::Center, VertAlign::Center)));

    auto border = std::make_shared<StaticFilledRect>(
        std::make_shared<RelativeBox>(RelativeValue(), RelativeValue()));
    border->setColor(Color(0, 0, 0));
    skin->addElement(border);

    auto fill = std::make_shared<StaticFilledRect>(
        std::make_shared<RelativeBox>(
            RelativeValue(RelType::ValueMinusPixels, 4.0f),
            RelativeValue(RelType::ValueMinusPixels, 4.0f),
            std::make_shared<AligningOffset>(HorAlign::Center, VertAlign::Center)));
    fill->setColor(Color(1.0f, 1.0f, 1.0f));
    skin->addElement(fill);
    return skin;
}

std::shared_ptr<TextBox> createTextBox()
{
    return std::make_shared<TextBox>(nullptr, createTextBoxSkin());
}

std::shared_ptr<CommonButtonListSkin> createButtonListSkin()
{
    auto buttonListSkin = std::make_shared<CommonButtonListSkin>(
        std::make_shared<FixedBox>(300.0f, 600.0f),
        std::make_shared<OffsettedBox>(), Direction::Vertical);
    buttonListSkin->adjustSize(true);
    return buttonListSkin;
}

std::shared_ptr<ComboBox> createComboBox(
    const std::vector<std::string>& variants,
    const std::vector<int>& indices = std::vector<int>())
{
    auto textListSkin = std::make_shared<CommonComboBoxSkin>(
        std::make_shared<FixedBox>(300.0f, 20.0f));
    textListSkin->setTextBoxSkin(createTextBoxSkin(
        std::make_shared<FixedBox>(280.0f, 20.0f)));
    textListSkin->setOpenButtonSkin(createButtonSkin(20.0f, 20.0f, "O"));
    textListSkin->setButtonListSkin(createButtonListSkin());
    textListSkin->setTextBoxDisabled(true);

    auto result = std::make_shared<ComboBox>(nullptr, textListSkin);
    auto itIndex = indices.begin();
    for (auto itVariant = variants.begin(); itVariant != variants.end(); ++itVariant) {
        const auto& text = *itVariant;
        auto button = createButton(300.0f, 20.0f, text);
        if (itIndex == indices.end())
            result->addButton(text, button);
        else
            result->addButton(text, button, *itIndex++);
    }
    return result;
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

void nameForPresentationSetter(StaticLabel* label, LinearLayout* propertiesLayout)
{
    if (propertiesLayout->objects().size() < 4)
        return;
    auto text = extractText(propertiesLayout, 3);
    if (text.empty())
        text = extractText(propertiesLayout, 2);
    label->setTextAndLoad(text);
}

void nameFromPropertiesSetter(
    StaticLabel* label, LinearLayout* propertiesLayout,
    const std::string& prefix, size_t sourceIndex)
{
    if (propertiesLayout->objects().size() < sourceIndex + 1)
        return;
    label->setTextAndLoad(mergeStrings(
        prefix, extractText(propertiesLayout, sourceIndex)));
}

void mapElementNameFromPropertiesSetter(StaticLabel* label, LinearLayout* propertiesLayout)
{
    if (propertiesLayout->objects().size() < 3)
        return;
    label->setTextAndLoad(extractText(propertiesLayout, 1) + " -> " + extractText(propertiesLayout, 2));
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
    int sourceID, const std::string& name, const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot)
{
    DesignViewBuilder builder(*snapshot);
    Serializer serializer(&builder);
    auto fictiveData = snapshot->model.toJsonValue(sourceID);
    if (!fictiveData->isMember(name))
        return;
    const auto& sourceData = (*fictiveData)[name];
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
    else if (sourceData.isString())
        serializer << "" << sourceData.asString();
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
        obj = snapshot->presentation->loadPattern(types[id]->name);
    }
    DesignViewBuilder builder(*snapshot);
    Serializer serializer(&builder);
    serializer << "" << obj;
}

void addObjectFromFile(
    const TextBox* textBox,
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot)
{
    std::shared_ptr<IObject> obj;
    try {
        deserializeFromJsonFile(textBox->text(), obj);
    } catch (std::exception& ex) {
        std::cerr << "Error while loading object from file: " << textBox->text()
            << ". Reason: " << ex.what() << ". Adding empty object";
    }
    DesignViewBuilder builder(*snapshot);
    Serializer serializer(&builder);
    serializer << "" << obj;
}

void updateView(const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot)
{
    snapshot->treeView.countBoxes();
    snapshot->treeView.loadResources();
}

void addPrimitiveValueToArray(
    int sourceID, const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot)
{
    addPrimitiveValueFromSource(sourceID, "newElement", snapshot);
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
    const TextBox* textBox,
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot)
{
    addObjectFromFile(textBox, snapshot);
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
    addPrimitiveValueFromSource(keySourceID, "newKey", snapshot);
    
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
    addElementToMap(keySourceID, keysArrayNodeID, valuesArrayNodeID, snapshot,
        std::bind(addPrimitiveValueFromSource, valueSourceID, "newValue", snapshot));
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
    const TextBox* textBox,
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot)
{
    addElementToMap(keySourceID, keysArrayNodeID, valuesArrayNodeID, snapshot,
        std::bind(addObjectFromFile, textBox, snapshot));
}

void replaceObject(
    const DesignViewBuilder::TypesList& typesList,
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot,
    size_t updatersToSaveNum,
    const std::string& typeName,
    int variantID)
{
    std::shared_ptr<IObject> obj;
    if (variantID >= 0 && variantID < static_cast<int>(typesList.types.size())) {
        obj = snapshot->presentation->loadPattern(typesList.types[variantID]->name);
        snapshot->properties->type = typesList.types[variantID];
    } else {
        snapshot->properties->type = nullptr;
    }
    auto& node = snapshot->model.get(snapshot->modelNodeID);
    auto updatersToSave = node.updaters();
    updatersToSave.resize(std::min(updatersToSaveNum, updatersToSave.size()));
    snapshot->model.clearNode(snapshot->modelNodeID);
    for (auto it = updatersToSave.begin(); it != updatersToSave.end(); ++it)
        snapshot->model.addUpdater(snapshot->modelNodeID, *it);

    const auto& objects = snapshot->properties->layout->objects();
    std::vector<std::shared_ptr<IObject>> objectsToSave(
        objects.begin(), objects.begin() + std::min(objects.size(), typesList.indexInLayout + 1));
    snapshot->properties->layout->clear();
    for (auto it = objectsToSave.begin(); it != objectsToSave.end(); ++it)
        snapshot->properties->layout->addObject(*it);
    snapshot->treeView.removeChildren(snapshot->properties->id);

    if (obj) {
        DesignViewBuilder builder(*snapshot);
        try {
            Serializer objectSerializer(&builder);
            if (const ISerializable* serObj = dynamic_cast<const ISerializable*>(obj.get())) {
                serObj->serialize(objectSerializer);
            } else {
                THROW_EX() << "Type " << typeName << " (type_index: " << typeid(*obj).name()
                    << ") is not serializable";
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
    updateView(snapshot);
}

void textEditCallbackAdapter(const std::function<void()>& callback, const std::string&)
{
    if (callback)
        callback();
}

void textListCallbackAdapter(const std::function<void()>& callback, const std::string&, int)
{
    if (callback)
        callback();
}

void removeArrayElement(const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot)
{
    snapshot->model.remove(snapshot->modelNodeID);
    snapshot->treeView.removeSubtree(snapshot->properties->id);
    snapshot->propertiesMenu.removeObject(snapshot->properties->id);
    --(*snapshot->properties->collectionSize);
    updateView(snapshot);
}

void removeMapElement(
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot,
    int keyNodeID)
{
    snapshot->model.remove(keyNodeID);
    removeArrayElement(snapshot);
}
}

DesignViewBuilder::DesignViewBuilder(
    TreeView& treeView,
    ObjectsSelector& propertiesMenu,
    DesignModel& model,
    const std::shared_ptr<Presentation>& presentation,
    int rootID)
    : m_treeView(treeView)
    , m_propertiesMenu(propertiesMenu)
    , m_model(model)
    , m_presentation(presentation)
{
    auto props = std::make_shared<Properties>();
    props->id = rootID;
    m_properties.push_back(props);
    m_curModelNodeID = DesignModel::ROOT_ID;

    m_switchsGroup = std::make_shared<RadioButtonGroup>();
    m_switchsGroup->setCallback(
        std::bind(&ObjectsSelector::select, &m_propertiesMenu, std::placeholders::_1));
}

DesignViewBuilder::Snapshot::Snapshot(
    DesignViewBuilder& builder,
    const Properties& properties,
    ObjType::Enum objType)
    : treeView(builder.m_treeView)
    , propertiesMenu(builder.m_propertiesMenu)
    , model(builder.m_model)
    , presentation(builder.m_presentation)
    , curName(builder.m_curName)
    , modelNodeID(builder.m_curModelNodeID)
    , switchsGroup(builder.m_switchsGroup)
    , properties(std::make_shared<Properties>(properties))
    , objType(objType)
{
    if (objType == ObjType::Array)
        arrayType = SerializationTag::Array;
}

DesignViewBuilder::DesignViewBuilder(Snapshot& snapshot)
    : m_treeView(snapshot.treeView)
    , m_propertiesMenu(snapshot.propertiesMenu)
    , m_model(snapshot.model)
    , m_presentation(snapshot.presentation)
    , m_curName(snapshot.curName)
    , m_curModelNodeID(snapshot.modelNodeID)
    , m_switchsGroup(snapshot.switchsGroup)
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
            propertyPresentation = m_presentation->propertyByName(properties->type->name, name);
    } else {
        propertyPresentation = isMainPresentation
            ? properties->presentationFromParent : properties->keyPresentationFromParent;
    }
    if (propertyPresentation) {
        if (propertyPresentation->presentationType() == PropertyPresentation::Enum) {
            auto enumPropertyPresentation = dynamic_cast<const EnumPropertyPresentation*>(propertyPresentation);
            if (auto enumPresentation = m_presentation->enumByName(enumPropertyPresentation->type)) {
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
                m_model.addUpdater(m_curModelNodeID, modelUpdater);
                comboBox->setCallback(std::bind(textListCallbackAdapter,
                    properties->buttonTextUpdater, std::placeholders::_1, std::placeholders::_2));
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
            m_model.addUpdater(m_curModelNodeID, createConstUpdater(EMPTY_TAG, true));
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
    m_model.addUpdater(m_curModelNodeID, modelUpdater);
    comboBox->setCallback(std::bind(textListCallbackAdapter,
        properties->buttonTextUpdater, std::placeholders::_1, std::placeholders::_2));
    if (name.empty())
        properties->buttonTextUpdater();
}

void DesignViewBuilder::writeString(const std::string& name, const std::string& value)
{
    if (name == TYPE_NAME_TAG) {
        m_properties.push_back(createProperties(m_curName, value));
        m_objTypes.back() = ObjType::Object;

        auto typePresentation = m_presentation->typeByName(value);
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
            m_model.addUpdater(m_curModelNodeID, createConstUpdater(name, value));
            m_model.addUpdater(m_curModelNodeID, createConstUpdater(EMPTY_TAG, false));
        }
        return;
    }

    addProperty(name, "string", value, &updateProperty<std::string>);
}

void DesignViewBuilder::startObject(const std::string& name)
{
    m_objTypes.push_back(ObjType::Unknown);
    m_curName = name;
    m_curModelNodeID = m_model.add(m_curModelNodeID, DesignModel::Node::Object, name).id;
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
    m_curModelNodeID = m_model.get(m_curModelNodeID).parentID;
}

void DesignViewBuilder::startArray(const std::string& name, SerializationTag::Type tag)
{
    auto prevModeNodeID = m_curModelNodeID;
    m_curModelNodeID = m_model.add(m_curModelNodeID, DesignModel::Node::Array, name).id;
    if (tag == SerializationTag::Array) {
        auto props = createProperties(m_curName, "array");
        addStaticTypeLabel(props->layout, "array");
        props->collectionSize.reset(new int(0));
        m_model.addUpdater(prevModeNodeID, std::bind(
            collectionSizeUpdater, props->collectionSize, std::placeholders::_1));
        m_properties.push_back(props);
        if (auto arrayPresentation = dynamic_cast<const ArrayPresentation*>(props->presentationFromParent)) {
            auto snapshot = std::make_shared<Snapshot>(*this, *props, ObjType::Array);
            auto addingButton = createButton(300.0f, 20.0f, "Add element");
            props->layout->addObject(addingButton);
            auto* elementPresentation = arrayPresentation->elementType.get();
            if (elementPresentation->presentationType() == PropertyPresentation::Primitive
                || elementPresentation->presentationType() == PropertyPresentation::Enum) {
                auto fictiveNodeID = addFictiveNode("newElement", elementPresentation);
                addingButton->setCallback(std::bind(addPrimitiveValueToArray, fictiveNodeID, snapshot));
            }

            if (elementPresentation->presentationType() == PropertyPresentation::Object) {
                auto typesList = createTypesList("newElement", elementPresentation);
                if (!typesList.types.empty())
                    addingButton->setCallback(std::bind(
                        addObjectToArray, typesList.comboBox, typesList.types, snapshot));
                else
                    addingButton->setSelectionState(SelectionState::Disabled);
                auto fileLoader = createObjectFromFileLoadingLayout(props->layout);
                fileLoader.button->setCallback(std::bind(
                    addObjectFromFileToArray, fileLoader.textBox, snapshot));
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
        m_model.addUpdater(prevModeNodeID, std::bind(
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
            auto addingButton = createButton(300.0f, 20.0f, "Add element");
            props->layout->addObject(addingButton);
            auto fictiveKeyNodeID = addFictiveNode("newKey", mapPresentation->keyType.get());

            if (valuePresentation->presentationType() == PropertyPresentation::Primitive
                || valuePresentation->presentationType() == PropertyPresentation::Enum) {
                auto fictiveValueNodeID = addFictiveNode("newValue", valuePresentation.get());
                addingButton->setCallback(std::bind(addPrimitiveElementToMap,
                    fictiveKeyNodeID, fictiveValueNodeID,
                    m_mapProperties.back()->keysArrayNodeID, m_curModelNodeID, snapshot));
            }

            if (valuePresentation->presentationType() == PropertyPresentation::Object) {
                auto typesList = createTypesList("newValue", valuePresentation.get());
                if (!typesList.types.empty())
                    addingButton->setCallback(
                        std::bind(addObjectToMap,
                            fictiveKeyNodeID, m_mapProperties.back()->keysArrayNodeID,
                            m_curModelNodeID, typesList.comboBox, typesList.types, snapshot));
                else
                    addingButton->setSelectionState(SelectionState::Disabled);
                auto fileLoader = createObjectFromFileLoadingLayout(props->layout);
                fileLoader.button->setCallback(
                    std::bind(addObjectFromFileToMap,
                        fictiveKeyNodeID, m_mapProperties.back()->keysArrayNodeID,
                        m_curModelNodeID, fileLoader.textBox, snapshot));
            }
        }

        m_objTypes.back() = ObjType::Map;
        m_arrayTypes.push_back(tag);
        return;
    }

    m_curName = name;
    if (m_objTypes.back() == ObjType::Array || m_objTypes.back() == ObjType::Map)
        m_properties.push_back(currentPropertiesForPrimitive("primitive array"));
    m_objTypes.push_back(ObjType::PrimitiveArray);
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
    m_curModelNodeID = m_model.get(m_curModelNodeID).parentID;
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
    textBox->setCallback(std::bind(textEditCallbackAdapter,
        properties->buttonTextUpdater, std::placeholders::_1));
    layout->addObject(textBox);
    properties->layout->addObject(layout);

    DesignModel::UpdateModelFunc modelUpdater = std::bind(
        updater, textBox.get(), name, std::placeholders::_1);
    m_model.addUpdater(m_curModelNodeID, modelUpdater);
}

std::shared_ptr<DesignViewBuilder::Properties> DesignViewBuilder::createPropertiesImpl(int parentID)
{
    auto buttonSkin = createSwitchButtonSkin();
    auto buttonLabel = createSwitchButtonLabel();
    buttonSkin->addElement(buttonLabel);
    auto button = std::make_shared<RadioButton>(nullptr, buttonSkin);
    auto props = std::make_shared<Properties>();
    props->id = m_treeView.addObject(parentID, button);
    props->switchButtonLabel = buttonLabel.get();
    auto layout = createPropertiesListLayout();
    props->layout = layout.get();
    m_propertiesMenu.addObject(props->id, layout);
    button->setIndexInGroup(props->id);
    button->setGroup(m_switchsGroup);
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
            auto buttonsLayout = createPropertyLayout();
            auto removingButton = createButton(300.0f, 20.0f, "Remove element");
            buttonsLayout->addObject(removingButton);
            props->layout->addObject(buttonsLayout);
            auto snapshot = std::make_shared<Snapshot>(*this, *props, ObjType::Array);
            snapshot->properties->collectionSize = curCollectionSize;
            if (m_objTypes.back() != ObjType::Unknown && m_objTypes.back() != ObjType::PrimitiveArray)
                snapshot->modelNodeID = m_model.nextID();
            removingButton->setCallback(std::bind(removeArrayElement, snapshot));
        }

        if (typeName == "TypePresentation" || typeName == "EnumPresentation") {
            props->type = m_presentation->typeByName(typeName);
            props->buttonTextUpdater = std::bind(
                nameForPresentationSetter, props->switchButtonLabel, props->layout);
            return props;
        }

        PropertyPresentation::Type propertyType = PropertyPresentation::Object;
        if (auto parentPresentation = dynamic_cast<const ArrayPresentation*>(m_properties.back()->presentationFromParent)) {
            auto thisPresentation = parentPresentation->elementType.get();
            propertyType = thisPresentation->presentationType();
            props->presentationFromParent = thisPresentation;
        }
        if (propertyType == PropertyPresentation::Object)
            props->type = m_presentation->typeByName(typeName);
        props->buttonTextUpdater = std::bind(
            &nameFromPropertiesSetter, props->switchButtonLabel, props->layout, "element", 1);
        return props;
    }
    
    if (parentObj == ObjType::Map) {
        if (m_arrayTypes.back() == SerializationTag::Keys) {
            (*curCollectionSize)++;
            auto props = createPropertiesImpl(parentID);
            props->buttonTextUpdater = std::bind(
                &mapElementNameFromPropertiesSetter, props->switchButtonLabel, props->layout);
            if (auto parentPresentation = dynamic_cast<const MapPresentation*>(m_properties.back()->presentationFromParent)) {
                props->presentationFromParent = parentPresentation->valueType.get();
                props->keyPresentationFromParent = parentPresentation->keyType.get();
            }

            auto buttonsLayout = createPropertyLayout();
            auto removingButton = createButton(300.0f, 20.0f, "Remove element");
            buttonsLayout->addObject(removingButton);
            props->layout->addObject(buttonsLayout);

            m_mapProperties.back()->elements.push_back(MapProperties::Element(
                props, m_model.nextID(), removingButton.get()));
            return props;
        }

        auto& mapProperties = m_mapProperties.back();
        auto& mapElement = mapProperties->elements.at(mapProperties->currentElem++);
        const auto& props = mapElement.properties;
        {
            auto snapshot = std::make_shared<Snapshot>(*this, *props, ObjType::Map);
            snapshot->properties->collectionSize = curCollectionSize;
            if (m_objTypes.back() != ObjType::Unknown && m_objTypes.back() != ObjType::PrimitiveArray)
                snapshot->modelNodeID = m_model.nextID();
            mapElement.removingButton->setCallback(std::bind(
                removeMapElement, snapshot, mapElement.keyNodeID));
        }
        PropertyPresentation::Type propertyType = props->presentationFromParent
            ? props->presentationFromParent->presentationType()
            : PropertyPresentation::Object;
        props->type = m_presentation->typeByName(typeName);
        return props;
    }
    auto props = createPropertiesImpl(parentID);
    props->type = m_presentation->typeByName(typeName);
    if (!name.empty() && !m_properties.empty() && m_properties.back()->type) {
        props->presentationFromParent = m_presentation->propertyByName(
            m_properties.back()->type->name, name);
    }
    props->buttonTextUpdater = std::bind(
        &nameFromPropertiesSetter, props->switchButtonLabel, props->layout,
        propertyNameFromPresentation(name), 0);
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
    if (m_objTypes.back() == ObjType::Array || m_objTypes.back() == ObjType::Map)
        return createProperties("", typeName);
    return m_properties.back();
}

DesignViewBuilder::ObjType::Enum DesignViewBuilder::parentObjType() const
{
    ObjType::Enum parentObj = ObjType::Unknown;
    for (auto it = m_objTypes.rbegin(); it != m_objTypes.rend(); ++it)
        if (*it != ObjType::Unknown && *it != ObjType::PrimitiveArray) {
            parentObj = *it;
            break;
        }
    return parentObj;
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
            if (auto* propertyPresentation = m_presentation->propertyByName(type->name, name))
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
    auto fictiveNodeID = m_model.addFictiveNode().id;
    m_curModelNodeID = fictiveNodeID;
    props->presentationFromParent = elementPresentation;
    m_objTypes.back() = ObjType::FictiveObject;

    // add input UI connected to fictive data node
    Serializer serializer(this);
    serializeDefaultValue(serializer, name, m_presentation, elementPresentation);
                
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
        result.types = m_presentation->derivedTypesByBaseTypeName(objectPresentation->baseType);
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
    m_model.addUpdater(m_curModelNodeID, std::bind(updateTypeTag, typesList, std::placeholders::_1));
    m_model.addUpdater(m_curModelNodeID, std::bind(updateEmptyTag, typesList, std::placeholders::_1));
    auto snapshot = std::make_shared<Snapshot>(*this, props, ObjType::Object);
    typesList.comboBox->setCallback(std::bind(
        replaceObject, typesList, snapshot, m_model.get(m_curModelNodeID).updaters().size(),
        std::placeholders::_1, std::placeholders::_2));
}

void DesignViewBuilder::addStaticTypeLabel(
    LinearLayout* propertiresLayout, const std::string& typeName)
{
    auto layout = createPropertyLayout();
    layout->addObject(createLabel("type"));
    layout->addObject(createConstValueLabel(typeName));
    propertiresLayout->addObject(layout);
}

DesignViewBuilder::ObjectFromFileLoadingLayout
DesignViewBuilder::createObjectFromFileLoadingLayout(
    LinearLayout* propertiresLayout)
{
    ObjectFromFileLoadingLayout result;
    auto layout = createPropertyLayout();
    auto fileNameBox = createTextBox();
    result.textBox = fileNameBox.get();
    layout->addObject(fileNameBox);
    auto addFromFileButton = createButton(300.0f, 20.0f, "Add element from file");
    result.button = addFromFileButton.get();
    layout->addObject(addFromFileButton);
    propertiresLayout->addObject(layout);
    return result;
}

} }

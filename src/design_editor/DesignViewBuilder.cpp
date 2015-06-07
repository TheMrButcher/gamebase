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
#include <gamebase/engine/AnimatedTextEditSkin.h>
#include <gamebase/engine/CommonButtonListSkin.h>
#include <gamebase/engine/CommonTextListSkin.h>
#include <gamebase/engine/TextList.h>
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

std::shared_ptr<StaticLabel> createSwitchButtonLabel()
{
    auto text = std::make_shared<StaticLabel>(
        std::make_shared<RelativeBox>(RelativeValue(), RelativeValue()));
    AlignProperties properties;
    properties.horAlign = HorAlign::Center;
    properties.vertAlign = VertAlign::Center;
    properties.enableStacking = false;
    text->setAlignProperties(properties);
    text->setAdjustSize(false);
    return text;
}

std::shared_ptr<StaticLabel> createLabel(const std::string& text)
{
    auto label = std::make_shared<StaticLabel>(
        std::make_shared<FixedBox>(200.0f, 20.0f));
    AlignProperties alignProperties;
    alignProperties.horAlign = HorAlign::Right;
    alignProperties.vertAlign = VertAlign::Center;
    alignProperties.enableStacking = false;
    label->setAlignProperties(alignProperties);
    label->setAdjustSize(false);
    label->setText(text + ":");
    return label;
}

std::shared_ptr<AnimatedTextEditSkin> createTextEditSkin(
    const std::shared_ptr<IRelativeBox>& box = std::make_shared<FixedBox>(300.0f, 20.0f))
{
    auto skin = std::make_shared<AnimatedTextEditSkin>(box);

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

std::shared_ptr<TextEdit> createTextEdit()
{
    return std::make_shared<TextEdit>(nullptr, createTextEditSkin());
}

std::shared_ptr<CommonButtonListSkin> createButtonListSkin()
{
    auto buttonListSkin = std::make_shared<CommonButtonListSkin>(
        std::make_shared<FixedBox>(300.0f, 600.0f),
        std::make_shared<OffsettedBox>(), Direction::Vertical);
    buttonListSkin->adjustSize(true);
    return buttonListSkin;
}

std::shared_ptr<TextList> createTextList(
    const std::vector<std::string>& variants,
    const std::vector<int>& indices = std::vector<int>())
{
    auto textListSkin = std::make_shared<CommonTextListSkin>(
        std::make_shared<FixedBox>(300.0f, 20.0f));
    textListSkin->setTextEditSkin(createTextEditSkin(
        std::make_shared<FixedBox>(280.0f, 20.0f)));
    textListSkin->setOpenButtonSkin(createButtonSkin(20.0f, 20.0f, "O"));
    textListSkin->setButtonListSkin(createButtonListSkin());
    textListSkin->setTextEditDisabled(true);

    auto result = std::make_shared<TextList>(nullptr, textListSkin);
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
void updateProperty(TextEdit* textEdit, std::string name, Json::Value* data)
{
    setDataFromString<T>(data, name, textEdit->text());
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

void textSetter(StaticLabel* label, const std::string& text)
{
    label->setTextAndLoad(text);
}

std::string extractText(LinearLayout* propertiesLayout, size_t index)
{
    if (propertiesLayout->objects().size() <= index)
        return std::string();
    LinearLayout* layout = dynamic_cast<LinearLayout*>(propertiesLayout->objects()[index].get());
    if (!layout || layout->objects().size() < 2)
        return std::string();
    if (TextEdit* textEdit = dynamic_cast<TextEdit*>(layout->objects()[1].get()))
        return textEdit->text();
    if (TextList* textList = dynamic_cast<TextList*>(layout->objects()[1].get()))
        return textList->text();
    return std::string();
}

void nameFromPresentationSetter(StaticLabel* label, LinearLayout* propertiesLayout)
{
    if (propertiesLayout->objects().size() < 2)
        return;
    auto text = extractText(propertiesLayout, 1);
    if (text.empty())
        text = extractText(propertiesLayout, 0);
    label->setTextAndLoad(text);
}

void valueFromPropertiesSetter(StaticLabel* label, LinearLayout* propertiesLayout, const std::string& prefix)
{
    if (propertiesLayout->objects().size() < 1)
        return;
    label->setTextAndLoad(mergeStrings(prefix, extractText(propertiesLayout, 0)));
}

void mapElementFromPropertiesSetter(StaticLabel* label, LinearLayout* propertiesLayout)
{
    if (propertiesLayout->objects().size() < 2)
        return;
    label->setTextAndLoad(extractText(propertiesLayout, 0) + " -> " + extractText(propertiesLayout, 1));
}

void mapKeyFromPropertiesSetter(StaticLabel* label, LinearLayout* propertiesLayout, const std::string& suffix)
{
    if (propertiesLayout->objects().size() < 1)
        return;
    label->setTextAndLoad(extractText(propertiesLayout, 0) + " -> " + suffix);
}

void updateBoolProperty(TextList* textList, std::string name, Json::Value* data)
{
    setData(data, name, textList->text() == "true" ? true : false);
}

void updateEnumProperty(TextList* textList, std::string name, Json::Value* data)
{
    setData(data, name, textList->currentVariantID());
}
                
void serializeDefaultValue(Serializer& serializer, const std::string& name,
    const std::shared_ptr<Presentation>& presentation,
    const std::shared_ptr<IPropertyPresentation>& propertyPresentation)
{
    if (propertyPresentation->presentationType() == PropertyPresentation::Primitive) {
        switch (dynamic_cast<PrimitivePropertyPresentation*>(propertyPresentation.get())->type) {
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
        auto enumPropertyPresentation = dynamic_cast<EnumPropertyPresentation*>(propertyPresentation.get());
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
    snapshot->model.update(sourceID);
    const auto& fictiveData = snapshot->model.get(sourceID).data();
    if (!fictiveData.isMember(name))
        return;
    const auto& sourceData = fictiveData[name];
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
    TextList* textList,
    const std::vector<std::shared_ptr<TypePresentation>>& types,
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot)
{
    auto obj = types.at(textList->currentVariantID())->loadPatternValue();
    if (!obj)
        return;
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
    TextList* textList,
    const std::vector<std::shared_ptr<TypePresentation>>& types,
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot)
{
    addObjectFromPattern(textList, types, snapshot);
    updateView(snapshot);
}

void addElementToMap(
    int keySourceID, int keysArrayNodeID, int valuesArrayNodeID,
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot,
    const std::function<void()>& addValueFunc)
{
    snapshot->mapProperties = std::make_shared<DesignViewBuilder::MapProperties>();
    snapshot->mapProperties->elements.resize(
        *snapshot->properties->collectionSize, nullptr);
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
    TextList* textList,
    const std::vector<std::shared_ptr<TypePresentation>>& types,
    const std::shared_ptr<DesignViewBuilder::Snapshot>& snapshot)
{
    addElementToMap(keySourceID, keysArrayNodeID, valuesArrayNodeID, snapshot,
        std::bind(addObjectFromPattern, textList, types, snapshot));
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
    std::string fullName = propertyName(name);
    if (m_objTypes.back() == ObjType::PrimitiveArray) {
        fullName = fullName + PRIMITIVE_ARRAY_ELEMENT_SUFFIX.get(
            m_arrayTypes.back(), m_primitiveElementIndex++);
    }
    addProperty(propertyName(name), "double", boost::lexical_cast<std::string>(d),
        &updateProperty<double>);
}

void DesignViewBuilder::writeInt(const std::string& name, int i)
{
    if (name == COLLECTION_SIZE_TAG)
        return;

    bool isObject = parentObjType() == ObjType::Object;
    bool isMainPresentation = !(parentObjType() == ObjType::Map && m_arrayTypes.back() == SerializationTag::Keys);
    auto properties = currentPropertiesForPrimitive("int");
    std::shared_ptr<IPropertyPresentation> propertyPresentation;
    if (isObject) {
        if (properties->type)
            propertyPresentation = m_presentation->abstractPropertyByName(properties->type->name, name);
    } else {
        propertyPresentation = isMainPresentation
            ? properties->presentationFromParent : properties->keyPresentationFromParent;
    }
    if (propertyPresentation) {
        if (propertyPresentation->presentationType() == PropertyPresentation::Enum) {
            auto enumPropertyPresentation = dynamic_cast<EnumPropertyPresentation*>(propertyPresentation.get());
            if (auto enumPresentation = m_presentation->enumByName(enumPropertyPresentation->type)) {
                std::vector<std::string> enumValuesNames;
                std::vector<int> enumValues;
                for (auto it = enumPresentation->values.begin(); it != enumPresentation->values.end(); ++it) {
                    enumValues.push_back(it->first);
                    enumValuesNames.push_back(it->second);
                }
                auto layout = createPropertyLayout();
                layout->addObject(createLabel(propertyNameFromPresentation(propertyName(name))));
                auto textList = createTextList(enumValuesNames, enumValues);
                textList->setText(enumPresentation->values.at(i));
                layout->addObject(textList);
                properties->layout->addObject(layout);

                DesignModel::UpdateModelFunc modelUpdater = std::bind(
                    updateEnumProperty, textList.get(), name, std::placeholders::_1);
                m_model.get(m_curModelNodeID).updaters.push_back(modelUpdater);
                return;
            }
        }
    }
    addProperty(propertyName(name), boost::lexical_cast<std::string>(i),
        &updateProperty<int>, properties->layout.get());
}

void DesignViewBuilder::writeUInt(const std::string& name, unsigned int i)
{
    addProperty(propertyName(name), "unsigned int", boost::lexical_cast<std::string>(i),
        &updateProperty<unsigned int>);
}

void DesignViewBuilder::writeInt64(const std::string& name, int64_t i)
{
    addProperty(propertyName(name), "int64", boost::lexical_cast<std::string>(i),
        &updateProperty<int64_t>);
}

void DesignViewBuilder::writeUInt64(const std::string& name, uint64_t i)
{
    addProperty(propertyName(name), "unsigned int64", boost::lexical_cast<std::string>(i),
        &updateProperty<uint64_t>);
}

void DesignViewBuilder::writeBool(const std::string& name, bool b)
{
    static const char* TEXT_VARIANTS[] = { "false", "true" };
    static const std::vector<std::string> TEXT_VARIANTS_VEC(TEXT_VARIANTS, TEXT_VARIANTS + 2);

    if (name == EMPTY_TAG) {
        m_model.get(m_curModelNodeID).updaters.push_back(createConstUpdater(name, b));
        if (m_objTypes.back() == ObjType::Unknown) {
            m_properties.push_back(createProperties(m_curName, "empty"));
            m_objTypes.back() = ObjType::Object;
        }

        return;
    }

    auto layout = createPropertyLayout();
    layout->addObject(createLabel(propertyNameFromPresentation(propertyName(name))));
    auto textList = createTextList(TEXT_VARIANTS_VEC);
    textList->setText(b ? TEXT_VARIANTS[1] : TEXT_VARIANTS[0]);
    layout->addObject(textList);
    currentPropertiesForPrimitive("bool")->layout->addObject(layout);

    DesignModel::UpdateModelFunc modelUpdater = std::bind(
        updateBoolProperty, textList.get(), name, std::placeholders::_1);
    m_model.get(m_curModelNodeID).updaters.push_back(modelUpdater);
}

void DesignViewBuilder::writeString(const std::string& name, const std::string& value)
{
    if (name == TYPE_NAME_TAG) {
        m_model.get(m_curModelNodeID).updaters.push_back(createConstUpdater(name, value));
        m_properties.push_back(createProperties(m_curName, value));
        m_objTypes.back() = ObjType::Object;
        auto typePresentation = m_presentation->typeByName(value);
        auto presentationFromParent = m_properties.back()->presentationFromParent;
        if (auto* objectPresentation = dynamic_cast<ObjectPresentation*>(presentationFromParent.get())) {
            auto typesList = createTypesList("type", presentationFromParent);
            typesList.textList->setText(typePresentation->nameInUI);
        }
        return;
    }

    addProperty(propertyName(name), "string", value,
        &updateProperty<std::string>);
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
        props->collectionSize.reset(new int(0));
        m_model.get(prevModeNodeID).updaters.push_back(std::bind(
            collectionSizeUpdater, props->collectionSize, std::placeholders::_1));
        m_properties.push_back(props);
        if (auto arrayPresentation = dynamic_cast<ArrayPresentation*>(props->presentationFromParent.get())) {
            auto snapshot = std::make_shared<Snapshot>(*this, *props, ObjType::Array);
            auto addingButton = createButton(300.0f, 20.0f, "Add element");
            props->layout->addObject(addingButton);
            auto elementPresentation = arrayPresentation->elementType;
            if (elementPresentation->presentationType() == PropertyPresentation::Primitive
                || elementPresentation->presentationType() == PropertyPresentation::Enum) {
                auto fictiveNodeID = addFictiveNode("newElement", elementPresentation);
                addingButton->setCallback(std::bind(addPrimitiveValueToArray, fictiveNodeID, snapshot));
            }

            if (elementPresentation->presentationType() == PropertyPresentation::Object) {
                auto typesList = createTypesList("newElement", elementPresentation);
                if (!typesList.types.empty())
                    addingButton->setCallback(std::bind(
                        addObjectToArray, typesList.textList, typesList.types, snapshot));
                else
                    addingButton->setSelectionState(SelectionState::Disabled);
            }
        }
        m_objTypes.back() = ObjType::Array;
        m_arrayTypes.push_back(tag);
        return;
    }
    if (tag == SerializationTag::Keys) {
        auto props = createProperties(m_curName, "map");
        m_properties.push_back(props);
        props->collectionSize.reset(new int(0));
        m_model.get(prevModeNodeID).updaters.push_back(std::bind(
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
        if (auto mapPresentation = dynamic_cast<MapPresentation*>(props->presentationFromParent.get())) {
            auto snapshot = std::make_shared<Snapshot>(*this, *props, ObjType::Map);
            auto valuePresentation = mapPresentation->valueType;
            auto addingButton = createButton(300.0f, 20.0f, "Add element");
            props->layout->addObject(addingButton);
            auto fictiveKeyNodeID = addFictiveNode("newKey", mapPresentation->keyType);

            if (valuePresentation->presentationType() == PropertyPresentation::Primitive
                || valuePresentation->presentationType() == PropertyPresentation::Enum) {
                auto fictiveValueNodeID = addFictiveNode("newValue", valuePresentation);
                addingButton->setCallback(std::bind(addPrimitiveElementToMap,
                    fictiveKeyNodeID, fictiveValueNodeID,
                    m_mapProperties.back()->keysArrayNodeID, m_curModelNodeID, snapshot));
            }

            if (valuePresentation->presentationType() == PropertyPresentation::Object) {
                auto typesList = createTypesList("newValue", valuePresentation);
                if (!typesList.types.empty())
                    addingButton->setCallback(
                        std::bind(addObjectToMap,
                            fictiveKeyNodeID, m_mapProperties.back()->keysArrayNodeID,
                            m_curModelNodeID, typesList.textList, typesList.types, snapshot));
                else
                    addingButton->setSelectionState(SelectionState::Disabled);
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
    const std::function<void(TextEdit*, std::string, Json::Value*)>& updater)
{
    addProperty(name, initialValue, updater,
        currentPropertiesForPrimitive(typeName)->layout.get());
}

void DesignViewBuilder::addProperty(
    const std::string& name,
    const std::string& initialValue,
    const std::function<void(TextEdit*, std::string, Json::Value*)>& updater,
    LinearLayout* propertiesLayout)
{
    auto layout = createPropertyLayout();
    layout->addObject(createLabel(propertyNameFromPresentation(name)));
    auto textEdit = createTextEdit();
    textEdit->setName("value");
    textEdit->setText(initialValue);
    layout->addObject(textEdit);
    propertiesLayout->addObject(layout);

    DesignModel::UpdateModelFunc modelUpdater = std::bind(
        updater, textEdit.get(), name, std::placeholders::_1);
    m_model.get(m_curModelNodeID).updaters.push_back(modelUpdater);
}

std::shared_ptr<DesignViewBuilder::Properties> DesignViewBuilder::createPropertiesImpl(int parentID)
{
    auto buttonSkin = createSwitchButtonSkin();
    auto buttonLabel = createSwitchButtonLabel();
    buttonSkin->addElement(buttonLabel);
    auto button = std::make_shared<RadioButton>(nullptr, buttonSkin);
    auto props = std::make_shared<Properties>();
    props->id = m_treeView.addObject(parentID, button);
    props->switchButtonLabel = buttonLabel;
    props->layout = createPropertiesListLayout();
    m_propertiesMenu.addObject(props->id, props->layout);
    button->setIndexInGroup(props->id);
    button->setGroup(m_switchsGroup);
    return props;
}

std::shared_ptr<DesignViewBuilder::Properties> DesignViewBuilder::createProperties(
    const std::string& name, const std::string& typeName)
{
    int parentID = m_properties.back()->id;
    ObjType::Enum parentObj = parentObjType();
    auto typeNameInUI = typeName;
    if (auto typePresentation = m_presentation->typeByName(typeName)) {
        if (!typePresentation->nameInUI.empty())
            typeNameInUI = typePresentation->nameInUI;
    }

    if (parentObj == ObjType::Array) {
        (*m_properties.back()->collectionSize)++;
        auto props = createPropertiesImpl(parentID);
        if (typeName == "TypePresentation" || typeName == "EnumPresentation") {
            props->type = m_presentation->typeByName(typeName);
            props->buttonTextUpdater = std::bind(
                nameFromPresentationSetter, props->switchButtonLabel.get(), props->layout.get());
            return props;
        }

        if (auto parentPresentation = dynamic_cast<ArrayPresentation*>(m_properties.back()->presentationFromParent.get())) {
            auto thisPresentation = parentPresentation->elementType;
            if (thisPresentation->presentationType() == PropertyPresentation::Primitive
                || thisPresentation->presentationType() == PropertyPresentation::Enum) {
                props->buttonTextUpdater = std::bind(
                    &valueFromPropertiesSetter, props->switchButtonLabel.get(), props->layout.get(), "element");
            } else {
                if (thisPresentation->presentationType() == PropertyPresentation::Object)
                    props->type = m_presentation->typeByName(typeName);
                props->buttonTextUpdater = std::bind(
                    &textSetter, props->switchButtonLabel.get(), mergeStrings("element", typeNameInUI));
            }
            props->presentationFromParent = thisPresentation;
        } else {
            props->type = m_presentation->typeByName(typeName);
            props->buttonTextUpdater = std::bind(
                &textSetter, props->switchButtonLabel.get(), mergeStrings("element", typeNameInUI));
        }
        return props;
    }
    
    if (parentObj == ObjType::Map) {
        if (m_arrayTypes.back() == SerializationTag::Keys) {
            (*m_properties.back()->collectionSize)++;
            auto props = createPropertiesImpl(parentID);
            if (auto parentPresentation = dynamic_cast<MapPresentation*>(m_properties.back()->presentationFromParent.get())) {
                props->presentationFromParent = parentPresentation->valueType;
                props->keyPresentationFromParent = parentPresentation->keyType;
            }
            m_mapProperties.back()->elements.push_back(props);
            return props;
        }

        auto& mapProperties = m_mapProperties.back();
        auto props = mapProperties->elements.at(mapProperties->currentElem++);
        auto thisPresentation = props->presentationFromParent;
        if (thisPresentation) {
            if (thisPresentation->presentationType() == PropertyPresentation::Primitive
                || thisPresentation->presentationType() == PropertyPresentation::Enum) {
                props->buttonTextUpdater = std::bind(
                    &mapElementFromPropertiesSetter, props->switchButtonLabel.get(), props->layout.get());
            } else {
                if (thisPresentation->presentationType() == PropertyPresentation::Object)
                    props->type = m_presentation->typeByName(typeName);
                props->buttonTextUpdater = std::bind(
                    &mapKeyFromPropertiesSetter, props->switchButtonLabel.get(), props->layout.get(), typeNameInUI);
            }
        } else {
            props->type = m_presentation->typeByName(typeName);
            props->buttonTextUpdater = std::bind(
                &mapKeyFromPropertiesSetter, props->switchButtonLabel.get(), props->layout.get(), typeNameInUI);
        }

        return props;
    }
    auto props = createPropertiesImpl(parentID);
    props->type = m_presentation->typeByName(typeName);
    if (!name.empty() && !m_properties.empty() && m_properties.back()->type) {
        props->presentationFromParent = m_presentation->abstractPropertyByName(
            m_properties.back()->type->name, name);
    }
    std::string buttonText = mergeStrings(propertyNameFromPresentation(name), typeNameInUI);
    props->buttonTextUpdater = std::bind(&textSetter, props->switchButtonLabel.get(), buttonText);
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
    m_properties.pop_back();
}

std::string DesignViewBuilder::propertyNameFromPresentation(const std::string& name)
{
    if (!m_properties.empty()) {
        if (auto type = m_properties.back()->type) {
            if (auto propertyPresentation = m_presentation->abstractPropertyByName(type->name, name))
                return propertyPresentation->nameInUI;
        }
    }
    return name;
}

int DesignViewBuilder::addFictiveNode(
    const std::string& name,
    const std::shared_ptr<IPropertyPresentation>& elementPresentation)
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
    const std::shared_ptr<IPropertyPresentation>& propertyPresentation)
{
    DesignViewBuilder::TypesList result;
    auto objectPresentation = dynamic_cast<ObjectPresentation*>(propertyPresentation.get());
    result.types = m_presentation->derivedTypesByBaseTypeName(objectPresentation->baseType);
    if (result.types.empty())
        return result;
    std::vector<std::string> typesNames;
    for (auto it = result.types.begin(); it != result.types.end(); ++it)
        typesNames.push_back((*it)->nameInUI);
    if (objectPresentation->canBeEmpty)
        typesNames.push_back("None");

    auto layout = createPropertyLayout();
    layout->addObject(createLabel(label));
    auto textList = createTextList(typesNames);
    textList->setText(typesNames[0]);
    layout->addObject(textList);
    m_properties.back()->layout->addObject(layout);
    result.textList = textList.get();
    return result;
}

} }

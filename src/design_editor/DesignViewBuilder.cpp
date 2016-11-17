/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "DesignViewBuilder.h"
#include <dvb/Styles.h>
#include <dvb/PrimitiveArrayElementSuffix.h>
#include <dvb/Helpers.h>
#include <dvb/Operations.h>
#include "tools.h"
#include "Settings.h"
#include <gamebase/impl/serial/JsonDeserializer.h>
#include <json/value.h>
#include <fstream>

namespace gamebase { namespace editor {

DesignViewBuilder::DesignViewBuilder(
    TreeView& treeView,
    Selector propertiesMenu,
    DesignModel& model,
    const std::shared_ptr<Presentation>& presentation,
    const std::shared_ptr<PropsMenuToolBar>& toolBar,
    Layout propertiesMenuArea,
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

    m_context->switchsGroup.setCallback(
        std::bind(&SharedContext::onSelection, m_context.get()));
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
    if (name == impl::COLLECTION_SIZE_TAG)
        return;

    bool isObject = parentObjType() == ObjType::Object;
    bool isMainPresentation = !(parentObjType() == ObjType::Map && m_arrayTypes.back() == impl::SerializationTag::Keys);
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
                layout.add(createLabel(propertyNameFromPresentation(propertyName(name))));
                auto comboBox = createComboBox(enumValuesNames, enumValues);
                comboBox.setText(enumPresentation->values.at(i));
                layout.add(comboBox);
                properties->layout.add(layout);

                DesignModel::UpdateModelFunc modelUpdater = std::bind(
                    updateEnumProperty, comboBox, name, std::placeholders::_1);
                m_context->model.addUpdater(m_curModelNodeID, modelUpdater);
                comboBox.setCallback(properties->buttonTextUpdater);
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

    if (name == impl::EMPTY_TAG) {
        if (m_objTypes.back() != ObjType::Unknown)
            return;

        m_properties.push_back(createProperties(m_curName, EMPTY_TYPE_NAME));
        m_objTypes.back() = ObjType::Object;

        auto presentationFromParent = m_properties.back()->presentationFromParent;
        auto typesList = createTypesList(g_textBank.get("type"), presentationFromParent, noneLabel());
        auto comboBox = typesList.comboBox;
        auto textVariants = comboBox.variants();
        comboBox.setText(noneLabel());
        if (textVariants.size() > 1
            || (textVariants.size() == 1 && textVariants.front() != noneLabel())) {
            createObjectReplaceCallbacks(typesList);
        } else {
            m_context->model.addUpdater(m_curModelNodeID, createConstUpdater(impl::EMPTY_TAG, true));
        }
        return;
    }

    auto layout = createPropertyLayout();
    layout.add(createLabel(propertyNameFromPresentation(propertyName(name))));
    auto comboBox = createComboBox(TEXT_VARIANTS_VEC);
    comboBox.setText(b ? TEXT_VARIANTS[1] : TEXT_VARIANTS[0]);
    layout.add(comboBox);
    auto properties = currentPropertiesForPrimitive("bool");
    properties->layout.add(layout);

    DesignModel::UpdateModelFunc modelUpdater = std::bind(
        updateBoolProperty, comboBox, name, std::placeholders::_1);
    m_context->model.addUpdater(m_curModelNodeID, modelUpdater);
    comboBox.setCallback(properties->buttonTextUpdater);
    if (name.empty())
        properties->buttonTextUpdater();
}

void DesignViewBuilder::writeString(const std::string& name, const std::string& value)
{
    if (name == impl::TYPE_NAME_TAG) {
        m_properties.push_back(createProperties(m_curName, value));
        m_objTypes.back() = ObjType::Object;

        auto typePresentation = m_context->presentation->typeByName(value);
        auto presentationFromParent = m_properties.back()->presentationFromParent;
        auto typesList = createTypesList(g_textBank.get("type"), presentationFromParent, value);
        auto thisTypeName = value;
        auto comboBox = typesList.comboBox;
        auto textVariants = comboBox.variants();
        if (typePresentation) {
            auto it = std::find(textVariants.begin(), textVariants.end(),
                typePresentation->nameInUI);
            if (it != textVariants.end())
                thisTypeName = typePresentation->nameInUI;
        }
        comboBox.setText(thisTypeName);
        if (textVariants.size() > 1
            || (textVariants.size() == 1 && textVariants.front() != thisTypeName)) {
            createObjectReplaceCallbacks(typesList);
        } else {
            m_context->model.addUpdater(m_curModelNodeID, createConstUpdater(name, value));
            m_context->model.addUpdater(m_curModelNodeID, createConstUpdater(impl::EMPTY_TAG, false));
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

void DesignViewBuilder::startArray(const std::string& name, impl::SerializationTag::Type tag)
{
    auto prevModeNodeID = m_curModelNodeID;
    m_curModelNodeID = m_context->model.add(m_curModelNodeID, DesignModel::Node::Array, name).id;
    if (tag == impl::SerializationTag::Array) {
        auto props = createProperties(m_curName, "array");
        addStaticTypeLabel(props->layout, g_textBank.get("array"));
        props->collectionSize.reset(new int(0));
        m_context->model.addUpdater(prevModeNodeID, std::bind(
            collectionSizeUpdater, props->collectionSize, std::placeholders::_1));
        m_properties.push_back(props);
        if (auto arrayPresentation = dynamic_cast<const ArrayPresentation*>(props->presentationFromParent)) {
            auto snapshot = std::make_shared<Snapshot>(*this, *props, ObjType::Array);
            auto* elementPresentation = arrayPresentation->elementType.get();
            if (elementPresentation->presentationType() == PropertyPresentation::Primitive
                || elementPresentation->presentationType() == PropertyPresentation::Enum) {
                auto fictiveNodeID = addFictiveNode("newValue", elementPresentation);
                m_context->nodes[props->id].callbacks[ButtonKey::Add] = std::bind(
                    addPrimitiveValueToArray, fictiveNodeID, snapshot);
            }

            if (elementPresentation->presentationType() == PropertyPresentation::Object) {
                auto typesList = createTypesList(g_textBank.get("newValue"), elementPresentation);
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
    if (tag == impl::SerializationTag::Keys) {
        auto props = createProperties(m_curName, "map");
        addStaticTypeLabel(props->layout, g_textBank.get("map"));
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

    if (tag == impl::SerializationTag::Values) {
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
                auto typesList = createTypesList(g_textBank.get("newValue"), valuePresentation.get());
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
    const std::function<void(TextBox, std::string, Json::Value*)>& updater)
{
    auto properties = currentPropertiesForPrimitive(typeName);
    addProperty(propertyName(name), initialValue, updater, properties.get());
    if (name.empty())
        properties->buttonTextUpdater();
}

void DesignViewBuilder::addProperty(
    const std::string& name,
    const std::string& initialValue,
    const std::function<void(TextBox, std::string, Json::Value*)>& updater,
    Properties* properties)
{
    auto layout = createPropertyLayout();
    layout.add(createLabel(propertyNameFromPresentation(name)));
    auto textBox = createTextBox();
    textBox.setText(initialValue);
    textBox.setCallback(properties->buttonTextUpdater);
    layout.add(textBox);
    properties->layout.add(layout);

    DesignModel::UpdateModelFunc modelUpdater = std::bind(
        updater, textBox, name, std::placeholders::_1);
    m_context->model.addUpdater(m_curModelNodeID, modelUpdater);
}

std::shared_ptr<Properties> DesignViewBuilder::createPropertiesImpl(int parentID)
{
    auto button = loadObj<RadioButton>("ui\\SwitchButton.json");
    auto props = std::make_shared<Properties>();
    props->id = m_context->treeView.addObject(parentID, button.getImpl().getShared());
    props->switchButtonLabel = button.child<Label>("label");
    auto layout = loadObj<Layout>("ui\\PropertiesLayout.json");
    m_context->propertiesMenu.insert(props->id, layout);
    m_context->switchsGroup.insert(props->id, button);
    props->layout = makeRaw(layout);
    return props;
}

std::shared_ptr<Properties> DesignViewBuilder::createProperties(
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
                nameForPresentationSetter, props->switchButtonLabel, props->layout);
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
            &nameFromPropertiesSetter, props->switchButtonLabel, props->layout, g_textBank.get("element"), 0);
        return props;
    }
    
    if (parentObj == ObjType::Map) {
        if (m_arrayTypes.back() == impl::SerializationTag::Keys) {
            (*curCollectionSize)++;
            auto props = createPropertiesImpl(parentID);
            props->buttonTextUpdater = std::bind(
                &mapElementNameFromPropertiesSetter, props->switchButtonLabel, props->layout);
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
            snapshot->arrayType = impl::SerializationTag::Values;
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
    if (typeName != "array" && typeName != "map") {
        props->type = m_context->presentation->typeByName(typeName);
    }
    if (!name.empty() && !m_properties.empty() && m_properties.back()->type) {
        props->presentationFromParent = m_context->presentation->propertyByName(
            m_properties.back()->type->name, name);
    }
    props->buttonTextUpdater = std::bind(
        &nameFromPropertiesSetter, props->switchButtonLabel, props->layout,
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
            return g_textBank.get("element");
        if (parentObj == ObjType::Map) {
            impl::SerializationTag::Type arrayType = m_arrayTypes.back();
            if (m_objTypes.back() == ObjType::PrimitiveArray)
                arrayType = *(m_arrayTypes.rbegin() + 1);
            if (arrayType == impl::SerializationTag::Keys)
                return g_textBank.get("key");
            if (arrayType == impl::SerializationTag::Values)
                return g_textBank.get("value");
            THROW_EX() << "Bad map's serialialization tag: " << arrayType;
        }
        return m_curName;
    }
    return nameFromSerializer;
}

std::shared_ptr<Properties> DesignViewBuilder::currentPropertiesForPrimitive(
    const std::string& typeName)
{
    auto parent = parentObjType();
    if (parent == ObjType::Array || parent == ObjType::Map)
        return createProperties("", typeName);
    return m_properties.back();
}

ObjType::Enum DesignViewBuilder::parentObjType() const
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

std::shared_ptr<Properties> DesignViewBuilder::currentProperties()
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
    if (name == "newKey" || name == "newValue")
        return g_textBank.get(name);
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
    impl::Serializer serializer(this);
    serializeDefaultValue(serializer, name, m_context->presentation, elementPresentation);
                
    // restore state
    props->presentationFromParent = presentationFromParent;
    m_curModelNodeID = modelNodeID;
    return fictiveNodeID;
}

TypesList DesignViewBuilder::createTypesList(
    const std::string& label,
    const IPropertyPresentation* propertyPresentation,
    const std::string& variantIfNoPresentation)
{
    TypesList result;
    std::vector<std::string> typesNames;
    auto objectPresentation = dynamic_cast<const ObjectPresentation*>(propertyPresentation);
    if (objectPresentation) {
        result.types = m_context->presentation->derivedTypesByBaseTypeName(objectPresentation->baseType);
        for (auto it = result.types.begin(); it != result.types.end(); ++it)
            typesNames.push_back((*it)->nameInUI);
        if (objectPresentation->canBeEmpty)
            typesNames.push_back(noneLabel());
    }

    if (typesNames.empty())
        typesNames.push_back(variantIfNoPresentation);

    auto layout = createPropertyLayout();
    layout.add(createLabel(label));
    auto comboBox = createComboBox(typesNames);
    comboBox.setText(typesNames[0]);
    layout.add(comboBox);
    auto propertiesLayout = m_properties.back()->layout;
    result.indexInLayout = propertiesLayout.size();
    propertiesLayout.add(layout);
    result.comboBox = makeRaw(comboBox);
    return result;
}

void DesignViewBuilder::createObjectReplaceCallbacks(TypesList& typesList)
{
    const auto& props = *m_properties.back();
    m_context->model.addUpdater(m_curModelNodeID, std::bind(updateTypeTag, typesList, std::placeholders::_1));
    m_context->model.addUpdater(m_curModelNodeID, std::bind(updateEmptyTag, typesList, std::placeholders::_1));
    auto snapshot = std::make_shared<Snapshot>(*this, props, ObjType::Object);
    typesList.comboBox.setCallback(std::bind(
        replaceObjectWithPattern, typesList, snapshot,
        m_context->model.get(m_curModelNodeID).updatersNum(),
        typesList.comboBox));
}

void DesignViewBuilder::addStaticTypeLabel(
    Layout propertiesLayout, const std::string& typeName)
{
    auto layout = createPropertyLayout();
    layout.add(createLabel(g_textBank.get("type")));
    layout.add(createRightLabel(typeName));
    propertiesLayout.add(layout);
}

} }

/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "DesignViewBuilder.h"
#include <dvb/Styles.h>
#include <dvb/PrimitiveArrayElementSuffix.h>
#include <dvb/Helpers.h>
#include <dvb/Operations.h>
#include <dvb/ColorDialog.h>
#include "tools.h"
#include "Settings.h"
#include <gamebase/serial/LoadObj.h>
#include <json/value.h>
#include <fstream>

namespace gamebase { namespace editor {

namespace {
struct HiddenLevel {
    HiddenLevel() : level(nullptr) {}
    void init(int* level) { this->level = level; ++(*level); }
    ~HiddenLevel() { if (level) --(*level); }
    operator bool() const { return level != nullptr; }
    int* level;
};
}

DesignViewBuilder::DesignViewBuilder(
    TreeView& treeView,
    Selector propertiesMenu,
    DesignModel& model,
    const std::shared_ptr<Presentation>& presentation,
    const std::shared_ptr<PropsMenuToolBar>& toolBar,
    Layout propertiesMenuArea,
    int rootID)
    : m_context(std::make_shared<SharedContext>(treeView, propertiesMenu, model))
    , m_levelOfHidden(0)
{
    m_context->presentation = presentation;
    m_context->toolBar = toolBar;
    m_context->propertiesMenuArea = propertiesMenuArea;

    auto props = std::make_shared<Properties>();
    props->id = rootID;
    m_properties.push_back(props);
    m_curModelNodeID = DesignModel::ROOT_ID;

    m_context->switchsGroup.setCallback(
		[context = m_context.get()]() { context->onSelection(); });
    model.setFlusher([context = m_context]() { context->sync(); });
}

DesignViewBuilder::DesignViewBuilder(Snapshot& snapshot)
    : m_context(snapshot.context)
    , m_curName(snapshot.curName)
    , m_curModelNodeID(snapshot.modelNodeID)
    , m_levelOfHidden(snapshot.levelOfHidden)
{
    m_objTypes.push_back(snapshot.objType);
    if (snapshot.arrayType)
        m_arrayTypes.push_back(*snapshot.arrayType);
    m_properties.push_back(snapshot.properties);
}

DesignViewBuilder::~DesignViewBuilder() {}

void DesignViewBuilder::writeFloat(const std::string& name, float f)
{
    writeDouble(name, static_cast<double>(f));
}

void DesignViewBuilder::writeDouble(const std::string& name, double d)
{
    if (m_objTypes.back() == ObjType::PrimitiveArray
		&& m_arrayTypes.back() == impl::SerializationTag::GLColor) {
		if (m_primitiveElementIndex == 0) {
			auto properties = currentPropertiesForPrimitive(name, "color");
			HiddenLevel hiddenLevel;
			if (parentObjType() == ObjType::Object && properties->type) {
				auto propertyPresentation = m_context->presentation->propertyByName(
					properties->type->name, name);
				if (propertyPresentation
					&& !IVisibilityCondition::allowShow(propertyPresentation->visibilityCond, *m_context, m_properties.back()))
					hiddenLevel.init(&m_levelOfHidden);
			}
            setupProperty(properties.get(), name, createColorProperty());
		}
		auto properties = currentPropertiesForPrimitive(name, "color");
        auto colorProperty = dynamic_cast<IColorProperty*>(properties->list.back().get());
        if (!colorProperty)
            THROW_EX() << "Inconsistent state, expected color property as current";
        colorProperty->setColorComponent(m_primitiveElementIndex, static_cast<float>(d));
		++m_primitiveElementIndex;
		return;
    }

    addProperty(name, "double", createSimpleProperty(d));
	++m_primitiveElementIndex;
}

void DesignViewBuilder::writeInt(const std::string& name, int i)
{
    if (name == impl::COLLECTION_SIZE_TAG)
        return;

    bool isObject = parentObjType() == ObjType::Object;
    bool isMainPresentation = !(parentObjType() == ObjType::MapElement && name == impl::MAP_KEY_TAG);
    auto properties = currentPropertiesForPrimitive(name, "int");
    const IPropertyPresentation* propertyPresentation = nullptr;
    if (isObject) {
        if (properties->type)
            propertyPresentation = m_context->presentation->propertyByName(properties->type->name, name);
    } else {
        propertyPresentation = isMainPresentation
            ? properties->presentationFromParent : properties->keyPresentationFromParent;
    }
    HiddenLevel hiddenLevel;
    if (propertyPresentation) {
        if (!IVisibilityCondition::allowShow(propertyPresentation->visibilityCond, *m_context, m_properties.back()))
            hiddenLevel.init(&m_levelOfHidden);
        if (propertyPresentation->presentationType() == PropertyPresentation::Enum) {
            auto enumPropertyPresentation = dynamic_cast<const EnumPropertyPresentation*>(propertyPresentation);
            if (auto enumPresentation = m_context->presentation->enumByName(enumPropertyPresentation->type)) {
                setupProperty(properties.get(), name,
                    createEnumProperty(enumPresentation, i));
                return;
            }
        }
    }
    addProperty(properties.get(), propertyName(name), createSimpleProperty(i));
    if (name.empty())
        properties->updateLabel();
}

void DesignViewBuilder::writeUInt(const std::string& name, unsigned int i)
{
    addProperty(name, "unsigned int", createSimpleProperty(i));
}

void DesignViewBuilder::writeInt64(const std::string& name, int64_t i)
{
    addProperty(name, "int64", createSimpleProperty(i));
}

void DesignViewBuilder::writeUInt64(const std::string& name, uint64_t i)
{
    addProperty(name, "unsigned int64", createSimpleProperty(i));
}

void DesignViewBuilder::writeBool(const std::string& name, bool b)
{
    if (name == impl::EMPTY_TAG) {
        if (!b)
            return;
        if (m_objTypes.back() != ObjType::Unknown)
            return;

        if (auto* objPresentation = dynamic_cast<const ObjectPresentation*>(presentationFromParent(m_curName))) {
            if (!objPresentation->canBeEmpty) {
                auto defaultObj = m_context->presentation->loadPattern(objPresentation->baseType);
                if (defaultObj) {
                    insertObjBody(*this, defaultObj);
                    return;
                }
            }
        }

        auto props = createProperties(m_curName, EMPTY_TYPE_NAME);
        m_properties.push_back(props);
        m_objTypes.back() = ObjType::Object;

        const IPropertyPresentation* presentationFromParent = nullptr;
        if (m_properties.back()->isInline)
            std::cerr << "Warning: inlined object is empty" << std::endl;
        else
            presentationFromParent = props->presentationFromParent;
        setupClassNameProperty(props, createEmptyClassNameProperty(
            m_context->presentation.get(), presentationFromParent));
        return;
    }

    auto properties = currentPropertiesForPrimitive(name, "bool");
    HiddenLevel hiddenLevel;
    if (parentObjType() == ObjType::Object && properties->type) {
        auto propertyPresentation = m_context->presentation->propertyByName(
            properties->type->name, name);
        if (propertyPresentation
            && !IVisibilityCondition::allowShow(propertyPresentation->visibilityCond, *m_context, m_properties.back()))
            hiddenLevel.init(&m_levelOfHidden);
    }
    setupProperty(properties.get(), name, createBoolProperty(b));
}

void DesignViewBuilder::writeString(const std::string& name, const std::string& value)
{
    if (name == impl::TYPE_NAME_TAG) {
        auto props = createProperties(m_curName, value);
        m_properties.push_back(props);
        m_objTypes.back() = ObjType::Object;

        const IPropertyPresentation* presentationFromParent = !props->isInline
            ? props->presentationFromParent : nullptr;
        auto typePresentation = m_context->presentation->typeByName(value);
        setupClassNameProperty(props, createClassNameProperty(
            m_context->presentation.get(), presentationFromParent, typePresentation, value));
        return;
    }

	bool isObject = parentObjType() == ObjType::Object;
    bool isMainPresentation = !(parentObjType() == ObjType::MapElement && name == impl::MAP_KEY_TAG);
    auto properties = currentPropertiesForPrimitive(name, "string");
	const IPropertyPresentation* propertyPresentation = nullptr;
    if (isObject) {
        if (properties->type)
            propertyPresentation = m_context->presentation->propertyByName(properties->type->name, name);
    } else {
        propertyPresentation = isMainPresentation
            ? properties->presentationFromParent : properties->keyPresentationFromParent;
    }
    HiddenLevel hiddenLevel;
    if (propertyPresentation) {
        if (!IVisibilityCondition::allowShow(propertyPresentation->visibilityCond, *m_context, m_properties.back()))
            hiddenLevel.init(&m_levelOfHidden);
		if (propertyPresentation->presentationType() == PropertyPresentation::SpecialString) {
			auto stringPresentation = dynamic_cast<const SpecialStringPresentation*>(propertyPresentation);
			
			switch (stringPresentation->type) {
			case SpecialString::Font:
                setupProperty(properties.get(), name, createFontProperty(value));
                break;

			case SpecialString::ImagePath:
                setupProperty(properties.get(), name, createImagePathProperty(value));
				break;

            case SpecialString::SoundPath:
                setupProperty(properties.get(), name, createSoundPathProperty(value));
                break;
            }
            return;
        }
    }

    addProperty(properties.get(), propertyName(name), createSimpleProperty(value));
    if (name.empty())
        properties->updateLabel();
}

void DesignViewBuilder::startObject(const std::string& name)
{
    m_curName = name;
    if (parentObjType() == ObjType::Map) {
        m_objTypes.push_back(ObjType::MapElement);
        m_curModelNodeID = m_context->model.add(m_curModelNodeID, DesignModel::Node::Object, name).id;
        
        int parentID = m_properties.back()->id;
        auto props = createPropertiesImpl(parentID);
        props->setLabelUpdater(
            [treeView = &m_context->treeView, props = props.get()](Label label)
        {
            mapElementNameFromPropertiesSetter(treeView, label, props);
        });
        if (auto parentPresentation = dynamic_cast<const MapPresentation*>(m_properties.back()->presentationFromParent)) {
            props->presentationFromParent = parentPresentation->valueType.get();
            props->keyPresentationFromParent = parentPresentation->keyType.get();
        }
        m_properties.push_back(props);

        {
            auto snapshot = std::make_shared<Snapshot>(*this, props, ObjType::MapElement);
            m_context->nodes[props->id].callbacks[ButtonKey::Remove] =
                [snapshot]() { removeMapElement(snapshot); };
        }
        return;
    }

    m_objTypes.push_back(ObjType::Unknown);
    m_curModelNodeID = m_context->model.add(m_curModelNodeID, DesignModel::Node::Object, name).id;
}

void DesignViewBuilder::finishObject()
{
    auto objType = m_objTypes.back();
    if ((objType == ObjType::Object && parentObjType() != ObjType::MapElement)
        || objType == ObjType::MapElement)
        finishCurrentProperties();
    m_objTypes.pop_back();
    m_curModelNodeID = m_context->model.get(m_curModelNodeID).parentID;
}

void DesignViewBuilder::startArray(const std::string& name, impl::SerializationTag::Type tag)
{
    if (tag == impl::SerializationTag::Keys || tag == impl::SerializationTag::Values)
        THROW_EX() << "Array in field " << name << " has unsupported legacy tag";

    m_curName = name;
    m_curModelNodeID = m_context->model.add(m_curModelNodeID, DesignModel::Node::Array, name).id;
    if (tag == impl::SerializationTag::Array) {
        auto props = createProperties(m_curName, "array");
        addStaticTypeLabel(props.get(), g_textBank.get("array"));
        m_properties.push_back(props);
        if (auto arrayPresentation = dynamic_cast<const ArrayPresentation*>(props->presentationFromParent)) {
            auto snapshot = std::make_shared<Snapshot>(*this, props, ObjType::Array);
            auto* elementPresentation = arrayPresentation->elementType.get();
            if (elementPresentation->presentationType() == PropertyPresentation::Primitive
                || elementPresentation->presentationType() == PropertyPresentation::Enum) {
                auto elementProperty = addFictiveProperty("newValue", elementPresentation);
				m_context->nodes[props->id].callbacks[ButtonKey::Add] =
					[elementProperty, snapshot]() { addPrimitiveValueToArray(elementProperty, snapshot); };
            }

            if (elementPresentation->presentationType() == PropertyPresentation::Object) {
                auto fictiveProperty = createClassNameProperty(
                    m_context->presentation.get(), elementPresentation, nullptr, "");
                fictiveProperty->setFictive(true);
                fictiveProperty->setName("newValue");
                fictiveProperty->setNameUI(g_textBank.get("newValue"));
                props->list.push_back(fictiveProperty);
				m_context->nodes[props->id].callbacks[ButtonKey::Add] =
					[classNameProperty = fictiveProperty.get(), snapshot]()
                {
                    addObjectToArray(classNameProperty, snapshot);
                };

				std::function<void(const std::string&)> pathProcessor =
					[snapshot](const std::string& path) { addObjectFromFileToArray(path, snapshot); };
				m_context->nodes[props->id].callbacks[ButtonKey::AddFromFile] =
					[pathProcessor]() { initLocalDesignPathDialog(pathProcessor, ExtFilePathDialog::Config::Open); };
				m_context->nodes[props->id].callbacks[ButtonKey::AddFromClipboard] =
					[snapshot]() { addObjectFromClipboardToArray(snapshot); };
            }
        }
        m_objTypes.push_back(ObjType::Array);
        m_arrayTypes.push_back(tag);
        return;
    }

    if (tag == impl::SerializationTag::Map) {
        auto props = createProperties(m_curName, "map");
        addStaticTypeLabel(props.get(), g_textBank.get("map"));
        m_properties.push_back(props);

        if (auto mapPresentation = dynamic_cast<const MapPresentation*>(props->presentationFromParent)) {
            auto snapshot = std::make_shared<Snapshot>(*this, props, ObjType::Map);
            auto valuePresentation = mapPresentation->valueType;
            auto keyProperty = addFictiveProperty("newKey", mapPresentation->keyType.get());
            if (valuePresentation->presentationType() == PropertyPresentation::Primitive
                || valuePresentation->presentationType() == PropertyPresentation::Enum) {
                auto valueProperty = addFictiveProperty("newValue", valuePresentation.get());
				m_context->nodes[props->id].callbacks[ButtonKey::Add] =
					[keyProperty, valueProperty, snapshot]()
				{
					addPrimitiveElementToMap(keyProperty, valueProperty, snapshot);
				};
            }

            if (valuePresentation->presentationType() == PropertyPresentation::Object) {
                auto fictiveProperty = createClassNameProperty(
                    m_context->presentation.get(), valuePresentation.get(), nullptr, "");
                fictiveProperty->setFictive(true);
                fictiveProperty->setName("newValue");
                fictiveProperty->setNameUI(g_textBank.get("newValue"));
                props->list.push_back(fictiveProperty);
                auto classNameProperty = fictiveProperty.get();

                m_context->nodes[props->id].callbacks[ButtonKey::Add] =
				[keyProperty, classNameProperty, snapshot]()
				{
					addObjectToMap(keyProperty, classNameProperty, snapshot);
				};

                std::function<void(const std::string&)> pathProcessor =
				[keyProperty, snapshot]
				(const std::string& path)
				{
					addObjectFromFileToMap(keyProperty, path, snapshot);
				};
                    
				m_context->nodes[props->id].callbacks[ButtonKey::AddFromFile] =
					[pathProcessor]() { initLocalDesignPathDialog(pathProcessor, ExtFilePathDialog::Config::Open); };
                m_context->nodes[props->id].callbacks[ButtonKey::AddFromClipboard] =
				[keyProperty, snapshot]()
				{
					addObjectFromClipboardToMap(keyProperty, snapshot);
				};
            }
        }
        m_objTypes.push_back(ObjType::Map);
        m_arrayTypes.push_back(tag);
        return;
    }

    m_objTypes.push_back(ObjType::PrimitiveArray);
    auto parent = parentObjType();
    if (parent == ObjType::Array || parent == ObjType::MapElement)
        m_properties.push_back(createProperties("", "primitive array"));
    m_primitiveElementIndex = 0;
    m_arrayTypes.push_back(tag);
}

void DesignViewBuilder::finishArray()
{
    if (m_objTypes.back() == ObjType::PrimitiveArray) {
        m_objTypes.pop_back();
        if (m_objTypes.back() == ObjType::Array || m_objTypes.back() == ObjType::MapElement)
            finishCurrentProperties();
    } else {
        finishCurrentProperties();
        m_objTypes.pop_back();
    }
    m_arrayTypes.pop_back();
    m_curModelNodeID = m_context->model.get(m_curModelNodeID).parentID;
}

void DesignViewBuilder::addProperty(
    const std::string& name,
    const std::string& typeName,
    const std::shared_ptr<IProperty>& prop)
{
    auto properties = currentPropertiesForPrimitive(name, typeName);
    HiddenLevel hiddenLevel;
	auto propName = propertyName(name);
    if (parentObjType() == ObjType::Object && properties->type) {
        auto propertyPresentation = m_context->presentation->propertyByName(
            properties->type->name, propName);
        if (propertyPresentation
			&& !IVisibilityCondition::allowShow(propertyPresentation->visibilityCond, *m_context, m_properties.back())) {
			hiddenLevel.init(&m_levelOfHidden);
		}
    }
    addProperty(properties.get(), propName, prop);
    if (name.empty())
        properties->updateLabel();
}

void DesignViewBuilder::addProperty(
    Properties* properties,
    const std::string& name,
    const std::shared_ptr<IProperty>& prop)
{
	auto fullName = propertyNameFromPresentation(name);
	if (m_objTypes.back() == ObjType::PrimitiveArray)
		fullName += PRIMITIVE_ARRAY_ELEMENT_SUFFIX.get(m_arrayTypes.back(), m_primitiveElementIndex);

    prop->setName(name);
    prop->setNameUI(fullName);
    prop->setHidden(isHidden());
    if (m_objTypes.back() != ObjType::FictiveObject) {
        prop->setModelNodeID(m_curModelNodeID);
        prop->addUpdater(m_context->model);
    }
    properties->list.push_back(prop);
}

std::shared_ptr<Properties> DesignViewBuilder::createPropertiesImpl(int parentID, bool isInline)
{
    auto props = std::make_shared<Properties>();
    if (isInline) {
        props->id = parentID;
        props->isInline = true;
        m_properties.back()->inlined.push_back(props);
    } else {
        if (isHidden()) {
            props->id = m_context->treeView.genID();
        } else {
            auto button = loadObj<RadioButton>("ui\\SwitchButton.json");
            props->id = m_context->treeView.addObject(parentID, button.getImpl().getShared());
            props->setLabel(button.child<Label>("label"));
            m_context->switchsGroup.insert(props->id, button);
        }
        m_context->nodes[props->id].props = props;
    }
    return props;
}

const IPropertyPresentation* DesignViewBuilder::presentationFromParent(
    const std::string& name) const
{
    if (m_properties.empty())
        return nullptr;
    ObjType::Enum parentObj = parentObjType();
    if (parentObj == ObjType::Array) {
        if (auto parentPresentation = dynamic_cast<const ArrayPresentation*>(m_properties.back()->presentationFromParent))
            return parentPresentation->elementType.get();
        return nullptr;
    }
    if (parentObj == ObjType::MapElement) {
        if (auto parentPresentation = dynamic_cast<const MapPresentation*>(m_properties.back()->presentationFromParent))
            return parentPresentation->valueType.get();
        return nullptr;
    }
    if (!name.empty() && m_properties.back()->type) {
        return m_context->presentation->propertyByName(
            m_properties.back()->type->name, name);
    }
    return nullptr;
}

std::shared_ptr<Properties> DesignViewBuilder::createProperties(
    const std::string& name, const std::string& typeName)
{
    int parentID = m_properties.back()->id;
    ObjType::Enum parentObj = parentObjType();

    if (parentObj == ObjType::Array) {
        auto props = createPropertiesImpl(parentID);

        {
            auto snapshot = std::make_shared<Snapshot>(*this, props, ObjType::Array);
            if (m_objTypes.back() != ObjType::Unknown && m_objTypes.back() != ObjType::PrimitiveArray)
                snapshot->modelNodeID = m_context->model.nextID();
			m_context->nodes[props->id].callbacks[ButtonKey::Remove] =
				[snapshot]() { removeArrayElement(snapshot); };
			m_context->nodes[props->id].callbacks[ButtonKey::Down] =
				[model = &m_context->model, treeView = &m_context->treeView,
				modelNodeID = snapshot->modelNodeID, id = props->id]()
			{
				moveArrayElementDown(model, treeView, modelNodeID, id);
			};
			m_context->nodes[props->id].callbacks[ButtonKey::Up] =
				[model = &m_context->model, treeView = &m_context->treeView,
				modelNodeID = snapshot->modelNodeID, id = props->id]()
			{
				moveArrayElementUp(model, treeView, modelNodeID, id);
			};
        }

        if (typeName == "TypePresentation" || typeName == "EnumPresentation") {
            props->type = m_context->presentation->typeByName(typeName);
			props->setLabelUpdater(
				[treeView = &m_context->treeView, props = props.get()](Label label)
			{
				nameForPresentationSetter(treeView, label, props);
			});
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

            auto snapshot = std::make_shared<Snapshot>(*this, m_properties.back(), ObjType::Array);
            snapshot->modelNodeID = m_context->model.get(m_curModelNodeID).parentID;

            std::function<void(const std::string&)> pathProcessor =
				[snapshot, modelNodeID = m_curModelNodeID, id = props->id](const std::string& path)
			{
				replaceArrayElementFromFile(path, snapshot, modelNodeID, id);
			};
			m_context->nodes[props->id].callbacks[ButtonKey::ReplaceFromFile] =
				[pathProcessor]() { initLocalDesignPathDialog(pathProcessor, ExtFilePathDialog::Config::Open); };
            m_context->nodes[props->id].callbacks[ButtonKey::Paste] =
				[snapshot, modelNodeID = m_curModelNodeID, id = props->id]()
			{
				pasteArrayElement(snapshot, modelNodeID, id);
			};

            createObjectCallbacks(props->id);
        }
        props->setLabelUpdater(
			[treeView = &m_context->treeView, props = props.get(),
            elementStr = g_textBank.get("element")](Label label)
		{
			nameFromPropertiesSetter(treeView, label, props, elementStr, 0);
		});
        return props;
    }
    
    if (parentObj == ObjType::MapElement) {
        const auto& props = m_properties.back();
        if (name != impl::MAP_VALUE_TAG)
            return props;

        PropertyPresentation::Type propertyType = props->presentationFromParent
            ? props->presentationFromParent->presentationType()
            : PropertyPresentation::Object;
        if (propertyType == PropertyPresentation::Object) {
            props->type = m_context->presentation->typeByName(typeName);

            auto snapshot = std::make_shared<Snapshot>(*this, props, ObjType::MapElement);
            snapshot->modelNodeID = m_context->model.get(m_curModelNodeID).parentID;

            std::function<void(const std::string&)> pathProcessor =
				[snapshot, modelNodeID = m_curModelNodeID](const std::string& path)
			{
				replaceMapElementFromFile(path, snapshot, modelNodeID);
			};
			m_context->nodes[props->id].callbacks[ButtonKey::ReplaceFromFile] =
				[pathProcessor]() { initLocalDesignPathDialog(pathProcessor, ExtFilePathDialog::Config::Open); };
            m_context->nodes[props->id].callbacks[ButtonKey::Paste] =
				[snapshot, modelNodeID = m_curModelNodeID]()
			{
				pasteMapElement(snapshot, modelNodeID);
			};

            createObjectCallbacks(props->id);
        }
        return props;
    }
    const IPropertyPresentation* presentationFromParent = nullptr;
    bool isInline = false;
    bool isHiddenLevel = false;
    if (!name.empty() && !m_properties.empty() && m_properties.back()->type) {
        presentationFromParent = m_context->presentation->propertyByName(
            m_properties.back()->type->name, name);
    }
    if (auto* objPresentation = dynamic_cast<const ObjectPresentation*>(presentationFromParent))
        isInline = objPresentation->isInline;
    if (presentationFromParent
        && !IVisibilityCondition::allowShow(presentationFromParent->visibilityCond, *m_context, m_properties.back())) {
        isHiddenLevel = true;
        m_levelOfHidden++;
    }
    auto props = createPropertiesImpl(parentID, isInline);
    props->isHiddenLevel = isHiddenLevel;
    if (typeName != "array" && typeName != "map") {
        props->type = m_context->presentation->typeByName(typeName);
    }
    props->presentationFromParent = presentationFromParent;

    auto nameInUI = propertyNameFromPresentation(name);
    if (isInline) {
        props->setLabelUpdater([nameInUI](Label label) { label.setText(nameInUI); });
    } else {
		props->setLabelUpdater(
			[treeView = &m_context->treeView, props = props.get(), nameInUI](Label label)
		{
			nameFromPropertiesSetter(treeView, label, props, nameInUI, 0);
		});

        if (typeName != "array" && typeName != "map" && !m_properties.empty()) {
            auto snapshot = std::make_shared<Snapshot>(*this, m_properties.back(), ObjType::Object);
            snapshot->modelNodeID = m_context->model.get(m_curModelNodeID).parentID;

            std::function<void(const std::string&)> pathProcessor =
				[snapshot, modelNodeID = m_curModelNodeID, id = props->id](const std::string& path)
			{
				replaceMemberFromFile(path, snapshot, modelNodeID, id);
			};
			m_context->nodes[props->id].callbacks[ButtonKey::ReplaceFromFile] =
				[pathProcessor]() { initLocalDesignPathDialog(pathProcessor, ExtFilePathDialog::Config::Open); };
            m_context->nodes[props->id].callbacks[ButtonKey::Paste] =
				[snapshot, modelNodeID = m_curModelNodeID, id = props->id]()
			{
				pasteMember(snapshot, modelNodeID, id);
			};

            createObjectCallbacks(props->id);
        }
    }

    return props;
}

std::string DesignViewBuilder::propertyName(const std::string& nameFromSerializer)
{
    ObjType::Enum parentObj = parentObjType();
    if (nameFromSerializer.empty()) {
        if (parentObj == ObjType::Array)
            return g_textBank.get("element");
        return m_curName;
    }
    return nameFromSerializer;
}

std::shared_ptr<Properties> DesignViewBuilder::currentPropertiesForPrimitive(
    const std::string& name, const std::string& typeName)
{
    auto parent = parentObjType();
    if (parent == ObjType::Array || parent == ObjType::MapElement)
        return createProperties(name, typeName);
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
    std::function<void(const std::string&)> pathProcessor =
		[model = &m_context->model, modelNodeID = m_curModelNodeID](const std::string& path)
	{
		saveNode(model, modelNodeID, path);
	};
	m_context->nodes[propsID].callbacks[ButtonKey::Save] =
		[pathProcessor]() { initLocalDesignPathDialog(pathProcessor, ExtFilePathDialog::Config::Save); };
    m_context->nodes[propsID].callbacks[ButtonKey::Copy] =
		[model = &m_context->model, modelNodeID = m_curModelNodeID]()
	{
		copyNode(model, modelNodeID);
	};
    if (m_context->nodes[propsID].callbacks.count(ButtonKey::Remove) > 0) {
        m_context->nodes[propsID].callbacks[ButtonKey::Cut] =
            [context = m_context, propsID]()
        {
            cutNode(context, propsID);
        };
    }
}

std::shared_ptr<Properties> DesignViewBuilder::currentProperties()
{
    return m_properties.back();
}

void DesignViewBuilder::finishCurrentProperties()
{
    m_properties.back()->updateLabel();
    if (isHidden() && m_properties.back()->isHiddenLevel)
        m_levelOfHidden--;
    m_properties.pop_back();
}

std::string DesignViewBuilder::propertyNameFromPresentation(const std::string& name)
{
    if (name == "newKey" || name == "newValue"
        || name == impl::MAP_KEY_TAG || name == impl::MAP_VALUE_TAG)
        return g_textBank.get(name);
    if (!m_properties.empty()) {
        if (auto type = m_properties.back()->type) {
            if (auto* propertyPresentation = m_context->presentation->propertyByName(type->name, name))
                return propertyPresentation->nameInUI;
        }
    }
    return name;
}

IProperty* DesignViewBuilder::addFictiveProperty(
    const std::string& name,
    const IPropertyPresentation* elementPresentation)
{
    auto& props = m_properties.back();
    auto modelNodeID = m_curModelNodeID;
    auto presentationFromParent = props->presentationFromParent;
                
    // add fictive node and prepare to adding input UI
    props->presentationFromParent = elementPresentation;
    m_objTypes.push_back(ObjType::FictiveObject);

    // add input UI connected to fictive data node
    impl::Serializer serializer(this, impl::SerializationMode::ForcedFull);
    serializeDefaultValue(serializer, name, m_context->presentation, elementPresentation);
                
    // restore state
    m_objTypes.pop_back();
    props->presentationFromParent = presentationFromParent;
    
    return props->list.back().get();
}

void DesignViewBuilder::addStaticTypeLabel(
    Properties* props, const std::string& typeName)
{
    auto staticProperty = createStaticProperty(typeName);
    staticProperty->setNameUI(g_textBank.get("type"));
    props->list.push_back(staticProperty);
}

void DesignViewBuilder::chooseColor(FilledRect colorRect)
{
	std::function<void(const Color&)> callback =
		[colorRect](const Color& color) mutable
	{
		colorRect.setColor(color);
	};
	getColorDialog().showWithColor(colorRect.color(), callback);
}

void DesignViewBuilder::setupProperty(
    Properties* props,
    const std::string& name,
    const std::shared_ptr<IProperty>& prop)
{
    prop->setName(name);
    prop->setNameUI(propertyNameFromPresentation(propertyName(name)));
    prop->setHidden(isHidden());
    if (m_objTypes.back() != ObjType::FictiveObject) {
        prop->setModelNodeID(m_curModelNodeID);
        prop->addUpdater(m_context->model);
    }
    props->list.push_back(prop);
    if (name.empty())
        props->updateLabel();
}

void DesignViewBuilder::setupClassNameProperty(
    const std::shared_ptr<Properties>& props,
    const std::shared_ptr<IClassNameProperty>& classNameProperty)
{
    classNameProperty->setHidden(props->isInline);
    classNameProperty->setNameUI(g_textBank.get("type"));
    classNameProperty->setModelNodeID(m_curModelNodeID);
    classNameProperty->addUpdater(m_context->model);
    props->list.push_back(classNameProperty);
    createObjectReplaceCallbacks(props, classNameProperty.get());
}

void DesignViewBuilder::createObjectReplaceCallbacks(
    const std::shared_ptr<Properties>& props,
    IClassNameProperty* classNameProperty)
{
    auto snapshot = std::make_shared<Snapshot>(*this, props, ObjType::Object);
    classNameProperty->setCallback([
        propertiesToSaveNum = props->list.size(), classNameProperty, snapshot]()
    {
        replaceObjectWithPattern(classNameProperty, propertiesToSaveNum, snapshot);
    });
}

} }

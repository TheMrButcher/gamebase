/**
 * Copyright (c) 2016 Slavnejshev Filipp
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
#include <gamebase/impl/serial/JsonDeserializer.h>
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

Color modifyColor(Color color, int componentIndex, float value)
{
	switch (componentIndex) {
	case 0: color.r = value; break;
	case 1: color.g = value; break;
	case 2: color.b = value; break;
	case 3: color.a = value; break;
	}
	return color;
}
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
		&& m_arrayTypes.back() == impl::SerializationTag::Color) {
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
			auto layout = createPropertyLayout();
			layout.add(createLabel(propertyNameFromPresentation(propertyName(name))));
			auto colorRectButton = createColorRect();
			layout.add(colorRectButton);
			layout.add(createSpacer());
			layout.setVisible(!isHidden());
			properties->layout.add(layout);

			auto colorRect = colorRectButton.child<FilledRect>("colorRect");
			DesignModel::UpdateModelFunc modelUpdater = [colorRect](auto* data)
			{
				updateColorProperty(colorRect, data);
			};
			m_context->model.addUpdater(m_curModelNodeID, modelUpdater);
			colorRectButton.setCallback([this, colorRect]() { chooseColor(colorRect); });
		}
		auto properties = currentPropertiesForPrimitive(name, "color");
		auto layout = properties->layout.get<Layout>(properties->layout.size() - 1);
		auto colorRect = layout.child<FilledRect>("colorRect");
		colorRect.setColor(modifyColor(colorRect.color(), m_primitiveElementIndex, static_cast<float>(d)));
		++m_primitiveElementIndex;
		return;
    }

    addProperty(name, "double", toUIString(d), &updateProperty<double>);
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
                layout.setVisible(!isHidden());
                properties->layout.add(layout);

				DesignModel::UpdateModelFunc modelUpdater = [comboBox, name](auto* data)
				{
					updateEnumProperty(comboBox, name, data);
				};  
                m_context->model.addUpdater(m_curModelNodeID, modelUpdater);
                comboBox.setCallback(properties->labelUpdater());
                if (name.empty())
                    properties->updateLabel();
                return;
            }
        }
    }
    addProperty(propertyName(name), boost::lexical_cast<std::string>(i),
        &updateProperty<int>, properties.get());
    if (name.empty())
        properties->updateLabel();
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

        m_properties.push_back(createProperties(m_curName, EMPTY_TYPE_NAME));
        m_objTypes.back() = ObjType::Object;

        bool addedTags = false;
        if (m_properties.back()->isInline) {
            std::cerr << "Warning: inlined object is empty" << std::endl;
        } else {
            auto presentationFromParent = m_properties.back()->presentationFromParent;
            auto typesList = createTypesList(g_textBank.get("type"), presentationFromParent, noneLabel());
            auto comboBox = typesList.comboBox;
            auto textVariants = comboBox.variants();
            comboBox.setText(noneLabel());
            if (textVariants.size() > 1
                || (textVariants.size() == 1 && textVariants.front() != noneLabel())) {
                createObjectReplaceCallbacks(typesList);
                addedTags = true;
            }
        }
        if (!addedTags)
            m_context->model.addUpdater(m_curModelNodeID, createConstUpdater(impl::EMPTY_TAG, true));
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
    auto layout = createPropertyLayout();
    layout.add(createLabel(propertyNameFromPresentation(propertyName(name))));
    auto checkBox = createCheckBox();
    checkBox.setChecked(b);
    layout.add(checkBox);
    layout.add(createSpacer());
    layout.setVisible(!isHidden());
    properties->layout.add(layout);

	DesignModel::UpdateModelFunc modelUpdater = [checkBox, name](auto* data)
	{
		updateBoolProperty(checkBox, name, data);
	};
    m_context->model.addUpdater(m_curModelNodeID, modelUpdater);
    checkBox.setCallback(properties->labelUpdater());
    if (name.empty())
        properties->updateLabel();
}

void DesignViewBuilder::writeString(const std::string& name, const std::string& value)
{
    if (name == impl::TYPE_NAME_TAG) {
        m_properties.push_back(createProperties(m_curName, value));
        m_objTypes.back() = ObjType::Object;

        bool addedTags = false;
        if (!m_properties.back()->isInline) {
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
                addedTags = true;
            }
        }

        if (!addedTags) {
            m_context->model.addUpdater(m_curModelNodeID, createConstUpdater(name, value));
            m_context->model.addUpdater(m_curModelNodeID, createConstUpdater(impl::EMPTY_TAG, false));
        }
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
			
            auto layout = createPropertyLayout();
            layout.add(createLabel(propertyNameFromPresentation(propertyName(name))));

			switch (stringPresentation->type) {
			case SpecialString::Font:
				{
					auto comboBox = createComboBox(impl::fontStorage().fontNames());
					comboBox.setText(value);
					layout.add(comboBox);

					DesignModel::UpdateModelFunc modelUpdater = [comboBox, name](auto* data)
					{
						updateFontProperty(comboBox, name, data);
					};
					m_context->model.addUpdater(m_curModelNodeID, modelUpdater);
					comboBox.setCallback(properties->labelUpdater());
				} break;

			case SpecialString::ImagePath:
				{
					auto textBox = createImagePathTextBox();
					textBox.setText(value);
					layout.add(textBox);

					auto choosePathButton = createChoosePathButton();
					choosePathButton.setCallback(
						[textBox = makeRaw(textBox)]() { chooseImage(textBox); });
					layout.add(choosePathButton);

					DesignModel::UpdateModelFunc modelUpdater = [textBox, name](auto* data)
					{
						updateProperty<std::string>(textBox, name, data);
					};
					m_context->model.addUpdater(m_curModelNodeID, modelUpdater);
					textBox.setCallback(properties->labelUpdater());
				} break;
            }

            layout.setVisible(!isHidden());
            properties->layout.add(layout);
            if (name.empty())
                properties->updateLabel();
            return;
        }
    }

    addProperty(propertyName(name), value, &updateProperty<std::string>, properties.get());
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
            [treeView = &m_context->treeView, label = props->label(),
            layout = props->layout]()
        {
            mapElementNameFromPropertiesSetter(treeView, label, layout);
        });
        if (auto parentPresentation = dynamic_cast<const MapPresentation*>(m_properties.back()->presentationFromParent)) {
            props->presentationFromParent = parentPresentation->valueType.get();
            props->keyPresentationFromParent = parentPresentation->keyType.get();
        }
        m_properties.push_back(props);

        {
            auto snapshot = std::make_shared<Snapshot>(*this, *props, ObjType::MapElement);
            /*if (m_objTypes.back() != ObjType::Unknown && m_objTypes.back() != ObjType::PrimitiveArray)
                snapshot->modelNodeID = m_context->model.nextID();*/
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
        addStaticTypeLabel(props->layout, g_textBank.get("array"));
        m_properties.push_back(props);
        if (auto arrayPresentation = dynamic_cast<const ArrayPresentation*>(props->presentationFromParent)) {
            auto snapshot = std::make_shared<Snapshot>(*this, *props, ObjType::Array);
            auto* elementPresentation = arrayPresentation->elementType.get();
            if (elementPresentation->presentationType() == PropertyPresentation::Primitive
                || elementPresentation->presentationType() == PropertyPresentation::Enum) {
                auto fictiveNodeID = addFictiveNode("newValue", elementPresentation);
				m_context->nodes[props->id].callbacks[ButtonKey::Add] =
					[fictiveNodeID, snapshot]() { addPrimitiveValueToArray(fictiveNodeID, snapshot); };
            }

            if (elementPresentation->presentationType() == PropertyPresentation::Object) {
                auto typesList = createTypesList(g_textBank.get("newValue"), elementPresentation);
                if (!typesList.types.empty()) {
					m_context->nodes[props->id].callbacks[ButtonKey::Add] =
						[typesList, snapshot]() { addObjectToArray(typesList.comboBox, typesList.types, snapshot); };
                }

				std::function<void(const std::string&)> pathProcessor =
					[snapshot](const std::string& path) { addObjectFromFileToArray(path, snapshot); };
				m_context->nodes[props->id].callbacks[ButtonKey::AddFromFile] =
					[pathProcessor]() { initLocalDesignPathDialog(pathProcessor); };
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
        addStaticTypeLabel(props->layout, g_textBank.get("map"));
        m_properties.push_back(props);

        if (auto mapPresentation = dynamic_cast<const MapPresentation*>(props->presentationFromParent)) {
            auto snapshot = std::make_shared<Snapshot>(*this, *props, ObjType::Map);
            auto valuePresentation = mapPresentation->valueType;
            auto fictiveKeyNodeID = addFictiveNode("newKey", mapPresentation->keyType.get());

            if (valuePresentation->presentationType() == PropertyPresentation::Primitive
                || valuePresentation->presentationType() == PropertyPresentation::Enum) {
                auto fictiveValueNodeID = addFictiveNode("newValue", valuePresentation.get());
				m_context->nodes[props->id].callbacks[ButtonKey::Add] =
					[fictiveKeyNodeID, fictiveValueNodeID, snapshot]()
				{
					addPrimitiveElementToMap(fictiveKeyNodeID, fictiveValueNodeID, snapshot);
				};
            }

            if (valuePresentation->presentationType() == PropertyPresentation::Object) {
                auto typesList = createTypesList(g_textBank.get("newValue"), valuePresentation.get());
                if (!typesList.types.empty()) {
                    m_context->nodes[props->id].callbacks[ButtonKey::Add] =
					[fictiveKeyNodeID, typesList, snapshot]()
					{
						addObjectToMap(
							fictiveKeyNodeID, typesList.comboBox, typesList.types, snapshot);
					};
                }

                std::function<void(const std::string&)> pathProcessor =
				[fictiveKeyNodeID, snapshot]
				(const std::string& path)
				{
					addObjectFromFileToMap(fictiveKeyNodeID, path, snapshot);
				};
                    
				m_context->nodes[props->id].callbacks[ButtonKey::AddFromFile] =
					[pathProcessor]() { initLocalDesignPathDialog(pathProcessor); };
                m_context->nodes[props->id].callbacks[ButtonKey::AddFromClipboard] =
				[fictiveKeyNodeID, snapshot]()
				{
					addObjectFromClipboardToMap(fictiveKeyNodeID, snapshot);
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
    const std::string& initialValue,
    const std::function<void(TextBox, std::string, Json::Value*)>& updater)
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
    addProperty(propName, initialValue, updater, properties.get());
    if (name.empty())
        properties->updateLabel();
}

void DesignViewBuilder::addProperty(
    const std::string& name,
    const std::string& initialValue,
    const std::function<void(TextBox, std::string, Json::Value*)>& updater,
    Properties* properties)
{
    auto layout = name == impl::REG_NAME_TAG
        ? createNameLayout() : createPropertyLayout();

	auto fullName = propertyNameFromPresentation(name);
	if (m_objTypes.back() == ObjType::PrimitiveArray)
		fullName += PRIMITIVE_ARRAY_ELEMENT_SUFFIX.get(m_arrayTypes.back(), m_primitiveElementIndex);

    layout.add(createLabel(fullName));
    auto textBox = createTextBox();
    textBox.setText(initialValue);
    textBox.setCallback(properties->labelUpdater());
    layout.add(textBox);
    layout.setVisible(!isHidden());
    properties->layout.add(layout);

	DesignModel::UpdateModelFunc modelUpdater = [updater, textBox, name](auto* data)
	{
		updater(textBox, name, data);
	};
    m_context->model.addUpdater(m_curModelNodeID, modelUpdater);
}

std::shared_ptr<Properties> DesignViewBuilder::createPropertiesImpl(int parentID, bool isInline)
{
    auto props = std::make_shared<Properties>();
    if (isInline) {
        auto parentLayout = m_properties.back()->layout;
        if (!isHidden()) {
            Layout fullLayout = loadObj<Layout>("ui\\InlinedObjLayout.json");
            props->layout = fullLayout.child<Layout>("inner");
            props->setLabel(fullLayout.child<Label>("label"));
            if (!m_properties.back()->isInline)
                fullLayout.add(loadObj<DrawObj>("ui\\HorLine.json"));
            parentLayout.add(fullLayout);
        } else {
            Layout layout = loadObj<Layout>("ui\\PropertiesLayout.json");
            parentLayout.add(layout);
            props->layout = makeRaw(layout);
        }
        props->id = parentID;
        props->isInline = true;
    } else {
        Layout layout = loadObj<Layout>("ui\\PropertiesLayout.json");
        if (isHidden()) {
            props->id = m_context->treeView.genID();
        } else {
            auto button = loadObj<RadioButton>("ui\\SwitchButton.json");
            props->id = m_context->treeView.addObject(parentID, button.getImpl().getShared());
            props->setLabel(button.child<Label>("label"));
            m_context->switchsGroup.insert(props->id, button);
        }
        m_context->propertiesMenu.insert(props->id, layout);
        props->layout = makeRaw(layout);
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
            auto snapshot = std::make_shared<Snapshot>(*this, *props, ObjType::Array);
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
				[treeView = &m_context->treeView, label = props->label(),
				layout = props->layout]()
			{
				nameForPresentationSetter(treeView, label, layout);
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

            auto snapshot = std::make_shared<Snapshot>(*this, *m_properties.back(), ObjType::Array);
            snapshot->modelNodeID = m_context->model.get(m_curModelNodeID).parentID;

            std::function<void(const std::string&)> pathProcessor =
				[snapshot, modelNodeID = m_curModelNodeID, id = props->id](const std::string& path)
			{
				replaceArrayElementFromFile(path, snapshot, modelNodeID, id);
			};
			m_context->nodes[props->id].callbacks[ButtonKey::ReplaceFromFile] =
				[pathProcessor]() { initLocalDesignPathDialog(pathProcessor); };
            m_context->nodes[props->id].callbacks[ButtonKey::Paste] =
				[snapshot, modelNodeID = m_curModelNodeID, id = props->id]()
			{
				pasteArrayElement(snapshot, modelNodeID, id);
			};

            createObjectCallbacks(props->id);
        }
        props->setLabelUpdater(
			[treeView = &m_context->treeView, label = props->label(),
			layout = props->layout, elementStr = g_textBank.get("element")]()
		{
			nameFromPropertiesSetter(treeView, label, layout, elementStr, 0);
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

            auto snapshot = std::make_shared<Snapshot>(*this, *props, ObjType::MapElement);
            snapshot->modelNodeID = m_context->model.get(m_curModelNodeID).parentID;

            std::function<void(const std::string&)> pathProcessor =
				[snapshot, modelNodeID = m_curModelNodeID](const std::string& path)
			{
				replaceMapElementFromFile(path, snapshot, modelNodeID);
			};
			m_context->nodes[props->id].callbacks[ButtonKey::ReplaceFromFile] =
				[pathProcessor]() { initLocalDesignPathDialog(pathProcessor); };
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
        props->setLabelText(nameInUI);
    } else {
		props->setLabelUpdater(
			[treeView = &m_context->treeView, label = props->label(),
			layout = props->layout, nameInUI]()
		{
			nameFromPropertiesSetter(treeView, label, layout, nameInUI, 0);
		});

        if (typeName != "array" && typeName != "map" && !m_properties.empty()) {
            auto snapshot = std::make_shared<Snapshot>(*this, *m_properties.back(), ObjType::Object);
            snapshot->modelNodeID = m_context->model.get(m_curModelNodeID).parentID;

            std::function<void(const std::string&)> pathProcessor =
				[snapshot, modelNodeID = m_curModelNodeID, id = props->id](const std::string& path)
			{
				replaceMemberFromFile(path, snapshot, modelNodeID, id);
			};
			m_context->nodes[props->id].callbacks[ButtonKey::ReplaceFromFile] =
				[pathProcessor]() { initLocalDesignPathDialog(pathProcessor); };
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
		[pathProcessor]() { initLocalDesignPathDialog(pathProcessor); };
    m_context->nodes[propsID].callbacks[ButtonKey::Copy] =
		[model = &m_context->model, modelNodeID = m_curModelNodeID]()
	{
		copyNode(model, modelNodeID);
	};
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
    m_objTypes.push_back(ObjType::FictiveObject);

    // add input UI connected to fictive data node
    impl::Serializer serializer(this, impl::SerializationMode::ForcedFull);
    serializeDefaultValue(serializer, name, m_context->presentation, elementPresentation);
                
    // restore state
    m_objTypes.pop_back();
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
    } else if (variantIfNoPresentation != noneLabel()) {
        if (auto typePresentation = m_context->presentation->typeByName(variantIfNoPresentation)) {
            result.types.push_back(typePresentation);
            typesNames.push_back(typePresentation->nameInUI);
        }
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
    m_context->model.addUpdater(m_curModelNodeID,
		[typesList](auto* data) { updateTypeTag(typesList, data); });
    m_context->model.addUpdater(m_curModelNodeID,
		[typesList](auto* data) { updateEmptyTag(typesList, data); });
    auto snapshot = std::make_shared<Snapshot>(*this, props, ObjType::Object);
    typesList.comboBox.setCallback(
		[typesList, snapshot,
		updatersNum = m_context->model.get(m_curModelNodeID).updatersNum()]()
	{
		replaceObjectWithPattern(typesList, snapshot, updatersNum, typesList.comboBox);
	});
}

void DesignViewBuilder::addStaticTypeLabel(
    Layout propertiesLayout, const std::string& typeName)
{
    auto layout = createPropertyLayout();
    layout.add(createLabel(g_textBank.get("type")));
    layout.add(createRightLabel(typeName));
    propertiesLayout.add(layout);
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

} }

/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "Helpers.h"
#include <tools.h>
#include <TypePresentation.h>
#include <DesignViewBuilder.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <iomanip>

namespace gamebase { namespace editor {

namespace {
std::string tryFindName(const PropertyList& props)
{
    auto it = std::find_if(props.begin(), props.end(), [](const auto& prop)
    {
        return prop->isNameProperty();
    });
    if (it == props.end())
        return std::string();
    return (*it)->toString();
}
}

const std::string& noneLabel()
{
    static const std::string LABEL = g_textBank.get("None");
    return LABEL;
}

std::string mergeStrings(const std::string& str1, const std::string& str2)
{
    bool bothNotEmpty = !str1.empty() && !str2.empty();
    return str1 + (bothNotEmpty ? std::string(" : ") : "") + str2;
}

void updateView(TreeView* view)
{
    view->update();
}

void updateView(const std::shared_ptr<Snapshot>& snapshot)
{
    updateView(&snapshot->context->treeView);
    snapshot->context->sync();
}

void updateView(
    const std::shared_ptr<Snapshot>& snapshot,
    int propsID)
{
    updateView(snapshot);
    auto& switchsGroup = snapshot->context->switchsGroup;
    if (switchsGroup.selected() != propsID)
        dynamic_cast<impl::RadioButton*>(snapshot->context->treeView.getObject(propsID).get())->setChecked();
    else
        snapshot->context->select(propsID);
}

void nameForPresentationSetter(
    TreeView* treeView, Label label, Properties* props)
{
    if (props->list.size() < 3)
        return;
    auto text = props->list[2]->toString();
    if (text.empty())
        text = props->list[1]->toString();
    if (text == label.text())
        return;
    label.setText(text);
    updateView(treeView);
}

void nameFromPropertiesSetter(
    TreeView* treeView, Label label, Properties* props,
    const std::string& prefix, size_t sourceIndex)
{
    if (props->list.size() < sourceIndex + 1)
        return;
    auto name = tryFindName(props->list);
    if (name.empty())
        name = props->list[sourceIndex]->toString();
    auto text = mergeStrings(prefix, name);
    if (text == label.text())
        return;
    label.setText(text);
    updateView(treeView);
}

void mapElementNameFromPropertiesSetter(
    TreeView* treeView, Label label, Properties* props)
{
    if (props->list.size() < 2)
        return;
    auto name = tryFindName(props->list);
    if (name.empty())
        name = props->list[1]->toString();
    if (name.size() > 50)
        name = g_textBank.get("large_text");
    auto text = props->list[0]->toString() + " => " + name;
    if (text == label.text())
        return;
    label.setText(text);
    updateView(treeView);
}

void serializeDefaultValue(
    impl::Serializer& serializer, const std::string& name,
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
    IProperty* source,
    const std::shared_ptr<Snapshot>& snapshot,
    const IIndexablePropertyPresentation* presentation)
{
    DesignViewBuilder builder(*snapshot);
    impl::Serializer serializer(&builder, impl::SerializationMode::ForcedFull);
    addPrimitiveValueFromSource(source, serializer, "", snapshot, presentation);
}

void addPrimitiveValueFromSource(
    IProperty* source,
    impl::Serializer& serializer,
    const std::string& resultName,
    const std::shared_ptr<Snapshot>& snapshot,
    const IIndexablePropertyPresentation* presentation)
{
    snapshot->properties->sync();

    Json::Value fictiveData(Json::objectValue);
    source->makeUpdater()(&fictiveData);
    if (!fictiveData.isMember(source->name()))
        return;
    const auto& sourceData = fictiveData[source->name()];
    if (presentation) {
        PrimitiveType::Enum type = PrimitiveType::Int;
        if (auto primitivePropertyPresentation = dynamic_cast<const PrimitivePropertyPresentation*>(presentation))
            type = primitivePropertyPresentation->type;
        switch (type) {
        case PrimitiveType::Float:
        case PrimitiveType::Double:
            if (!sourceData.isDouble())
                THROW_EX() << "Wrong type of key, expected double";
            serializer << resultName << sourceData.asDouble();
            break;

        case PrimitiveType::Int:
            if (!sourceData.isInt())
                THROW_EX() << "Wrong type of key, expected integer";
            serializer << resultName << sourceData.asInt();
            break;

        case PrimitiveType::UInt:
            if (!sourceData.isUInt())
                THROW_EX() << "Wrong type of key, expected unsigned integer";
            serializer << resultName << sourceData.asUInt();
            break;

        case PrimitiveType::Int64:
            if (!sourceData.isInt64())
                THROW_EX() << "Wrong type of key, expected integer64";
            serializer << resultName << sourceData.asInt64();
            break;

        case PrimitiveType::UInt64:
            if (!sourceData.isUInt64())
                THROW_EX() << "Wrong type of key, expected unsigned integer64";
            serializer << resultName << sourceData.asUInt64();
            break;

        case PrimitiveType::Bool:
            if (!sourceData.isBool())
                THROW_EX() << "Wrong type of key, expected boolean";
            serializer << resultName << sourceData.asBool();
            break;

        case PrimitiveType::String:
            serializer << resultName << sourceData.asString();
            break;
        }
    } else {
        if (sourceData.isDouble())
            serializer << resultName << sourceData.asDouble();
        else if (sourceData.isInt())
            serializer << resultName << sourceData.asInt();
        else if (sourceData.isUInt())
            serializer << resultName << sourceData.asUInt();
        else if (sourceData.isInt64())
            serializer << resultName << sourceData.asInt64();
        else if (sourceData.isUInt64())
            serializer << resultName << sourceData.asUInt64();
        else if (sourceData.isBool())
            serializer << resultName << sourceData.asBool();
        else
            serializer << resultName << sourceData.asString();
    }
}

std::string toUIString(double d)
{
	std::ostringstream ss;
	ss << std::fixed << std::setprecision(3) << d;
	std::string str = ss.str();
	if (str.empty())
		return "0";
	size_t i = str.size() - 1;
	for (; i > 0; --i) {
		char k = str[i];
		if (k != '0' && k != '.')
			break;
		if (k == '.') {
			--i;
			break;
		}
	}
	if (i == str.size() - 1)
		return str;
	return str.substr(0, i + 1);
}

} }

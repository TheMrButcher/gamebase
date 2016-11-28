/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "Helpers.h"
#include <tools.h>
#include <TypePresentation.h>
#include <DesignViewBuilder.h>
#include <gamebase/impl/serial/ISerializer.h>

namespace gamebase { namespace editor {

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

void collectionSizeUpdater(std::shared_ptr<int> sharedSize, Json::Value* data)
{
    (*data)[impl::COLLECTION_SIZE_TAG] = Json::Value(*sharedSize);
}

std::string extractText(Layout propertiesLayout, size_t index)
{
    if (propertiesLayout.size() <= index)
        return std::string();
    Layout layout = tryCast<Layout>(propertiesLayout.get<DrawObj>(index));
    if (!layout || layout.size() < 2)
        return std::string();
    auto sourceObj = layout.get<DrawObj>(1);
    if (auto textBox = tryCast<TextBox>(sourceObj))
        return textBox.text();
    if (auto comboBox = tryCast<ComboBox>(sourceObj))
        return comboBox.text();
    if (auto label = tryCast<Label>(sourceObj))
        return label.text();
    return std::string();
}

void nameForPresentationSetter(Label label, Layout propertiesLayout)
{
    if (propertiesLayout.size() < 3)
        return;
    auto text = extractText(propertiesLayout, 2);
    if (text.empty())
        text = extractText(propertiesLayout, 1);
    label.setText(text);
}

void nameFromPropertiesSetter(
    Label label, Layout propertiesLayout,
    const std::string& prefix, size_t sourceIndex)
{
    if (propertiesLayout.size() < sourceIndex + 1)
        return;
    label.setText(mergeStrings(
        prefix, extractText(propertiesLayout, sourceIndex)));
}

void mapElementNameFromPropertiesSetter(Label label, Layout propertiesLayout)
{
    if (propertiesLayout.size() < 2)
        return;
    label.setText(extractText(propertiesLayout, 0) + " => " + extractText(propertiesLayout, 1));
}

void updateBoolProperty(CheckBox checkBox, std::string name, Json::Value* data)
{
    setData(data, name, checkBox.isChecked());
}

void updateEnumProperty(ComboBox comboBox, std::string name, Json::Value* data)
{
    setData(data, name, comboBox.selected());
}

void updateTypeTag(const TypesList& typesList, Json::Value* data)
{
    auto id = typesList.comboBox.selected();
    if (id < static_cast<int>(typesList.types.size()) && (id >= 0 || typesList.comboBox.text() != noneLabel())) {
        if (id < 0) {
            (*data)[impl::TYPE_NAME_TAG] = Json::Value(typesList.comboBox.text());
        } else {
            (*data)[impl::TYPE_NAME_TAG] = Json::Value(typesList.types[id]->name);
        }
    } else {
        if (data->isMember(impl::TYPE_NAME_TAG))
            data->removeMember(impl::TYPE_NAME_TAG);
    }
}

void updateEmptyTag(const TypesList& typesList, Json::Value* data)
{
    auto id = typesList.comboBox.selected();
    (*data)[impl::EMPTY_TAG] = Json::Value(
        id >= static_cast<int>(typesList.types.size()) || (id < 0 && typesList.comboBox.text() == noneLabel()));
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
    int sourceID, const std::string& name,
    const std::shared_ptr<Snapshot>& snapshot,
    const IIndexablePropertyPresentation* presentation)
{
    DesignViewBuilder builder(*snapshot);
    impl::Serializer serializer(&builder);
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

} }

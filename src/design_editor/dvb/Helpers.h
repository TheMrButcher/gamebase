/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <dvb/TypesList.h>
#include <dvb/Snapshot.h>
#include <DesignModel.h>
#include <gamebase/Gamebase.h>
#include <json/value.h>

namespace gamebase { namespace editor {
class TreeView;

const std::string& noneLabel();
std::string mergeStrings(const std::string& str1, const std::string& str2);

void updateView(TreeView* view);
void updateView(const std::shared_ptr<Snapshot>& snapshot);
void updateView(const std::shared_ptr<Snapshot>& snapshot, int propsID);

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
void updateProperty(TextBox textBox, std::string name, Json::Value* data)
{
    setDataFromString<T>(data, name, textBox.text());
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

void collectionSizeUpdater(std::shared_ptr<int> sharedSize, Json::Value* data);
std::string extractText(Layout propertiesLayout, size_t index);

void nameForPresentationSetter(
    TreeView* treeView, Label label, Layout propertiesLayout);
void nameFromPropertiesSetter(
    TreeView* treeView, Label label, Layout propertiesLayout,
    const std::string& prefix, size_t sourceIndex);
void mapElementNameFromPropertiesSetter(
    TreeView* treeView, Label label, Layout propertiesLayout);

void updateBoolProperty(CheckBox checkBox, std::string name, Json::Value* data);
void updateEnumProperty(ComboBox comboBox, std::string name, Json::Value* data);
void updateTypeTag(const TypesList& typesList, Json::Value* data);
void updateEmptyTag(const TypesList& typesList, Json::Value* data);

void serializeDefaultValue(
    impl::Serializer& serializer, const std::string& name,
    const std::shared_ptr<Presentation>& presentation,
    const IPropertyPresentation* propertyPresentation);

void addPrimitiveValueFromSource(
    int sourceID, const std::string& name,
    const std::shared_ptr<Snapshot>& snapshot,
    const IIndexablePropertyPresentation* presentation);
} }

/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <dvb/Snapshot.h>
#include <DesignModel.h>
#include <json/value.h>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

namespace gamebase { namespace editor {
class TreeView;

const std::string& noneLabel();
std::string mergeStrings(const std::string& str1, const std::string& str2);

void updateView(TreeView* view);
void updateView(const std::shared_ptr<Snapshot>& snapshot);
void updateView(const std::shared_ptr<Snapshot>& snapshot, int propsID);

template <typename T>
void setData(Json::Value* data, const std::string& name, const T& value)
{
    if (data->isArray())
        data->append(Json::Value(value));
    else
        (*data)[name] = Json::Value(value);
}

template <typename T>
void parseData(std::string valueStr, T& value)
{
    value = 0;
    std::string trimmedStr = std::move(valueStr);
    boost::algorithm::trim(trimmedStr);
    if (!trimmedStr.empty()) {
        try {
            value = boost::lexical_cast<T>(trimmedStr);
        }
        catch (boost::bad_lexical_cast& ex) {
            std::cerr << "Can't cast \"" << valueStr << "\" to type " << typeid(T).name()
                << ". Reason: " << ex.what();
        }
    }
}

template <>
inline void parseData<std::string>(std::string valueStr, std::string& value)
{
    value = std::move(valueStr);
}

void nameForPresentationSetter(
    TreeView* treeView, Label label, Properties* props);
void nameFromPropertiesSetter(
    TreeView* treeView, Label label, Properties* props,
    const std::string& prefix, size_t sourceIndex);
void mapElementNameFromPropertiesSetter(
    TreeView* treeView, Label label, Properties* props);

void serializeDefaultValue(
    impl::Serializer& serializer, const std::string& name,
    const std::shared_ptr<Presentation>& presentation,
    const IPropertyPresentation* propertyPresentation);

void addPrimitiveValueFromSource(
    IProperty* source,
    const std::shared_ptr<Snapshot>& snapshot,
    const IIndexablePropertyPresentation* presentation);

void addPrimitiveValueFromSource(
    IProperty* source,
    impl::Serializer& serializer,
    const std::string& resultName,
    const std::shared_ptr<Snapshot>& snapshot,
    const IIndexablePropertyPresentation* presentation);

std::string toUIString(double d);

} }

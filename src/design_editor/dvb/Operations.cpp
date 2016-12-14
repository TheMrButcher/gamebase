/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "Operations.h"
#include <DesignViewBuilder.h>
#include <dvb/Helpers.h>
#include <gamebase/impl/serial/JsonSerializer.h>
#include <tools.h>
#include <fstream>

namespace gamebase { namespace editor {

void addObject(
    const std::shared_ptr<impl::IObject>& obj,
    const std::shared_ptr<Snapshot>& snapshot)
{
    DesignViewBuilder builder(*snapshot);
    impl::Serializer serializer(&builder);
    serializer << "" << obj;
}

void addObjectFromPattern(
    ComboBox comboBox,
    const std::vector<const TypePresentation*>& types,
    const std::shared_ptr<Snapshot>& snapshot)
{
    auto id = comboBox.selected();
    std::shared_ptr<impl::IObject> obj;
    if (id < 0 || id >= static_cast<int>(types.size())) {
        std::cerr << "Wrong index of type: " << id << ". Adding empty object";
    } else {
        obj = snapshot->context->presentation->loadPattern(types[id]->name);
    }
    addObject(obj, snapshot);
}

void addObjectFromFile(
    const std::string& pathToFile,
    const std::shared_ptr<Snapshot>& snapshot)
{
    std::shared_ptr<impl::IObject> obj;
    deserializeFromJsonFile(pathToFile, obj);
    addObject(obj, snapshot);
}

void addPrimitiveValueToArray(
    int sourceID, const std::shared_ptr<Snapshot>& snapshot)
{
    const IIndexablePropertyPresentation* elementPresentation = 0;
    if (auto arrayPresentation = dynamic_cast<const ArrayPresentation*>(snapshot->properties->presentationFromParent))
        elementPresentation = dynamic_cast<const IIndexablePropertyPresentation*>(arrayPresentation->elementType.get());
    addPrimitiveValueFromSource(sourceID, "newValue", snapshot, elementPresentation);
    updateView(snapshot);
}

void addObjectToArray(
    ComboBox comboBox,
    const std::vector<const TypePresentation*>& types,
    const std::shared_ptr<Snapshot>& snapshot)
{
    addObjectFromPattern(comboBox, types, snapshot);
    updateView(snapshot);
}

void addObjectFromFileToArray(
    const std::string& pathToFile,
    const std::shared_ptr<Snapshot>& snapshot)
{
    addObjectFromFile(pathToFile, snapshot);
    updateView(snapshot);
}

void addObjectFromClipboardToArray(
    const std::shared_ptr<Snapshot>& snapshot)
{
    std::shared_ptr<impl::IObject> obj;
    deserializeFromJson(g_clipboard, obj);
    addObject(obj, snapshot);
    updateView(snapshot);
}

void addElementToMap(
    int keySourceID, int keysArrayNodeID, int valuesArrayNodeID,
    const std::shared_ptr<Snapshot>& snapshot,
    const std::function<void()>& addValueFunc)
{
    snapshot->mapProperties = std::make_shared<MapProperties>();
    snapshot->mapProperties->elements.resize(*snapshot->properties->collectionSize);
    snapshot->arrayType = impl::SerializationTag::Keys;
    snapshot->modelNodeID = keysArrayNodeID;

    const IIndexablePropertyPresentation* keyPresentation = 0;
    if (auto mapPresentation = dynamic_cast<const MapPresentation*>(snapshot->properties->presentationFromParent))
        keyPresentation = dynamic_cast<const IIndexablePropertyPresentation*>(mapPresentation->keyType.get());
    addPrimitiveValueFromSource(keySourceID, "newKey", snapshot, keyPresentation);
    
    snapshot->mapProperties->currentElem = *snapshot->properties->collectionSize - 1;
    snapshot->arrayType = impl::SerializationTag::Values;
    snapshot->modelNodeID = valuesArrayNodeID;
    addValueFunc();
    
    snapshot->mapProperties.reset();
    updateView(snapshot);
}

void addPrimitiveElementToMap(
    int keySourceID, int valueSourceID,
    int keysArrayNodeID, int valuesArrayNodeID,
    const std::shared_ptr<Snapshot>& snapshot)
{
    const IIndexablePropertyPresentation* valuePresentation = 0;
    if (auto mapPresentation = dynamic_cast<const MapPresentation*>(snapshot->properties->presentationFromParent))
        valuePresentation = dynamic_cast<const IIndexablePropertyPresentation*>(mapPresentation->valueType.get());
    addElementToMap(keySourceID, keysArrayNodeID, valuesArrayNodeID, snapshot,
        std::bind(addPrimitiveValueFromSource, valueSourceID, "newValue", snapshot, valuePresentation));
}

void addObjectToMap(
    int keySourceID,  int keysArrayNodeID, int valuesArrayNodeID,
    ComboBox comboBox,
    const std::vector<const TypePresentation*>& types,
    const std::shared_ptr<Snapshot>& snapshot)
{
    addElementToMap(keySourceID, keysArrayNodeID, valuesArrayNodeID, snapshot,
        std::bind(addObjectFromPattern, comboBox, types, snapshot));
}

void addObjectFromFileToMap(
    int keySourceID,  int keysArrayNodeID, int valuesArrayNodeID,
    const std::string& pathToFile,
    const std::shared_ptr<Snapshot>& snapshot)
{
    std::shared_ptr<impl::IObject> obj;
    deserializeFromJsonFile(pathToFile, obj);
    addElementToMap(keySourceID, keysArrayNodeID, valuesArrayNodeID, snapshot,
        std::bind(addObject, obj, snapshot));
}

void addObjectFromClipboardToMap(
    int keySourceID,  int keysArrayNodeID, int valuesArrayNodeID,
    const std::shared_ptr<Snapshot>& snapshot)
{
    std::shared_ptr<impl::IObject> obj;
    deserializeFromJson(g_clipboard, obj);
    addElementToMap(keySourceID, keysArrayNodeID, valuesArrayNodeID, snapshot,
        std::bind(addObject, obj, snapshot));
}

void replaceObjectWith(
    const std::shared_ptr<Snapshot>& snapshot,
    size_t updatersToSaveNum,
    size_t fieldsInLayoutToSave,
    const std::shared_ptr<impl::IObject>& obj)
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

    auto newObjectsNum = std::min(snapshot->properties->layout.size(), fieldsInLayoutToSave);
    while (snapshot->properties->layout.size() != newObjectsNum)
        snapshot->properties->layout.remove(static_cast<int>(snapshot->properties->layout.size() - 1));
    context.treeView.removeChildren(snapshot->properties->id);

    if (obj) {
        DesignViewBuilder builder(*snapshot);
        insertObjBody(builder, obj, false);
    }
    snapshot->properties->updateLabel();
    updateView(snapshot, snapshot->properties->id);
}

void replaceObjectWithPattern(
    const TypesList& typesList,
    const std::shared_ptr<Snapshot>& snapshot,
    size_t updatersToSaveNum,
    ComboBox comboBox)
{
    const auto& typeName = comboBox.text();
    int variantID = comboBox.selected();

    std::shared_ptr<impl::IObject> obj;
    if (variantID >= 0 && variantID < static_cast<int>(typesList.types.size())) {
        obj = snapshot->context->presentation->loadPattern(typesList.types[variantID]->name);
        snapshot->properties->type = typesList.types[variantID];
    } else {
        snapshot->properties->type = nullptr;
    }

    replaceObjectWith(snapshot, updatersToSaveNum, typesList.indexInLayout + 1, obj);
}

void removeArrayElement(const std::shared_ptr<Snapshot>& snapshot)
{
    auto& context = *snapshot->context;
    auto& props = *snapshot->properties;
    context.model.remove(snapshot->modelNodeID);
    context.treeView.removeSubtree(props.id);
    context.propertiesMenu.remove(props.id);
    context.nodes.erase(props.id);
    context.toolBar->clear();
    --(*props.collectionSize);
    updateView(snapshot);
    snapshot->context->propertiesMenuArea.update();
}

void removeMapElement(const std::shared_ptr<Snapshot>& snapshot, int keyNodeID)
{
    snapshot->context->model.remove(keyNodeID);
    removeArrayElement(snapshot);
}

void replaceMember(
    const std::shared_ptr<impl::IObject>& obj,
    const std::shared_ptr<Snapshot>& snapshot,
    int oldNodeID,
    int oldPropsID)
{
    auto& context = *snapshot->context;
    auto& oldNode = context.model.get(oldNodeID);
    auto parentNodeID = context.model.get(oldNodeID).parentID;
    auto nameInParent = oldNode.nameInParent;
    auto newPropertiesID = context.treeView.nextID();

    {
        DesignViewBuilder builder(*snapshot);
        impl::Serializer serializer(&builder);
        serializer << nameInParent << obj;
    }

    context.model.remove(oldNodeID);
    context.treeView.swapInParents(oldPropsID, newPropertiesID);
    context.treeView.removeSubtree(oldPropsID);
    context.propertiesMenu.remove(oldPropsID);
    context.nodes.erase(oldPropsID);

    updateView(snapshot, newPropertiesID);
}

void replaceMemberFromFile(
    const std::string& fileName,
    const std::shared_ptr<Snapshot>& snapshot,
    int oldNodeID,
    int oldPropsID)
{
    std::shared_ptr<impl::IObject> obj;
    deserializeFromJsonFile(fileName, obj);
    replaceMember(obj, snapshot, oldNodeID, oldPropsID);
}

void pasteMember(
    const std::shared_ptr<Snapshot>& snapshot,
    int oldNodeID,
    int oldPropsID)
{
    std::shared_ptr<impl::IObject> obj;
    deserializeFromJson(g_clipboard, obj);
    replaceMember(obj, snapshot, oldNodeID, oldPropsID);
}

void replaceArrayElement(
    const std::shared_ptr<impl::IObject>& obj,
    const std::shared_ptr<Snapshot>& snapshot,
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
    const std::shared_ptr<Snapshot>& snapshot,
    int oldNodeID,
    int oldPropsID)
{
    std::shared_ptr<impl::IObject> obj;
    deserializeFromJsonFile(fileName, obj);
    replaceArrayElement(obj, snapshot, oldNodeID, oldPropsID);
}

void pasteArrayElement(
    const std::shared_ptr<Snapshot>& snapshot,
    int oldNodeID,
    int oldPropsID)
{
    std::shared_ptr<impl::IObject> obj;
    deserializeFromJson(g_clipboard, obj);
    replaceArrayElement(obj, snapshot, oldNodeID, oldPropsID);
}

void replaceMapElement(
    const std::shared_ptr<impl::IObject>& obj,
    const std::shared_ptr<Snapshot>& snapshot,
    int oldNodeID)
{
    auto& context = *snapshot->context;
    
    auto& props = *snapshot->mapProperties->elements.front().properties;
    context.treeView.removeChildren(props.id);
    auto keyLayout = props.layout.get<DrawObj>(0);
    props.layout.clear();
    props.layout.add(keyLayout);

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
    const std::shared_ptr<Snapshot>& snapshot,
    int oldNodeID)
{
    std::shared_ptr<impl::IObject> obj;
    deserializeFromJsonFile(fileName, obj);
    replaceMapElement(obj, snapshot, oldNodeID);
}

void pasteMapElement(const std::shared_ptr<Snapshot>& snapshot, int oldNodeID)
{
    std::shared_ptr<impl::IObject> obj;
    deserializeFromJson(g_clipboard, obj);
    replaceMapElement(obj, snapshot, oldNodeID);
}

void moveArrayElementUp(DesignModel* model, TreeView* treeView, int nodeID, int propsID)
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

void moveArrayElementDown(DesignModel* model, TreeView* treeView, int nodeID, int propsID)
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

void saveNode(DesignModel* model, int nodeID, const std::string& fileName)
{
    auto jsonStr = model->toString(nodeID, impl::JsonFormat::Styled);
    std::shared_ptr<impl::IObject> obj;
    impl::deserializeFromJson(jsonStr, obj);
    if (obj)
        impl::serializeToJsonFile(*obj, impl::JsonFormat::Styled, fileName);
}

void copyNode(DesignModel* model, int nodeID)
{
    g_clipboard = model->toString(nodeID, impl::JsonFormat::Fast);
}

void insertObjBody(
    DesignViewBuilder& builder,
    const std::shared_ptr<impl::IObject>& obj,
    bool insertTypeTag)
{
    if (!obj)
        THROW_EX() << "Object is null, can't insert body";
    try {
        impl::Serializer objectSerializer(&builder);
        if (const impl::IRegistrable* regObj = dynamic_cast<const impl::IRegistrable*>(obj.get())) {
            objectSerializer << impl::REG_NAME_TAG << regObj->name();
        }
        if (const impl::IDrawable* drawObj = dynamic_cast<const impl::IDrawable*>(obj.get())) {
            objectSerializer << impl::VISIBLE_TAG << drawObj->isVisible();
        }
        const auto& typeTraits = impl::SerializableRegister::instance().typeTraits(typeid(*obj));
        if (insertTypeTag) {
            objectSerializer << impl::TYPE_NAME_TAG << typeTraits.typeName;
            objectSerializer << impl::EMPTY_TAG << false;
        }
        if (const auto& serialize = typeTraits.serialize) {
            serialize(obj.get(), objectSerializer);
        } else {
            THROW_EX() << "Object with type_index=" << typeid(*obj).name() << " is not serializable";
        }
    } catch (std::exception& ex) {
        std::cout << "Error while replacing (serialization step) object. Reason: " << ex.what() << std::endl;
    }
}

} }

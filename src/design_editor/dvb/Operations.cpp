/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "Operations.h"
#include <DesignViewBuilder.h>
#include <dvb/Helpers.h>
#include <gamebase/impl/serial/JsonSerializer.h>
#include <tools.h>
#include <fstream>

namespace gamebase { namespace editor {

namespace {
std::shared_ptr<impl::IObject> loadFromFile(const std::string& fname)
{
	RegisterSwitcher switcher;
    std::shared_ptr<impl::IObject> obj;
    impl::deserializeFromJsonFile(fname, obj);
	return obj;
}

std::shared_ptr<impl::IObject> loadFromString(const std::string& designStr)
{
	RegisterSwitcher switcher;
    std::shared_ptr<impl::IObject> obj;
    impl::deserializeFromJson(designStr, obj);
	return obj;
}

void setPathToTextBox(
    TextBox textBox, const std::string& relativePathLocal, const std::string& fileNameLocal)
{
	textBox.setText(addSlash(toUnicode(relativePathLocal)) + toUnicode(fileNameLocal));
}

std::shared_ptr<impl::IObject> objectPattern(
    IClassNameProperty* classNameProperty, const SnapshotPtr& snapshot)
{
    auto className = classNameProperty->className();
    std::shared_ptr<impl::IObject> obj;
    if (className)
        obj = snapshot->context->presentation->loadPattern(*className);
    return obj;
}
}

void addObject(
    const std::shared_ptr<impl::IObject>& obj, const SnapshotPtr& snapshot)
{
    DesignViewBuilder builder(*snapshot);
    impl::Serializer serializer(&builder, impl::SerializationMode::ForcedFull);
    serializer << "" << obj;
}

void addObjectFromPattern(
    IClassNameProperty* classNameProperty, const SnapshotPtr& snapshot)
{
    snapshot->properties->sync();
    addObject(objectPattern(classNameProperty, snapshot), snapshot);
}

void addObjectFromFile(
    const std::string& fileName, const SnapshotPtr& snapshot)
{
    snapshot->properties->sync();
	addObject(loadFromFile(fileName), snapshot);
}

void addPrimitiveValueToArray(IProperty* source, const SnapshotPtr& snapshot)
{
    snapshot->properties->sync();
    const IIndexablePropertyPresentation* elementPresentation = 0;
    if (auto arrayPresentation = dynamic_cast<const ArrayPresentation*>(snapshot->properties->presentationFromParent))
        elementPresentation = dynamic_cast<const IIndexablePropertyPresentation*>(arrayPresentation->elementType.get());
    addPrimitiveValueFromSource(source, snapshot, elementPresentation);
    updateView(snapshot);
}

void addObjectToArray(
    IClassNameProperty* classNameProperty, const SnapshotPtr& snapshot)
{
    snapshot->properties->sync();
    addObjectFromPattern(classNameProperty, snapshot);
    updateView(snapshot);
}

void addObjectFromFileToArray(
    const std::string& fileName, const SnapshotPtr& snapshot)
{
    snapshot->properties->sync();
    addObjectFromFile(fileName, snapshot);
    updateView(snapshot);
}

void addObjectFromClipboardToArray(const SnapshotPtr& snapshot)
{
    snapshot->properties->sync();
	addObject(loadFromString(g_clipboard), snapshot);
    updateView(snapshot);
}

void addElementToMap(
    IProperty* keySource, const SnapshotPtr& snapshot,
    const std::function<void(impl::Serializer&)>& addValueFunc)
{
    DesignViewBuilder builder(*snapshot);
    builder.startObject("");

    const IIndexablePropertyPresentation* keyPresentation = 0;
    if (auto mapPresentation = dynamic_cast<const MapPresentation*>(snapshot->properties->presentationFromParent))
        keyPresentation = dynamic_cast<const IIndexablePropertyPresentation*>(mapPresentation->keyType.get());

    impl::Serializer keySerializer(&builder, impl::SerializationMode::ForcedFull);
    addPrimitiveValueFromSource(
        keySource, keySerializer, impl::MAP_KEY_TAG, snapshot, keyPresentation);

    impl::Serializer valueSerializer(&builder, impl::SerializationMode::ForcedFull);
    addValueFunc(valueSerializer);

    builder.finishObject();
    updateView(snapshot);
}

void addPrimitiveElementToMap(
    IProperty* keySource, IProperty* valueSource, const SnapshotPtr& snapshot)
{
    snapshot->properties->sync();
    const IIndexablePropertyPresentation* valuePresentation = 0;
    if (auto mapPresentation = dynamic_cast<const MapPresentation*>(snapshot->properties->presentationFromParent))
        valuePresentation = dynamic_cast<const IIndexablePropertyPresentation*>(mapPresentation->valueType.get());
    addElementToMap(keySource, snapshot,
		[valueSource, snapshot, valuePresentation](impl::Serializer& serializer)
	{
		addPrimitiveValueFromSource(
            valueSource, serializer, impl::MAP_VALUE_TAG, snapshot, valuePresentation);
	});
}

void addObjectToMap(
    IProperty* keySource, IClassNameProperty* valueSource, const SnapshotPtr& snapshot)
{
    snapshot->properties->sync();
    addElementToMap(keySource, snapshot,
		[obj = objectPattern(valueSource, snapshot), snapshot](impl::Serializer& serializer)
    {
        serializer << impl::MAP_VALUE_TAG << obj;
    });
}

void addObjectFromFileToMap(
    IProperty* keySource, const std::string& fileName, const SnapshotPtr& snapshot)
{
    snapshot->properties->sync();
    addElementToMap(keySource, snapshot,
		[obj = loadFromFile(fileName), snapshot](impl::Serializer& serializer)
    {
        serializer << impl::MAP_VALUE_TAG << obj;
    });
}

void addObjectFromClipboardToMap(IProperty* keySource, const SnapshotPtr& snapshot)
{
    snapshot->properties->sync();
	addElementToMap(keySource, snapshot,
		[obj = loadFromString(g_clipboard), snapshot](impl::Serializer& serializer)
    {
        serializer << impl::MAP_VALUE_TAG << obj;
    });
}

void replaceObjectWith(
    const std::shared_ptr<impl::IObject>& obj,
    size_t propertiesToSaveNum,
    const SnapshotPtr& snapshot)
{
    auto& context = *snapshot->context;
    auto& node = context.model.get(snapshot->modelNodeID);
    auto& props = *snapshot->properties;
    context.model.clearNode(snapshot->modelNodeID);
    props.list.resize(std::min(props.list.size(), propertiesToSaveNum));
    auto remainedProps = props.list;
    snapshot->properties->inlined.clear();
    context.treeView.removeChildren(props.id);

    if (obj) {
        DesignViewBuilder builder(*snapshot);
        insertObjBody(builder, obj, false);
    }
    for (const auto& prop : remainedProps)
        prop->addUpdater(snapshot->context->model);
    props.updateLabel();
    updateView(snapshot, props.id);
}

void replaceObjectWithPattern(
    IClassNameProperty* source,
    size_t propertiesToSaveNum,
    const SnapshotPtr& snapshot)
{
    snapshot->properties->sync();
    auto className = source->className();
    std::shared_ptr<impl::IObject> obj;
    if (className) {
        obj = snapshot->context->presentation->loadPattern(*className);
        snapshot->properties->type = snapshot->context->presentation->typeByName(*className);
    } else {
        snapshot->properties->type = nullptr;
    }

    replaceObjectWith(obj, propertiesToSaveNum, snapshot);
}

void removeArrayElement(const SnapshotPtr& snapshot)
{
    auto& context = *snapshot->context;
    auto& props = *snapshot->properties;
    context.model.remove(snapshot->modelNodeID);
    context.treeView.removeSubtree(props.id);
    context.propertiesMenu.remove(props.id);
    context.nodes.erase(props.id);
    context.toolBar->clear();
    updateView(snapshot);
    snapshot->context->propertiesMenuArea.update();
}

void removeMapElement(const SnapshotPtr& snapshot)
{
    removeArrayElement(snapshot);
}

void replaceMember(
    const std::shared_ptr<impl::IObject>& obj,
    const SnapshotPtr& snapshot,
    int oldNodeID,
    int oldPropsID)
{
    snapshot->properties->sync();
    auto& context = *snapshot->context;
    auto& oldNode = context.model.get(oldNodeID);
    auto parentNodeID = context.model.get(oldNodeID).parentID;
    auto nameInParent = oldNode.nameInParent;
    auto newPropertiesID = context.treeView.nextID();

    {
        DesignViewBuilder builder(*snapshot);
        impl::Serializer serializer(&builder, impl::SerializationMode::ForcedFull);
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
    const SnapshotPtr& snapshot,
    int oldNodeID,
    int oldPropsID)
{
    replaceMember(loadFromFile(fileName), snapshot, oldNodeID, oldPropsID);
}

void pasteMember(
    const SnapshotPtr& snapshot,
    int oldNodeID,
    int oldPropsID)
{
    replaceMember(loadFromString(g_clipboard), snapshot, oldNodeID, oldPropsID);
}

void replaceArrayElement(
    const std::shared_ptr<impl::IObject>& obj,
    const SnapshotPtr& snapshot,
    int oldNodeID,
    int oldPropsID)
{
    snapshot->context->sync();
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
    const SnapshotPtr& snapshot,
    int oldNodeID,
    int oldPropsID)
{
    replaceArrayElement(loadFromFile(fileName), snapshot, oldNodeID, oldPropsID);
}

void pasteArrayElement(
    const SnapshotPtr& snapshot,
    int oldNodeID,
    int oldPropsID)
{
    replaceArrayElement(loadFromString(g_clipboard), snapshot, oldNodeID, oldPropsID);
}

void replaceMapElement(
    const std::shared_ptr<impl::IObject>& obj,
    const SnapshotPtr& snapshot,
    int oldNodeID)
{
    snapshot->properties->sync();
    auto& context = *snapshot->context;
    
    auto& props = *snapshot->properties;
    context.treeView.removeChildren(props.id);
	auto keyProperty = props.list[0];
    props.list.clear();
	props.list.push_back(keyProperty);

    int newNodeID = context.model.nextID();

    DesignViewBuilder builder(*snapshot);
    impl::Serializer valueSerializer(&builder, impl::SerializationMode::ForcedFull);
    valueSerializer << impl::MAP_VALUE_TAG << obj;

    auto& parent = context.model.get(snapshot->modelNodeID);
    parent.swap(oldNodeID, newNodeID);
    context.model.remove(oldNodeID);

    updateView(snapshot, props.id);
}

void replaceMapElementFromFile(
    const std::string& fileName,
    const SnapshotPtr& snapshot,
    int oldNodeID)
{
    replaceMapElement(loadFromFile(fileName), snapshot, oldNodeID);
}

void pasteMapElement(const SnapshotPtr& snapshot, int oldNodeID)
{
    replaceMapElement(loadFromString(g_clipboard), snapshot, oldNodeID);
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
        impl::serializeToJsonFile(*obj, impl::SerializationMode::Default, fileName);
}

void copyNode(DesignModel* model, int nodeID)
{
    auto jsonStr = model->toString(nodeID, impl::JsonFormat::Fast);
	std::shared_ptr<impl::IObject> obj;
    impl::deserializeFromJson(jsonStr, obj);
    if (obj)
        g_clipboard = impl::serializeToJson(*obj, impl::SerializationMode::Compressed);
}

void insertObjBody(
    DesignViewBuilder& builder,
    const std::shared_ptr<impl::IObject>& obj,
    bool insertTypeTag)
{
    if (!obj)
        THROW_EX() << "Object is null, can't insert body";
    try {
        impl::Serializer objectSerializer(&builder, impl::SerializationMode::ForcedFull);
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

void chooseImage(TextBox textBox)
{
	auto curPath = textBox.text();
	auto& dialog = getImagePathDialog();
	dialog.setFilePath(curPath);
	dialog.setCallbacks([textBox](
		const std::string& relativePathLocal, const std::string& fileNameLocal)
	{
		setPathToTextBox(textBox, relativePathLocal, fileNameLocal);
	});
	dialog.init();
}

} }

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

void setPathToTextBox(TextBox textBox, const std::string& relativePathLocal, const std::string& fileNameLocal)
{
	textBox.setText(addSlash(toUnicode(relativePathLocal)) + toUnicode(fileNameLocal));
}

std::shared_ptr<impl::IObject> objectPattern(
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
    return obj;
}
}

void addObject(
    const std::shared_ptr<impl::IObject>& obj,
    const std::shared_ptr<Snapshot>& snapshot)
{
    DesignViewBuilder builder(*snapshot);
    impl::Serializer serializer(&builder, impl::SerializationMode::ForcedFull);
    serializer << "" << obj;
}

void addObjectFromPattern(
    ComboBox comboBox,
    const std::vector<const TypePresentation*>& types,
    const std::shared_ptr<Snapshot>& snapshot)
{
    addObject(objectPattern(comboBox, types, snapshot), snapshot);
}

void addObjectFromFile(
    const std::string& fileName,
    const std::shared_ptr<Snapshot>& snapshot)
{
	addObject(loadFromFile(fileName), snapshot);
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
    const std::string& fileName,
    const std::shared_ptr<Snapshot>& snapshot)
{
    addObjectFromFile(fileName, snapshot);
    updateView(snapshot);
}

void addObjectFromClipboardToArray(
    const std::shared_ptr<Snapshot>& snapshot)
{
	addObject(loadFromString(g_clipboard), snapshot);
    updateView(snapshot);
}

void addElementToMap(
    int keySourceID, const std::shared_ptr<Snapshot>& snapshot,
    const std::function<void(impl::Serializer&)>& addValueFunc)
{
    DesignViewBuilder builder(*snapshot);
    builder.startObject("");

    const IIndexablePropertyPresentation* keyPresentation = 0;
    if (auto mapPresentation = dynamic_cast<const MapPresentation*>(snapshot->properties->presentationFromParent))
        keyPresentation = dynamic_cast<const IIndexablePropertyPresentation*>(mapPresentation->keyType.get());

    impl::Serializer keySerializer(&builder, impl::SerializationMode::ForcedFull);
    addPrimitiveValueFromSource(
        keySourceID, "newKey", keySerializer,
        impl::MAP_KEY_TAG, snapshot, keyPresentation);

    impl::Serializer valueSerializer(&builder, impl::SerializationMode::ForcedFull);
    addValueFunc(valueSerializer);

    builder.finishObject();
    updateView(snapshot);
}

void addPrimitiveElementToMap(
    int keySourceID, int valueSourceID,
    const std::shared_ptr<Snapshot>& snapshot)
{
    const IIndexablePropertyPresentation* valuePresentation = 0;
    if (auto mapPresentation = dynamic_cast<const MapPresentation*>(snapshot->properties->presentationFromParent))
        valuePresentation = dynamic_cast<const IIndexablePropertyPresentation*>(mapPresentation->valueType.get());
    addElementToMap(keySourceID, snapshot,
		[valueSourceID, snapshot, valuePresentation](impl::Serializer& serializer)
	{
		addPrimitiveValueFromSource(
            valueSourceID, "newValue", serializer,
            impl::MAP_VALUE_TAG, snapshot, valuePresentation);
	});
}

void addObjectToMap(
    int keySourceID, ComboBox comboBox,
    const std::vector<const TypePresentation*>& types,
    const std::shared_ptr<Snapshot>& snapshot)
{
    addElementToMap(keySourceID, snapshot,
		[obj = objectPattern(comboBox, types, snapshot), snapshot](impl::Serializer& serializer)
    {
        serializer << impl::MAP_VALUE_TAG << obj;
    });
}

void addObjectFromFileToMap(
    int keySourceID, const std::string& fileName,
    const std::shared_ptr<Snapshot>& snapshot)
{
    addElementToMap(keySourceID, snapshot,
		[obj = loadFromFile(fileName), snapshot](impl::Serializer& serializer)
    {
        serializer << impl::MAP_VALUE_TAG << obj;
    });
}

void addObjectFromClipboardToMap(
    int keySourceID, const std::shared_ptr<Snapshot>& snapshot)
{
	addElementToMap(keySourceID, snapshot,
		[obj = loadFromString(g_clipboard), snapshot](impl::Serializer& serializer)
    {
        serializer << impl::MAP_VALUE_TAG << obj;
    });
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
    updateView(snapshot);
    snapshot->context->propertiesMenuArea.update();
}

void removeMapElement(const std::shared_ptr<Snapshot>& snapshot)
{
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
    const std::shared_ptr<Snapshot>& snapshot,
    int oldNodeID,
    int oldPropsID)
{
    replaceMember(loadFromFile(fileName), snapshot, oldNodeID, oldPropsID);
}

void pasteMember(
    const std::shared_ptr<Snapshot>& snapshot,
    int oldNodeID,
    int oldPropsID)
{
    replaceMember(loadFromString(g_clipboard), snapshot, oldNodeID, oldPropsID);
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
    replaceArrayElement(loadFromFile(fileName), snapshot, oldNodeID, oldPropsID);
}

void pasteArrayElement(
    const std::shared_ptr<Snapshot>& snapshot,
    int oldNodeID,
    int oldPropsID)
{
    replaceArrayElement(loadFromString(g_clipboard), snapshot, oldNodeID, oldPropsID);
}

void replaceMapElement(
    const std::shared_ptr<impl::IObject>& obj,
    const std::shared_ptr<Snapshot>& snapshot,
    int oldNodeID)
{
    auto& context = *snapshot->context;
    
    auto& props = *snapshot->properties;
    context.treeView.removeChildren(props.id);
	auto keyLayout = props.layout.getImpl()->objects()[0];
    props.layout.clear();
	props.layout.getImpl()->addObject(keyLayout);

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
    const std::shared_ptr<Snapshot>& snapshot,
    int oldNodeID)
{
    replaceMapElement(loadFromFile(fileName), snapshot, oldNodeID);
}

void pasteMapElement(const std::shared_ptr<Snapshot>& snapshot, int oldNodeID)
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

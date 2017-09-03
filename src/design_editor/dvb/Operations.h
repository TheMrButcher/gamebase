/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <dvb/Snapshot.h>
#include <gamebase/Gamebase.h>

namespace gamebase { namespace editor {

void addObject(
    const std::shared_ptr<impl::IObject>& obj, const SnapshotPtr& snapshot);
void addObjectFromPattern(
    IClassNameProperty* classNameProperty, const SnapshotPtr& snapshot);
void addObjectFromFile(
    const std::string& pathToFile, const SnapshotPtr& snapshot);

void addPrimitiveValueToArray(IProperty* source, const SnapshotPtr& snapshot);
void addObjectToArray(
    IClassNameProperty* classNameProperty, const SnapshotPtr& snapshot);
void addObjectFromFileToArray(
    const std::string& fileName, const SnapshotPtr& snapshot);
void addObjectFromClipboardToArray(const SnapshotPtr& snapshot);

void addElementToMap(
    IProperty* keySource, const SnapshotPtr& snapshot,
    const std::function<void(impl::Serializer&)>& addValueFunc);
void addPrimitiveElementToMap(
    IProperty* keySource, IProperty* valueSource, const SnapshotPtr& snapshot);
void addObjectToMap(
    IProperty* keySource, IClassNameProperty* valueSource, const SnapshotPtr& snapshot);
void addObjectFromFileToMap(
    IProperty* keySource, const std::string& fileName, const SnapshotPtr& snapshot);
void addObjectFromClipboardToMap(IProperty* keySource, const SnapshotPtr& snapshot);

void replaceObjectWith(
    const std::shared_ptr<impl::IObject>& obj,
    size_t propertiesToSaveNum,
    const SnapshotPtr& snapshot);
void replaceObjectWithPattern(
    IClassNameProperty* source,
    size_t propertiesToSaveNum,
    const SnapshotPtr& snapshot);

void removeArrayElement(const SnapshotPtr& snapshot);
void removeMapElement(const SnapshotPtr& snapshot);

void replaceMember(
    const std::shared_ptr<impl::IObject>& obj,
    const SnapshotPtr& snapshot,
    int oldNodeID,
    int oldPropsID);
void replaceMemberFromFile(
    const std::string& fileName,
    const SnapshotPtr& snapshot,
    int oldNodeID,
    int oldPropsID);
void pasteMember(
    const SnapshotPtr& snapshot,
    int oldNodeID,
    int oldPropsID);

void replaceArrayElement(
    const std::shared_ptr<impl::IObject>& obj,
    const SnapshotPtr& snapshot,
    int oldNodeID,
    int oldPropsID);
void replaceArrayElementFromFile(
    const std::string& fileName,
    const SnapshotPtr& snapshot,
    int oldNodeID,
    int oldPropsID);
void pasteArrayElement(
    const SnapshotPtr& snapshot,
    int oldNodeID,
    int oldPropsID);

void replaceMapElement(
    const std::shared_ptr<impl::IObject>& obj,
    const SnapshotPtr& snapshot,
    int oldNodeID);
void replaceMapElementFromFile(
    const std::string& fileName,
    const SnapshotPtr& snapshot,
    int oldNodeID);
void pasteMapElement(const SnapshotPtr& snapshot, int oldNodeID);

void moveArrayElementUp(DesignModel* model, TreeView* treeView, int nodeID, int propsID);
void moveArrayElementDown(DesignModel* model, TreeView* treeView, int nodeID, int propsID);
void saveNode(DesignModel* model, int nodeID, const std::string& fileName);
void copyNode(DesignModel* model, int nodeID);

void insertObjBody(
    DesignViewBuilder& builder,
    const std::shared_ptr<impl::IObject>& obj,
    bool insertTypeTag = true);

void chooseImage(TextBox textBox);

} }

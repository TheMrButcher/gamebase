/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <dvb/Snapshot.h>
#include <dvb/TypesList.h>
#include <gamebase/Gamebase.h>

namespace gamebase { namespace editor {

void addObject(
    const std::shared_ptr<impl::IObject>& obj,
    const std::shared_ptr<Snapshot>& snapshot);
void addObjectFromPattern(
    ComboBox comboBox,
    const std::vector<const TypePresentation*>& types,
    const std::shared_ptr<Snapshot>& snapshot);
void addObjectFromFile(
    const std::string& pathToFile,
    const std::shared_ptr<Snapshot>& snapshot);

void updateView(TreeView* view);
void updateView(const std::shared_ptr<Snapshot>& snapshot);
void updateView(const std::shared_ptr<Snapshot>& snapshot, int propsID);

void addPrimitiveValueToArray(int sourceID, const std::shared_ptr<Snapshot>& snapshot);
void addObjectToArray(
    ComboBox comboBox,
    const std::vector<const TypePresentation*>& types,
    const std::shared_ptr<Snapshot>& snapshot);
void addObjectFromFileToArray(
    const std::string& pathToFile,
    const std::shared_ptr<Snapshot>& snapshot);
void addObjectFromClipboardToArray(const std::shared_ptr<Snapshot>& snapshot);

void addElementToMap(
    int keySourceID, int keysArrayNodeID, int valuesArrayNodeID,
    const std::shared_ptr<Snapshot>& snapshot,
    const std::function<void()>& addValueFunc);
void addPrimitiveElementToMap(
    int keySourceID, int valueSourceID,
    int keysArrayNodeID, int valuesArrayNodeID,
    const std::shared_ptr<Snapshot>& snapshot);
void addObjectToMap(
    int keySourceID,  int keysArrayNodeID, int valuesArrayNodeID,
    ComboBox comboBox,
    const std::vector<const TypePresentation*>& types,
    const std::shared_ptr<Snapshot>& snapshot);
void addObjectFromFileToMap(
    int keySourceID,  int keysArrayNodeID, int valuesArrayNodeID,
    const std::string& pathToFile,
    const std::shared_ptr<Snapshot>& snapshot);
void addObjectFromClipboardToMap(
    int keySourceID,  int keysArrayNodeID, int valuesArrayNodeID,
    const std::shared_ptr<Snapshot>& snapshot);

void replaceObjectWith(
    const std::shared_ptr<Snapshot>& snapshot,
    size_t updatersToSaveNum,
    size_t fieldsInLayoutToSave,
    const std::shared_ptr<impl::IObject>& obj);
void replaceObjectWithPattern(
    const TypesList& typesList,
    const std::shared_ptr<Snapshot>& snapshot,
    size_t updatersToSaveNum,
    ComboBox comboBox);

void removeArrayElement(const std::shared_ptr<Snapshot>& snapshot);
void removeMapElement(const std::shared_ptr<Snapshot>& snapshot, int keyNodeID);

void replaceMember(
    const std::shared_ptr<impl::IObject>& obj,
    const std::shared_ptr<Snapshot>& snapshot,
    int oldNodeID,
    int oldPropsID);
void replaceMemberFromFile(
    const std::string& fileName,
    const std::shared_ptr<Snapshot>& snapshot,
    int oldNodeID,
    int oldPropsID);
void pasteMember(
    const std::shared_ptr<Snapshot>& snapshot,
    int oldNodeID,
    int oldPropsID);

void replaceArrayElement(
    const std::shared_ptr<impl::IObject>& obj,
    const std::shared_ptr<Snapshot>& snapshot,
    int oldNodeID,
    int oldPropsID);
void replaceArrayElementFromFile(
    const std::string& fileName,
    const std::shared_ptr<Snapshot>& snapshot,
    int oldNodeID,
    int oldPropsID);
void pasteArrayElement(
    const std::shared_ptr<Snapshot>& snapshot,
    int oldNodeID,
    int oldPropsID);

void replaceMapElement(
    const std::shared_ptr<impl::IObject>& obj,
    const std::shared_ptr<Snapshot>& snapshot,
    int oldNodeID);
void replaceMapElementFromFile(
    const std::string& fileName,
    const std::shared_ptr<Snapshot>& snapshot,
    int oldNodeID);
void pasteMapElement(const std::shared_ptr<Snapshot>& snapshot, int oldNodeID);

void moveArrayElementUp(DesignModel* model, TreeView* treeView, int nodeID, int propsID);
void moveArrayElementDown(DesignModel* model, TreeView* treeView, int nodeID, int propsID);
void saveNode(DesignModel* model, int nodeID, const std::string& fileName);
void copyNode(DesignModel* model, int nodeID);

} }

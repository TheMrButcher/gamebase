/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

namespace gamebase { namespace impl {

enum class SerializationVersion {
    VER2,
    VER3
};

enum class SerializationMode {
    Default,
    ForcedFull,
    Compressed
};

///////////////// LEGACY ////////////////////////////////
static const char* COLLECTION_SIZE_TAG = "_size";
static const char* COLLECTION_ELEMENTS_TAG = "_elements";
static const char* MAP_KEYS_TAG = "_keys";
static const char* MAP_VALUES_TAG = "_values";
///////////////// LEGACY ////////////////////////////////

static const char* VERSION_TAG = "_version";
static const char* SERIALIZATION_VER3_STR = "VER3";

static const char* TYPE_NAME_TAG = "_typeName";
static const char* BASE_TYPE_NAME_TAG = "_baseTypeName";
static const char* ELEM_TYPE_NAME_TAG = "_elemTypeName";

static const char* EMPTY_TAG = "_empty";

static const char* REG_NAME_TAG = "_name";
static const char* VISIBLE_TAG = "_visible";

static const char* MAP_KEY_TAG = "_key";
static const char* MAP_VALUE_TAG = "_value";

struct SerializationTag {
    enum Type {
        Array,
        Keys,
        Values,
        Vec2,
        Matrix2,
        Transform2,
        BoundingBox,
        GLColor,
        Map
    };
};

} }

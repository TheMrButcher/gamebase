#pragma once

namespace gamebase {

static const char* TYPE_NAME_TAG = "typeName";

static const char* ELEM_TYPE_NAME_TAG = "elemTypeName";

static const char* COLLECTION_SIZE_TAG = "size";

static const char* COLLECTION_ELEMENTS_TAG = "elements";

static const char* MAP_KEYS_TAG = "keys";

static const char* MAP_VALUES_TAG = "values";

struct SerializationTag {
    enum Type {
        Array,
        Keys,
        Values,
        Vec2,
        Matrix2,
        Transform2,
        BoundingBox,
        Color
    };
};

}

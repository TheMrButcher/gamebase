#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/tools/Size.h>
#include <stdint.h>
#include <string>
#include <memory>
#include <vector>

namespace gamebase { namespace impl {

struct Image {
    Image(std::vector<uint8_t>&& data, const Size& size)
        : data(std::move(data))
        , size(size)
    {}

    const std::vector<uint8_t> data;
    const Size size;
};

GAMEBASE_API std::unique_ptr<Image> defaultImage();

GAMEBASE_API std::unique_ptr<Image> whiteRectImage();

GAMEBASE_API std::unique_ptr<Image> loadImageFromFile(const std::string& fname);

} }

/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/graphics/Image.h>
#include "src/impl/global/Config.h"
#include <Magick++/Blob.h>
#include <Magick++/Image.h>
#include <iostream>

namespace gamebase { namespace impl {

std::unique_ptr<Image> defaultImage()
{
    return whiteRectImage();
}

std::unique_ptr<Image> whiteRectImage()
{
    const size_t SIZE = 16;
    std::vector<uint8_t> data(SIZE * SIZE * 4, 255);
    return std::unique_ptr<Image>(new Image(
        std::move(data), Size(SIZE, SIZE)));
}

std::unique_ptr<Image> loadImageFromFile(const std::string& fname)
{
    try {
        Magick::Image image(config().imagesPath + fname);
        Magick::Blob blob;
        image.write(&blob, "RGBA");
        std::vector<uint8_t> data(blob.length());
        memcpy(&data.front(), blob.data(), blob.length());
        std::cout << "Loaded image " << fname << std::endl;
        return std::unique_ptr<Image>(new Image(std::move(data),
            Size(image.size().width(), image.size().height())));
    } catch (const Magick::Exception& ex) {
        std::cerr << "Error while trying to load image: " << fname
            << ", reason: " << ex.what() << std::endl;
        return defaultImage();
    }
}

} }

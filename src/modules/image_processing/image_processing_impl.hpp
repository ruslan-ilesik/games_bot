//
// Created by ilesik on 7/31/24.
//

#pragma once

#include "image_processing.hpp"
#include "image_impl.hpp"
#include <string>
#include <memory>

namespace gb {

    class Image_processing_impl:public Image_processing {
    public:
        Image_processing_impl(): Image_processing("image_processing",{}){};
        ~Image_processing_impl() override = default;

        Image_ptr create_image(const std::string& file) override;
        Image_ptr create_image(size_t x, size_t y, const Color &color) override;

        void stop() override;
        void run() override;

        void innit(const Modules& modules)override;
    };

    extern "C" Module_ptr create();
} // gb



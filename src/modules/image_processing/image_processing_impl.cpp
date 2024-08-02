//
// Created by ilesik on 7/31/24.
//

#include "image_processing_impl.hpp"

namespace gb {
    Image_ptr gb::Image_processing_impl::create_image(const std::string &file) {
        return std::dynamic_pointer_cast<Image>(std::make_shared<Image_impl>(file));
    }

    void Image_processing_impl::stop() {

    }

    void Image_processing_impl::run() {

    }

    void Image_processing_impl::innit(const Modules &modules) {

    }

    Image_ptr Image_processing_impl::create_image(size_t x, size_t y, const Color &color) {
        return std::dynamic_pointer_cast<Image>(std::make_shared<Image_impl>(x,y,color));
    }

    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Image_processing_impl>());
    }
} // gb
//
// Created by ilesik on 7/31/24.
//

#pragma once

#include "image_processing.hpp"
#include "image_impl.hpp"
#include <string>
#include <memory>
#include <shared_mutex>

namespace gb {

    class Image_processing_impl:public Image_processing, public std::enable_shared_from_this<Image_processing>{
        std::map<std::string,image_generator_t> _image_cache;
        std::shared_mutex _mutex;
    public:
        Image_processing_impl();
        ~Image_processing_impl() override = default;

        Image_ptr create_image(const std::string& file) override;
        Image_ptr create_image(size_t x, size_t y, const Color &color) override;
        Image_ptr create_image(const Vector2i& resolution, const Color &color) override;

        void stop() override;
        void run() override;

        void cache_create(const std::string& name, const image_generator_t&) override;

        void cache_create(const std::vector<std::pair<std::string,image_generator_t>>& generators) override;

        void cache_remove(const std::string& name) override;

        void cache_remove(const std::vector<std::string>& generators) override;

        void cache_remove(const std::vector<std::pair<std::string,image_generator_t>>& generators) override;

        Image_ptr cache_get(const std::string& name, const Vector2i& resolution) override;

        void init(const Modules& modules)override;
    };

    extern "C" Module_ptr create();
} // gb



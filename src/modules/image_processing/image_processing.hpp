//
// Created by ilesik on 7/31/24.
//

#pragma once
#include "image.hpp"
#include <src/module/module.hpp>
namespace gb{

    typedef std::function<Image_ptr(const Vector2i& resolution)> image_generator_t;

    class Image_processing : public Module{
    public:
        Image_processing(const std::string& name, const std::vector<std::string>& dependencies): Module(name,dependencies){};

        virtual ~Image_processing() = default;

        virtual Image_ptr create_image(const std::string& file) = 0;

        virtual Image_ptr create_image(size_t x, size_t y, const Color &color) = 0;

        virtual void cache_create(const std::string& name, const image_generator_t& generator) = 0;

        virtual void cache_create(const std::vector<std::pair<std::string,image_generator_t>>& generators) = 0;

        virtual void cache_remove(const std::string& name) = 0;

        virtual void cache_remove(const std::vector<std::string>& generators) = 0;

        virtual void cache_remove(const std::vector<std::pair<std::string,image_generator_t>>& generators) = 0;

        virtual Image_ptr cache_get(const std::string& name, const Vector2i& resolution) = 0;
    };

    typedef std::shared_ptr<Image_processing> Image_processing_ptr;
} //gb

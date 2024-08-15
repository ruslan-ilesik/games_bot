//
// Created by ilesik on 7/31/24.
//

#include "image_processing_impl.hpp"

#include <filesystem>

namespace gb {
    static const std::string base_dir_path = "./cache/image/";

    Image_processing_impl::Image_processing_impl(): Image_processing("image_processing", {"config"}) {
    }

    Image_ptr Image_processing_impl::create_image(const std::string &file) {
        return std::dynamic_pointer_cast<Image>(std::make_shared<Image_impl>(file));
    }

    void Image_processing_impl::stop() {
    }

    void Image_processing_impl::run() {
    }

    void Image_processing_impl::cache_create(const std::string &name,const image_generator_t &image_generator) { {
            std::shared_lock lk(_mutex);
            if (_image_cache.contains(name)) {
                throw std::runtime_error("Image_processing_impl error: image cache" + name + " already exists");
            }
        }
        if (!std::filesystem::exists(base_dir_path + name) || !std::filesystem::is_directory(base_dir_path + name)) {
            std::filesystem::create_directory(base_dir_path + name);
        }
        std::unique_lock lk(_mutex);
        _image_cache.emplace(name, image_generator);
    }

    void Image_processing_impl::cache_create(const std::vector<std::pair<std::string, image_generator_t>> &generators) {
        for(auto& i : generators) {
            cache_create(i.first,i.second);
        }
    }

    void Image_processing_impl::cache_remove(const std::string &name) {
        {
            std::unique_lock lk(_mutex);
            _image_cache.erase(name);
        }
        std::filesystem::remove_all(base_dir_path + name);
    }

    void Image_processing_impl::cache_remove(const std::vector<std::string> &generators) {
        for(const auto& i:generators) {
            cache_remove(i);
        }
    }

    void Image_processing_impl::cache_remove(const std::vector<std::pair<std::string, image_generator_t>> &generators) {
        for(const auto& i:generators) {
            cache_remove(i.first);
        }
    }

    Image_ptr Image_processing_impl::cache_get(const std::string &name, const Vector2i &resolution) {
        auto filename = base_dir_path+name+"/"+to_string(resolution)+".jpg";
        if (!_image_cache.contains(name)) {
            throw std::runtime_error("Image cache does not contain image generator "+ name);
        }
        if (std::filesystem::exists(filename)) {
            return create_image(filename);
        }

        auto image = _image_cache.at(name)(resolution);
        auto image_impl = std::static_pointer_cast<Image_impl>(image);
        cv::imwrite(filename,image_impl->get_image());
        return image;
    }

    void Image_processing_impl::innit(const Modules &modules) {
    }

    Image_ptr Image_processing_impl::create_image(size_t x, size_t y, const Color &color) {
        return std::dynamic_pointer_cast<Image>(std::make_shared<Image_impl>(x, y, color));
    }

    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Image_processing_impl>());
    }
} // gb

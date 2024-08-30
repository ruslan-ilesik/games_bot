//
// Created by ilesik on 7/31/24.
//

#pragma once

#include <functional>
#include <memory>
#include <src/module/module.hpp>
#include <string>
#include <vector>
#include "image.hpp"

namespace gb {

    /**
     * @brief Forward declaration of the Image_processing class.
     */
    class Image_processing;

    /**
     * @brief Shared pointer type for Image_processing.
     */
    typedef std::shared_ptr<Image_processing> Image_processing_ptr;

    /**
     * @brief Type definition for an image generator function.
     *
     * This type defines a function that generates an Image based on the provided
     * Image_processing context and resolution.
     *
     * @param image_processing A shared pointer to the Image_processing context.
     * @param resolution The resolution for the image to be generated.
     * @return Image_ptr A shared pointer to the generated Image.
     */
    typedef std::function<Image_ptr(Image_processing_ptr image_processing, const Vector2i &resolution)>
        image_generator_t;

    /**
     * @brief Abstract base class for image processing modules.
     *
     * The Image_processing class provides an interface for managing image creation,
     * caching, and retrieval. It extends the Module class to integrate with
     * a modular system.
     */
    class Image_processing : public Module {
    public:
        /**
         * @brief Constructs an Image_processing instance with specified name and dependencies.
         *
         * @param name The name of the module.
         * @param dependencies A vector of names of dependencies required by this module.
         */
        Image_processing(const std::string &name, const std::vector<std::string> &dependencies) :
            Module(name, dependencies) {}

        /**
         * @brief Virtual destructor for Image_processing.
         *
         * Ensures proper cleanup of derived classes.
         */
        virtual ~Image_processing() = default;

        /**
         * @brief Creates an image from a file.
         *
         * @param file The path to the image file.
         * @return Image_ptr A shared pointer to the created Image.
         */
        virtual Image_ptr create_image(const std::string &file) = 0;

        /**
         * @brief Creates an image with specified dimensions and color.
         *
         * @param x The width of the image.
         * @param y The height of the image.
         * @param color The color to fill the image with.
         * @return Image_ptr A shared pointer to the created Image.
         */
        virtual Image_ptr create_image(size_t x, size_t y, const Color &color) = 0;

        /**
         * @brief Creates an image with specified resolution and color.
         *
         * @param resolution The resolution of the image.
         * @param color The color to fill the image with.
         * @return Image_ptr A shared pointer to the created Image.
         */
        virtual Image_ptr create_image(const Vector2i &resolution, const Color &color) = 0;

        /**
         * @brief Creates a new entry in the image cache with a generator function.
         *
         * @param name The name to associate with the image generator.
         * @param generator The image generator function to associate with the name.
         */
        virtual void cache_create(const std::string &name, const image_generator_t &generator) = 0;

        /**
         * @brief Creates multiple entries in the image cache with generator functions.
         *
         * @param generators A vector of pairs where each pair contains a name and an image generator.
         */
        virtual void cache_create(const std::vector<std::pair<std::string, image_generator_t>> &generators) = 0;

        /**
         * @brief Removes an entry from the image cache.
         *
         * @param name The name of the image generator to remove from the cache.
         */
        virtual void cache_remove(const std::string &name) = 0;

        /**
         * @brief Removes multiple entries from the image cache.
         *
         * @param names A vector of names of the image generators to remove from the cache.
         */
        virtual void cache_remove(const std::vector<std::string> &names) = 0;

        /**
         * @brief Removes multiple entries from the image cache.
         *
         * @param generators A vector of pairs where each pair contains a name and an image generator.
         */
        virtual void cache_remove(const std::vector<std::pair<std::string, image_generator_t>> &generators) = 0;

        /**
         * @brief Retrieves an image from the cache or generates it if not present.
         *
         * @param name The name of the image generator to use.
         * @param resolution The resolution of the image to retrieve.
         * @return Image_ptr A shared pointer to the retrieved or generated Image.
         */
        virtual Image_ptr cache_get(const std::string &name, const Vector2i &resolution) = 0;
    };

} // namespace gb

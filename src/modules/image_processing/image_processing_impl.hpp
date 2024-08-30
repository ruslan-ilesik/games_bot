//
// Created by ilesik on 7/31/24.
//

#pragma once

#include <memory>
#include <shared_mutex>
#include <string>
#include "image_impl.hpp"
#include "image_processing.hpp"

namespace gb {

    /**
     * @brief Concrete implementation of the Image_processing interface.
     *
     * The Image_processing_impl class provides functionalities to manage image creation, caching, and retrieval.
     * It utilizes an image cache to store and manage images and supports operations for creating, removing,
     * and retrieving images, as well as managing image caching.
     */
    class Image_processing_impl : public Image_processing, public std::enable_shared_from_this<Image_processing> {
        std::map<std::string, image_generator_t> _image_cache; ///< Map to store image generators associated with names.
        std::shared_mutex _mutex; ///< Mutex for synchronizing access to the image cache.

    public:
        /**
         * @brief Default constructor for Image_processing_impl.
         *
         * Initializes the Image_processing_impl instance.
         */
        Image_processing_impl();

        /**
         * @brief Destructor for Image_processing_impl.
         *
         * The destructor is virtual to ensure proper cleanup of derived classes.
         */
        ~Image_processing_impl() override = default;

        /**
         * @brief Creates an image from a file.
         *
         * @param file The path to the image file.
         * @return Image_ptr A shared pointer to the created Image.
         */
        Image_ptr create_image(const std::string &file) override;

        /**
         * @brief Creates an image with specified dimensions and color.
         *
         * @param x The width of the image.
         * @param y The height of the image.
         * @param color The color to fill the image with.
         * @return Image_ptr A shared pointer to the created Image.
         */
        Image_ptr create_image(size_t x, size_t y, const Color &color) override;

        /**
         * @brief Creates an image with specified resolution and color.
         *
         * @param resolution The resolution of the image.
         * @param color The color to fill the image with.
         * @return Image_ptr A shared pointer to the created Image.
         */
        Image_ptr create_image(const Vector2i &resolution, const Color &color) override;

        /**
         * @brief Stops the image processing operations.
         *
         * This method is a placeholder and does not perform any actions in this implementation.
         */
        void stop() override;

        /**
         * @brief Runs the image processing operations.
         *
         * This method is a placeholder and does not perform any actions in this implementation.
         */
        void run() override;

        /**
         * @brief Creates a new entry in the image cache.
         *
         * @param name The name to associate with the image generator.
         * @param image_generator The image generator function to associate with the name.
         * @throws std::runtime_error If the cache already contains the specified name.
         */
        void cache_create(const std::string &name, const image_generator_t &image_generator) override;

        /**
         * @brief Creates multiple entries in the image cache.
         *
         * @param generators A vector of pairs where each pair contains a name and an image generator.
         */
        void cache_create(const std::vector<std::pair<std::string, image_generator_t>> &generators) override;

        /**
         * @brief Removes an entry from the image cache and deletes associated files.
         *
         * @param name The name of the image generator to remove from the cache.
         */
        void cache_remove(const std::string &name) override;

        /**
         * @brief Removes multiple entries from the image cache and deletes associated files.
         *
         * @param generators A vector of names of the image generators to remove from the cache.
         */
        void cache_remove(const std::vector<std::string> &generators) override;

        /**
         * @brief Removes multiple entries from the image cache and deletes associated files.
         *
         * @param generators A vector of pairs where each pair contains a name and an image generator.
         */
        void cache_remove(const std::vector<std::pair<std::string, image_generator_t>> &generators) override;

        /**
         * @brief Retrieves an image from the cache or generates it if not present.
         *
         * @param name The name of the image generator to use.
         * @param resolution The resolution of the image to retrieve.
         * @return Image_ptr A shared pointer to the retrieved or generated Image.
         * @throws std::runtime_error If the image cache does not contain the specified image generator.
         */
        Image_ptr cache_get(const std::string &name, const Vector2i &resolution) override;

        /**
         * @brief Initializes the Image_processing_impl with specified modules.
         *
         * @param modules The modules to initialize.
         */
        void init(const Modules &modules) override;
    };

    /**
     * @brief Factory function to create a new instance of Image_processing_impl.
     *
     * @return Module_ptr A shared pointer to the newly created Image_processing_impl instance.
     */
    extern "C" Module_ptr create();

} // namespace gb

//
// Created by ilesik on 7/31/24.
//

#pragma once
#include "image.hpp"

#include <opencv2/opencv.hpp>

namespace gb {

    /**
     * @brief Concrete implementation of the Image interface using OpenCV.
     *
     * The Image_impl class provides functionality to load, manipulate, and save images using OpenCV.
     */
    class Image_impl : public Image {
        cv::Mat _image; ///< OpenCV matrix that stores the image data.

        /**
         * @brief Converts the internal image to RGBA format.
         *
         * This method is used to ensure that the image is in the RGBA format required for various operations.
         */
        void convert_to_rgba();

        /**
         * @brief Converts a Color object to an OpenCV Scalar.
         *
         * @param color The Color object to convert.
         * @return cv::Scalar The OpenCV Scalar representation of the color.
         */
        static cv::Scalar color_to_cv_scalar(const Color &color);

        /**
         * @brief Converts a Vector2i object to an OpenCV Point.
         *
         * @param v The Vector2i object to convert.
         * @return cv::Point The OpenCV Point representation of the vector.
         */
        static cv::Point vector_to_cv_point(const Vector2i &v);


        /**
         * @brief Blends 2 images with given alpha weight.
         *
         * @param base_image Background image.
         * @param overlay Foreground image.
         * @param alpha Alpha value between 0 and 1.
         */
        static void blend_images(cv::Mat &base_image, const cv::Mat &overlay, float alpha);

    public:
        /**
         * @brief Destructor for the Image_impl class.
         *
         * The destructor is virtual to ensure proper cleanup of derived classes.
         */
        ~Image_impl() override = default;

        /**
         * @brief Constructs an Image_impl from a file.
         *
         * Loads an image from the specified file and converts it to RGBA format.
         *
         * @param file The path to the image file.
         * @throws std::runtime_error If the image file cannot be loaded.
         */
        explicit Image_impl(const std::string &file);

        /**
         * @brief Constructs an Image_impl with a specified size and color.
         *
         * Initializes an image of the specified size with the given color.
         *
         * @param x The width of the image.
         * @param y The height of the image.
         * @param color The color to fill the image with.
         */
        Image_impl(size_t x, size_t y, const Color &color);

        /**
         * @brief Converts the image to a string representation.
         *
         * @return std::pair<std::string, std::string> A pair where the first element is the file encoding
         *         (e.g., ".jpg") and the second element is the image data in string format.
         */
        std::pair<std::string, std::string> convert_to_string() override;

        /**
         * @brief Draws a line on the image.
         *
         * @param from The starting point of the line.
         * @param to The ending point of the line.
         * @param color The color of the line.
         * @param thickness The thickness of the line.
         */
        void draw_line(const Vector2i &from, const Vector2i &to, const Color &color, int thickness) override;

        /**
         * @brief Draws text on the image.
         *
         * @param text The text to draw.
         * @param position The position where the text will be drawn.
         * @param font_scale The scale of the font.
         * @param color The color of the text.
         * @param thickness The thickness of the text.
         */
        void draw_text(const std::string &text, const Vector2i &position, double font_scale, const Color &color,
                       int thickness) override;

        /**
         * @brief Draws rectangle on the image.
         *
         * @param position_start Starting left top position of rectangle
         * @param position_end Ending right bottom position of rectangle
         * @param color Color of rectangle
         * @param thickness Thickness of rectangle line. -1 - fill circle.
         */
        void draw_rectangle(const Vector2i &position_start, const Vector2i &position_end, const Color &color,
                            int thickness) override;

        /**
         * @brief Draws circle on the image.
         *
         * @param position Center position of circle on image.
         * @param radius Radius of circle.
         * @param color Color of the circle.
         * @param thickness Thickness of circle line. -1 - fill circle.
         */
        void draw_circle(const Vector2i &position, int radius, const Color &color, int thickness) override;

        /**
         * @brief Rotates image by given angle in clockwise direction.
         *
         * @note Image size can change after rotation.
         *
         * @param angle Angle in degrees to rotate image on.
         */
        void rotate(double angle) override;

        /**
         * @brief Calculates and returns size of rendered text in pixels.
         *
         * @param text Text which render size should be calculated.
         * @param font_scale The scale of the font.
         * @param thickness The thickness of the text.
         * @return Size which rendered text would take on screen.
         */
        Vector2i get_text_size(const std::string &text, double font_scale, int thickness) override;

        /**
         * @breif Overlays given image with current on provided position.
         *
         * @param image Image to overlay with.
         * @param position Top left corner of overlay.
         */
        void overlay_image(Image_ptr &image, const Vector2i &position) override;


        /**
         * @brief Resizes image to given size.
         *
         * @param size Size of resized image.
         */
        void resize(const Vector2i &size) override;

        /**
         * @brief Draws polygon by given points.
         *
         * @param contour Vector of points between which polygons should be drawn.
         * @param color Color of line polygons will be drawn.
         * @param lines_color Color of lines of polygon.
         * @param thickness thickness of polygon line.
         */
        void draw_polygon(const std::vector<Vector2i> &contour, const Color &color, const Color &lines_color,
                          int thickness) override;

        /**
         * @brief Gets a reference to the internal OpenCV image matrix.
         *
         * @return cv::Mat& Reference to the internal image matrix.
         */
        cv::Mat &get_image();
    };

} // namespace gb

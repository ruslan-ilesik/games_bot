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
    static cv::Scalar color_to_cv_scalar(const Color& color);

    /**
     * @brief Converts a Vector2i object to an OpenCV Point.
     *
     * @param v The Vector2i object to convert.
     * @return cv::Point The OpenCV Point representation of the vector.
     */
    static cv::Point vector_to_cv_point(const Vector2i& v);

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
    explicit Image_impl(const std::string& file);

    /**
     * @brief Constructs an Image_impl with a specified size and color.
     *
     * Initializes an image of the specified size with the given color.
     *
     * @param x The width of the image.
     * @param y The height of the image.
     * @param color The color to fill the image with.
     */
    Image_impl(size_t x, size_t y, const Color& color);

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
    void draw_line(const Vector2i& from, const Vector2i& to, const Color& color, int thickness) override;

    /**
     * @brief Draws text on the image.
     *
     * @param text The text to draw.
     * @param position The position where the text will be drawn.
     * @param font_scale The scale of the font.
     * @param color The color of the text.
     * @param thickness The thickness of the text.
     */
    void draw_text(const std::string& text, const Vector2i& position, double font_scale, const Color& color, int thickness) override;

    /**
     * @brief Gets a reference to the internal OpenCV image matrix.
     *
     * @return cv::Mat& Reference to the internal image matrix.
     */
    cv::Mat& get_image();
};

} // namespace gb

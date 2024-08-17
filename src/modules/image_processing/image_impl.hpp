//
// Created by ilesik on 7/31/24.
//

#pragma once
#include "image.hpp"

#include <opencv2/opencv.hpp>


namespace gb {

    class Image_impl: public Image {
        cv::Mat _image;

        void convert_to_rgba();

        static cv::Scalar color_to_cv_scalar(const Color& color);

        static cv::Point vector_to_cv_point(const Vector2i& v);

    public:

        ~Image_impl() override = default;

        explicit Image_impl(const std::string& file);

        Image_impl(size_t x, size_t y, const Color& color);

        std::pair<std::string,std::string> convert_to_string() override;

        void draw_line(const Vector2i& from, const Vector2i& to, const Color& color, int thickness) override;

        void draw_text(const std::string& text, const Vector2i& position, double font_scale, const Color& color, int thickness) override;

        cv::Mat& get_image();
    };

} // gb



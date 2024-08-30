//
// Created by ilesik on 7/31/24.
//

#include "image_impl.hpp"

#include <iostream>

namespace gb {
    Image_impl::Image_impl(const std::string &file) {
        _image = cv::imread(file, cv::IMREAD_COLOR);
        if (_image.empty()) {
            throw std::runtime_error("error loading image " + file);
        }
        convert_to_rgba();
    }

    void Image_impl::convert_to_rgba() {
        cv::Mat rgba_image;
        cv::cvtColor(_image, rgba_image, cv::COLOR_BGR2BGRA);
        _image = rgba_image;
    }

    cv::Scalar Image_impl::color_to_cv_scalar(const Color &color) {
        return {
            static_cast<double>(color.b),
            static_cast<double>(color.g),
            static_cast<double>(color.r),
            static_cast<double>(std::max(0, std::min(255, static_cast<int>(floor(color.a * 256.0)))))
        };
    }


    cv::Point Image_impl::vector_to_cv_point(const Vector2i &v) {
        return {v.x, v.y};
    }

    Image_impl::Image_impl(size_t x, size_t y, const Color &color) {
        _image = {static_cast<int>(x), static_cast<int>(y),CV_8UC4, color_to_cv_scalar(color)};
    }

    std::pair<std::string, std::string> Image_impl::convert_to_string() {
        std::vector<uchar> buf;
        cv::imencode(".jpg", _image, buf);
        std::string s(buf.begin(), buf.end());
        return {".jpg", s};
    }

    void Image_impl::draw_line(const Vector2i &from, const Vector2i &to, const Color &color, int thickness) {
        cv::line(_image, vector_to_cv_point(from), vector_to_cv_point(to), color_to_cv_scalar(color), thickness,
                 cv::LINE_8);
    }

    void Image_impl::draw_text(const std::string &text, const Vector2i &position, double font_scale, const Color &color,
                               int thickness) {
        cv::putText(_image, text, vector_to_cv_point(position), cv::FONT_HERSHEY_DUPLEX, font_scale,
                    color_to_cv_scalar(color), thickness);
    }

    cv::Mat &Image_impl::get_image() {
        return _image;
    }
} // gb

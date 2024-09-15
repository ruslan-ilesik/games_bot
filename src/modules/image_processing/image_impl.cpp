//
// Created by ilesik on 7/31/24.
//

#include "image_impl.hpp"

#include <iostream>

namespace gb {
    Image_impl::Image_impl(const std::string &file) {
        _image = cv::imread(file, cv::IMREAD_UNCHANGED);
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


    cv::Point Image_impl::vector_to_cv_point(const Vector2i &v) { return {v.x, v.y}; }

    void Image_impl::blend_images(cv::Mat &base_image, const cv::Mat &overlay, float alpha) {
        cv::addWeighted(overlay, alpha, base_image, 1.0 - alpha, 0, base_image);
    }

    Image_impl::Image_impl(size_t x, size_t y, const Color &color) {
        _image = {static_cast<int>(y), static_cast<int>(x),CV_8UC4, color_to_cv_scalar(color)};
    }

    std::pair<std::string, std::string> Image_impl::convert_to_string() {
        std::vector<uchar> buf;
        cv::imencode(".jpg", _image, buf);
        std::string s(buf.begin(), buf.end());
        return {".jpg", s};
    }

    void Image_impl::draw_line(const Vector2i &from, const Vector2i &to, const Color &color, int thickness) {
        // Create a copy of the image as overlay
        cv::Mat overlay;
        _image.copyTo(overlay);

        // Draw the line on the overlay
        cv::line(overlay, vector_to_cv_point(from), vector_to_cv_point(to), color_to_cv_scalar(color), thickness, cv::LINE_8);

        // Blend the overlay with the base image using the alpha value from the color
        blend_images(_image, overlay, color.a);
    }


    void Image_impl::draw_text(const std::string &text, const Vector2i &position, double font_scale, const Color &color, int thickness) {
        // Create a copy of the image as overlay
        cv::Mat overlay;
        _image.copyTo(overlay);

        // Draw the text on the overlay
        cv::putText(overlay, text, vector_to_cv_point(position), cv::FONT_HERSHEY_DUPLEX, font_scale, color_to_cv_scalar(color), thickness);

        // Blend the overlay with the base image using the alpha value from the color
        blend_images(_image, overlay, color.a);
    }

    void Image_impl::draw_rectangle(const Vector2i &position_start, const Vector2i &position_end, const Color &color, int thickness) {
        // Create a copy of the image as overlay
        cv::Mat overlay;
        _image.copyTo(overlay);

        // Draw the rectangle on the overlay
        cv::rectangle(overlay, vector_to_cv_point(position_start), vector_to_cv_point(position_end), color_to_cv_scalar(color), thickness);

        // Blend the overlay with the base image using the alpha value from the color
        blend_images(_image, overlay, color.a);
    }

    void Image_impl::draw_circle(const Vector2i &position, const int radius, const Color &color, const int thickness) {
        // Create a copy of the image as overlay
        cv::Mat overlay;
        _image.copyTo(overlay);

        // Draw the circle on the overlay
        cv::circle(overlay, vector_to_cv_point(position), radius, color_to_cv_scalar(color), thickness);

        // Blend the overlay with the base image using the alpha value from the color
        blend_images(_image, overlay, color.a);
    }


    void Image_impl::rotate(double angle) {
        // Get the image center
        cv::Point2f src_center(_image.cols / 2.0F, _image.rows / 2.0F);

        // Calculate the rotation matrix
        cv::Mat rot_mat = cv::getRotationMatrix2D(src_center, angle, 1.0);

        // Compute the new bounding dimensions after rotation
        double abs_cos = std::abs(rot_mat.at<double>(0, 0));
        double abs_sin = std::abs(rot_mat.at<double>(0, 1));

        int new_width = static_cast<int>(_image.rows * abs_sin + _image.cols * abs_cos);
        int new_height = static_cast<int>(_image.rows * abs_cos + _image.cols * abs_sin);

        // Adjust the rotation matrix to move the image to the center of the new dimensions
        rot_mat.at<double>(0, 2) += (new_width / 2.0 - src_center.x);
        rot_mat.at<double>(1, 2) += (new_height / 2.0 - src_center.y);

        // Perform the affine transformation (rotation)
        cv::Mat dst;
        cv::warpAffine(_image, dst, rot_mat, cv::Size(new_width, new_height));

        // Update the original image with the rotated result
        _image = dst;
    }

    Vector2i Image_impl::get_text_size(const std::string &text, double font_scale, int thickness) {
        cv::Size2i t_size = cv::getTextSize(text,cv::FONT_HERSHEY_DUPLEX,font_scale,thickness,0);
        return {t_size.width,t_size.height};
    }

    void Image_impl::overlay_image(Image_ptr &image, const Vector2i &position) {
        int x = position.x;
        int y = position.y;
        auto upcoming = std::static_pointer_cast<Image_impl>(image);
        auto handle_cv_8uc4 = [=, this](int i, int j) {
            if (upcoming->_image.at<cv::Vec4b>(j, i)[3] > 10) {
                _image.at<cv::Vec4b>(y + j, x + i) = upcoming->_image.at<cv::Vec4b>(j, i);
            }
        };

        auto handle_cv_8uc3 = [=, this](int i, int j) {
            _image.at<cv::Vec4b>(y + j, x + i)[0] = upcoming->_image.at<cv::Vec3b>(j, i)[0];
            _image.at<cv::Vec4b>(y + j, x + i)[1] = upcoming->_image.at<cv::Vec3b>(j, i)[1];
            _image.at<cv::Vec4b>(y + j, x + i)[2] = upcoming->_image.at<cv::Vec3b>(j, i)[2];
            _image.at<cv::Vec4b>(y + j, x + i)[3] = 255;
        };

        for (int i = 0; i < upcoming->_image.cols; i++) {
            for (int j = 0; j < upcoming->_image.rows; j++) {
                if (j + y >= _image.rows) {
                    break;
                }

                if (x + i >= _image.cols) {
                    return;
                }

                switch (upcoming->_image.channels()) {
                    case 3: {
                        handle_cv_8uc3(i, j);
                        break;
                    }

                    case 4: {
                        handle_cv_8uc4(i, j);
                        break;
                    }

                    default: {
                    }
                }
            }
        }
    }

    void Image_impl::resize(const Vector2i &size) {
        cv::resize(_image,_image,vector_to_cv_point(size));
    }

    cv::Mat &Image_impl::get_image() {
        return _image;
    }
} // gb

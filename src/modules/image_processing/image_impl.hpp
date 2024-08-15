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
    public:

        ~Image_impl() override = default;

        explicit Image_impl(const std::string& file);

        Image_impl(size_t x, size_t y, const Color& color);

        std::pair<std::string,std::string> convert_to_string() override;

        cv::Mat& get_image();
    };

} // gb



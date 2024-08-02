//
// Created by ilesik on 7/31/24.
//

#include "image_impl.hpp"

namespace gb {

    Image_impl::Image_impl(const std::string &file) {
        _image = cv::imread(file,cv::IMREAD_COLOR);
        if(_image.empty())
        {
            throw std::runtime_error("error loading image "+file);
        }
        convert_to_rgba();
    }

    void Image_impl::convert_to_rgba() {
        cv::Mat rgba_image;
        cv::cvtColor(_image, rgba_image, cv::COLOR_BGR2BGRA);
        _image = rgba_image;
    }

    cv::Scalar Image_impl::color_to_cv_scalar(const Color &color) {
        return (color.b,color.g,color.r,std::max(0, std::min(255, (int)floor(color.a * 256.0))));
    }

    Image_impl::Image_impl(size_t x, size_t y, const Color &color) {
        _image = {static_cast<int>(x), static_cast<int>(y),CV_8UC4, color_to_cv_scalar(color)};
    }

    std::pair<std::string, std::string> Image_impl::convert_to_string() {
        std::vector<uchar> buf;
        cv::imencode(".jpg",_image,buf);
        std::string s(buf.begin(), buf.end());
        return {".jpg",s};
    }

} // gb
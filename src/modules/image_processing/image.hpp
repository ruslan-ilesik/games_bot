//
// Created by ilesik on 7/31/24.
//

#pragma once
#include <memory>

namespace gb{

    struct Color{
        char r,g,b;
        float a; //between 0 and 1;
    };

    class Image{
    public:
        virtual ~Image() = default;

        //should return encoding of file like ".jpg" and second is actual data;
        virtual std::pair<std::string,std::string> convert_to_string() = 0;
    };

    typedef std::shared_ptr<Image> Image_ptr;

} //gb

//
// Created by rusla on 1/27/2024.
//

#pragma once
#include <limits>
#include <string>
#include <charconv>
#include <iostream>
#include <unordered_map>

#include <drogon/utils/Utilities.h>

std::string get_param_or(const std::unordered_map<std::string,std::string,drogon::utils::internal::SafeStringHash>& params, const std::string&param, const std::string& default_value);

template <typename T>
constexpr T validate_number(const std::string& number,const T& default_value, const T& min = std::numeric_limits<T>::min(), const T& max = std::numeric_limits<T>::max()) {
    T result{};
    auto [ptr, ec] = std::from_chars(number.data(), number.data() + number.size(), result);
    if (ec == std::errc()) {
        if (result >= min && result <=max) {
            return result;
        }
    }
    return  default_value;
}
template <typename T>
constexpr T validate_number(const T& number,const T& default_value, const T& min = std::numeric_limits<T>::min(), const T& max = std::numeric_limits<T>::max()) {
    if (number >= min && number <=max) {
        return number;
    }
    //std::cout << "parse failed for: " << number <<" limits min:"<<min <<" max: " << max <<'\n';
    return default_value;
}
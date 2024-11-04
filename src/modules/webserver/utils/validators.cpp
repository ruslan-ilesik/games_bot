//
// Created by rusla on 1/27/2024.
//

#include "validators.hpp"

#include <iostream>
#include <stdexcept>


std::string get_param_or(
    const std::unordered_map<std::string, std::string, drogon::utils::internal::SafeStringHash>& params,
    const std::string& param, const std::string& default_value) {
    if (!params.contains(param)) {
        //std::cout << "param not found " << param <<'\n';
        return default_value;
    }
    return params.at(param);
}

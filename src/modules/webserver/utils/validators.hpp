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

/**
 * @brief Retrieves a parameter value from a map or returns a default value if the parameter is not present.
 *
 * @param params The map containing parameter key-value pairs.
 * @param param The key of the parameter to retrieve.
 * @param default_value The value to return if the parameter is not found in the map.
 * @return The value associated with the key, or the default value if the key is not found.
 */
std::string get_param_or(const std::unordered_map<std::string, std::string, drogon::utils::internal::SafeStringHash>& params,
                         const std::string& param,
                         const std::string& default_value);

/**
 * @brief Validates and converts a string representation of a number within specified bounds.
 *
 * @tparam T The numeric type to validate (e.g., int, float, double).
 * @param number The string representation of the number to validate.
 * @param default_value The default value to return if validation fails.
 * @param min The minimum allowable value (default: numeric limits of the type).
 * @param max The maximum allowable value (default: numeric limits of the type).
 * @return The validated number, or the default value if validation fails or the number is out of bounds.
 */
template <typename T>
constexpr T validate_number(const std::string& number,
                            const T& default_value,
                            const T& min = std::numeric_limits<T>::min(),
                            const T& max = std::numeric_limits<T>::max()) {
    T result{};
    auto [ptr, ec] = std::from_chars(number.data(), number.data() + number.size(), result);
    if (ec == std::errc()) {
        if (result >= min && result <= max) {
            return result;
        }
    }
    return default_value;
}

/**
 * @brief Validates a numeric value within specified bounds.
 *
 * @tparam T The numeric type to validate (e.g., int, float, double).
 * @param number The numeric value to validate.
 * @param default_value The default value to return if validation fails.
 * @param min The minimum allowable value (default: numeric limits of the type).
 * @param max The maximum allowable value (default: numeric limits of the type).
 * @return The validated number, or the default value if the number is out of bounds.
 */
template <typename T>
constexpr T validate_number(const T& number,
                            const T& default_value,
                            const T& min = std::numeric_limits<T>::min(),
                            const T& max = std::numeric_limits<T>::max()) {
    if (number >= min && number <= max) {
        return number;
    }
    return default_value;
}

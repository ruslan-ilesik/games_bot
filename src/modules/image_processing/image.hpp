//
// Created by ilesik on 7/31/24.
//

#pragma once
#include <string>
#include <sstream>
#include <memory>

namespace gb{

    struct Color{
        char r,g,b;
        float a; //between 0 and 1;
    };

    template<typename T>
    requires std::three_way_comparable_with<T, T> &&
             requires (T a) {
                 { a + a } -> std::same_as<T>;
                 { a - a } -> std::same_as<T>;
                 { std::to_string(a) } -> std::same_as<std::string>;
             }
    class Vector2 {
    public:
        T x, y;

        // Constructor
        Vector2(const T& x_val, const T& y_val) : x(x_val), y(y_val) {}

        // Implement the <=> operator
        template<typename U>
        requires std::three_way_comparable_with<T, U>
        auto operator<=>(const Vector2<U>& other) const {
            if (auto cmp = x <=> other.x; cmp != 0) return cmp;
            return y <=> other.y;
        }

        // Addition operator
        template<typename U>
        requires requires (T a, U b) { a + b; }
        auto operator+(const Vector2<U>& other) const {
            using Result_type = decltype(T{} + U{});
            return Vector2<Result_type>{x + other.x, y + other.y};
        }

        // Subtraction operator
        template<typename U>
        requires requires (T a, U b) { a - b; }
        auto operator-(const Vector2<U>& other) const {
            using Result_type = decltype(T{} - U{});
            return Vector2<Result_type>{x - other.x, y - other.y};
        }

        // Compound assignment addition
        template<typename U>
        requires requires (T& a, U b) { a += b; }
        Vector2& operator+=(const Vector2<U>& other) {
            x += other.x;
            y += other.y;
            return *this;
        }

        // Compound assignment subtraction
        template<typename U>
        requires requires (T& a, U b) { a -= b; }
        Vector2& operator-=(const Vector2<U>& other) {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        // Multiplication by a scalar value
        template<typename Scalar>
        requires requires (T a, Scalar s) { a * s; }
        auto operator*(const Scalar& scalar) const {
            using Result_type = decltype(T{} * scalar);
            return Vector2<Result_type>{x * scalar, y * scalar};
        }

        // Division by a scalar value
        template<typename Scalar>
        requires requires (T a, Scalar s) { a / s; }
        auto operator/(const Scalar& scalar) const {
            using Result_type = decltype(T{} / scalar);
            return Vector2<Result_type>{x / scalar, y / scalar};
        }

        // Compound assignment multiplication by a scalar value
        template<typename Scalar>
        requires requires (T& a, Scalar s) { a *= s; }
        Vector2& operator*=(const Scalar& scalar) {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        // Compound assignment division by a scalar value
        template<typename Scalar>
        requires requires (T& a, Scalar s) { a /= s; }
        Vector2& operator/=(const Scalar& scalar) {
            x /= scalar;
            y /= scalar;
            return *this;
        }
    };

    // Overload to_string for Vector2
    template<typename T>
    requires requires (T a) { std::to_string(a); }
    std::string to_string(const gb::Vector2<T>& vec) {
        return "Vector2(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ")";
    }

    typedef Vector2<int> Vector2i;
    typedef Vector2<double> Vector2d;

    class Image{
    public:
        virtual ~Image() = default;

        //should return encoding of file like ".jpg" and second is actual data;
        virtual std::pair<std::string,std::string> convert_to_string() = 0;
    };

    typedef std::shared_ptr<Image> Image_ptr;

} //gb



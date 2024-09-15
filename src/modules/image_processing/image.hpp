//
// Created by ilesik on 7/31/24.
//

#pragma once
#include <memory>
#include <sstream>
#include <string>

namespace gb {

    /**
     * @brief Represents a color with RGBA components.
     *
     * The Color class holds the color information using red, green, and blue components
     * (each as an unsigned char) and an alpha channel (as a float) representing opacity.
     */
    struct Color {
        unsigned char r; ///< Red component (0 to 255).
        unsigned char g; ///< Green component (0 to 255).
        unsigned char b; ///< Blue component (0 to 255).
        float a; ///< Alpha component (opacity) ranging from 0.0 to 1.0.

        /**
         * @brief Constructs a Color with specified RGB and alpha values.
         *
         * @param r Red component (0 to 255).
         * @param g Green component (0 to 255).
         * @param b Blue component (0 to 255).
         * @param a Alpha component (opacity) ranging from 0.0 to 1.0. Default is 1.0.
         */
        Color(const unsigned char r, const unsigned char g, const unsigned char b, const float a = 1) :
            r(r), g(g), b(b), a(a) {}
    };

    /**
     * @brief A 2D vector class template.
     *
     * The Vector2 class represents a 2D vector with generic coordinate type T. It supports
     * various vector operations including addition, subtraction, and scaling.
     *
     * @tparam T The type of the coordinates (must support arithmetic operations and string conversion).
     */
    template<typename T>
        requires std::three_way_comparable_with<T, T> && requires(T a) {
            { a + a } -> std::same_as<T>;
            { a - a } -> std::same_as<T>;
            { std::to_string(a) } -> std::same_as<std::string>;
        }
    class Vector2 {
    public:
        T x; ///< The x-coordinate of the vector.
        T y; ///< The y-coordinate of the vector.

        /**
         * @brief Constructs a Vector2 with specified x and y values.
         *
         * @param x_val The x-coordinate value.
         * @param y_val The y-coordinate value.
         */
        constexpr Vector2(const T &x_val = 0, const T &y_val = 0) : x(x_val), y(y_val) {}

        /**
         * @brief Conversion constructor to allow implicit casting between Vector2 types.
         *
         * @tparam U The other vector's coordinate type.
         * @param other The other vector to convert from.
         */
        template<typename U>
            requires std::convertible_to<U, T>
        constexpr Vector2(const Vector2<U> &other) : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) {}

        /**
         * @brief Compares this vector with another Vector2 using the spaceship operator.
         *
         * @param other The vector to compare with.
         * @return auto The result of the comparison.
         */
        template<typename U>
            requires std::three_way_comparable_with<T, U>
        constexpr auto operator<=>(const Vector2<U> &other) const {
            if (auto cmp = x <=> other.x; cmp != 0)
                return cmp;
            return y <=> other.y;
        }

        /**
         * @brief Adds another Vector2 to this vector.
         *
         * @param other The vector to add.
         * @return Vector2 The resulting vector after addition.
         */
        template<typename U>
            requires requires(T a, U b) { a + b; }
        constexpr auto operator+(const Vector2<U> &other) const {
            using Result_type = decltype(T{} + U{});
            return Vector2<Result_type>{x + other.x, y + other.y};
        }

        /**
         * @brief Subtracts another Vector2 from this vector.
         *
         * @param other The vector to subtract.
         * @return Vector2 The resulting vector after subtraction.
         */
        template<typename U>
            requires requires(T a, U b) { a - b; }
        constexpr auto operator-(const Vector2<U> &other) const {
            using Result_type = decltype(T{} - U{});
            return Vector2<Result_type>{x - other.x, y - other.y};
        }

        /**
         * @brief Performs compound assignment addition with another Vector2.
         *
         * @param other The vector to add.
         * @return Vector2& Reference to this vector after addition.
         */
        template<typename U>
            requires requires(T &a, U b) { a += b; }
        constexpr Vector2 &operator+=(const Vector2<U> &other) {
            x += other.x;
            y += other.y;
            return *this;
        }

        /**
         * @brief Performs compound assignment subtraction with another Vector2.
         *
         * @param other The vector to subtract.
         * @return Vector2& Reference to this vector after subtraction.
         */
        template<typename U>
            requires requires(T &a, U b) { a -= b; }
        constexpr Vector2 &operator-=(const Vector2<U> &other) {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        /**
         * @brief Multiplies this vector by a scalar value.
         *
         * @param scalar The scalar value to multiply by.
         * @return Vector2 The resulting vector after multiplication.
         */
        template<typename Scalar>
            requires requires(T a, Scalar s) { a *s; }
        constexpr auto operator*(const Scalar &scalar) const {
            using Result_type = decltype(T{} * scalar);
            return Vector2<Result_type>{x * scalar, y * scalar};
        }

        /**
         * @brief Divides this vector by a scalar value.
         *
         * @param scalar The scalar value to divide by.
         * @return Vector2 The resulting vector after division.
         */
        template<typename Scalar>
            requires requires(T a, Scalar s) { a / s; }
        constexpr auto operator/(const Scalar &scalar) const {
            using Result_type = decltype(T{} / scalar);
            return Vector2<Result_type>{x / scalar, y / scalar};
        }

        /**
         * @brief Performs compound assignment multiplication by a scalar value.
         *
         * @param scalar The scalar value to multiply by.
         * @return Vector2& Reference to this vector after multiplication.
         */
        template<typename Scalar>
            requires requires(T &a, Scalar s) { a *= s; }
        constexpr Vector2 &operator*=(const Scalar &scalar) {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        /**
         * @brief Performs compound assignment division by a scalar value.
         *
         * @param scalar The scalar value to divide by.
         * @return Vector2& Reference to this vector after division.
         */
        template<typename Scalar>
            requires requires(T &a, Scalar s) { a /= s; }
        constexpr Vector2 &operator/=(const Scalar &scalar) {
            x /= scalar;
            y /= scalar;
            return *this;
        }
    };


    /**
     * @brief Converts a Vector2 to a string representation.
     *
     * @param vec The Vector2 to convert.
     * @return std::string The string representation of the Vector2.
     */
    template<typename T>
        requires requires(T a) { std::to_string(a); }
    std::string to_string(const gb::Vector2<T> &vec) {
        return "Vector2(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ")";
    }

    /**
     * @brief Defines a 2D vector with integer coordinates.
     */
    typedef Vector2<int> Vector2i;

    /**
     * @brief Defines a 2D vector with double coordinates.
     */
    typedef Vector2<double> Vector2d;

    /**
     * @brief Definition of Image class to define custom type.
     */
    class Image;

    /**
     * @brief Defines a shared pointer type for the Image class.
     */
    typedef std::shared_ptr<Image> Image_ptr;

    /**
     * @brief Abstract base class representing an image.
     *
     * The Image class provides methods for converting the image to a string format,
     * drawing lines, text, ect onto the image, and is intended to be extended by
     * concrete image classes that implement these methods.
     */
    class Image {
    public:
        virtual ~Image() = default;

        /**
         * @brief Converts the image to a string representation.
         *
         * @return std::pair<std::string, std::string> A pair where the first element is
         *         the file encoding (e.g., ".jpg") and the second element is the actual image data.
         */
        virtual std::pair<std::string, std::string> convert_to_string() = 0;

        /**
         * @brief Draws a line on the image.
         *
         * @param from The starting point of the line.
         * @param to The ending point of the line.
         * @param color The color of the line.
         * @param thickness The thickness of the line.
         */
        virtual void draw_line(const Vector2i &from, const Vector2i &to, const Color &color, int thickness) = 0;

        /**
         * @brief Draws text on the image.
         *
         * @param text The text to draw.
         * @param position The position where the text will be drawn.
         * @param font_scale The scale of the font.
         * @param color The color of the text.
         * @param thickness The thickness of the text.
         */
        virtual void draw_text(const std::string &text, const Vector2i &position, double font_scale, const Color &color,
                               int thickness) = 0;


        /**
         * @brief Calculates and returns size of rendered text in pixels.
         *
         * @param text Text which render size should be calculated.
         * @param font_scale The scale of the font.
         * @param thickness The thickness of the text.
         * @return Size which rendered text would take on screen.
         */
        virtual Vector2i get_text_size(const std::string &text, double font_scale, int thickness) = 0;

        /**
         * @brief Draws rectangle on the image.
         *
         * @param position_start Starting left top position of rectangle
         * @param position_end Ending right bottom position of rectangle
         * @param color Color of rectangle
         * @param thickness Thickness of rectangle line. -1 - fill circle.
         */
        virtual void draw_rectangle(const Vector2i &position_start, const Vector2i &position_end, const Color &color,
                                    int thickness) = 0;

        /**
         * @brief Draws circle on the image.
         *
         * @param position Center position of circle on image.
         * @param radius Radius of circle.
         * @param color Color of the circle.
         * @param thickness Thickness of circle line. -1 - fill circle.
         */
        virtual void draw_circle(const Vector2i &position, int radius, const Color &color, int thickness) = 0;

        /**
         * @brief Rotates image by given angle in clockwise direction.
         *
         * @note Image size can change after rotation.
         *
         * @param angle Angle in degrees to rotate image on.
         */
        virtual void rotate(double angle) = 0;

        /**
         * @breif Overlays given image with current on provided position.
         *
         * @param image Image to overlay with.
         * @param position Top left corner of overlay.
         */
        virtual void overlay_image(Image_ptr &image, const Vector2i &position = {0, 0}) = 0;

        /**
         * @brief Resizes image to given size.
         *
         * @param size Size of resized image.
         */
        virtual void resize(const Vector2i &size) = 0;
    };


} // namespace gb

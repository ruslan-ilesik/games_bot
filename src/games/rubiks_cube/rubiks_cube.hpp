#pragma once

#include <iostream>

namespace rubiks_cube {

    /**
     * @class Rubiks_cube_engine
     * @brief A class to represent the Rubik's Cube engine and its operations.
     */
    class Rubiks_cube_engine {
    public:
        /// Default constructor
        Rubiks_cube_engine() = default;

        /**
         * @var draw
         * @brief A 3x3x3 array representing the visual state of three cube sides for drawing.
         */
        char draw[3][3][3];

        /**
         * @var cube
         * @brief A 6x3x3 array representing the current state of the Rubik's Cube.
         * The sides are initialized as:
         * - White ('W') for the top
         * - Orange ('O') for the left
         * - Green ('G') for the front
         * - Red ('R') for the right
         * - Blue ('B') for the back
         * - Yellow ('Y') for the bottom
         */
        char cube[6][3][3] = {
            {{'W', 'W', 'W'}, {'W', 'W', 'W'}, {'W', 'W', 'W'}}, {{'O', 'O', 'O'}, {'O', 'O', 'O'}, {'O', 'O', 'O'}},
            {{'G', 'G', 'G'}, {'G', 'G', 'G'}, {'G', 'G', 'G'}}, {{'R', 'R', 'R'}, {'R', 'R', 'R'}, {'R', 'R', 'R'}},
            {{'B', 'B', 'B'}, {'B', 'B', 'B'}, {'B', 'B', 'B'}}, {{'Y', 'Y', 'Y'}, {'Y', 'Y', 'Y'}, {'Y', 'Y', 'Y'}}};

        /**
         * @var cube_copy
         * @brief A constant reference to the solved state of the Rubik's Cube.
         */
        static constexpr char cube_copy[6][3][3] = {
            {{'W', 'W', 'W'}, {'W', 'W', 'W'}, {'W', 'W', 'W'}}, {{'O', 'O', 'O'}, {'O', 'O', 'O'}, {'O', 'O', 'O'}},
            {{'G', 'G', 'G'}, {'G', 'G', 'G'}, {'G', 'G', 'G'}}, {{'R', 'R', 'R'}, {'R', 'R', 'R'}, {'R', 'R', 'R'}},
            {{'B', 'B', 'B'}, {'B', 'B', 'B'}, {'B', 'B', 'B'}}, {{'Y', 'Y', 'Y'}, {'Y', 'Y', 'Y'}, {'Y', 'Y', 'Y'}}};

        /// Index for the upper side of the cube
        int up_side{0};
        /// Index for the left side of the cube
        int left_side{1};
        /// Index for the front side of the cube
        int front_side{2};
        /// Index for the right side of the cube
        int right_side{3};
        /// Index for the back side of the cube
        int back_side{4};
        /// Index for the bottom side of the cube
        int down_side{5};

        /**
         * @brief Resets the Rubik's Cube to its initial solved state.
         */
        void reset();

        /**
         * @brief Rotates a side of the cube 90 degrees clockwise.
         * @param arr A 6x3x3 array representing the current state of the cube.
         * @param side The index of the side to rotate.
         */
        void clockwise(char arr[6][3][3], int side);

        /**
         * @brief Rotates a side of the cube 90 degrees counterclockwise.
         * @param arr A 6x3x3 array representing the current state of the cube.
         * @param side The index of the side to rotate.
         */
        void anticlockwise(char arr[6][3][3], int side);

        /**
         * @brief Updates the draw array with the current state of the cube for visual rendering.
         */
        void update_for_draw();

        /**
         * @brief Rotates the right face of the cube 90 degrees clockwise.
         */
        void r();

        /**
         * @brief Rotates the left face of the cube 90 degrees clockwise.
         */
        void l();

        /**
         * @brief Rotates the upper face of the cube 90 degrees clockwise.
         */
        void u();

        /**
         * @brief Rotates the down face of the cube 90 degrees clockwise.
         */
        void d();

        /**
         * @brief Rotates the front face of the cube 90 degrees clockwise.
         */
        void f();

        /**
         * @brief Rotates the back face of the cube 90 degrees clockwise.
         */
        void b();

        /**
         * @brief Rotates the middle slice parallel to the right and left faces.
         */
        void m();

        /**
         * @brief Rotates the equatorial slice parallel to the upper and down faces.
         */
        void e();

        /**
         * @brief Rotates the standing slice parallel to the front and back faces.
         */
        void s();

        /**
         * @brief Rotates the entire cube along the X-axis (equivalent to rotating the right face clockwise).
         */
        void x();

        /**
         * @brief Rotates the entire cube along the Y-axis (equivalent to rotating the upper face clockwise).
         */
        void y();

        /**
         * @brief Rotates the entire cube along the Z-axis (equivalent to rotating the front face clockwise).
         */
        void z();
    };

} // namespace rubiks_cube

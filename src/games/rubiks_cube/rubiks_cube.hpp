#pragma once

#include <iostream>


namespace rubiks_cube {

class Rubiks_cube_engine{
    public:
    Rubiks_cube_engine() = default;
    char draw[3][3][3];
    char cube[6][3][3] = {
            {{'W', 'W', 'W'}, {'W', 'W', 'W'}, {'W', 'W', 'W'}},
            {{'O', 'O', 'O'}, {'O', 'O', 'O'}, {'O', 'O', 'O'}},
            {{'G', 'G', 'G'}, {'G', 'G', 'G'}, {'G', 'G', 'G'}},
            {{'R', 'R', 'R'}, {'R', 'R', 'R'}, {'R', 'R', 'R'}},
            {{'B', 'B', 'B'}, {'B', 'B', 'B'}, {'B', 'B', 'B'}},
            {{'Y', 'Y', 'Y'}, {'Y', 'Y', 'Y'}, {'Y', 'Y', 'Y'}}
    };
    char cube_copy[6][3][3] = {
            {{'W', 'W', 'W'}, {'W', 'W', 'W'}, {'W', 'W', 'W'}},
            {{'O', 'O', 'O'}, {'O', 'O', 'O'}, {'O', 'O', 'O'}},
            {{'G', 'G', 'G'}, {'G', 'G', 'G'}, {'G', 'G', 'G'}},
            {{'R', 'R', 'R'}, {'R', 'R', 'R'}, {'R', 'R', 'R'}},
            {{'B', 'B', 'B'}, {'B', 'B', 'B'}, {'B', 'B', 'B'}},
            {{'Y', 'Y', 'Y'}, {'Y', 'Y', 'Y'}, {'Y', 'Y', 'Y'}}
    };
    int up_side{0};
    int left_side{1};
    int front_side{2};
    int right_side{3};
    int back_side{4};
    int down_side{5};

    void reset();

    void clockwise(char arr[6][3][3], int side);

    void anticlockwise(char arr[6][3][3], int side);

    void update_for_draw();

    void r();

    void l();

    void u();

    void d();

    void f();

    void b();

    void m();

    void e();

    void s();

    void x();

    void y();

    void z();
};

}

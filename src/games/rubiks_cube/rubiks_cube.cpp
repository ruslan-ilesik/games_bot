//
// Created by rusla on 10/26/2022.
//
#include "./rubiks_cube.hpp"


void rubiks_cube::Rubiks_cube_engine::reset() {
    for (int i=0; i<6; i++) {
        for (int j=0; j<3; j++) {
            for (int k=0; k<3; k++) {
                cube[i][j][k] = cube_copy[i][j][k];
            }
        }
    }
    up_side = 0;
    left_side = 1;
    front_side = 2;
    right_side = 3;
    back_side = 4;
    down_side = 5;
}

void rubiks_cube::Rubiks_cube_engine::clockwise(char (*arr)[3][3], int side) {
    cube[side][0][0] = arr[side][2][0];
    cube[side][0][1] = arr[side][1][0];
    cube[side][0][2] = arr[side][0][0];
    cube[side][1][2] = arr[side][0][1];
    cube[side][2][2] = arr[side][0][2];
    cube[side][2][1] = arr[side][1][2];
    cube[side][2][0] = arr[side][2][2];
    cube[side][1][0] = arr[side][2][1];
}

void rubiks_cube::Rubiks_cube_engine::anticlockwise(char (*arr)[3][3], int side) {
    cube[side][0][0] = arr[side][0][2];
    cube[side][0][1] = arr[side][1][2];
    cube[side][0][2] = arr[side][2][2];
    cube[side][1][2] = arr[side][2][1];
    cube[side][2][2] = arr[side][2][0];
    cube[side][2][1] = arr[side][1][0];
    cube[side][2][0] = arr[side][0][0];
    cube[side][1][0] = arr[side][0][1];
}

void rubiks_cube::Rubiks_cube_engine::update_for_draw() {
    int sides[] = {up_side,left_side,front_side};
    for (int i = 0; i < static_cast<int>(std::size(sides));i++){
        for (int j = 0; j < static_cast<int>(std::size(this->cube[sides[i]]));j++){
            for (int k = 0; k < static_cast<int>(std::size(this->cube[sides[i]][j])); k++){
                if (i == 0){
                    this->draw[i][j][k] = this->cube[sides[i]][k][2-j];
                }
                else{
                    this->draw[i][j][k] = this->cube[sides[i]][j][k];
                }
            }
        }
    }
}

void rubiks_cube::Rubiks_cube_engine::r() {
    char temp[6][3][3];
    for (int i=0; i<6; i++) {
        for (int j=0; j<3; j++) {
            for (int k=0; k<3; k++) {
                temp[i][j][k] = cube[i][j][k];
            }
        }
    }
    for (int j=0; j<3; j++) {
        cube[up_side][j][2] = temp[front_side][j][2];
        cube[front_side][j][2] = temp[down_side][j][2];
    }
    cube[down_side][0][2] = temp[back_side][2][0];
    cube[down_side][1][2] = temp[back_side][1][0];
    cube[down_side][2][2] = temp[back_side][0][0];
    cube[back_side][2][0] = temp[up_side][0][2];
    cube[back_side][1][0] = temp[up_side][1][2];
    cube[back_side][0][0] = temp[up_side][2][2];
    clockwise(temp, right_side);
}

void rubiks_cube::Rubiks_cube_engine::l() {
    char temp[6][3][3];
    for (int i=0; i<6; i++) {
        for (int j=0; j<3; j++) {
            for (int k=0; k<3; k++) {
                temp[i][j][k] = cube[i][j][k];
            }
        }
    }
    for (int j=0; j<3; j++) {
        cube[front_side][j][0] = temp[up_side][j][0];
        cube[down_side][j][0] = temp[front_side][j][0];
    }
    cube[up_side][0][0] = temp[back_side][2][2];
    cube[up_side][1][0] = temp[back_side][1][2];
    cube[up_side][2][0] = temp[back_side][0][2];
    cube[back_side][2][2] = temp[down_side][0][0];
    cube[back_side][1][2] = temp[down_side][1][0];
    cube[back_side][0][2] = temp[down_side][2][0];
    clockwise(temp, left_side);
}

void rubiks_cube::Rubiks_cube_engine::u() {
    char temp[6][3][3];
    for (int i=0; i<6; i++) {
        for (int j=0; j<3; j++) {
            for (int k=0; k<3; k++) {
                temp[i][j][k] = cube[i][j][k];
            }
        }
    }
    for (int k=0; k<3; k++) {
        cube[left_side][0][k] = temp[front_side][0][k];
        cube[front_side][0][k] = temp[right_side][0][k];
        cube[right_side][0][k] = temp[back_side][0][k];
        cube[back_side][0][k] = temp[left_side][0][k];
    }
    clockwise(temp, up_side);
}

void rubiks_cube::Rubiks_cube_engine::d() {
    char temp[6][3][3];
    for (int i=0; i<6; i++) {
        for (int j=0; j<3; j++) {
            for (int k=0; k<3; k++) {
                temp[i][j][k] = cube[i][j][k];
            }
        }
    }
    for (int k=0; k<3; k++) {
        cube[left_side][2][k] = temp[back_side][2][k];
        cube[front_side][2][k] = temp[left_side][2][k];
        cube[right_side][2][k] = temp[front_side][2][k];
        cube[back_side][2][k] = temp[right_side][2][k];
    }
    clockwise(temp, down_side);
}

void rubiks_cube::Rubiks_cube_engine::f() {
    char temp[6][3][3];
    for (int i=0; i<6; i++) {
        for (int j=0; j<3; j++) {
            for (int k=0; k<3; k++) {
                temp[i][j][k] = cube[i][j][k];
            }
        }
    }
    cube[up_side][2][0] = temp[left_side][2][2];
    cube[up_side][2][1] = temp[left_side][1][2];
    cube[up_side][2][2] = temp[left_side][0][2];
    cube[left_side][2][2] = temp[down_side][0][2];
    cube[left_side][1][2] = temp[down_side][0][1];
    cube[left_side][0][2] = temp[down_side][0][0];
    cube[down_side][0][2] = temp[right_side][0][0];
    cube[down_side][0][1] = temp[right_side][1][0];
    cube[down_side][0][0] = temp[right_side][2][0];
    cube[right_side][0][0] = temp[up_side][2][0];
    cube[right_side][1][0] = temp[up_side][2][1];
    cube[right_side][2][0] = temp[up_side][2][2];
    clockwise(temp, front_side);
}

void rubiks_cube::Rubiks_cube_engine::b() {
    char temp[6][3][3];
    for (int i=0; i<6; i++) {
        for (int j=0; j<3; j++) {
            for (int k=0; k<3; k++) {
                temp[i][j][k] = cube[i][j][k];
            }
        }
    }
    cube[up_side][0][0] = temp[right_side][0][2];
    cube[up_side][0][1] = temp[right_side][1][2];
    cube[up_side][0][2] = temp[right_side][2][2];
    cube[right_side][0][2] = temp[down_side][2][2];
    cube[right_side][1][2] = temp[down_side][2][1];
    cube[right_side][2][2] = temp[down_side][2][0];
    cube[down_side][2][2] = temp[left_side][2][0];
    cube[down_side][2][1] = temp[left_side][1][0];
    cube[down_side][2][0] = temp[left_side][0][0];
    cube[left_side][2][0] = temp[up_side][0][0];
    cube[left_side][1][0] = temp[up_side][0][1];
    cube[left_side][0][0] = temp[up_side][0][2];
    clockwise(temp, back_side);
}

void rubiks_cube::Rubiks_cube_engine::m() {
    char temp[6][3][3];
    for (int i=0; i<6; i++) {
        for (int j=0; j<3; j++) {
            for (int k=0; k<3; k++) {
                temp[i][j][k] = cube[i][j][k];
            }
        }
    }
    for (int j=0; j<3; j++) {
        cube[front_side][j][1] = temp[up_side][j][1];
        cube[down_side][j][1] = temp[front_side][j][1];
    }
    cube[up_side][0][1] = temp[back_side][2][1];
    cube[up_side][1][1] = temp[back_side][1][1];
    cube[up_side][2][1] = temp[back_side][0][1];
    cube[back_side][2][1] = temp[down_side][0][1];
    cube[back_side][1][1] = temp[down_side][1][1];
    cube[back_side][0][1] = temp[down_side][2][1];
}

void rubiks_cube::Rubiks_cube_engine::e() {
    char temp[6][3][3];
    for (int i=0; i<6; i++) {
        for (int j=0; j<3; j++) {
            for (int k=0; k<3; k++) {
                temp[i][j][k] = cube[i][j][k];
            }
        }
    }
    for (int k=0; k<3; k++) {
        cube[front_side][1][k] = temp[left_side][1][k];
        cube[left_side][1][k] = temp[back_side][1][k];
        cube[back_side][1][k] = temp[right_side][1][k];
        cube[right_side][1][k] = temp[front_side][1][k];
    }
}

void rubiks_cube::Rubiks_cube_engine::s() {
    char temp[6][3][3];
    for (int i=0; i<6; i++) {
        for (int j=0; j<3; j++) {
            for (int k=0; k<3; k++) {
                temp[i][j][k] = cube[i][j][k];
            }
        }
    }
    cube[up_side][1][0] = temp[left_side][2][1];
    cube[up_side][1][1] = temp[left_side][1][1];
    cube[up_side][1][2] = temp[left_side][0][1];
    cube[left_side][0][1] = temp[down_side][1][0];
    cube[left_side][1][1] = temp[down_side][1][1];
    cube[left_side][2][1] = temp[down_side][1][2];
    cube[down_side][1][0] = temp[right_side][2][1];
    cube[down_side][1][1] = temp[right_side][1][1];
    cube[down_side][1][2] = temp[right_side][0][1];
    cube[right_side][0][1] = temp[up_side][1][0];
    cube[right_side][1][1] = temp[up_side][1][1];
    cube[right_side][2][1] = temp[up_side][1][2];
}

void rubiks_cube::Rubiks_cube_engine::x() {
    char temp[6][3][3];
    for (int i=0; i<6; i++) {
        for (int j=0; j<3; j++) {
            for (int k=0; k<3; k++) {
                temp[i][j][k] = cube[i][j][k];
            }
        }
    }
    clockwise(temp, right_side);
    anticlockwise(temp, left_side);
    for (int x=0, y=2; x<3 && y>=0; x++, y--) {
        cube[back_side][0][x] = temp[up_side][2][y];
        cube[back_side][1][x] = temp[up_side][1][y];
        cube[back_side][2][x] = temp[up_side][0][y];
        cube[up_side][0][x] = temp[front_side][0][x];
        cube[up_side][1][x] = temp[front_side][1][x];
        cube[up_side][2][x] = temp[front_side][2][x];
        cube[front_side][0][x] = temp[down_side][0][x];
        cube[front_side][1][x] = temp[down_side][1][x];
        cube[front_side][2][x] = temp[down_side][2][x];
        cube[down_side][0][x] = temp[back_side][2][y];
        cube[down_side][1][x] = temp[back_side][1][y];
        cube[down_side][2][x] = temp[back_side][0][y];
    }
}

void rubiks_cube::Rubiks_cube_engine::y() {
    char temp[6][3][3];
    for (int i=0; i<6; i++) {
        for (int j=0; j<3; j++) {
            for (int k=0; k<3; k++) {
                temp[i][j][k] = cube[i][j][k];
            }
        }
    }
    clockwise(temp, up_side);
    anticlockwise(temp, down_side);
    for (int x=0; x<3; x++) {
        for (int y=0; y<3; y++) {
            cube[front_side][x][y] = temp[right_side][x][y];
            cube[right_side][x][y] = temp[back_side][x][y];
            cube[back_side][x][y] = temp[left_side][x][y];
            cube[left_side][x][y] = temp[front_side][x][y];
        }
    }
}

void rubiks_cube::Rubiks_cube_engine::z() {
    char temp[6][3][3];
    for (int i=0; i<6; i++) {
        for (int j=0; j<3; j++) {
            for (int k=0; k<3; k++) {
                temp[i][j][k] = cube[i][j][k];
            }
        }
    }
    clockwise(temp, front_side);
    anticlockwise(temp, back_side);
    for (int x=0, y=2; x<3 && y>=0; x++, y--) {
        cube[up_side][0][x] = temp[left_side][y][0];
        cube[up_side][1][x] = temp[left_side][y][1];
        cube[up_side][2][x] = temp[left_side][y][2];
        cube[left_side][0][x] = temp[down_side][y][0];
        cube[left_side][1][x] = temp[down_side][y][1];
        cube[left_side][2][x] = temp[down_side][y][2];
        cube[down_side][0][x] = temp[right_side][y][0];
        cube[down_side][1][x] = temp[right_side][y][1];
        cube[down_side][2][x] = temp[right_side][y][2];
        cube[right_side][0][x] = temp[up_side][y][0];
        cube[right_side][1][x] = temp[up_side][y][1];
        cube[right_side][2][x] = temp[up_side][y][2];
    }
}

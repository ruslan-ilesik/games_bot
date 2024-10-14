//
// Created by rusla on 2/10/2023.
//

#include "minesweeper.hpp"

#include <stdexcept>
#include <set>



minesweeper_engine::Minesweeper::Minesweeper(int harness_level) {
    if ( harness_level < 1 || harness_level > 3){
        throw std::invalid_argument( "harness_level should be less than 4 and bigger than 0" );
    }
    std::array<std::array<int,2>,3> sizes = {{{9,9},{16,16},{30,16}}};
    std::array<int,2> size = sizes[harness_level-1];
    this->level = harness_level;
    this->flags_cnt = std::array<int,3>({10,40,99})[this->level-1];
    for (int x =0;x<size[0];x++){
        std::vector<char> temp;
        for (int y = 0 ; y < size[1];y++){
            temp.push_back(0);
        }
        this->_board.push_back(temp);
        this->visible_board.push_back(temp);
    }

}

void minesweeper_engine::Minesweeper::generate_safe_field(std::array<int, 2> pos) {
    int bombs_amount = std::array<int,3>({10,40,99})[this->level-1];

    for (int i =0; i < bombs_amount;i++){
        while (1){
            std::uniform_int_distribution<int> dis(0, this->_board.size()-1);
            std::uniform_int_distribution<int> dis2(0, this->_board[0].size()-1);
            std::array<int,2> p({dis(_rd),dis2(_rd)});
            if ((abs(p[0]-pos[0])<=1 && abs(p[1]-pos[1])<=1) || this->_board[p[0]][p[1]] == 9){
                continue;
            }
            else{
                this->_board[p[0]][p[1]] = 9;
                break;
            }
        }
    }

    //COUNT BOMBS AROUND EACH SQUARE


    for (int x =0; x < static_cast<int>(this->_board.size());x++){
        for (int y =0; y < static_cast<int>(this->_board[0].size());y++){
            if ( this->_board[x][y] == 9){
                continue;
            }
            int cnt = 0;
            for (auto offset : this->_offsets){
                if (x+offset[0] < 0 || x+offset[0] >= static_cast<int>(this->_board.size()) || y+offset[1] < 0 || y+offset[1] >= static_cast<int>(this->_board[0].size())){
                    continue;
                }
                cnt += this->_board[x+offset[0]][y+offset[1]] == 9;
            }
            this->_board[x][y] = cnt;
        }
    }
    _is_stated = true;
}

std::vector<int> minesweeper_engine::Minesweeper::get_possible_moves(int col) {
    std::array<std::array<int,2>,8> offsets{{{1,0},{1,1},{0,1},{-1,0},{-1,-1},{0,-1},{1,-1},{-1,1}}};
    std::set<int> cols ;
    for (int x =(col != -1? col:0) ; x < static_cast<int>((col != -1? col+1:this->visible_board.size()));x++){
        for (int y =0 ; y <  static_cast<int>(this->visible_board[x].size());y++){
            if (this->visible_board[x][y] == 0 || this->visible_board[x][y] == 10){
                cols.insert((col != -1? y :x));
            }
            else{
                for (auto offset : offsets){
                    if (x+offset[0] < 0 || x+offset[0] >= static_cast<int>(this->_board.size()) || y+offset[1] < 0 || y+offset[1] >= static_cast<int>(this->_board[0].size())){
                        continue;
                    }
                    if (this->visible_board[x+offset[0]][y+offset[1]] == 0){
                        cols.insert((col != -1? y :x));
                        break;
                    }
                }
            }
        }
    }
    return std::vector<int>(cols.begin(), cols.end());
}

minesweeper_engine::STATES minesweeper_engine::Minesweeper::make_action(minesweeper_engine::ACTIONS action, std::array<int, 2> pos) {
    if (action == DIG){
        this->visible_board[pos[0]][pos[1]] = this->_board[pos[0]][pos[1]];
        if (this->visible_board[pos[0]][pos[1]] == 0){
            this->open_all_zeros_around(pos);
        }
    }
    else if (action == PLACE_FLAG){
        this->visible_board[pos[0]][pos[1]] = 10;
        this->flags_cnt--;
    }
    else if (action == REMOVE_FLAG){
        this->visible_board[pos[0]][pos[1]] = 0;
        this->flags_cnt++;
    }
    else if (action == REMOVE_FLAG_AND_DIG){
        this->flags_cnt++;
        this->visible_board[pos[0]][pos[1]] = this->_board[pos[0]][pos[1]];
        if (this->visible_board[pos[0]][pos[1]] == 0){
            this->visible_board[pos[0]][pos[1]] = 11;
        }
    }
    else{
        this->open_all_zeros_around(pos);
    }

    if(!_is_stated) {
        return CONTINUE;
    }
    for (int x = 0 ; x < static_cast<int>(this->_board.size());x++){
        for (int y = 0 ; y < static_cast<int>(this->_board[0].size());y++){
            if (this->visible_board[x][y] == 9){
                return LOSE;
            }
        }
    }

    for (int x = 0 ; x < static_cast<int>(this->_board.size());x++){
        for (int y = 0 ; y < static_cast<int>(this->_board[0].size());y++){
            if (this->_board[x][y] == 9 && this->visible_board[x][y] == 0){
                return CONTINUE;
            }
        }
    }
    return WIN;
}

void minesweeper_engine::Minesweeper::open_all_zeros_around(const std::array<int, 2>& pos, bool fst) {
    if (this->visible_board[pos[0]][pos[1]] != 0 && !fst){
        return;
    }

    this->visible_board[pos[0]][pos[1]] = this->_board[pos[0]][pos[1]];
    if (this->visible_board[pos[0]][pos[1]] == 0){
        this->visible_board[pos[0]][pos[1]] = 11;
    }
    if (fst || this->visible_board[pos[0]][pos[1]] == 11){
        for (auto i :this->_offsets){
            if (pos[0] + i[0] < 0 || pos[0] + i[0] >= static_cast<int>(this->_board.size()) || pos[1] + i[1] < 0 || pos[1] + i[1] >= static_cast<int>(this->_board[0].size())) {
                continue;
            }
            this->open_all_zeros_around({pos[0] + i[0], pos[1] + i[1]}, false);
        }
    }


}

void minesweeper_engine::Minesweeper::end_of_game() {
    for (int x = 0 ; x < static_cast<int>(this->_board.size());x++){
        for (int y= 0 ; y < static_cast<int>(this->_board[x].size());y++){
            if (this->_board[x][y] == 9){
                if (this->visible_board[x][y] != 10){
                    this->visible_board[x][y] = 9;
                }
            }
            else if (this->visible_board[x][y] == 10){
                this->visible_board[x][y] = 12;
            }
        }
    }
}

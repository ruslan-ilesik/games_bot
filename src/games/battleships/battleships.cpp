//
// Created by rusla on 29.08.2023.
//

#include "./battleships.hpp"

#include <map>
#include <string>
#include <iostream>


namespace  battleships_engine{
    std::map<Ship_types,int> ship_sizes{
            {Ship_types::Carrier, 5},
            {Ship_types::Battleship,4},
            {Ship_types::Destroyer,3},
            {Ship_types::Submarine,3},
            {Ship_types::Patrol_Boat,2}
    };

    std::map<Ship_types, std::string> ship_type_to_string{
            {Ship_types::Carrier, "Carrier"},
            {Ship_types::Battleship, "Battleship"},
            {Ship_types::Destroyer, "Destroyer"},
            {Ship_types::Submarine, "Submarine"},
            {Ship_types::Patrol_Boat, "Patrol Boat"}
    };


};


battleships_engine::Battleships::Battleships(): player_1(0), player_2(1),player_now(&player_1),is_game_started(false) {
}

battleships_engine::Player &battleships_engine::Battleships::get_player_by_ind(short ind) {
    if (ind == 0){
        return player_1;
    }
    else if (ind == 1){
        return player_2;
    }
    else{
        throw std::out_of_range("index out of range, expected (0,1), but got " + std::to_string(ind));
    }
}

void battleships_engine::Battleships::start_game() {
    if (!this->is_game_can_be_started()){
        throw std::runtime_error("Not all players are ready. Have you forgot to call make_ready() on each of them?");
    }
    this->is_game_started = true;
    this->player_now = &player_1;
}

battleships_engine::Player* battleships_engine::Battleships::get_player_now() {return player_now;}

bool battleships_engine::Battleships::is_game_can_be_started() {
    return player_1.is_ready() && player_2.is_ready();
}

bool battleships_engine::Battleships::is_game_active() {return is_game_started;}

std::set<int> battleships_engine::Battleships::get_free_cols_for_attack() {
    std::set<int> cols;
    auto& field  = get_enemy_field();
    for (int x=0; x <10;x++){
        for (int y = 0; y < 10; y++){
            if (field[x][y] != Cell_states::MISSED &&  field[x][y] != Cell_states::DAMAGED){
                cols.insert(x);
                break;
            }
        }
    }
    return cols;
}

battleships_engine::Field &battleships_engine::Battleships::get_enemy_field() {
    return (&this->player_1) == this->player_now ? player_2.get_field() : player_1.get_field() ;
}

std::set<int> battleships_engine::Battleships::get_free_rows_for_attack(int col) {
    std::set<int> rows;
    auto& field  = get_enemy_field();
    for (int y = 0; y < 10; y++){
        if (field[col][y]  != Cell_states::MISSED  &&  field[col][y] != Cell_states::DAMAGED){
            rows.insert(y);
        }
    }
    return rows;
}

bool battleships_engine::Battleships::attack(const battleships_engine::Cell &position) {
    auto& field = this->get_enemy_field();
    if (field[position.x][position.y] == Cell_states::MISSED || field[position.x][position.y] == Cell_states::DAMAGED){
        throw std::runtime_error(std::string(position)+" was already attacked");
    }
    bool is_damaged = false;
    for (const auto& i: this->get_enemy().get_ships()){
        is_damaged = is_damaged || i->attack(position,&field);
    }
    if (is_damaged){
        field[position.x][position.y] = Cell_states::DAMAGED;
    }
    else{
        field[position.x][position.y] = Cell_states::MISSED;

        if ((&player_1) == player_now){
            player_now = &player_2;
        }
        else{
            player_now = &player_1;
        }
    }
    return is_damaged;

}

battleships_engine::Player &battleships_engine::Battleships::get_enemy() {
    return (&this->player_1) == this->player_now ? this->player_2 : this->player_1;
}

int battleships_engine::Battleships::get_winner() {
    bool win = true;
    for (auto& i: player_1.get_ships()){
        win = win && i->is_dead();
    }
    if (win){
        return 1;
    }
    win = true;
    for (auto& i: player_2.get_ships()){
        win = win && i->is_dead();
    }
    if (win){
        return 0;
    }
    return -1;


}

battleships_engine::Player::Player(short id) {
    this->id = id;
    for (auto & i : this->my_field){
        for (auto & j : i){
            j = Cell_states::EMPTY;
        }
    }
    this->ships ={
                      std::unique_ptr<Ship>(new class Carrier),
                      std::unique_ptr<Ship>(new class Battleship),
                      std::unique_ptr<Ship>(new class Destroyer),
                      std::unique_ptr<Ship>(new class Submarine),
                      std::unique_ptr<Ship>(new class Patrol_Boat)
              };

}

void battleships_engine::Player::make_ready() {
    if (!this->is_ready()){
            if (!this->can_be_ready()) {
                throw std::runtime_error("not all ships of this player is placed");
            }
        }
        this->ready = true;
    }


bool battleships_engine::Player::is_ready() {
    return this->ready;
}

bool battleships_engine::Player::can_be_ready() {
    for (auto & ship : this->ships) {
        if (!ship->is_placed()) {
            return false;
        }
    }
    return true;
}

std::vector<battleships_engine::Ship*> battleships_engine::Player::get_unplaced_ships() {
    std::vector<Ship*> v;
    for (auto& i : ships){
        if (!i->is_placed()){
            v.push_back(i.get());
        }
    }
    return v;
}

std::vector<battleships_engine::Ship *> battleships_engine::Player::get_placed_ships() {
    std::vector<Ship*> v;
    for (auto& i : ships){
        if (i->is_placed()){
            v.push_back(i.get());
        }
    }
    return v;
}


battleships_engine::Cell::Cell(int x, int y, bool killed) {
    this->x = x;
    this->y = y;
    this->killed = killed;
}

bool battleships_engine::Cell::is_near(const Cell& cell) const {
    return (this->x == cell.x || this->x+1 == cell.x || this->x-1 == cell.x) && (this->y == cell.y || this->y == cell.y+1 || this->y == cell.y-1);
}

void battleships_engine::Cell::place(battleships_engine::Field &field, int x, int y) {
    if (x > 9 || y > 9 || x < 0 || y < 0){
        throw std::out_of_range("Position cords must be between 0 and 9, not "+std::to_string(x)+","+std::to_string(y));
    }
    // Clear the old position if it was occupied
    if (this->x != -1) {
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                int newX = this->x + i;
                int newY = this->y + j;
                if (newX >= 1 && newX <= 8 && newY >= 1 && newY <= 8) {
                    field[newX][newY] = Cell_states::EMPTY;
                }
            }
        }
    }

    // Set the new position as occupied
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int newX = x + i;
            int newY = y + j;
            if (newX >= 1 && newX <= 8 && newY >= 1 && newY <= 8) {
                field[newX][newY] = Cell_states::OCCUPIED;
            }
        }
    }
    this->x = x;
    this->y = y;
}

void battleships_engine::Cell::unplace(battleships_engine::Field& field) {
    if (this->x != -1) {
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                int newX = this->x + i;
                int newY = this->y + j;
                if (newX >= 0 && newX <= 9 && newY >= 0 && newY <= 9) {
                    field[newX][newY] = Cell_states::EMPTY;
                }
            }
        }
    }
    this->x = -1;
    this->y = -1;
}

battleships_engine::Cell::operator std::string() const {
    return std::format("Cell: ({}, {}, is_dead: {} )",this->x,this->y,(this->killed ? "true" : "false"));
}


battleships_engine::Ship::Ship(battleships_engine::Ship_types s_t) {
    this->rotated = false;
    this->my_type = s_t;
    for (int i =0; i< ship_sizes[this->my_type];i++){
        this->cells.push_back(Cell(-1,-1));
    }

}

bool battleships_engine::Ship::is_dead() {
    bool dead= true;
    for (const auto& i: cells){
        dead = dead & i.killed;
    }
    return dead;
}

bool battleships_engine::Ship::is_rotated() const {
    return this->rotated;
}

bool battleships_engine::Ship::is_rotatable(const Ships_container &ships) {
        if (!this->is_placed()){
            return true;
        }

        if (this->is_rotated()){
            int y = this->cells[0].y;
            if (y+ship_sizes[this->my_type] >10){
                return false;
            }
            Cell temp_cell = Cell(this->cells[0].x,y);
            for (int i = 1; i < ship_sizes[this->my_type]; i++){
                temp_cell.y = y+i;
                for (const auto& ship : ships){
                    if (ship.get() != this && ship->is_overlapping(temp_cell)){
                        return false;
                    }
                }
            }
        }
        else{
            //std::cout << this->cells[0] << '\n';
            int x = this->cells[0].x;
            if (x+ship_sizes[this->my_type] >10){
                return false;
            }
            Cell temp_cell = Cell(x,this->cells[0].y);
            for (int i = 1; i < ship_sizes[this->my_type]; i++){
                temp_cell.x = x+i;
                for (const auto& ship : ships){
                    if (ship.get() != this && ship->is_overlapping(temp_cell)){
                        return false;
                    }
                }
            }
        }
        return true;
}

bool battleships_engine::Ship::is_overlapping(const Cell& cell) {
    if (!this->is_placed()){
        return false;
    }
    for(const auto& my_cell : this->cells){
        if (cell.is_near(my_cell)){
            return true;
        }
    }
    return false;
}

bool battleships_engine::Ship::is_placed() {
    //std::cout << "Position of fst cell is "  << this->cells[0].x << " " <<  this->cells[0].y << '\n';
    return this->cells[0].x != -1;
}

void battleships_engine::Ship::rotate(const Ships_container& ships,Field& my_field) {
    if (!this->is_rotatable(ships)){
        throw std::runtime_error(std::format("Ship can not be rotated in current position: {}, is_roated: {}",std::string(cells[0]),is_rotated()));
    }


    if (!this->is_placed()){
        this->rotated = !this->rotated;
        return;
    }

    Cell temp{this->cells[0].x, this->cells[0].y};
    this->unplace(my_field);
    this->rotated = !this->rotated;
    this->place(ships,my_field,temp);
    if (this->is_rotated()){
        for (int i =1; i < static_cast<int>(this->cells.size());i++){
            this->cells[i].x  = this->cells[0].x;
            this->cells[i].y  = this->cells[0].y+i;
        }
    }
    else{
        for (int i =1; i < static_cast<int>(this->cells.size());i++){
            this->cells[i].y  = this->cells[0].y;
            this->cells[i].x  = this->cells[0].x+i;
        }
    }

}

bool battleships_engine::Ship::can_be_placed(const battleships_engine::Ships_container &ships,
                                             const battleships_engine::Cell &cell) {

    Cell temp_cell = Cell(cell.x,cell.y);
    if (this->is_rotated()){
        if (temp_cell.x+this->cells.size()-1 > 9){
            return false;
        }
        for (int i =0; i < static_cast<int>(this->cells.size()); i++){
            temp_cell.x = cell.x+i;
            for (const auto& i : ships){
                if (i.get()!= this && i->is_overlapping(temp_cell)){
                    return false;
                }
            }
        }
        return true;
    }
    else{
        if (temp_cell.y+this->cells.size()-1 > 9){
            return false;
        }
        for (int i =0; i < static_cast<int>(this->cells.size()); i++){
            temp_cell.y = cell.y+i;
            for (const auto& i : ships){
                if (i.get()!= this && i->is_overlapping(temp_cell)){
                    return false;
                }
            }
        }
        return true;
    }
}

void battleships_engine::Ship::place(const Ships_container& ships,Field& field,const Cell& cell) {
   // std::cout << cell << '\n';
    if (!this->can_be_placed(ships,cell)){
        throw std::runtime_error("Ship can`t be placed in current position with current rotation");
    }

    if (this->is_rotated()){
        Cell temp_cell = {cell.x,cell.y};
        for (int i =0; i < static_cast<int>(this->cells.size()); i++){
            temp_cell.x = cell.x+i;
            this->cells[i].place(field,temp_cell.x,temp_cell.y);
        }
    }
    else{
        Cell temp_cell = {cell.x,cell.y};
        for (int i =0; i < static_cast<int>(this->cells.size()); i++){
            temp_cell.y = cell.y+i;
            this->cells[i].place(field,temp_cell.x,temp_cell.y);
        }
    }
    //std::cout << "Position of fst cell is "  << this->cells[0].x << " " <<  this->cells[0].y << '\n';
}

battleships_engine::Ship_types battleships_engine::Ship::get_type() { return this->my_type;}

const std::vector<battleships_engine::Cell> &battleships_engine::Ship::get_cells() {return this->cells;}

std::array<int, 2> battleships_engine::Ship::get_position() {return {cells[0].x,cells[0].y};}

void battleships_engine::Ship::unplace(Field& field) {
    for (auto& i : cells){
        i.unplace(field);
    }
}

battleships_engine::Ship::operator std::string() const {
    return ship_type_to_string[this->my_type];
}

bool battleships_engine::Ship::operator==(const std::string &str) const {
    return str == this->operator std::string();
}

bool battleships_engine::Ship::can_be_placed_in_any_rotation(const battleships_engine::Ships_container &ships,
                                                             battleships_engine::Cell &cell) {
    if (can_be_placed(ships,cell)){
        return true;
    }
    this->rotated = !this->rotated;
    if (can_be_placed(ships,cell)){
        this->rotated = !this->rotated;
        return true;
    }
    this->rotated = !this->rotated;
    return false;
}

void battleships_engine::Ship::place_same_position(Field& field) {
    Cell& cell = this->cells[0];
    if (this->is_rotated()){
        Cell temp_cell = {cell.x,cell.y};
        for (int i =0; i < static_cast<int>(this->cells.size()); i++){
            temp_cell.x = cell.x+i;
            this->cells[i].place(field,temp_cell.x,temp_cell.y);
        }
    }
    else{
        Cell temp_cell = {cell.x,cell.y};
        for (int i =0; i < static_cast<int>(this->cells.size()); i++){
            temp_cell.y = cell.y+i;
            this->cells[i].place(field,temp_cell.x,temp_cell.y);
        }
    }
}

bool battleships_engine::Ship::attack(const battleships_engine::Cell &position,Field* field) {
    const auto& missed_cell = [=](std::array<int,2> pos, std::array<int,2> offset = {0,0}){
        std::array<int,2> irl_post = {pos[0]+offset[0],pos[1]+offset[1]};
        if (irl_post[0] < 0 || irl_post[1] < 0 || irl_post[0] > 9 || irl_post[1] > 9 ){
            return ;
        }
        (*field)[irl_post[0]][irl_post[1]] = Cell_states::MISSED;
    };

    for (auto& i : this->cells){
        if (i.x == position.x && i.y == position.y){
            if (i.killed){
                throw std::runtime_error(std::string(position) + " is attacking cell " + std::string(i)+" which is already killed");
            }
            i.killed = true;
            (*field)[i.x][i.y] = Cell_states::DAMAGED;
            if (this->is_dead()){
                std::array<std::array<int,2>,2> offsets{};
                if (this->is_rotated()){
                    offsets[0] = {0,-1};
                    offsets[1] = {0,1};
                }
                else{
                    offsets[0] = {-1,0};
                    offsets[1] = {1,0};
                }
                std::array<int,2> temp_pos;
                //middle cells have very simple algorithm to draw
                for (int i =1; i < static_cast<int>(cells.size()-1);i++){
                    temp_pos = {cells[i].x,cells[i].y};
                    for (auto& offset : offsets){
                        missed_cell(temp_pos,offset);
                    }
                }

                //first cells require much more work
                temp_pos = {cells[0].x,cells[0].y};
                missed_cell(temp_pos, {-1,-1});
                missed_cell(temp_pos, {-1,0});
                missed_cell(temp_pos, {0,-1});
                if (this->is_rotated()){
                    missed_cell(temp_pos, {0,1});
                    missed_cell(temp_pos, {-1,1});
                }
                else{
                    missed_cell(temp_pos, {1,-1});
                    missed_cell(temp_pos, {1,0});
                }

                //last cells require much more work
                temp_pos = {cells[cells.size()-1].x,cells[cells.size()-1].y};
                missed_cell(temp_pos, {0,1});
                missed_cell(temp_pos, {1,0});
                missed_cell(temp_pos, {1,1});
                if (this->is_rotated()){
                    missed_cell(temp_pos, {0,-1});
                    missed_cell(temp_pos, {1,-1});
                }
                else{
                    missed_cell(temp_pos, {-1,0});
                    missed_cell(temp_pos, {-1,1});
                }
            }

            return true;
        }
    }
    return false;
}


battleships_engine::Field &battleships_engine::Player::get_field() {return my_field;}

battleships_engine::Ships_container &battleships_engine::Player::get_ships() {return ships;}

std::set<int> battleships_engine::Player::get_free_columns(battleships_engine::Ship *ship) {
    std::set<int> cols;
    Cell temp_cell(-1,-1);
    for (int i =0; i < 10; i++){
        for (int j = 0; j < 10; j++){
            temp_cell.x = i;
            temp_cell.y = j;
            if (ship->can_be_placed_in_any_rotation(this->ships,temp_cell)){
                cols.insert(i);
                break;
            }
        }
    }
    return cols;
}

std::set<int> battleships_engine::Player::get_free_positions_in_col(battleships_engine::Ship *ship, int col) {
    std::set<int> cells;
    Cell temp_cell(-1,-1);
    for (int j = 0; j < 10; j++){
        temp_cell.x = col;
        temp_cell.y = j;
        //std::cout << "cell pos " << temp_cell.x << " " << temp_cell.y << '\n';
        if (ship->can_be_placed_in_any_rotation(this->ships,temp_cell)){
            //std::cout<< "succes\n";
            cells.insert(j);
        }
    }
    return cells;
}

void battleships_engine::Player::random_place_ships() {
    //unplace all existing ships
    for (auto& i : this->get_placed_ships()){
        i->unplace(this->my_field);
    }
    // start random placing
    for (auto&i : ships){
        //std::cout << ship_type_to_string[i->get_type()] << '\n';
        auto free_cols = get_free_columns(i.get());
        int x = *std::next(free_cols.begin(),rand() % free_cols.size());
        auto free_in_col = get_free_positions_in_col(i.get(),x);
        int y = *std::next(free_in_col.begin(),rand() % free_in_col.size());


        if (i->can_be_placed(ships,{x,y})){
            i->rotate(ships,my_field);
            if (i->can_be_placed(ships,{x,y})) {
                if (randint(0,1) == 0){
                    i->rotate(ships,my_field);
                }
                i->place(ships,my_field,{x,y});
            }
            else{
                i->rotate(ships,my_field);
                i->place(ships,my_field,{x,y});
            }
        }
        else{
            i->rotate(ships,my_field);
            i->place(ships,my_field,{x,y});
        }


    }
}

int battleships_engine::Player::randint(int min, int max) {
    return min + (rand() % static_cast<int>(max - min + 1));
}


battleships_engine::Carrier::Carrier(): Ship(Ship_types::Carrier) {
}

battleships_engine::Battleship::Battleship(): Ship(Ship_types::Battleship) {
}

battleships_engine::Destroyer::Destroyer(): Ship(Ship_types::Destroyer) {
}

battleships_engine::Submarine::Submarine(): Ship(Ship_types::Submarine) {
}

battleships_engine::Patrol_Boat::Patrol_Boat(): Ship(Ship_types::Patrol_Boat) {
}


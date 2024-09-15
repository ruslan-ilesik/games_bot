//
// Created by rusla on 29.08.2023.
//

#pragma once
#include <array>
#include <map>
#include <memory>
#include <random>
#include <ranges>
#include <set>
#include <string>
#include <vector>

namespace battleships_engine {
    class Ship;

    /**
     * @enum Cell_states
     * @brief Represents the different possible states of a cell in the game field.
     */
    enum Cell_states {
        EMPTY, ///< The cell is empty (no ship, no hit).
        MISSED, ///< A shot was fired at this cell but it missed.
        DAMAGED, ///< A ship occupies this cell and it has been hit.
        OCCUPIED ///< A ship occupies this cell but it has not been hit.
    };

    /**
     * @enum Ship_types
     * @brief Enum representing the different types of ships available in the game.
     */
    enum Ship_types {
        Carrier, ///< Carrier ship (size 5).
        Battleship, ///< Battleship (size 4).
        Destroyer, ///< Destroyer (size 3).
        Submarine, ///< Submarine (size 3).
        Patrol_Boat ///< Patrol Boat (size 2).
    };

    /**
     * @var ship_sizes
     * @brief A map that associates each ship type with its corresponding size.
     *
     * This map is used to determine how many cells each ship occupies on the field.
     * - Carrier: 5 cells
     * - Battleship: 4 cells
     * - Destroyer: 3 cells
     * - Submarine: 3 cells
     * - Patrol Boat: 2 cells
     */
    extern std::map<Ship_types, int> ship_sizes;

    /**
     * @var ship_type_to_string
     * @brief A map that converts ship types to their string representations.
     *
     * This map is useful for displaying or logging ship types in a human-readable format.
     */
    extern std::map<Ship_types, std::string> ship_type_to_string;

    /**
     * @typedef Field
     * @brief A 10x10 game field where each cell holds a state defined by `Cell_states`.
     *
     * This array represents the player's game board where ships are placed, and attacks are tracked.
     */
    typedef std::array<std::array<Cell_states, 10>, 10> Field;

    /**
     * @typedef Ships_container
     * @brief A container that holds unique pointers to a player's ships.
     *
     * Each player has exactly 5 ships, one of each type.
     */
    typedef std::array<std::unique_ptr<Ship>, 5> Ships_container;

    /**
     * @class Player
     * @brief Represents a player in the Battleships game, managing their field and ships.
     */
    class Player {
    private:
        Field my_field{}; ///< The player's game field (10x10 grid).
        Ships_container ships; ///< The player's collection of ships (5 in total).
        bool ready = false; ///< Flag indicating if the player is ready to start the game.
        short id; ///< Player's unique ID (used for distinguishing between players).

        /**
         * @brief Generates a random integer between the specified range.
         * @param min The minimum value (inclusive).
         * @param max The maximum value (inclusive).
         * @return A random integer between min and max.
         */
        static int randint(int min, int max);

    public:
        /**
         * @brief Constructor for the Player class.
         * @param id The unique ID of the player.
         */
        Player(short id);

        /**
         * @brief Retrieves a list of the player's unplaced ships.
         * @return A vector containing pointers to ships that have not yet been placed on the field.
         */
        std::vector<Ship *> get_unplaced_ships();

        /**
         * @brief Retrieves a list of the player's placed ships.
         * @return A vector containing pointers to ships that are already placed on the field.
         */
        std::vector<Ship *> get_placed_ships();

        /**
         * @brief Provides access to the player's ships.
         * @return A reference to the Ships_container that holds the player's ships.
         */
        Ships_container &get_ships();

        /**
         * @brief Determines which columns are free for placing a ship.
         * @param ship Pointer to the ship to be placed.
         * @return A set of column indices where the ship can be placed.
         */
        std::set<int> get_free_columns(Ship *ship);

        /**
         * @brief Determines which positions in a given column are free for placing a ship.
         * @param ship Pointer to the ship to be placed.
         * @param col The column index to check.
         * @return A set of row indices in the specified column where the ship can be placed.
         */
        std::set<int> get_free_positions_in_col(Ship *ship, int col);

        /**
         * @brief Marks the player as ready to start the game.
         */
        void make_ready();

        /**
         * @brief Checks if the player is ready to start the game.
         * @return True if the player is ready, false otherwise.
         */
        bool is_ready();

        /**
         * @brief Checks if the player has placed all their ships and can be marked as ready.
         * @return True if all ships are placed and the player can be ready, false otherwise.
         */
        bool can_be_ready();

        /**
         * @brief Retrieves the player's game field.
         * @return A reference to the player's Field (10x10 grid).
         */
        Field &get_field();

        /**
         * @brief Randomly places all the player's ships on the field.
         */
        void random_place_ships();
    };

    /**
     * @class Cell
     * @brief Represents a cell on the game field. A cell has coordinates (x, y) and a killed state, which indicates
     * whether the cell is part of a ship that has been destroyed.
     */
    class Cell {
    public:
        int x; ///< X-coordinate of the cell (horizontal position on the field).
        int y; ///< Y-coordinate of the cell (vertical position on the field).
        bool killed; ///< Flag indicating if the cell is part of a ship that has been destroyed.

        /**
         * @brief Constructs a Cell with given coordinates and killed status.
         * @param x The x-coordinate of the cell.
         * @param y The y-coordinate of the cell.
         * @param killed (Optional) Whether the cell is part of a destroyed ship. Defaults to false.
         */
        Cell(int x, int y, bool killed = false);

        /**
         * @brief Constructs a Cell from a container of two integer values (x and y coordinates).
         * @tparam Container A container type that holds integers.
         * @param values A container with exactly two integer values representing the cell's coordinates.
         * @throws std::runtime_error if the container does not contain exactly two elements.
         */
        template<typename Container>
            requires(std::same_as<std::ranges::range_value_t<Container>, int>)
        Cell(const Container &values) : killed(false) {
            if (std::ranges::size(values) != 2) {
                throw std::runtime_error("There should be 2 values in the container");
            }
            this->x = values[0];
            this->y = values[1];
        }

        /**
         * @brief Checks if this cell is adjacent to another cell (horizontally or vertically).
         * @param cell The other cell to check adjacency with.
         * @return True if this cell is adjacent to the given cell, false otherwise.
         */
        bool is_near(const Cell &cell) const;

        /**
         * @brief Places the cell on a game field at the specified coordinates.
         * @param field The game field (10x10 grid) where the cell will be placed.
         * @param x The x-coordinate on the field where the cell should be placed.
         * @param y The y-coordinate on the field where the cell should be placed.
         */
        void place(Field &field, int x, int y);

        /**
         * @brief Removes the cell from the specified game field.
         * @param field The game field (10x10 grid) from which the cell should be removed.
         */
        void unplace(Field &field);

        /**
         * @brief Converts the Cell object to a string representation.
         * @return A string representing the cell's coordinates and its killed state.
         */
        operator std::string() const;

        /**
         * @brief Stream insertion operator for printing the cell.
         * @param os The output stream.
         * @param cell The cell to be printed.
         * @return The output stream with the cell information appended.
         */
        friend std::ostream &operator<<(std::ostream &os, const Cell &cell) {
            os << std::string(cell);
            return os;
        }
    };

    /**
     * @class Ship
     * @brief Represents a ship in the Battleship game. A ship can be rotated, placed on the field, and attacked.
     */
    class Ship {
    protected:
        Ship_types my_type; ///< The type of ship (Carrier, Battleship, etc.).
        bool rotated; ///< Whether the ship is rotated (true if vertical, false if horizontal).
        std::vector<Cell> cells; ///< The cells occupied by the ship on the field.

    public:
        /**
         * @brief Constructor for the Ship class.
         * @param s_t The type of ship (Ship_types).
         */
        Ship(Ship_types s_t);

        /**
         * @brief Returns the type of the ship.
         * @return The type of the ship (Ship_types).
         */
        Ship_types get_type();

        /**
         * @brief Returns the cells occupied by the ship.
         * @return A constant reference to a vector of cells occupied by the ship.
         */
        const std::vector<Cell> &get_cells();

        /**
         * @brief Returns the position of the ship on the field (starting cell).
         * @return An array with two integers representing the x and y coordinates of the starting position.
         */
        std::array<int, 2> get_position();

        /**
         * @brief Checks if the ship has been completely destroyed.
         * @return True if all the ship's cells are destroyed, false otherwise.
         */
        bool is_dead();

        /**
         * @brief Checks if the ship is rotated (vertical).
         * @return True if the ship is rotated vertically, false if it is horizontal.
         */
        bool is_rotated() const;

        /**
         * @brief Checks if the ship can be rotated in its current position without overlapping other ships.
         * @param ships The collection of all ships to check against.
         * @return True if the ship can be rotated, false otherwise.
         */
        bool is_rotatable(const Ships_container &ships);

        /**
         * @brief Checks if the ship overlaps with a given cell.
         * @param cell The cell to check against.
         * @return True if the ship overlaps with the specified cell, false otherwise.
         */
        bool is_overlapping(const Cell &cell);

        /**
         * @brief Checks if the ship has been placed on the field.
         * @return True if the ship is placed, false otherwise.
         */
        bool is_placed();

        /**
         * @brief Rotates the ship if possible, given the current ship arrangement and field.
         * @param ships The collection of ships to check for overlap.
         * @param my_field The field on which the ship is placed.
         */
        void rotate(const Ships_container &ships, Field &my_field);

        /**
         * @brief Checks if the ship can be placed in a specific position without colliding with other ships.
         * @param ships The collection of ships to check for collisions.
         * @param cell The cell where the ship is to be placed.
         * @return True if the ship can be placed, false otherwise.
         */
        bool can_be_placed(const Ships_container &ships, const Cell &cell);

        /**
         * @brief Checks if the ship can be placed in any rotation at a specific position.
         * @param ships The collection of ships to check for collisions.
         * @param cell The cell where the ship is to be placed.
         * @return True if the ship can be placed in any rotation, false otherwise.
         */
        bool can_be_placed_in_any_rotation(const Ships_container &ships, Cell &cell);

        /**
         * @brief Places the ship on the field at the specified position, ensuring no overlap with other ships.
         * @param ships The collection of ships to check for collisions.
         * @param field The field on which the ship is to be placed.
         * @param cell The cell where the ship is to be placed.
         */
        void place(const Ships_container &ships, Field &field, const Cell &cell);

        /**
         * @brief Places the ship at its current position on the field.
         * @param field The field on which the ship is placed.
         */
        void place_same_position(Field &field);

        /**
         * @brief Removes the ship from the field.
         * @param field The field from which the ship is to be removed.
         */
        void unplace(Field &field);

        /**
         * @brief Attacks the ship at a specific cell.
         * @param position The cell where the attack is directed.
         * @param field The field where the attack takes place.
         * @return True if the attack hits the ship, false if it's a miss.
         */
        bool attack(const Cell &position, Field *field);

        /**
         * @brief Converts the Ship object to a string representation.
         * @return A string representing the ship type and its state.
         */
        operator std::string() const;

        /**
         * @brief Compares the ship's type to a string representation.
         * @param str The string to compare to the ship type.
         * @return True if the string matches the ship's type, false otherwise.
         */
        bool operator==(const std::string &str) const;
    };

    /**
     * @class Carrier
     * @brief Represents a Carrier type ship.
     */
    class Carrier : public Ship {
    public:
        /**
         * @brief Constructs a Carrier type ship.
         */
        Carrier();
    };

    /**
     * @class Battleship
     * @brief Represents a Battleship type ship.
     */
    class Battleship : public Ship {
    public:
        /**
         * @brief Constructs a Battleship type ship.
         */
        Battleship();
    };

    /**
     * @class Destroyer
     * @brief Represents a Destroyer type ship.
     */
    class Destroyer : public Ship {
    public:
        /**
         * @brief Constructs a Destroyer type ship.
         */
        Destroyer();
    };

    /**
     * @class Submarine
     * @brief Represents a Submarine type ship.
     */
    class Submarine : public Ship {
    public:
        /**
         * @brief Constructs a Submarine type ship.
         */
        Submarine();
    };

    /**
     * @class Patrol_Boat
     * @brief Represents a Patrol Boat type ship.
     */
    class Patrol_Boat : public Ship {
    public:
        /**
         * @brief Constructs a Patrol Boat type ship.
         */
        Patrol_Boat();
    };


   /**
 * @class Battleships
 * @brief Represents the main game engine for the Battleships game. Manages the players, game state, and interactions during the game.
 */
class Battleships {
private:
    Player player_1;         ///< The first player.
    Player player_2;         ///< The second player.
    Player *player_now;      ///< Pointer to the current player (the one whose turn it is).
    bool is_game_started;    ///< Flag indicating whether the game has started.

public:
    /**
     * @brief Constructor for the Battleships class. Initializes the game and players.
     */
    Battleships();

    /**
     * @brief Gets the current player (the player whose turn it is).
     * @return Pointer to the current player.
     */
    Player *get_player_now();

    /**
     * @brief Gets the set of free columns available for attacking by the current player.
     * @return A set of integers representing the columns where attacks can be made.
     */
    std::set<int> get_free_cols_for_attack();

    /**
     * @brief Gets the set of free rows available for attacking in a specific column.
     * @param col The column to check for free rows.
     * @return A set of integers representing the rows where attacks can be made.
     */
    std::set<int> get_free_rows_for_attack(int col);

    /**
     * @brief Gets the enemy player's field for the current player.
     * @return A reference to the enemy player's field (grid) containing cell states.
     */
    Field &get_enemy_field();

    /**
     * @brief Gets the enemy player for the current player.
     * @return A reference to the enemy player.
     */
    Player &get_enemy();

    /**
     * @brief Retrieves a player by their index (1 for player 1, 2 for player 2).
     * @param ind The index of the player (1 or 2).
     * @return A reference to the player corresponding to the index.
     */
    Player &get_player_by_ind(short ind);

    /**
     * @brief Starts the game if all players are ready and sets the game as active.
     */
    void start_game();

    /**
     * @brief Checks if the game can be started (i.e., both players are ready).
     * @return True if the game can be started, false otherwise.
     */
    bool is_game_can_be_started();

    /**
     * @brief Checks if the game is currently active.
     * @return True if the game is active, false if it has not started or has ended.
     */
    bool is_game_active();

    /**
     * @brief Determines the winner of the game.
     * @return The ID of the winning player, or -1 if there is no winner yet.
     */
    int get_winner();

    /**
     * @brief Attacks the enemy player at a specific cell position.
     * @param position The position on the enemy's field to attack.
     * @return True if the attack is successful (hits a ship), false otherwise (miss).
     */
    bool attack(const Cell &position);
};


} // namespace battleships_engine

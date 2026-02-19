//
// Created by ilesik on 7/24/24.
//

#pragma once

#include <dpp/dpp.h>
#include <src/modules/discord/discord_achievements_processing/discord_achievements_processing.hpp>
#include <src/modules/discord/discord_interactions_handler/discord_button_click_handler/discord_button_click_handler.hpp>
#include "./discord_games_manager/discord_games_manager.hpp"
#include "src/module/module.hpp"
#include "src/modules/database/database.hpp"
#include "src/modules/discord/discord_bot/discord_bot.hpp"
#include "src/modules/image_processing/image_processing.hpp"
#include "src/modules/logging/logging.hpp"

namespace gb {

    /**
     * @enum USER_REMOVE_REASON
     * @brief Represents the reasons a user might be removed from a game.
     */
    enum class USER_REMOVE_REASON {
        UNKNOWN, ///< Reason unknown or not specified.
        TIMEOUT, ///< User was removed due to timeout.
        LOSE, ///< User was removed due to losing the game.
        WIN, ///< User was removed due to winning the game.
        DRAW, ///< User was removed due to the game ending in a draw.
        SAVED ///< User's game progress was saved and they were removed.
    };

    /**
     * @brief Converts a USER_REMOVE_REASON enum value to a string.
     *
     * @param r The USER_REMOVE_REASON value.
     * @return A string representation of the reason.
     * @throws std::runtime_error If the reason is unknown.
     */
    std::string to_string(USER_REMOVE_REASON r);

    /**
     * @struct Game_data_initialization
     * @brief Holds the initialization data required to start a Discord game.
     *
     * Aggregates all required module dependencies used by Discord-based games.
     */
    struct Game_data_initialization {
        std::string name; ///< The name of the game.
        Database_ptr db; ///< Pointer to the database module.
        Discord_bot_ptr bot; ///< Pointer to the Discord bot module.
        Discord_games_manager_ptr games_manager; ///< Pointer to the games manager module.
        Image_processing_ptr image_processing; ///< Pointer to the image processing module.
        Discord_button_click_handler_ptr button_click_handler; ///< Pointer to the button click handler module.
        Discord_achievements_processing_ptr achievements_processing; ///< Pointer to the achievements processing module.
        Logging_ptr log; ///< Pointer to the logging module used for diagnostics and error reporting.
    };

    /**
     * @brief Helper trait that normalizes coroutine return types.
     *
     * Converts supported coroutine return types into a unified
     * `dpp::task<std::string>` so the framework can always forward
     * optional game results to Discord_game::game_stop().
     *
     * @tparam T Coroutine return type to normalize.
     */
    template<typename T>
    struct task_result_normalizer;

    /**
     * @brief Normalizer specialization for `dpp::task<void>`.
     *
     * Awaits the task and returns an empty JSON object string.
     */
    template<>
    struct task_result_normalizer<dpp::task<void>> {
        static dpp::task<std::string> convert(dpp::task<void> t) {
            co_await t;
            co_return "{}";
        }
    };

    /**
     * @brief Normalizer specialization for `dpp::task<std::string>`.
     *
     * Transparently forwards the returned string.
     */
    template<>
    struct task_result_normalizer<dpp::task<std::string>> {
        static dpp::task<std::string> convert(dpp::task<std::string> t) { co_return co_await t; }
    };

    /**
     * @class Discord_game
     * @brief Base class for managing a game on Discord.
     *
     * Provides core functionality for:
     * - player management
     * - game lifecycle handling
     * - Discord interaction
     * - image attachment
     * - database tracking
     *
     * Derived classes implement the actual game logic.
     */
    class Discord_game {
    private:
        std::vector<dpp::snowflake> _players; ///< List of players participating in the game.
        size_t _current_player_ind = 0; ///< Index of the current player.
        Task<Database_return_t> _game_create_req; ///< Async request to create a DB game record.
        uint64_t _unique_game_id = std::numeric_limits<uint64_t>::max(); ///< Unique ID of the game.
        bool _is_game_started = false; ///< Indicates whether the game has started.
        bool _is_game_stopped = false; ///< Indicates whether the game has stopped.

        /**
         * @brief Type-erased stored game entry callback.
         *
         * Accepts a base pointer and vector of packed arguments.
         * Always resolves to `dpp::task<std::string>`.
         */
        std::function<dpp::task<std::string>(Discord_game *, std::vector<std::any>)> _stored_callback;

        /**
         * @brief Invokes a derived member function using type-erased arguments.
         *
         * Unpacks a vector of `std::any` into strongly typed arguments,
         * calls the member function, and normalizes its return value.
         *
         * @tparam Obj Derived game type.
         * @tparam Ret Coroutine return type.
         * @tparam Args Parameter pack of the member function.
         * @tparam I Index sequence for unpacking.
         *
         * @param obj Pointer to derived object.
         * @param mf Member function pointer.
         * @param args_any Packed arguments.
         *
         * @return Normalized coroutine resolving to a result string.
         *
         * @throws std::runtime_error On argument mismatch.
         */
        template<typename Obj, typename Ret, typename... Args, std::size_t... I>
        static dpp::task<std::string> call_from_any(Obj *obj, Ret (Obj::*mf)(Args...), std::vector<std::any> args_any,
                                                    std::index_sequence<I...>) {

            if (args_any.size() != sizeof...(Args)) {
                throw std::runtime_error("start_game: wrong number of arguments for callback");
            }

            try {
                auto task = (obj->*mf)(std::any_cast<std::decay_t<Args>>(std::move(args_any[I]))...);

                co_return co_await task_result_normalizer<Ret>::convert(std::move(task));
            } catch (const std::bad_any_cast &e) {
                throw std::runtime_error(std::string("start_game: argument type mismatch: ") + e.what());
            }
        }

    protected:
        Game_data_initialization _data; ///< Initialization data for the game.
        uint64_t _img_cnt = 0; ///< Counter for generated images.

    public:
        /**
         * @brief Result type for private message creation.
         *
         * Pair format:
         * - first: failure flag (true = failed)
         * - second: map of user → message
         */
        typedef std::pair<bool, std::map<dpp::snowflake, dpp::message>> Direct_messages_return;

        /**
         * @brief Destructor.
         *
         * Ensures the game is properly stopped.
         */
        virtual ~Discord_game();

        /**
         * @brief Returns basic module dependencies required for any game.
         *
         * @return Vector of dependency names.
         */
        static std::vector<std::string> get_basic_game_dependencies();

        /**
         * @brief Returns image generators used by the game.
         *
         * Derived classes should override.
         *
         * @return Vector of (name, generator) pairs.
         */
        static std::vector<std::pair<std::string, image_generator_t>> get_image_generators();

        /**
         * @brief Constructs a Discord_game.
         *
         * @param _data Initialization data.
         * @param players Participating players.
         */
        Discord_game(Game_data_initialization &_data, const std::vector<dpp::snowflake> &players);

        /**
         * @brief Templated constructor binding a derived coroutine entry point.
         *
         * Supports:
         * - dpp::task<void>
         * - dpp::task<std::string>
         *
         * The return value is normalized and forwarded to game_stop().
         */
        template<typename Obj, typename Ret, typename... Args>
        Discord_game(Game_data_initialization &_data, const std::vector<dpp::snowflake> &players,
                     Ret (Obj::*mf)(Args...)) : Discord_game(_data, players) {

            static_assert(std::is_same_v<Ret, dpp::task<void>> || std::is_same_v<Ret, dpp::task<std::string>>,
                          "Member function must return dpp::task<void> or dpp::task<std::string>");

            static_assert((!std::disjunction<std::is_lvalue_reference<Args>...>::value) ||
                              (std::conjunction<std::is_const<std::remove_reference_t<Args>>...>::value),
                          "Non-const lvalue references in callback parameters are not supported");

            _stored_callback = [mf](Discord_game *base_ptr, std::vector<std::any> args_any) -> dpp::task<std::string> {
                Obj *derived = static_cast<Obj *>(base_ptr);
                co_return co_await call_from_any(derived, mf, std::move(args_any), std::index_sequence_for<Args...>{});
            };
        }

        /**
         * @brief Starts the game using the stored coroutine.
         *
         * Handles full lifecycle:
         * - game_start()
         * - run coroutine
         * - normalize result
         * - game_stop()
         *
         * @note Hangman game does it manually in some specific cases.
         */
        template<typename... CallArgs>
        dpp::task<void> start_game(CallArgs &&...call_args) {
            if (!_stored_callback) {
                throw std::runtime_error("No run callback provided for this game");
            }

            static_assert(sizeof...(CallArgs) > 0, "start_game requires at least one argument (the event)");

            auto &first_arg = std::get<0>(std::tuple<CallArgs &&...>(call_args...));

            game_start(first_arg.command.channel_id, first_arg.command.guild_id);

            std::string additional_data = "";

            try {
                std::vector<std::any> packed;
                packed.reserve(sizeof...(CallArgs));
                (packed.emplace_back(std::decay_t<CallArgs>(std::forward<CallArgs>(call_args))), ...);

                additional_data = co_await _stored_callback(this, std::move(packed));
            } catch (...) {
                game_stop();
                throw;
            }

            if (!additional_data.empty()) {
                game_stop(additional_data);
            } else {
                game_stop();
            }
        }


        /**
         * @brief Retrieves the list of players.
         *
         * @return Vector of player snowflake IDs.
         */
        std::vector<dpp::snowflake> get_players();

        /**
         * @brief Advances turn to the next player.
         *
         * @return Snowflake ID of the next player.
         */
        dpp::snowflake next_player();

        /**
         * @brief Gets the current player.
         *
         * @return Snowflake ID of the current player.
         */
        dpp::snowflake get_current_player();

        /**
         * @brief Gets the index of the current player.
         *
         * @return Zero-based player index.
         */
        size_t get_current_player_index() const;

        /**
         * @brief Removes a player from the game.
         *
         * @param reason Reason for removal.
         * @param user Player snowflake ID.
         */
        void remove_player(USER_REMOVE_REASON reason, const dpp::snowflake &user);

        /**
         * @brief Starts the game and registers it with the manager.
         *
         * @param channel_id Channel where the game runs.
         * @param guild_id Guild where the game runs.
         */
        void game_start(const dpp::snowflake &channel_id, const dpp::snowflake &guild_id);

        /**
         * @brief Stops the game and deregisters it.
         *
         * @param additional_data Optional JSON data about the finished game.
         */
        void game_stop(const std::string &additional_data = "{}");

        /**
         * @brief Gets the game name.
         *
         * @return Game name string.
         */
        std::string get_name() const;

        /**
         * @brief Gets the unique game ID.
         *
         * Waits for DB creation if necessary.
         *
         * @return Unique game identifier.
         */
        uint64_t get_uid();

        /**
         * @brief Sets the current player index.
         *
         * @param index New player index.
         * @throws std::runtime_error If index is out of range.
         */
        void set_current_player_index(size_t index);

        /**
         * @brief Gets the number of generated images.
         *
         * @return Image counter.
         */
        uint64_t get_image_cnt() const;

        /**
         * @brief Sends private messages to users.
         *
         * @param ids Users to message.
         * @return Pair containing failure flag and message map.
         */
        dpp::task<Direct_messages_return> get_private_messages(const std::vector<dpp::snowflake> &ids);

        /**
         * @brief Attaches an image to a Discord message.
         *
         * @param m Message to modify.
         * @param image Image to attach.
         * @return Attachment URL.
         */
        std::string add_image(dpp::message &m, const Image_ptr &image);
    };

} // namespace gb

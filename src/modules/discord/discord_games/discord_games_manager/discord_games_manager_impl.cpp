//
// Created by ilesik on 7/24/24.
//

#include "discord_games_manager_impl.hpp"

namespace gb {
    void Discord_games_manager_impl::add_game(Discord_game *game) {
        std::unique_lock lock (_mutex);
        _games.push_back(game);
    }

    void Discord_games_manager_impl::remove_game(Discord_game *game) {
        std::unique_lock lock (_mutex);
        auto e = std::ranges::remove(_games, game);
        _games.erase(e.begin(),e.end());
        _cv.notify_all();
    }

    void Discord_games_manager_impl::stop() {
        std::mutex m;
        std::unique_lock lk (m);
        _cv.wait(lk,[this](){
            std::unique_lock lk (_mutex);
            return _games.empty();
        });

    }

    void Discord_games_manager_impl::run() {}

    void Discord_games_manager_impl::innit(const Modules &modules) {
    }

    Discord_games_manager_impl::Discord_games_manager_impl() : Discord_games_manager("discord_games_manager", {}) {}

    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_games_manager_impl>());
    }
} // gb


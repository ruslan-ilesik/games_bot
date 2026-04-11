//
// Created by ilesik on 4/11/26.
//

#pragma once
#include "discord_commands_balance_transactions.hpp"
#include "src/module/module.hpp"
#include "src/modules/discord/gamescoin_manager/gamescoin_manager.hpp"

namespace gb {
    class Discord_commands_balance_transactions_impl : public Discord_commands_balance_transactions {
        Gamescoin_manager_ptr _gamescoin_manager;

    protected:
        dpp::task<void> _command_callback(const dpp::slashcommand_t &event) override;

    public:
        Discord_commands_balance_transactions_impl();
        virtual ~Discord_commands_balance_transactions_impl() = default;

        void run() override {};
        void stop() override;

        void init(const Modules &modules) override;

    };

    extern "C" Module_ptr create();

} // namespace gb

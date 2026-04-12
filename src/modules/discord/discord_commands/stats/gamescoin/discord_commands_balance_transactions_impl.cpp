//
// Created by ilesik on 4/11/26.
//

#include "discord_commands_balance_transactions_impl.hpp"


namespace gb {
    dpp::task<void> Discord_commands_balance_transactions_impl::_command_callback(const dpp::slashcommand_t &event) {
        if (event.command.get_command_name() == "balance") {
            uint32_t balance = co_await _gamescoin_manager->get_user_balance(event.command.usr.id);
            dpp::embed embed = dpp::embed()
                                   .set_color(dpp::colors::green)
                                   .set_description(std::format("Your balance of Gamescoins is: {} :coin:", balance));
            _bot->reply(event, dpp::message().add_embed(embed));
        } else {

        }
        co_return;
    }
    Discord_commands_balance_transactions_impl::Discord_commands_balance_transactions_impl() :
        Discord_commands_balance_transactions("discord_commands_balance_transactions_impl", {"gamescoin_manager"}) {}

    void Discord_commands_balance_transactions_impl::stop() {
        _command_handler->remove_command("balance");
        _command_handler->remove_command("transactions");
        Discord_commands_balance_transactions::stop();
    }

    void Discord_commands_balance_transactions_impl::init(const Modules &modules) {
        Discord_commands_balance_transactions::init(modules);
        _gamescoin_manager = std::static_pointer_cast<Gamescoin_manager>(modules.at("gamescoin_manager"));

        _bot->add_pre_requirement([this]() {
            dpp::slashcommand command("balance", "Command to get your balance of Gamestokens", _bot->get_bot()->me.id);

            _command_handler->register_command(_discord->create_discord_command(
                command, _command_executor,
                {"Command to get your balance of Gamestokens globally earned available for spending", {"statistics"}}));
        });

        _bot->add_pre_requirement([this]() {
            dpp::slashcommand command("transactions", "Command to get your last transactions", _bot->get_bot()->me.id);

            _command_handler->register_command(_discord->create_discord_command(
                command, _command_executor,
                {"Command to get your last transactions of Gamestokens (buying, spending, ect) with optional filters",
                 {"statistics"}}));
        });
    }

    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_commands_balance_transactions_impl>());
    }
} // namespace gb

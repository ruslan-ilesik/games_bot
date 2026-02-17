//
// Created by ilesik on 10/3/24.
//

#include "discord_command_premium_impl.hpp"

namespace gb {
    dpp::task<void> Discord_command_premium_impl::_command_callback(const dpp::slashcommand_t &event) {
        auto r = _premium->get_users_premium_status(event.command.usr.id);
        dpp::embed embed = dpp::embed();
        dpp::message m = dpp::message();
        dpp::component c = dpp::component().set_type(dpp::cot_action_row);
        PREMIUM_STATUS status = co_await r;
        std::string status_str = to_string(status);
        if (status > PREMIUM_STATUS::NO_SUBSCRIPTION) {
            embed.set_color(dpp::colors::green)
                .set_description("Thank you for supporting GamesBot. You already have a subscription " +
                                 dpp::utility::emoji_mention("thanks", 903301260487315467));
        } else {
            embed.set_color(dpp::colors::red)
                .set_description("You don't have a subscription; you can purchase one to get benefits and "
                                 "support GamesBot!");
            c.add_component(dpp::component()
                                .set_label("Subscribe!")
                                .set_type(dpp::cot_button)
                                .set_style(dpp::cos_link)
                                .set_url(_premium->get_premium_buy_url()));
            m.add_component(c);
        }
        _bot->reply(event, m.add_embed(embed));
        co_return;
    }

    Discord_command_premium_impl::Discord_command_premium_impl() :
        Discord_command_premium("discord_command_premium", {"premium_manager"}) {}

    void Discord_command_premium_impl::stop() {
        _command_handler->remove_command("premium");
        Discord_command_premium::stop();
    }

    void Discord_command_premium_impl::init(const Modules &modules) {
        Discord_command_premium::init(modules);
        _premium = std::static_pointer_cast<Premium_manager>(modules.at("premium_manager"));
        _bot->add_pre_requirement([this]() {
            dpp::slashcommand command("premium", "Command to get information about premium status",
                                      _bot->get_bot()->me.id);

            _command_handler->register_command(_discord->create_discord_command(
                command,_command_executor,
                {"Nothing special, just do what description states :)", {"other"}}));
        });
    }
    void Discord_command_premium_impl::run() {}

    Module_ptr create() { return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_command_premium_impl>()); }
} // namespace gb

//
// Created by ilesik on 7/21/24.
//

#include "discord_button_click_handler_impl.hpp"

namespace gb {
    dpp::task<Button_click_return>
    gb::Discord_button_click_handler_impl::wait_for(dpp::message &m, const std::vector<dpp::snowflake> &users,
                                                    time_t timeout, bool generate_ids, bool clear_ids) {
        auto to_uint64 = [](const std::string &v) -> uint64_t {
            uint64_t value;
            std::istringstream iss(v);
            iss >> value;
            return value;
        };

        std::vector<uint64_t> ids;
        if (generate_ids) {
            ids = _cache.component_init(m);
        } else {
            ids = _cache.get_ids(m);
        }
        auto result = co_await dpp::when_any{
            _bot->get_bot()->on_button_click.when([this,&ids,&to_uint64,users](const dpp::button_click_t &b) {
                bool res = std::ranges::find(ids, to_uint64(b.custom_id)) != ids.end() &&
                           (users.empty() || std::ranges::find(users, b.command.member.user_id) != users.end());
                if (!res && std::ranges::find(ids, to_uint64(b.custom_id)) != ids.end()) {
                    _bot->reply_new(b, dpp::message().set_flags(dpp::m_ephemeral)
                                    .add_embed(dpp::embed()
                                        .set_color(dpp::colors::red)
                                        .set_title("Error")
                                        .set_description("This interaction is not for you!")
                                    )
                    );
                }
                return res;
            }),
            _bot->get_bot()->co_sleep(timeout)
        };

        if (result.index() == 0) {
            dpp::button_click_t click_event = result.get<0>();
            click_event.custom_id = _cache.get_value(to_uint64(click_event.custom_id));
            if (clear_ids) {
                _cache.clear_ids(ids);
            }
            co_return {click_event, false};
        }

        _cache.clear_ids(ids);
        co_return {{}, true};
    }

    void Discord_button_click_handler_impl::clear_ids(const dpp::message &m) {
        _cache.clear_ids(_cache.get_ids(m));
    }

    void Discord_button_click_handler_impl::run() {
    }

    void Discord_button_click_handler_impl::init(const Modules &modules) {
        _bot = std::static_pointer_cast<Discord_bot>(modules.at("discord_bot"));
    }

    void Discord_button_click_handler_impl::stop() {
        _cache.stop();
    }

    Discord_button_click_handler_impl::Discord_button_click_handler_impl()
        : Discord_button_click_handler("discord_button_click_handler", {"discord_bot"}) {
    }

    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_button_click_handler_impl>());
    }
} // gb

//
// Created by ilesik on 10/1/24.
//

#include "discord_reports_impl.hpp"

namespace gb {
    Discord_reports_impl::Discord_reports_impl() :
        Discord_reports("discord_reports", {"discord_statistics_collector", "discord_bot", "config"}) {}

    void Discord_reports_impl::run() {
        _loop_timer = _bot->get_bot()->start_timer(
            [this](const dpp::timer &t) -> dpp::task<void> {
                uint64_t servers_cnt = co_await _discord_stats->get_servers_cnt();
                uint64_t users_cnt = co_await _discord_stats->get_users_cnt();
                _bot->get_bot()->log(dpp::ll_info, "Running reports to websites!");
                _bot->get_bot()->set_presence(dpp::presence(
                    dpp::ps_online,
                    dpp::activity(dpp::at_game, std::format("on {} servers with {} users :)", servers_cnt, users_cnt),
                                  "", "")));

                auto bot_id = _bot->get_bot()->me.id;

                // submit data to discordbotlist
                std::string post_data;
                std::multimap<std::string, std::string> discordbotlist_args = {
                    {"Authorization", _config->get_value("discordbotlist_token")},
                    {"Content-Type", "application/json"}};
                post_data = "{\"guilds\": " + std::to_string(servers_cnt) + "}";
                _bot->get_bot()->request(
                    "https://discordbotlist.com/api/v1/bots/" + std::to_string(bot_id) + "/stats", dpp::m_post,
                    [this](const dpp::http_request_completion_t &e) {
                        _bot->get_bot()->log(dpp::ll_info,
                                             "Submitted server count to discordbotlist, response: " + e.body);
                    },
                    post_data, "application/json", discordbotlist_args);


                // submit data to discordscom
                std::multimap<std::string, std::string> discordscom_args = {
                    {"Authorization", _config->get_value("discordscom_token")}, {"Content-Type", "application/json"}};
                post_data = "{\"server_count\": " + std::to_string(servers_cnt) + "}";
                _bot->get_bot()->request(
                    "https://discords.com/bots/api/bot/" + std::to_string(bot_id), dpp::m_post,
                    [this](const dpp::http_request_completion_t &e) {
                        _bot->get_bot()->log(dpp::ll_info,
                                             "Submitted server count to discordscom, response: " + e.body);
                    },
                    post_data, "application/json", discordscom_args);


                // submit data to discordbotsgg
                std::multimap<std::string, std::string> discordbotsgg_args = {
                    {"Authorization", _config->get_value("discordbotsgg_token")}, {"Content-Type", "application/json"}};
                post_data = "{\"guildCount\": " + std::to_string(servers_cnt) + "}";
                _bot->get_bot()->request(
                    "https://discord.bots.gg/api/v1/bots/" + std::to_string(bot_id) + "/stats", dpp::m_post,
                    [this](const dpp::http_request_completion_t &e) {
                        _bot->get_bot()->log(dpp::ll_info,
                                             "Submitted server count to discordbotsgg, response: " + e.body);
                    },
                    post_data, "application/json", discordbotsgg_args);


                // submit data to topgg
                std::multimap<std::string, std::string> tgg_args = {
                    {"Authorization", _config->get_value("topgg_token")}};
                post_data = nlohmann::json({{"server_count", servers_cnt}}).dump();
                _bot->get_bot()->request(
                    "https://top.gg/api/bots/" + std::to_string(bot_id) + "/stats", dpp::m_post,
                    [this](const dpp::http_request_completion_t &e) {
                        _bot->get_bot()->log(dpp::ll_info, "Submitted server count to topgg, response: " + e.body);
                    },
                    post_data, "application/json", tgg_args);

                co_return;
            },
            60*10);
    }

    void Discord_reports_impl::stop() { _bot->get_bot()->stop_timer(_loop_timer); }

    void Discord_reports_impl::init(const Modules &modules) {
        _discord_stats =
            std::static_pointer_cast<Discord_statistics_collector>(modules.at("discord_statistics_collector"));
        _bot = std::static_pointer_cast<Discord_bot>(modules.at("discord_bot"));
        _config = std::static_pointer_cast<Config>(modules.at("config"));
    }

    Module_ptr create() { return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_reports_impl>()); }

} // namespace gb

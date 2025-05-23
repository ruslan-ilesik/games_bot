//
// Created by ilesik on 9/30/24.
//

#include "discord_statistics_collector_impl.hpp"

namespace gb {
    Discord_statistics_collector_impl::Discord_statistics_collector_impl() :
        Discord_statistics_collector("discord_statistics_collector",
                                     {"discord_bot", "config", "database", "database"}) {}

    void Discord_statistics_collector_impl::run() {
        _report_timer = _bot->get_bot()->start_timer(
            [this](const dpp::timer &t) { _db->background_execute_prepared_statement(_historic_report_stmt); },
            std::stoi(_config->get_value("discord_statistics_report_period")));
    }

    void Discord_statistics_collector_impl::stop() {
        _bot->get_bot()->stop_timer(_report_timer);
        _bot->get_bot()->on_guild_create.detach(_on_guild_create_handler);
        _bot->get_bot()->on_guild_delete.detach(_on_guild_delete_handler);
        _bot->get_bot()->on_guild_member_add.detach(_on_guild_member_add_handler);
        _bot->get_bot()->on_guild_member_remove.detach(_on_guild_member_remove_handler);
        _db->remove_prepared_statement(_add_guild_stmt);
        _db->remove_prepared_statement(_remove_guild_stmt);
        _db->remove_prepared_statement(_add_user_stmt);
        _db->remove_prepared_statement(_remove_user_stmt);
        _db->remove_prepared_statement(_historic_report_stmt);
        _db->remove_prepared_statement(_get_servers_cnt);
        _db->remove_prepared_statement(_get_users_cnt_stmt);
        _db->remove_prepared_statement(_get_users_cnt_on_server_stmt);
    }

    void Discord_statistics_collector_impl::init(const Modules &modules) {
        _config = std::static_pointer_cast<Config>(modules.at("config"));
        _bot = std::static_pointer_cast<Discord_bot>(modules.at("discord_bot"));
        _db = std::static_pointer_cast<Database>(modules.at("database"));

        _add_guild_stmt = _db->create_prepared_statement("INSERT INTO `guilds_users_data` (`guild_id`,`users_cnt`) \n"
                                                         "VALUES (?,?) ON DUPLICATE KEY UPDATE `users_cnt`=?;");
        _remove_guild_stmt = _db->create_prepared_statement("DELETE FROM `guilds_users_data` WHERE `guild_id`=?");

        _add_user_stmt = _db->create_prepared_statement(
            "UPDATE `guilds_users_data` SET `users_cnt`=`users_cnt`+1 WHERE `guild_id`=?");

        _remove_user_stmt = _db->create_prepared_statement(
            "UPDATE `guilds_users_data` SET `users_cnt`=`users_cnt`-1 WHERE `guild_id`=?");

        _historic_report_stmt = _db->create_prepared_statement(
            "INSERT INTO `guilds_users_count_history` (`record_time`,`guilds_cnt`,`users_cnt`) VALUES \n"
            "(UTC_TIMESTAMP(),(select count(*) from `guilds_users_data`),(select sum(`users_cnt`) from \n"
            "`guilds_users_data`));");

        _get_servers_cnt = _db->create_prepared_statement("select count(*) as cnt from `guilds_users_data`");
        _get_users_cnt_stmt = _db->create_prepared_statement("select sum(`users_cnt`) as cnt from `guilds_users_data`");
        _get_users_cnt_on_server_stmt =
            _db->create_prepared_statement("select `users_cnt` as cnt from `guilds_users_data` where `guild_id`=?");


        _bot->add_pre_requirement([this]() {
            sync_wait(_db->execute("DELETE FROM `guilds_users_data`;"));
            _on_guild_create_handler = _bot->get_bot()->on_guild_create([this](const dpp::guild_create_t &event) {
                _db->execute_prepared_statement(_add_guild_stmt, event.created.id, event.created.member_count,
                                                event.created.member_count);
            });

            _on_guild_delete_handler = _bot->get_bot()->on_guild_delete([this](const dpp::guild_delete_t &event) {
                _db->execute_prepared_statement(_remove_guild_stmt, event.guild_id);
            });

            _on_guild_member_add_handler =
                _bot->get_bot()->on_guild_member_add([this](const dpp::guild_member_add_t &event) {
                    _db->execute_prepared_statement(_add_user_stmt, event.adding_guild.id);
                });

            _on_guild_member_remove_handler =
                _bot->get_bot()->on_guild_member_remove([this](const dpp::guild_member_remove_t &event) {
                    _db->execute_prepared_statement(_remove_user_stmt, event.removing_guild.id);
                });
        });
    }

    Task<uint64_t> Discord_statistics_collector_impl::get_servers_cnt() {
        Database_return_t r = co_await _db->execute_prepared_statement(_get_servers_cnt);
        uint64_t value;
        std::string s = r.at(0).at("cnt");
        std::from_chars(s.data(), s.data() + s.size(), value);
        co_return value;
    }

    Task<uint64_t> Discord_statistics_collector_impl::get_users_cnt() {
        Database_return_t r = co_await _db->execute_prepared_statement(_get_users_cnt_stmt);
        uint64_t value;
        std::string s = r.at(0).at("cnt");
        std::from_chars(s.data(), s.data() + s.size(), value);
        co_return value;
    }

    Task<uint64_t> Discord_statistics_collector_impl::get_users_on_server_cnt(const dpp::snowflake &guild_id) {
        Database_return_t r = co_await _db->execute_prepared_statement(_get_users_cnt_on_server_stmt,guild_id);
        uint64_t value;
        std::string s = r.at(0).at("cnt");
        std::from_chars(s.data(), s.data() + s.size(), value);
        co_return value;
    }

    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_statistics_collector_impl>());
    }
} // namespace gb

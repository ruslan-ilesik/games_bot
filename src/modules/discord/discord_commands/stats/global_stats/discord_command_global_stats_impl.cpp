//
// Created by ilesik on 9/1/24.
//

#include "discord_command_global_stats_impl.hpp"


namespace gb {
    dpp::task<void> Discord_command_global_stats_impl::select_bot(const dpp::slashcommand_t &event) {
        auto command = event.command;
        auto cmd_data = command.get_command_interaction().options[0];
        if (cmd_data.options[0].name == "commands") {
            // limits from https://dev.mysql.com/doc/refman/8.4/en/datetime.html
            std::string time_from = "1000-01-01 00:00:00";
            std::string time_to = "9999-12-31 23:59:59";
            std::string command_name = "%"; // MYSQL Like will match any string
            for (auto &parameter: cmd_data.options[0].options) {
                if (parameter.name == "time_from" && std::holds_alternative<std::string>(parameter.value)) {
                    struct tm tm{};
                    time_from = std::get<std::string>(parameter.value);
                    if (strptime(time_from.c_str(), "%Y-%m-%d %H:%M:%S", &tm) == NULL) {
                        dpp::message m = dpp::message()
                                             .add_embed(dpp::embed()
                                                            .set_title(parameter.name + " parameter incorrect format.")
                                                            .set_color(dpp::colors::red))
                                             .set_flags(dpp::m_ephemeral);
                        _bot->reply(event, m);
                        co_return;
                    }
                } else if (parameter.name == "time_to" && std::holds_alternative<std::string>(parameter.value)) {
                    struct tm tm{};
                    time_to = std::get<std::string>(parameter.value);
                    if (strptime(time_to.c_str(), "%Y-%m-%d %H:%M:%S", &tm) == NULL) {
                        dpp::message m = dpp::message()
                                             .add_embed(dpp::embed()
                                                            .set_title(parameter.name + " parameter incorrect format.")
                                                            .set_color(dpp::colors::red))
                                             .set_flags(dpp::m_ephemeral);
                        _bot->reply(event, m);
                        co_return;
                    }
                } else if (parameter.name == "command_name" && std::holds_alternative<std::string>(parameter.value)) {
                    command_name = std::get<std::string>(parameter.value);
                }
            }

            Database_return_t r =
                co_await _db->execute_prepared_statement(_bot_commands_stmt, time_from, time_to, command_name);
            if (r.empty()) {
                dpp::message m = dpp::message().add_embed(
                    dpp::embed().set_title("No data was found by your parameters").set_color(dpp::colors::blue));
                _bot->reply(event, m);
                co_return;
            }
            dpp::embed emb;
            emb.set_color(dpp::colors::blue);
            int total_cmd = 0;
            uint64_t total_calls = 0;
            for (auto command_row: r) {
                total_cmd++;

                std::string calls_amount = command_row.at("amount");
                uint64_t calls_cnt;
                std::from_chars(calls_amount.data(), calls_amount.data() + calls_amount.size(), calls_cnt);
                total_calls += calls_cnt;

                emb.add_field(std::format("{}) {}", total_cmd, command_row.at("command")),
                              std::format("Was used {} time(s).\nLast use: <t:{}> (<t:{}:R>)", calls_cnt,
                                          command_row.at("last_use"), command_row.at("last_use")));
            }
            emb.set_title(
                std::format("There is {} different command(s) which were used {} times", total_cmd, total_calls));
            _bot->reply(event, dpp::message().add_embed(emb));
        } else {
            // limits from https://dev.mysql.com/doc/refman/8.4/en/datetime.html
            std::string time_from_start = "1000-01-01 00:00:00";
            std::string time_to_start = "9999-12-31 23:59:59";
            std::string time_from_end = "1000-01-01 00:00:00";
            std::string time_to_end = "9999-12-31 23:59:59";
            std::string game_name = "%"; // MYSQL Like will match any string

            for (auto &parameter: cmd_data.options[0].options) {
                if (parameter.name == "time_from_start" && std::holds_alternative<std::string>(parameter.value)) {
                    struct tm tm{};
                    time_from_start = std::get<std::string>(parameter.value);
                    if (strptime(time_from_start.c_str(), "%Y-%m-%d %H:%M:%S", &tm) == NULL) {
                        dpp::message m = dpp::message()
                                             .add_embed(dpp::embed()
                                                            .set_title(parameter.name + " parameter incorrect format.")
                                                            .set_color(dpp::colors::red))
                                             .set_flags(dpp::m_ephemeral);
                        _bot->reply(event, m);
                        co_return;
                    }
                } else if (parameter.name == "time_to_start" && std::holds_alternative<std::string>(parameter.value)) {
                    struct tm tm{};
                    time_to_start = std::get<std::string>(parameter.value);
                    if (strptime(time_to_start.c_str(), "%Y-%m-%d %H:%M:%S", &tm) == NULL) {
                        dpp::message m = dpp::message()
                                             .add_embed(dpp::embed()
                                                            .set_title(parameter.name + " parameter incorrect format.")
                                                            .set_color(dpp::colors::red))
                                             .set_flags(dpp::m_ephemeral);
                        _bot->reply(event, m);
                        co_return;
                    }
                } else if (parameter.name == "time_from_end" && std::holds_alternative<std::string>(parameter.value)) {
                    struct tm tm{};
                    time_from_end = std::get<std::string>(parameter.value);
                    if (strptime(time_from_end.c_str(), "%Y-%m-%d %H:%M:%S", &tm) == NULL) {
                        dpp::message m = dpp::message()
                                             .add_embed(dpp::embed()
                                                            .set_title(parameter.name + " parameter incorrect format.")
                                                            .set_color(dpp::colors::red))
                                             .set_flags(dpp::m_ephemeral);
                        _bot->reply(event, m);
                        co_return;
                    }
                } else if (parameter.name == "time_to_end" && std::holds_alternative<std::string>(parameter.value)) {
                    struct tm tm{};
                    time_to_end = std::get<std::string>(parameter.value);
                    if (strptime(time_to_end.c_str(), "%Y-%m-%d %H:%M:%S", &tm) == NULL) {
                        dpp::message m = dpp::message()
                                             .add_embed(dpp::embed()
                                                            .set_title(parameter.name + " parameter incorrect format.")
                                                            .set_color(dpp::colors::red))
                                             .set_flags(dpp::m_ephemeral);
                        _bot->reply(event, m);
                        co_return;
                    }
                } else if (parameter.name == "game_name" && std::holds_alternative<std::string>(parameter.value)) {
                    game_name = std::get<std::string>(parameter.value);
                }
            }

            Database_return_t r = co_await _db->execute_prepared_statement(
                _bot_games_stmt, time_from_start, time_to_start, time_from_end, time_to_end, game_name);
            if (r.empty()) {
                dpp::message m = dpp::message().add_embed(
                    dpp::embed().set_title("No data was found by your parameters").set_color(dpp::colors::blue));
                _bot->reply(event, m);
                co_return;
            }
            dpp::embed emb;
            emb.set_color(dpp::colors::blue);
            int total_games = 0;
            int active_games = 0;
            uint64_t total_calls = 0;
            for (auto game_row: r) {
                total_games++;

                std::string calls_amount = game_row.at("amount");
                uint64_t calls_cnt;
                std::from_chars(calls_amount.data(), calls_amount.data() + calls_amount.size(), calls_cnt);
                total_calls += calls_cnt;

                std::string active_amount_str = game_row.at("active_games");
                uint64_t active_cnt;
                std::from_chars(active_amount_str.data(), active_amount_str.data() + active_amount_str.size(),
                                active_cnt);
                active_games += active_cnt;

                emb.add_field(
                    std::format("{}) {}", total_games, game_row.at("game_name")),
                    std::format(
                        "Was played {} time(s).\nLast time played: <t:{}> (<t:{}:R>).\n {} game(s) are active now.",
                        game_row.at("amount"), game_row.at("last_game"), game_row.at("last_game"), active_cnt));
            }
            emb.set_title(std::format(
                "There is {} different game(s). They were played {} time(s). There is {} game(s) active now",
                total_games, total_calls, active_games));
            _bot->reply(event, dpp::message().add_embed(emb));
            co_return;
        }
    }

    dpp::task<void> Discord_command_global_stats_impl::select_me(const dpp::slashcommand_t &event) {
        auto command = event.command;
        auto cmd_data = command.get_command_interaction().options[0];
        if (cmd_data.options[0].name == "commands") {
            // limits from https://dev.mysql.com/doc/refman/8.4/en/datetime.html
            std::string time_from = "1000-01-01 00:00:00";
            std::string time_to = "9999-12-31 23:59:59";
            std::string command_name = "%"; // MYSQL Like will match any string
            for (auto &parameter: cmd_data.options[0].options) {
                if (parameter.name == "time_from" && std::holds_alternative<std::string>(parameter.value)) {
                    struct tm tm{};
                    time_from = std::get<std::string>(parameter.value);
                    if (strptime(time_from.c_str(), "%Y-%m-%d %H:%M:%S", &tm) == NULL) {
                        dpp::message m = dpp::message()
                                             .add_embed(dpp::embed()
                                                            .set_title(parameter.name + " parameter incorrect format.")
                                                            .set_color(dpp::colors::red))
                                             .set_flags(dpp::m_ephemeral);
                        _bot->reply(event, m);
                        co_return;
                    }
                } else if (parameter.name == "time_to" && std::holds_alternative<std::string>(parameter.value)) {
                    struct tm tm{};
                    time_to = std::get<std::string>(parameter.value);
                    if (strptime(time_to.c_str(), "%Y-%m-%d %H:%M:%S", &tm) == NULL) {
                        dpp::message m = dpp::message()
                                             .add_embed(dpp::embed()
                                                            .set_title(parameter.name + " parameter incorrect format.")
                                                            .set_color(dpp::colors::red))
                                             .set_flags(dpp::m_ephemeral);
                        _bot->reply(event, m);
                        co_return;
                    }
                } else if (parameter.name == "command_name" && std::holds_alternative<std::string>(parameter.value)) {
                    command_name = std::get<std::string>(parameter.value);
                }
            }
            Database_return_t r = co_await _db->execute_prepared_statement(_me_commands_stmt, event.command.usr.id,
                                                                           time_from, time_to, command_name);
            if (r.empty()) {
                dpp::message m = dpp::message().add_embed(
                    dpp::embed().set_title("No data was found by your parameters").set_color(dpp::colors::blue));
                _bot->reply(event, m);
                co_return;
            }
            dpp::embed emb;
            emb.set_color(dpp::colors::blue);
            int total_cmd = 0;
            uint64_t total_calls = 0;
            for (auto command_row: r) {
                total_cmd++;

                std::string calls_amount = command_row.at("amount");
                uint64_t calls_cnt;
                std::from_chars(calls_amount.data(), calls_amount.data() + calls_amount.size(), calls_cnt);
                total_calls += calls_cnt;

                emb.add_field(std::format("{}) {}", total_cmd, command_row.at("command")),
                              std::format("Was used {} time(s).\nLast use: <t:{}> (<t:{}:R>)", calls_cnt,
                                          command_row.at("last_use"), command_row.at("last_use")));
            }
            emb.set_title(
                std::format("There is {} different command(s) which were used {} times", total_cmd, total_calls));
            _bot->reply(event, dpp::message().add_embed(emb));
        } else {
            dpp::snowflake user2 =
                event.command.usr.id; // used as placeholder if no user were provided, with respect to sql will just
                                      // return all games with user, without second user condition.
            dpp::snowflake user = event.command.usr.id;
            // limits from https://dev.mysql.com/doc/refman/8.4/en/datetime.html
            std::string time_from_start = "1000-01-01 00:00:00";
            std::string time_to_start = "9999-12-31 23:59:59";
            std::string time_from_end = "1000-01-01 00:00:00";
            std::string time_to_end = "9999-12-31 23:59:59";
            std::string game_name = "%"; // MYSQL Like will match any string

            for (auto parameter: cmd_data.options[0].options) {
                if (parameter.name == "time_from_start" && std::holds_alternative<std::string>(parameter.value)) {
                    struct tm tm{};
                    time_from_start = std::get<std::string>(parameter.value);
                    if (strptime(time_from_start.c_str(), "%Y-%m-%d %H:%M:%S", &tm) == NULL) {
                        dpp::message m = dpp::message()
                                             .add_embed(dpp::embed()
                                                            .set_title(parameter.name + " parameter incorrect format.")
                                                            .set_color(dpp::colors::red))
                                             .set_flags(dpp::m_ephemeral);
                        _bot->reply(event, m);
                        co_return;
                    }
                } else if (parameter.name == "time_to_start" && std::holds_alternative<std::string>(parameter.value)) {
                    struct tm tm{};
                    time_to_start = std::get<std::string>(parameter.value);
                    if (strptime(time_to_start.c_str(), "%Y-%m-%d %H:%M:%S", &tm) == NULL) {
                        dpp::message m = dpp::message()
                                             .add_embed(dpp::embed()
                                                            .set_title(parameter.name + " parameter incorrect format.")
                                                            .set_color(dpp::colors::red))
                                             .set_flags(dpp::m_ephemeral);
                        _bot->reply(event, m);
                        co_return;
                    }
                } else if (parameter.name == "time_from_end" && std::holds_alternative<std::string>(parameter.value)) {
                    struct tm tm{};
                    time_from_end = std::get<std::string>(parameter.value);
                    if (strptime(time_from_end.c_str(), "%Y-%m-%d %H:%M:%S", &tm) == NULL) {
                        dpp::message m = dpp::message()
                                             .add_embed(dpp::embed()
                                                            .set_title(parameter.name + " parameter incorrect format.")
                                                            .set_color(dpp::colors::red))
                                             .set_flags(dpp::m_ephemeral);
                        _bot->reply(event, m);
                        co_return;
                    }
                } else if (parameter.name == "time_to_end" && std::holds_alternative<std::string>(parameter.value)) {
                    struct tm tm{};
                    time_to_end = std::get<std::string>(parameter.value);
                    if (strptime(time_to_end.c_str(), "%Y-%m-%d %H:%M:%S", &tm) == NULL) {
                        dpp::message m = dpp::message()
                                             .add_embed(dpp::embed()
                                                            .set_title(parameter.name + " parameter incorrect format.")
                                                            .set_color(dpp::colors::red))
                                             .set_flags(dpp::m_ephemeral);
                        _bot->reply(event, m);
                        co_return;
                    }
                } else if (parameter.name == "game_name" && std::holds_alternative<std::string>(parameter.value)) {
                    game_name = std::get<std::string>(parameter.value);
                }

                else if ((parameter.name == "user_id") && std::holds_alternative<std::string>(parameter.value)) {
                    std::string value = std::get<std::string>(parameter.value);
                    if (!std::all_of(value.begin(), value.end(), ::isdigit)) {
                        _bot->reply(event,
                                    dpp::message()
                                        .add_embed(dpp::embed()
                                                       .set_title(parameter.name + " should contain only digits.")
                                                       .set_color(dpp::colors::red))
                                        .set_flags(dpp::m_ephemeral));
                        co_return;
                    }
                    user2 = value;
                } else if ((parameter.name == "user") && std::holds_alternative<dpp::snowflake>(parameter.value)) {
                    user2 = std::get<dpp::snowflake>(parameter.value);
                }
            }
            Database_return_t r = co_await _db->execute_prepared_statement(
                _me_games_stmt, user2, user, time_from_start, time_to_start, time_from_end, time_to_end, game_name);
            if (r.empty()) {
                dpp::message m = dpp::message().add_embed(
                    dpp::embed().set_title("No data was found by your parameters").set_color(dpp::colors::blue));
                _bot->reply(event, m);
                co_return;
            }
            dpp::embed emb;
            emb.set_color(dpp::colors::blue);
            uint64_t total_cmd = 0;
            unsigned int active_games = 0;
            uint64_t win_cnt = 0;
            uint64_t lose_cnt = 0;
            uint64_t draw_cnt = 0;
            uint64_t total_calls = 0;
            for (auto &game_row: r) {
                total_cmd++;

                auto data = game_row.at("active_games");
                unsigned int active;
                std::from_chars(data.data(), data.data() + data.size(), active);
                active_games += active;

                data = game_row.at("win_games");
                uint64_t win;
                std::from_chars(data.data(), data.data() + data.size(), win);
                win_cnt += win;

                data = game_row.at("draw_games");
                uint64_t draw;
                std::from_chars(data.data(), data.data() + data.size(), draw);
                draw_cnt += draw;

                data = game_row.at("lose_games");
                uint64_t lose;
                std::from_chars(data.data(), data.data() + data.size(), lose);
                lose_cnt += lose;

                data = game_row.at("amount");
                uint64_t amount;
                std::from_chars(data.data(), data.data() + data.size(), amount);
                total_calls += amount;

                // calculate win percentage using formula https://en.wikipedia.org/wiki/Winning_percentage
                double percentage = ((draw * 0.5 + win) / amount) * 100;

                emb.add_field(
                    std::format("{}) {}", total_cmd, game_row.at("game_name")),
                    std::format(
                        "Was played {} time(s).\nLast time played: <t:{}> (<t:{}:R>).\n {} game(s) are active now.\n"
                        "You won {} time(s). You played in draw {} time(s). You lost {} time(s).\n"
                        "Win rate: {:.2f}%",
                        amount, game_row.at("last_game"), game_row.at("last_game"), active, win, draw, lose,
                        percentage));
            }

            // calculate win percentage using formula https://en.wikipedia.org/wiki/Winning_percentage
            double percentage = ((draw_cnt * 0.5 + win_cnt) / total_calls) * 100;
            emb.set_title(
                std::format("There is {} different games. They were played {} time(s). There is {} game(s) active.\n"
                            "You won {} time(s). You played in draw {} time(s). You lost {} time(s).\n"
                            "Win rate: {:.2f}%",
                            total_cmd, total_calls, active_games, win_cnt, draw_cnt, lose_cnt, percentage));
            _bot->reply(event, dpp::message().add_embed(emb));
        }
        co_return;
    }
    dpp::task<void> Discord_command_global_stats_impl::_command_callback(const dpp::slashcommand_t &event) {
        auto command = event.command;
        dpp::command_interaction cmd_data = command.get_command_interaction();
        if (cmd_data.options[0].name == "bot") {
            co_await select_bot(event);
        } else {
            co_await select_me(event);
        }
        co_return;
    }

    Discord_command_global_stats_impl::Discord_command_global_stats_impl() :
        Discord_command_global_stats("discord_command_global_stats", {"database"}) {}

    void Discord_command_global_stats_impl::init(const Modules &modules) {
        Discord_command_global_stats::init(modules);
        _db = std::static_pointer_cast<Database>(modules.at("database"));

        _bot_commands_stmt = _db->create_prepared_statement(
            R"XXX(
SELECT
    `command`,
     COUNT(*) as `amount`,
     UNIX_TIMESTAMP(MAX(`time`)) - (UNIX_TIMESTAMP(UTC_TIMESTAMP()) - unix_timestamp()) as 'last_use'
FROM commands_use
WHERE
    `time` >= ?
    AND `time` <= ?
    AND `command` LIKE ?
GROUP BY `command`
ORDER BY `amount` DESC;)XXX");

        _bot_games_stmt = _db->create_prepared_statement(R"XXX(
SELECT
	`game_name`,
    COUNT(*) AS `amount` ,
    UNIX_TIMESTAMP(MAX(`start_time`)) - (UNIX_TIMESTAMP(UTC_TIMESTAMP()) - unix_timestamp()) as 'last_game',
    sum(`game_state` = 'ACTIVE') as `active_games`
FROM `games_history`
where
	`start_time` >= ?
    and `start_time` <= ?
    and `end_time` >= ?
    and `end_time` <= ?
    and `game_name` LIKE ?
GROUP BY `game_name`
ORDER BY `amount` DESC;
)XXX");

        _me_commands_stmt = _db->create_prepared_statement(
            R"XXX(
SELECT
    `command`,
     COUNT(*) as `amount`,
     UNIX_TIMESTAMP(MAX(`time`)) - (UNIX_TIMESTAMP(UTC_TIMESTAMP()) - unix_timestamp()) as 'last_use'
FROM commands_use
WHERE
    `user_id` = ?
    AND `time` >= ?
    AND `time` <= ?
    AND `command` LIKE ?
GROUP BY `command`
ORDER BY `amount` DESC;)XXX");

        _me_games_stmt = _db->create_prepared_statement(R"XXX(
SELECT
    `gh`.`game_name`,
    UNIX_TIMESTAMP(MAX(`gh`.`start_time`)) - (UNIX_TIMESTAMP(UTC_TIMESTAMP()) - UNIX_TIMESTAMP()) AS `last_game`,

    SUM(`gh`.`game_state` = 'ACTIVE') AS `active_games`,

	-- Count of games where the first user has 'WIN'
    SUM(CASE WHEN `ugr1`.`result` = 'WIN' THEN 1 ELSE 0 END) AS `win_games`,

    -- Count of games where the first user has 'DRAW'
    SUM(CASE WHEN `ugr1`.`result` = 'DRAW' THEN 1 ELSE 0 END) AS `draw_games`,

    -- Count of games where the first user has 'LOSE' or 'TIMEOUT'
    SUM(CASE WHEN `ugr1`.`result` IN ('LOSE', 'TIMEOUT') THEN 1 ELSE 0 END) AS `lose_games`,

	-- Calculate the total amount by summing the above three conditions
    SUM(CASE WHEN `ugr1`.`result` = 'WIN' THEN 1 ELSE 0 END) +
    SUM(CASE WHEN `ugr1`.`result` = 'DRAW' THEN 1 ELSE 0 END) +
    SUM(CASE WHEN `ugr1`.`result` IN ('LOSE', 'TIMEOUT') THEN 1 ELSE 0 END) AS `amount`
FROM
    `games_history` AS `gh`
JOIN
    `user_game_results` AS `ugr1` ON `gh`.`id` = `ugr1`.`game`
INNER JOIN
    `user_game_results` AS `ugr2` ON `gh`.`id` = `ugr2`.`game`
    AND (`ugr2`.`user` = ?)
WHERE
    `ugr1`.`user` = ?
    AND `gh`.`start_time` >= ?
    AND `gh`.`start_time` <= ?
    AND `gh`.`end_time` >= ?
    AND `gh`.`end_time` <= ?
    AND `gh`.`game_name` LIKE ?
GROUP BY
    `gh`.`game_name`
ORDER BY
    `amount` DESC;
)XXX");


        _bot->add_pre_requirement([this]() {
            dpp::slashcommand command(
                "global_stats", "Command to get global statistics for user/bot games/commands across all servers.",
                _bot->get_bot()->me.id);
            command.add_option(
                dpp::command_option(dpp::co_sub_command_group, "bot", "Statistics about whole bot.")
                    .add_option(
                        dpp::command_option(dpp::co_sub_command, "commands",
                                            "Statistics about bot commands usage on this guild.")
                            .add_option(dpp::command_option(dpp::co_string, "time_from",
                                                            "Start of period in format \"YYYY-MM-DD hh:mm:ss\" in UTC "
                                                            "time, example: 2022-02-24 03:00:00.",
                                                            false))
                            .add_option(dpp::command_option(dpp::co_string, "time_to",
                                                            "End of period in format \"YYYY-MM-DD hh:mm:ss\" in UTC "
                                                            "time, example: 2022-02-24 03:00:00.",
                                                            false))
                            .add_option(dpp::command_option(dpp::co_string, "command_name", "Command name.", false)))
                    .add_option(dpp::command_option(dpp::co_sub_command, "games",
                                                    "Statistics about played games on this guild.")
                                    .add_option(dpp::command_option(dpp::co_string, "time_from_start",
                                                                    "Start of period in format \"YYYY-MM-DD hh:mm:ss\" "
                                                                    "in UTC time, example: 2022-02-24 03:00:00.",
                                                                    false))
                                    .add_option(dpp::command_option(dpp::co_string, "time_to_start",
                                                                    "End of period in format \"YYYY-MM-DD hh:mm:ss\" "
                                                                    "in UTC time, example: 2022-02-24 03:00:00.",
                                                                    false))
                                    .add_option(dpp::command_option(dpp::co_string, "time_from_end",
                                                                    "Start of period in format \"YYYY-MM-DD hh:mm:ss\" "
                                                                    "in UTC time, example: 2022-02-24 03:00:00.",
                                                                    false))
                                    .add_option(dpp::command_option(dpp::co_string, "time_to_end",
                                                                    "End of period in format \"YYYY-MM-DD hh:mm:ss\" "
                                                                    "in UTC time, example: 2022-02-24 03:00:00.",
                                                                    false))
                                    .add_option(dpp::command_option(dpp::co_string, "game_name", "Game name.", false))

                                    ));
            command.add_option(
                dpp::command_option(dpp::co_sub_command_group, "me", "Statistics about you.")
                    .add_option(
                        dpp::command_option(dpp::co_sub_command, "commands",
                                            "Statistics about commands you used on this guild.")
                            .add_option(dpp::command_option(dpp::co_string, "time_from",
                                                            "Start of period in format \"YYYY-MM-DD hh:mm:ss\" in UTC "
                                                            "time, example: 2022-02-24 03:00:00.",
                                                            false))
                            .add_option(dpp::command_option(dpp::co_string, "time_to",
                                                            "End of period in format \"YYYY-MM-DD hh:mm:ss\" in UTC "
                                                            "time, example: 2022-02-24 03:00:00.",
                                                            false))
                            .add_option(dpp::command_option(dpp::co_string, "command_name", "Command name.", false)))
                    .add_option(
                        dpp::command_option(dpp::co_sub_command, "games",
                                            "Statistics about your games played by you on this guild.")
                            .add_option(dpp::command_option(dpp::co_string, "time_from_start",
                                                            "Start of period in format \"YYYY-MM-DD hh:mm:ss\" in UTC "
                                                            "time, example: 2022-02-24 03:00:00.",
                                                            false))
                            .add_option(dpp::command_option(dpp::co_string, "time_to_start",
                                                            "End of period in format \"YYYY-MM-DD hh:mm:ss\" in UTC "
                                                            "time, example: 2022-02-24 03:00:00.",
                                                            false))
                            .add_option(dpp::command_option(dpp::co_string, "time_from_end",
                                                            "Start of period in format \"YYYY-MM-DD hh:mm:ss\" in UTC "
                                                            "time, example: 2022-02-24 03:00:00.",
                                                            false))
                            .add_option(dpp::command_option(dpp::co_string, "time_to_end",
                                                            "End of period in format \"YYYY-MM-DD hh:mm:ss\" in UTC "
                                                            "time, example: 2022-02-24 03:00:00.",
                                                            false))
                            .add_option(dpp::command_option(dpp::co_string, "game_name", "Game name.", false))
                            .add_option(
                                dpp::command_option(dpp::co_user, "user", "User you want to see stats with.", false))
                            .add_option(dpp::command_option(
                                dpp::co_string, "user_id",
                                "User id you want to see stats with (if you cant use \"user\" option).", false))));


            _command_handler->register_command(
                _discord->create_discord_command(command, _command_executor,
                                                 {"Shows global statistics for user/bot games/commands across all "
                                                  "servers providing a lot filtering abilities",
                                                  {"statistics"}}));
        });
    }

    void Discord_command_global_stats_impl::run() {}

    void Discord_command_global_stats_impl::stop() {
        _command_handler->remove_command("global_stats");
        Discord_command_global_stats::stop();
        _db->remove_prepared_statement(_bot_commands_stmt);
        _db->remove_prepared_statement(_bot_games_stmt);
        _db->remove_prepared_statement(_me_commands_stmt);
        _db->remove_prepared_statement(_me_games_stmt);
    }

    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_command_global_stats_impl>());
    }
} // namespace gb

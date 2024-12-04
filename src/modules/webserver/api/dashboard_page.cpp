//
// Created by ilesik on 11/20/24.
//

#include "dashboard_page.hpp"

#include <drogon/HttpAppFramework.h>
#include <drogon/drogon_callbacks.h>
#include <src/modules/webserver/utils/cookie_manager.hpp>
#include <src/modules/webserver/utils/type_conversions.hpp>

namespace gb {
    inline const std::vector<int> rows_cnt{10, 25, 50};
    inline const std::vector<std::string> order_by{
        "Datetime 🡫", "Datetime 🡩", "Game 🡫",
        "Game 🡩",     "Result 🡫",   "Result 🡩"}; // do not forget to edit sorting_stmts if u edit this


    void dashboard_page_api(Webserver_impl *server) {

        Prepared_statement get_favorite_game_stmt =
            server->db->create_prepared_statement("SELECT \n"
                                                  "    game_name AS most_played_game,\n"
                                                  "    COUNT(*) AS played_amount,\n"
                                                  "    (\n"
                                                  "        SELECT MAX(end_time)\n"
                                                  "        FROM user_game_results\n"
                                                  "        JOIN games_history ON `game` = games_history.id\n"
                                                  "        WHERE `user` = ?\n"
                                                  "    ) AS last_game_time\n"
                                                  "FROM \n"
                                                  "    user_game_results\n"
                                                  "JOIN \n"
                                                  "    games_history ON `game` = games_history.id\n"
                                                  "WHERE \n"
                                                  "    `user` = ?\n"
                                                  "GROUP BY \n"
                                                  "    game_name\n"
                                                  "ORDER BY \n"
                                                  "    played_amount DESC\n"
                                                  "LIMIT 1;");

        std::map<std::string, Prepared_statement> sorting_stmts;

        for (const std::string &i: order_by) {
            std::string asc_desc = i.ends_with("🡫") ? "desc" : "asc";
            std::string filter_name;
            if (i.starts_with("Datetime")) {
                filter_name = "start_time";
            } else if (i.starts_with("Game")) {
                filter_name = "game_name";
            } else if (i.starts_with("Result")) {
                filter_name = "result";
            } else {
                throw std::runtime_error("Unknown filter in dashboard games history table");
            }

            std::string sql = std::format(R"X(select  game_name, start_time, time_played, result from
    user_game_results ugr join games_history gr on
    ugr.game =  gr.id
    where user = ?
    order by {} {} limit ?,?)X",
                                          filter_name, asc_desc);
            Prepared_statement stmt = server->db->create_prepared_statement(sql);
            sorting_stmts.insert({i, stmt});
        }

        Prepared_statement get_pages_cnt_stmt = server->db->create_prepared_statement(
            "select ceil(count(*)/?) as pages_cnt from user_game_results where user = ?;");


        Prepared_statement get_win_lose_draw_cnt_stmt = server->db->create_prepared_statement(
            R"X(SELECT
            COUNT(CASE WHEN result = 'WIN' THEN 1 END) AS win_count,
            COUNT(CASE WHEN result IN ('LOSE', 'TIMEOUT') THEN 1 END) AS loss_count,
            COUNT(CASE WHEN result = 'DRAW' THEN 1 END) AS draw_count
        FROM user_game_results
        WHERE result IN ('WIN', 'LOSE', 'TIMEOUT', 'DRAW') and
        user = ?;)X");


        Prepared_statement get_time_played_and_amount_stmt =
            server->db->create_prepared_statement(R"X(WITH RECURSIVE date_range AS (
    SELECT DATE(UTC_TIMESTAMP()) AS day
    UNION ALL
    SELECT day - INTERVAL 1 DAY
    FROM date_range
    WHERE day > DATE_SUB(DATE(UTC_TIMESTAMP()), INTERVAL ? DAY)
)
SELECT
    dr.day AS day,
    COALESCE(SUM(TIME_TO_SEC(ugr.time_played)), 0) AS total_time_in_seconds,
    COALESCE(COUNT(ugr.game), 0) AS games_played
FROM date_range dr
LEFT JOIN (
    user_game_results ugr
    JOIN games_history gh
        ON ugr.game = gh.id
) ON DATE(gh.start_time) = dr.day AND ugr.user = ?
GROUP BY dr.day
ORDER BY dr.day asc;)X");


        Prepared_statement get_commands_amount_stmt = server->db->create_prepared_statement(R"X(
WITH RECURSIVE date_range AS (
    SELECT DATE(UTC_TIMESTAMP()) AS day
    UNION ALL
    SELECT day - INTERVAL 1 DAY
    FROM date_range
        WHERE day > DATE_SUB(DATE(UTC_TIMESTAMP()), INTERVAL ? DAY)
)
SELECT
    dr.day AS day,
    COALESCE(COUNT(commands_use.id), 0) as commands_amount
FROM date_range dr
LEFT JOIN
	commands_use
 ON DATE(commands_use.time) = dr.day AND commands_use.user_id = ?
GROUP BY dr.day
ORDER BY dr.day asc;
)X");

        server->on_stop.push_back([=]() {
            server->db->remove_prepared_statement(get_favorite_game_stmt);
            for (const auto &[key, value]: sorting_stmts) {
                server->db->remove_prepared_statement(value);
            }
            server->db->remove_prepared_statement(get_pages_cnt_stmt);
            server->db->remove_prepared_statement(get_win_lose_draw_cnt_stmt);
            server->db->remove_prepared_statement(get_time_played_and_amount_stmt);
            server->db->remove_prepared_statement(get_commands_amount_stmt);
        });

        drogon::app().registerHandler(
            "/api/dashboard/get-history-table",
            [=](drogon::HttpRequestPtr req,
                std::function<void(const drogon::HttpResponsePtr &)> callback) -> drogon::Task<> {
                std::pair<bool, Authorization_cookie> validation = co_await validate_authorization_cookie(server, req);
                Json::Value result;
                if (!validation.first) {
                    callback(drogon::HttpResponse::newHttpJsonResponse(result));
                    co_return;
                }
                std::string sort;
                std::string rows;
                uint64_t page;
                try {
                    auto para = req->getParameters();
                    sort = para.at("sort");
                    rows = para.at("rows");
                    page = std::stoull(para.at("page"), nullptr, 0);


                    if (sort.empty() || rows.empty() || std::ranges::find(order_by, sort) == order_by.end() ||
                        std::ranges::find(rows_cnt, std::stoi(rows)) == rows_cnt.end()) {
                        auto response = drogon::HttpResponse::newHttpResponse();
                        response->setStatusCode(drogon::k400BadRequest);
                        callback(response);
                        co_return;
                    }

                } catch (...) {
                    auto response = drogon::HttpResponse::newHttpResponse();
                    response->setStatusCode(drogon::k400BadRequest);
                    callback(response);
                    co_return;
                }
                auto pages_cnt_task =
                    server->db->execute_prepared_statement(get_pages_cnt_stmt, rows, validation.second.discord_user.id);
                auto table_data_task = server->db->execute_prepared_statement(
                    sorting_stmts.at(sort), validation.second.discord_user.id, std::stoi(rows) * page, rows);

                Database_return_t r = co_await pages_cnt_task;
                result["page_cnt"] = r.at(0).at("pages_cnt");
                r = co_await table_data_task;
                result["records"] = to_json(r);
                callback(drogon::HttpResponse::newHttpJsonResponse(result));
                co_return;
            });

        drogon::app().registerHandler(
            "/api/dashboard/get-history-table-order-options",
            [=](drogon::HttpRequestPtr req,
                std::function<void(const drogon::HttpResponsePtr &)> callback) -> drogon::Task<> {
                Json::Value result;
                result["rows_cnt"] = to_json(rows_cnt);
                result["order_by"] = to_json(order_by);
                auto response = drogon::HttpResponse::newHttpJsonResponse(result);
                callback(response);
                co_return;
            });

        drogon::app().registerHandler(
            "/api/dashboard/get-games-history-cvs",
            [=](drogon::HttpRequestPtr req,
                std::function<void(const drogon::HttpResponsePtr &)> callback) -> drogon::Task<> {
                static const int rows_per_query = 100;

                // Validate the authorization cookie
                auto validation = co_await validate_authorization_cookie(server, req);
                if (!validation.first) {
                    auto response = drogon::HttpResponse::newHttpResponse();
                    response->setStatusCode(drogon::k403Forbidden);
                    callback(response);
                    co_return;
                }

                bool has_premium = co_await server->premium_manager->get_users_premium_status(
                                       validation.second.discord_user.id) != PREMIUM_STATUS::NO_SUBSCRIPTION;
                if (!has_premium) {
                    auto response = drogon::HttpResponse::newHttpResponse();
                   response->setStatusCode(drogon::k403Forbidden);
                   callback(response);
                   co_return;
                }
                auto stream_response = drogon::HttpResponse::newAsyncStreamResponse(
                    [=](drogon::ResponseStreamPtr stream) mutable {
                        // Add CSV headers
                        if (!stream->send("game_name, start_time, time_played, result\n")) {
                            std::cerr << "Failed to send headers" << std::endl;
                            stream->close();
                            return;
                        }

                        int r_cnt = 0;
                        auto prepared_stmt = sorting_stmts.at("Datetime 🡩");

                        while (true) {
                            // Fetch a chunk of rows
                            Database_return_t rows = sync_wait(
                                server->db->execute_prepared_statement(prepared_stmt, validation.second.discord_user.id,
                                                                       rows_per_query * r_cnt, rows_per_query));

                            r_cnt++;

                            if (rows.empty()) {
                                stream->close();
                                return;
                            }

                            // Prepare CSV data for the chunk
                            std::ostringstream csvData;
                            for (const auto &row: rows) {
                                for (const auto &[key, value]: row) {
                                    csvData << value << ",";
                                }
                                csvData.seekp(-1, std::ios_base::end); // Remove trailing comma
                                csvData << '\n';
                            }

                            // Send CSV data chunk
                            if (!stream->send(csvData.str())) {
                                std::cerr << "Stream closed by client or error sending chunk" << std::endl;
                                stream->close();
                                return;
                            }
                        }
                    },
                    true); // Disable kickoff timeout

                // Set headers for file download
                stream_response->addHeader("Content-Type", "text/csv");
                stream_response->addHeader("Content-Disposition", "attachment; filename=\"history.csv\"");

                callback(stream_response);
                co_return;
            });

        drogon::app().registerHandler(
            "/api/dashboard/get-activity-graphs",
            [=](drogon::HttpRequestPtr req,
                std::function<void(const drogon::HttpResponsePtr &)> callback) -> drogon::Task<> {
                std::pair<bool, Authorization_cookie> validation = co_await validate_authorization_cookie(server, req);
                Json::Value result;
                if (!validation.first) {
                    callback(drogon::HttpResponse::newHttpJsonResponse(result));
                    co_return;
                }

                dpp::snowflake user_id = validation.second.discord_user.id;
                auto win_lose = server->db->execute_prepared_statement(get_win_lose_draw_cnt_stmt, user_id);
                int days = (co_await server->premium_manager->get_users_premium_status(user_id)) ==
                                   gb::PREMIUM_STATUS::NO_SUBSCRIPTION
                               ? 10
                               : 30;
                auto games_stats =
                    server->db->execute_prepared_statement(get_time_played_and_amount_stmt, days, user_id);
                auto commands_stats = server->db->execute_prepared_statement(get_commands_amount_stmt, days, user_id);

                Database_return_t r = co_await win_lose;
                result["win_lose"] = to_json(r);
                r = co_await games_stats;
                result["games_stats"] = to_json(r);
                r = co_await commands_stats;
                result["commands_stats"] = to_json(r);
                callback(drogon::HttpResponse::newHttpJsonResponse(result));
                co_return;
            });

        drogon::app().registerHandler(
            "/api/dashboard/get-header",
            [=](drogon::HttpRequestPtr req,
                std::function<void(const drogon::HttpResponsePtr &)> callback) -> drogon::Task<> {
                std::pair<bool, Authorization_cookie> validation = co_await validate_authorization_cookie(server, req);
                Json::Value result;
                if (!validation.first) {
                    callback(drogon::HttpResponse::newHttpJsonResponse(result));
                    co_return;
                }

                dpp::user &user = validation.second.discord_user;
                Database_return_t r =
                    co_await server->db->execute_prepared_statement(get_favorite_game_stmt, user.id, user.id);
                callback(drogon::HttpResponse::newHttpJsonResponse(to_json(r)));
                co_return;
            });


        drogon::app().registerHandler(
            "/api/dashboard/get-achievements-report",
            [=](drogon::HttpRequestPtr req,
                std::function<void(const drogon::HttpResponsePtr &)> callback) -> drogon::Task<> {
                std::pair<bool, Authorization_cookie> validation = co_await validate_authorization_cookie(server, req);
                Json::Value result;
                if (!validation.first) {
                    callback(drogon::HttpResponse::newHttpJsonResponse(result));
                    co_return;
                }

                dpp::user &user = validation.second.discord_user;
                Achievements_report report = server->achievements_manager->get_achievements_report(user.id);
                callback(drogon::HttpResponse::newHttpJsonResponse(to_json(report)));
                co_return;
            });
    }
} // namespace gb

//
// Created by ilesik on 11/20/24.
//

#include "dashboard_page.hpp"

#include <drogon/HttpAppFramework.h>
#include <drogon/drogon_callbacks.h>
#include <src/modules/webserver/utils/cookie_manager.hpp>
#include <src/modules/webserver/utils/type_conversions.hpp>

namespace gb {

    void dashboard_page_api(Webserver_impl *server) {

        Prepared_statement get_favorite_game_stmt = server->db->create_prepared_statement(
        "SELECT \n"
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
        "LIMIT 1;"
        );

        server->on_stop.push_back([=]() {
                server->db->remove_prepared_statement(get_favorite_game_stmt);
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

                dpp::user& user = validation.second.discord_user;
                Database_return_t r = co_await server->db->execute_prepared_statement(get_favorite_game_stmt,user.id,user.id);
                callback(drogon::HttpResponse::newHttpJsonResponse(to_json(r)));
                co_return;
            });
    }
} // namespace gb

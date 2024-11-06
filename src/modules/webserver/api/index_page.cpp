//
// Created by ilesik on 11/3/24.
//

#include "index_page.hpp"
#include "../utils/type_conversions.hpp"
#include "../utils/validators.hpp"

#include <drogon/HttpAppFramework.h>
#include <drogon/drogon_callbacks.h>


namespace gb {
    void index_page_api(Webserver_impl *server) {
        Prepared_statement get_games_images_stmt = server->db->create_prepared_statement(
            "select sum(images_generated) as icnt, count(*) as gcnt from games_history;");

        Prepared_statement get_reviews_stmt = server->db->create_prepared_statement(
            "SELECT review, rating, username, icon_path FROM reviews where id >=? order by id asc limit ?;");

        static int reviews_bucket_size = 5;

        server->on_stop.emplace_back([=]() {
            server->db->remove_prepared_statement(get_games_images_stmt);
            server->db->remove_prepared_statement(get_reviews_stmt);
        });

        drogon::app().registerHandler(
            "/api/get-index-page-counters",
            [=](drogon::HttpRequestPtr req,
                std::function<void(const drogon::HttpResponsePtr &)> callback) -> drogon::Task<> {
                auto server_cnt_task = server->discord_stats->get_servers_cnt();
                auto users_cnt_task = server->discord_stats->get_users_cnt();
                auto images_and_games_cnt_task = server->db->execute_prepared_statement(get_games_images_stmt);
                uint64_t servers_cnt = co_await server_cnt_task;
                uint64_t users_cnt = co_await users_cnt_task;
                Database_return_t images_and_games = co_await images_and_games_cnt_task;
                Json::Value ret;
                ret["users_cnt"] = users_cnt;
                ret["servers_cnt"] = servers_cnt;
                ret["images_cnt"] = images_and_games.at(0).at("icnt");
                ret["games_cnt"] = images_and_games.at(0).at("gcnt");
                auto resp = drogon::HttpResponse::newHttpJsonResponse(ret);
                callback(resp);
                co_return;
            });

        drogon::app().registerHandler(
            "/api/get-reviews",
            [=](drogon::HttpRequestPtr req,
                std::function<void(const drogon::HttpResponsePtr &)> callback) -> drogon::Task<> {
                auto params = req->parameters();
                int id = validate_number(get_param_or(params, "start_id", "0"), 0);
                Database_return_t r = co_await server->db->execute_prepared_statement(get_reviews_stmt, id, reviews_bucket_size);
                if (r.empty() && id != 0) {
                    id = 0;
                    r = co_await server->db->execute_prepared_statement(get_reviews_stmt, id, reviews_bucket_size);
                }
                Json::Value ret;
                ret["data"] = to_json(r);
                ret["next_id"] = id+reviews_bucket_size+1;
                auto resp = drogon::HttpResponse::newHttpJsonResponse(ret);
                callback(resp);
                co_return;
            });
    }

} // namespace gb

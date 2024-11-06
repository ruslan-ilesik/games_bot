//
// Created by ilesik on 11/5/24.
//

#include "commands_page.hpp"

#include <src/modules/webserver/utils/type_conversions.hpp>


namespace gb {

    void commands_page_api(Webserver_impl* server) {


        drogon::app().registerHandler(
         "/api/get-commands-list",
         [=](drogon::HttpRequestPtr req,
             std::function<void(const drogon::HttpResponsePtr &)> callback) -> drogon::Task<> {
             auto commands = server->commands_handler->get_commands();
             Json::Value ret = Json::Value(Json::arrayValue);
             for (auto& [name,command]: commands) {
                 Command_data command_data = command->get_command_data();
                 ret.append(to_json(command));
             }

             auto resp = drogon::HttpResponse::newHttpJsonResponse(ret);
             callback(resp);
             co_return;
        });
    }
}
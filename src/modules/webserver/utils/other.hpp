//
// Created by ilesik on 11/20/24.
//

#pragma once
#include <src/modules/webserver/webserver_impl.hpp>

namespace gb{
  void register_config_redirect_handler(Webserver_impl *server, const std::string &path, const std::string &config_key);

}
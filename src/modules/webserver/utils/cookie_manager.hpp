//
// Created by ilesik on 11/10/24.
//

#pragma once
#include <dpp/user.h>
#include <drogon/utils/coroutine.h>
#include <string>

namespace gb{

  struct Discord_user_credentials {
    std::string token_type;
    std::string access_token;
    uint64_t expires_in;
    std::string refresh_token;
    std::string scope;
  };

  drogon::Task<dpp::user> fetch_user_data(const Discord_user_credentials& credentials);

} // namespace gb

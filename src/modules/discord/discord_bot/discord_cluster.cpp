//
// Created by ilesik on 7/10/24.
//

#include "discord_cluster.hpp"

namespace gb {
    Discord_cluster::Discord_cluster(const std::string &token) : cluster(token,dpp::i_default_intents,0,0,1,true,dpp::cache_policy::cpol_none) {}
} // gb
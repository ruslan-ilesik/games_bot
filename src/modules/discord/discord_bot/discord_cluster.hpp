//
// Created by ilesik on 7/10/24.
//

#pragma once
#include <dpp/dpp.h>

namespace gb {

    //wrapper class for dpp cluster to have more flexible behaviour if needed.
    class Discord_cluster: public dpp::cluster {
    public:
        Discord_cluster(const std::string &token);
    };

} // gb



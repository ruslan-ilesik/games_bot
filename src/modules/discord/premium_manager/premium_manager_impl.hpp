//
// Created by ilesik on 10/2/24.
//

#pragma once
#include <src/modules/database/database.hpp>


#include "./premium_manager.hpp"

#include <thread>

namespace gb {

class Premium_manager_impl: public Premium_manager{
  Database_ptr _db;
  std::thread _background_worker;
  std::condition_variable _cv;
  std::mutex _mutex;
  bool _stop = false;

  Prepared_statement _remove_all_expired_subscriptions;
public:
  Premium_manager_impl();
  virtual  ~Premium_manager_impl() = default;

  void init(const Modules &modules) override;
  void run() override;
  void stop() override;

  Task<PREMIUM_STATUS> get_users_premium_status(const dpp::snowflake& user_id) override;

};

  extern "C" Module_ptr create();

} // gb

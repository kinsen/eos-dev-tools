#pragma once
#include <eosiolib/types.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/currency.hpp>
#include <string>

using std::string;
using namespace eosio;

class fomo3d : public currency, public contract
{

  public:
    fomo3d(account_name self) : currency(self), contract(self) {}

    //@abi action
    void transfer(account_name from, account_name to, asset quantity,
                  string memo);
};

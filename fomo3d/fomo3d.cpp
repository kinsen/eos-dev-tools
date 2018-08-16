#include "fomo3d.hpp"

using std::string;
using namespace eosio;

void fomo3d::transfer(account_name from, account_name to, asset quantity,
                      string memo)
{
    print("from:", name{from}, "\n");
    print("to:", name{to}, "\n");
    quantity.print();

    // require_auth(from);
    asset a{0, S(4, RAMCORE)}, a1{0, S(4, RAM)};
    a.print();
    a1.print();

    // asset test = eosio::token(N(eosio.token)).get_supply(eosio::symbol_type(CORE_SYMBOL).name());
    // test.print();

    action(
        permission_level{from, N(active)},
        N(eosio.token), N(transfer),
        std::make_tuple(from, _self, quantity, memo))
        .send();
}

EOSIO_ABI(fomo3d, (transfer));
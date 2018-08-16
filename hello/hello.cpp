#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

using namespace eosio;

class hello : public eosio::contract
{
  public:
    using contract::contract;
    /// @abi action
    void hi(account_name from, account_name to)
    {
        require_auth(from);

        print("Hello, from: ", eosio::name{from}, ", to:", name{to});
        action(
            permission_level{to, N(active)},
            N(hello.target), N(callme),
            std::make_tuple(to))
            .send();
    }
};

EOSIO_ABI(hello, (hi));
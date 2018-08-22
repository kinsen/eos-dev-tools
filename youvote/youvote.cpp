#include <eosiolib/eosio.hpp>

using namespace std;
using namespace eosio;

class youvote : public contract {
 private:
  /// @abi table
  struct poll {
    uint64_t key;
    uint64_t pollId;
    string pollName;
    uint8_t pollStatus = 0;
    string option;
    uint32_t count = 0;

    uint64_t primary_key() const { return key; }
    uint64_t by_pollId() const { return pollId; }
  };
  typedef eosio::multi_index<
      N(poll), poll,
      indexed_by<N(pollId), const_mem_fun<poll, uint64_t, &poll::by_pollId>>>
      pollstable;

  struct pollvotes {
    uint64_t key;
    uint64_t pollId;
    string pollName;
    string account;

    uint64_t primary_key() const { return key; }
    uint64_t by_pollId() const { return pollId; }
  };
  typedef eosio::multi_index<
      N(pollvotes), pollvotes,
      indexed_by<N(pollId),
                 const_mem_fun<pollvotes, uint64_t, &pollvotes::by_pollId>>>
      votes;

  pollstable _polls;
  votes _votes;

 public:
  youvote(account_name s) : contract(s), _polls(s, s), _votes(s, s) {}

  /// @abi action
  void version() { print("YouVote version 0.01"); }

  /// @abi action
  void addpoll(account_name s, string pollName) {
    // require_auth(s);
    print("Add poll ", pollName);

    // update the table to include a new poll
    _polls.emplace(get_self(), [&](auto &p) {
      p.key = _polls.available_primary_key();
      p.pollId = _polls.available_primary_key();
      p.pollName = pollName;
      p.pollStatus = 0;
      p.option = "";
      p.count = 0;
    });
  }

  /// @abi action
  void rmpoll(account_name s, string pollName) {
    // require_auth(s);
    print("Remove poll ", pollName);

    vector<uint64_t> keyForDeletion;

    for (auto &item : _polls) {
      if (item.pollName == pollName) {
        keyForDeletion.push_back(item.key);
      }
    }

    for (uint64_t key : keyForDeletion) {
      print("remove from _polls ", key);
      auto itr = _polls.find(key);
      if (itr != _polls.end()) {
        _polls.erase(itr);
      }
    }

    vector<uint64_t> keysForDeletionFromVotes;
    for (auto &item : _votes) {
      if (item.pollName == pollName) {
        keysForDeletionFromVotes.push_back(item.key);
      }
    }

    for (uint64_t key : keysForDeletionFromVotes) {
      print("remove from _votes", key);
      auto itr = _votes.find(key);
      if (itr != _votes.end()) {
        _votes.erase(itr);
      }
    }
  }

  /// @abi action
  void status(string pollName) {
    print("Change poll status ", pollName);

    vector<uint64_t> keysForModify;
    for (auto &item : _polls) {
      if (item.pollName == pollName) {
        keysForModify.push_back(item.key);
      }
    }

    for (auto &key : keysForModify) {
      print("modify _polls status ", key);
      auto itr = _polls.find(key);
      if (itr != _polls.end()) {
        _polls.modify(itr, get_self(),
                      [&](auto &p) { p.pollStatus = p.pollStatus; });
      }
    }
  }

  /// @abi action
  void statusreset(string pollName) {
    print("Reset poll status ", pollName);

    vector<uint64_t> keysForModify;

    for (auto &item : _polls) {
      if (item.pollName == pollName) {
        keysForModify.push_back(item.key);
      }
    }

    for (uint64_t key : keysForModify) {
      print("modify _polls status ", key);
      auto itr = _polls.find(key);
      if (itr != _polls.end()) {
        _polls.modify(itr, get_self(), [&](auto &p) { p.pollStatus = 0; });
      }
    }
  }

  /// @abi action
  void addpollopt(string pollName, string option) {
    print("Add poll option ", pollName, " option ", option);

    for (auto &item : _polls) {
      if (item.pollName == pollName) {
        if (item.pollStatus == 0) {
          _polls.emplace(get_self(), [&](auto &p) {
            p.key = _polls.available_primary_key();
            p.pollId = item.pollId;
            p.pollName = item.pollName;
            p.pollStatus = 0;
            p.option = option;
            p.count = 0;
          });
        } else {
          print("Can not add poll option ", pollName, " option ", option,
                " Poll has started or is finished.");
        }
        break;
      }
    }
  };

  /// @abi action
  void rmpollopt(string pollName, string option) {
    print("Remove poll option ", pollName, " option ", option);

    vector<uint64_t> keysForDeletion;
    for (auto &item : _polls) {
      if (item.pollName == pollName) {
        keysForDeletion.push_back(item.key);
      }
    }

    for (uint64_t key : keysForDeletion) {
      print("remove from _polls ", key);
      auto itr = _polls.find(key);
      if (itr != _polls.end()) {
        _polls.erase(itr);
      }
    }
  }

  /// @abi action
  void vote(string pollName, string option, string accountName) {
    print("vote for ", option, " in poll ", pollName, " by ", accountName);

    for (auto &item : _polls) {
      if (item.pollName == pollName) {
        if (item.pollStatus != 1) {
          print("Poll ", pollName, " is not open");
          return;
        }
        break;
      }
    }

    for (auto &vote : _votes) {
      if (vote.pollName == pollName && vote.account == accountName) {
        print(accountName, " has already voted in poll ", pollName);
        return;
      }
    }

    uint64_t pollId = 99999;
    for (auto &item : _polls) {
      if (item.pollName == pollName && item.option == option) {
        pollId = item.pollId;
        _polls.modify(item, get_self(),
                      [&](auto &p) { p.count = p.count + 1; });
      }
    }

    _votes.emplace(get_self(), [&](auto &pv) {
      pv.key = _votes.available_primary_key();
      pv.pollId = pollId;
      pv.pollName = pollName;
      pv.account = accountName;
    });
  }
};

EOSIO_ABI(youvote, (version)(addpoll)(rmpoll)(status)(statusreset)(addpollopt)(
                       rmpollopt)(vote))
#!/bin/bash

# Regular Colors
BLACK='\033[0;30m'        # Black
RED='\033[0;31m'          # Red
GREEN='\033[0;32m'        # Green
YELLOW='\033[0;33m'       # Yellow
BLUE='\033[0;34m'         # Blue
PURPLE='\033[0;35m'       # Purple
CYAN='\033[0;36m'         # Cyan
WHITE='\033[0;37m'        # White
NC='\033[0m'              # No Color

USAGE="${RED}Usage: eos-kylin [create|issue|cleos_cmd] account_name${NC}"

function create_account()
{
    check_arguments $@
    curl http://faucet.cryptokylin.io/create_account\?$1
}

function get_token()
{
    check_arguments $@
    echo "curl http://faucet.cryptokylin.io/get_token?$1"
    curl http://faucet.cryptokylin.io/get_token?$1
}

function cleos_cmd()
{
    check_arguments $@
    cleos -u https://api.kylin-testnet.eospace.io $@
}

function check_arguments()
{
    if [ ! $1 ]; then
        echo -e $USAGE
        exit
    fi
}

check_arguments $@

case "$1" in
    "create")
        create_account ${@:2}
        ;;
    "issue")
        get_token ${@:2}
        ;;
    *)
    # Should not occur
        cleos_cmd $@
        # echo -e ${USAGE}
        ;;
esac


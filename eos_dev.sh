#!/bin/bash

cleos="docker exec eos-dev /opt/eosio/bin/cleos"

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

USAGE="${RED}Usage: eos-dev [start|stop|restart|status]${NC}"

function check_container(){
    if [ "$(docker ps -aq -f name=eos-dev)" ]; then
        echo -e "${RED}docker container eos-dev was running.${NC}"
        echo -e "${RED}if you want to restart, must run below command at first.${NC}"
        echo -e "${YELLOW}docker container kill eos-dev${NC}"
        
        exit
    fi
}

function start(){
    check_container
    echo -e "${GREEN}1.运行Docker容器,并运行节点${NC}"
    # P.S. 参数增加 filter-on 使得可以获取用户的action,否则查询不到action所在的transaction.
    # https://developers.eos.io/eosio-cpp/docs/exchange-deposit-withdraw#section-activate-log-filtering
    docker run --name eos-dev --rm -d -p 8888:8888 -p 8900:8900 eosio/eos /bin/bash -c "nodeos -e -p eosio \
        --contracts-console \
        --filter-on=* \
        --plugin eosio::wallet_plugin \
        --plugin eosio::chain_api_plugin \
        --plugin eosio::history_api_plugin \
        --replay-blockchain \
        --http-validate-host=0 \
        --http-server-address=0.0.0.0:8888"

    echo -e "${GREEN}2.运行钱包服务${NC}"
    docker exec -d eos-dev /bin/bash -c "keosd --http-validate-host=0 --http-server-address=0.0.0.0:8900"
    curl http://localhost:8900

    echo -e "\n${GREEN}3.创建钱包并导入私网eosio秘钥${NC}"
    ${cleos} wallet create -n eosio
    ${cleos} wallet import -n eosio  --private-key 5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3

    echo -e "${GREEN}4.部署基础合约${NC}"

    echo -e "${GREEN}4.1 部署基础合约 eosio.bios ${NC}"
    ${cleos} set contract eosio /contracts/eosio.bios -p eosio@active
    echo -e "${GREEN}4.2 部署基础合约 eosio.token ${NC}"
    #$ cleos create key
    #Private key: 5Jmsawgsp1tQ3GD6JyGCwy1dcvqKZgX6ugMVMdjirx85iv5VyPR    
    #Public key: EOS7ijWCBmoXBi3CgtK7DJxentZZeTkeUnaSDvyro9dq7Sd1C3dC4
    ${cleos} wallet import -n eosio --private-key 5Jmsawgsp1tQ3GD6JyGCwy1dcvqKZgX6ugMVMdjirx85iv5VyPR
    ${cleos} create account eosio eosio.token EOS7ijWCBmoXBi3CgtK7DJxentZZeTkeUnaSDvyro9dq7Sd1C3dC4
    ${cleos} set contract eosio.token /contracts/eosio.token/
    ${cleos} push action eosio.token create '["eosio","10000000000000.0000 EOS"]' -p eosio.token
    ${cleos} push action eosio.token issue '["eosio","1000000000000.0000 EOS"]' -p eosio

    echo -e "${GREEN}4.3 部署基础合约 Exchange ${NC}"
    ${cleos} create account eosio exchange EOS7ijWCBmoXBi3CgtK7DJxentZZeTkeUnaSDvyro9dq7Sd1C3dC4
    ${cleos} set contract exchange /contracts/exchange -p exchange@active

    echo -e "${GREEN}4.4 部署基础合约 eosio.msig ${NC}"
    ${cleos} create account eosio eosio.msig EOS7ijWCBmoXBi3CgtK7DJxentZZeTkeUnaSDvyro9dq7Sd1C3dC4
    ${cleos} set contract eosio.msig /contracts/eosio.msig -p eosio.msig@active
}

function restart(){
    stop
    start
}

function stop(){
    if [ "$(docker ps -aq -f name=eos-dev)" ]; then
        echo -e "${RED}stop docker eos-dev...${NC}"
        docker container kill eos-dev
    fi
}

function status(){
    if [ "$(docker ps -aq -f name=eos-dev)" ]; then
        echo -e "${YELLOW}running...${NC}"
    else
        echo -e "${WHITE}no service..${NC}"
    fi
}


if [ ! $1 ]; then
    echo -e $USAGE
    exit
fi

case "$1" in
    "start")
        start
        ;;
    "stop")
        stop
        ;;
    "restart")
        restart
        ;;
    "status")
        status
        ;;
    *)
    # Should not occur
        echo -e ${USAGE}
        ;;
esac
#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_Dance_Base {
public:
    static const int SUCCESS = 0;
};

//============================================================================
// R179 群仙乱舞
//============================================================================
void sxd_client::dance() {

    for (;;) {
        auto data = this->Mod_Dance_Base_get_dance_info();
        if (data[0].asInt() == 0)
            return;
        int dan_state = 0;
        if (data[1].asInt() == 1)
            dan_state = 1;
        if (data[2].asInt() == 1)
            dan_state = 2;

        if (dan_state == 0)
            return;
        if (dan_state == 1) {
            this->Mod_Town_Base_enter_town(1);
            data = this->Mod_Dance_Base_start_dance();
            if (data[0].asInt() != Mod_Dance_Base::SUCCESS) {
                common::log(boost::str(boost::format("【群仙乱舞】跳舞失败，result[%1%]") % data[0]), iEdit);
                return;
            }
            common::log("【群仙乱舞】跳舞", iEdit);
        } else if (dan_state == 2) {
            this->Mod_Town_Base_enter_town(1);
            data = this->Mod_Dance_Base_receive_award();
            if (data[0].asInt() != Mod_Dance_Base::SUCCESS) {
                common::log(boost::str(boost::format("【群仙乱舞】领取奖励失败，result[%1%]") % data[0]), iEdit);
                return;
            }
            common::log("【群仙乱舞】领取奖励", iEdit);
        }
    }
}

//============================================================================
// R179 面板
// "module":76,"action":0,"request":[],"response":[Utils.ByteUtil,Utils.ByteUtil,Utils.ByteUtil]
// DanceData.as 25:
//     this.bDanceTime = param1[0] == 1;
//     this.danState = 0;
//     if(param1[1] == 1)
//        this.danState = 1;
//     if(param1[2] == 1)
//        this.danState = 2;
// Example
// 未到跳舞时间
//     [ 0, 0, 0 ]
// 时间到了
//     [ 1, 1, 0 ]
// 跳舞后
//     [ 1, 1, 1 ]
// 领取后
//     [ 1, 1, 0 ]
//============================================================================
Json::Value sxd_client::Mod_Dance_Base_get_dance_info() {
    Json::Value data;
    return this->send_and_receive(data, 76, 0);
}

//============================================================================
// R179 开始跳舞
// "module":76,"action":1,"request":[],"response":[Utils.UByteUtil]
// Example
//     [ 1 ]
//     [ 0 ] 成功
//============================================================================
Json::Value sxd_client::Mod_Dance_Base_start_dance() {
    Json::Value data;
    return this->send_and_receive(data, 76, 1);
}

//============================================================================
// R179 领取奖励
// "module":76,"action":2,"request":[],"response":[Utils.UByteUtil]
// Example
//     [ 1 ]
//     [ 0 ] 成功
//============================================================================
Json::Value sxd_client::Mod_Dance_Base_receive_award() {
    Json::Value data;
    return this->send_and_receive(data, 76, 2);
}

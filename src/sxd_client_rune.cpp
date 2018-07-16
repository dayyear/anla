#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_Rune_Base {
public:
    static const int SUCCEED = 0;
};

//============================================================================
// 招财神符
//============================================================================
void sxd_client::rune() {
    auto data = this->Mod_Rune_Base_rune_list();
    int count = data[0].asInt();
    int ingot = data[1].asInt();
    if (count && ingot == 2) {
        data = this->Mod_Rune_Base_rune_use();
        if (data[0].asInt() != Mod_Rune_Base::SUCCEED) {
            common::log(boost::str(boost::format("【招财神符】使用失败，result[%1%]") % data[0]));
            return;
        }
        common::log(boost::str(boost::format("【招财神符】消费 [元宝×%1%]，获得 [铜钱×%2%]") % ingot % data[1]));
    }
}

//============================================================================
// R180 面板
// "module":16,"action":1,"request":[],"response":[Utils.IntUtil,Utils.IntUtil,Utils.IntUtil]
// RuneController.as:
//     _loc2_.count = _loc1_[0];
//     _loc2_.ingot = _loc1_[1];
//     _loc2_.coins = _loc1_[2];
// Example
//     [ 1, 2, 800000 ]
//============================================================================
Json::Value sxd_client::Mod_Rune_Base_rune_list() {
    Json::Value data;
    return this->send_and_receive(data, 16, 1);
}

//============================================================================
// R180 使用
// "module":16,"action":2,"request":[],"response":[Utils.UByteUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil]
// RuneController.as:
//     _loc2_.msg = _loc1_[0];
//     _loc2_.getCoins = _loc1_[1];
//     _loc2_.count = _loc1_[2];
//     _loc2_.ingot = _loc1_[3];
//     _loc2_.coins = _loc1_[4];
//     _loc2_.crit = _loc1_[5];
// Example
//     [ 0, 800000, 0, 4, 800000, 10 ]
//============================================================================
Json::Value sxd_client::Mod_Rune_Base_rune_use() {
    Json::Value data;
    return this->send_and_receive(data, 16, 2);
}

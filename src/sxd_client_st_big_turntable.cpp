#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_StBigTurntable_Base {
public:
    static const int SUCCESS = 0;
};

//============================================================================
// 游乐城大转盘
//============================================================================
void sxd_client::st_big_turntable() {
    for (;;) {
        Json::Value data = this->Mod_StBigTurntable_Base_get_big_turntable_info();
        if (!data[0].asInt())
            break;

        data = this->Mod_StBigTurntable_Base_turntable();
        if (data[0].asInt() != Mod_StBigTurntable_Base::SUCCESS) {
            common::log(boost::str(boost::format("【游乐城大转盘】抽奖失败，result[%1%]") % data[0]), 0);
            return;
        }
        common::log(boost::str(boost::format("【游乐城大转盘】抽奖获得 [%1%×%2%]") % db.get_code(version, "Item", data[2].asInt())["text"] % data[3]));
    }

}

//============================================================================
// R179 面板
// "module":286, "action":0, "request":[], "response":[Utils.IntUtil,Utils.IntUtil,Utils.ShortUtil,Utils.ShortUtil,[Utils.ByteUtil,Utils.ShortUtil,Utils.IntUtil,Utils.IntUtil,Utils.ByteUtil],Utils.IntUtil,Utils.IntUtil]
// BigTurntableData.as 31:
//     this._bigTurntableInfoObject["ticket_num"] = param1[0];
//     this._bigTurntableInfoObject["ingot_cost"] = param1[1];
//     this._bigTurntableInfoObject["had_buy_times"] = param1[2];
//     this._bigTurntableInfoObject["max_buy_times"] = param1[3];
//     this._bigTurntableInfoObject["begin_timestamp"] = param1[5];
//     this._bigTurntableInfoObject["end_timestamp"] = param1[6];
// Example
//     [ 5, 20, 0, 5, [ [ 1, 3, 4251, 2, 0 ], [ 2, 22, 1743, 30, 0 ], [ 3, 37, 4481, 500, 0 ], [ 4, 35, 4481, 2000, 0 ], [ 5, 11, 1845, 20, 0 ], [ 6, 31, 4649, 3, 0 ], [ 7, 26, 2452, 8, 0 ], [ 8, 36, 4481, 300, 0 ], [ 9, 17, 2395, 6, 0 ], [ 10, 20, 1747, 3400000, 0 ] ], 1526313600, 1529855999 ]
//============================================================================
Json::Value sxd_client::Mod_StBigTurntable_Base_get_big_turntable_info() {
    Json::Value data;
    return this->send_and_receive(data, 286, 0);
}

//============================================================================
// R179 抽奖
// "module":286,"action":1,"request":[],"response":[Utils.UByteUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.ShortUtil,[Utils.ByteUtil,Utils.ShortUtil,Utils.IntUtil,Utils.IntUtil,Utils.ByteUtil]]
// BigTurntableData.as 67:
//     this._turntableObject["result"] = param1[0];
//     this._turntableObject["result_seq"] = param1[1];
//     this._turntableObject["result_item_id"] = param1[2];
//     this._turntableObject["result_item_num"] = param1[3];
//     this._turntableObject["ticket_num"] = param1[4];
//     this._turntableObject["had_buy_times"] = param1[5];
//     for each(_loc2_ in param1[6])
//         _loc3_["seq"] = _loc2_[0];
//         _loc3_["award_id"] = _loc2_[1];
//         _loc3_["item_id"] = _loc2_[2];
//         _loc3_["item_num"] = _loc2_[3];
//         _loc3_["is_get"] = _loc2_[4] == 0?false:true;
// Example
//     [ 0, 6, 4649, 3, 4, 0, [ [ 1, 3, 4251, 2, 0 ], [ 2, 22, 1743, 30, 0 ], [ 3, 37, 4481, 500, 0 ], [ 4, 35, 4481, 2000, 0 ], [ 5, 11, 1845, 20, 0 ], [ 6, 31, 4649, 3, 1 ], [ 7, 26, 2452, 8, 0 ], [ 8, 36, 4481, 300, 0 ], [ 9, 17, 2395, 6, 0 ], [ 10, 20, 1747, 3400000, 0 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_StBigTurntable_Base_turntable() {
    Json::Value data;
    return this->send_and_receive(data, 286, 1);
}

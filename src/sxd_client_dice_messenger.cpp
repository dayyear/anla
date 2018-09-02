#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_DiceMessenger_Base {
public:
    static const int SUCCESS = 0;
};

//============================================================================
// 骰子使者
//============================================================================
void sxd_client::dice_messenger() {
    for (;;) {
        Json::Value data = this->Mod_DiceMessenger_Base_get_info();
        int all_times = data[3].asInt();
        int today_time = data[4].asInt();
        if (all_times == today_time)
            break;

        data = this->Mod_DiceMessenger_Base_play_dice();
        if (data[0].asInt() != Mod_DiceMessenger_Base::SUCCESS) {
            common::log(boost::str(boost::format("【骰子使者】掷一下失败，result[%1%]") % data[0]), 0);
            return;
        }
        std::vector<std::string> draws;
        std::transform(data[1].begin(), data[1].end(), std::back_inserter(draws), [](const Json::Value& x) {
            return x[0].asString();
        });
        common::log(boost::str(boost::format("【骰子使者】掷一下，获得骰子点数 [%1%]") % boost::algorithm::join(draws, "，")), iEdit);

        data = this->Mod_DiceMessenger_Base_get_award();
        if (data[0].asInt() != Mod_DiceMessenger_Base::SUCCESS) {
            common::log(boost::str(boost::format("【骰子使者】领取奖励失败，result[%1%]") % data[0]), iEdit);
            return;
        }
        std::vector<std::string> items;
        std::transform(data[1].begin(), data[1].end(), std::back_inserter(items), [this](const Json::Value& x) {
            return boost::str(boost::format("[%1%×%2%]") % db.get_code(version, "Item", x[0].asInt())["text"] % x[1]);
        });
        common::log(boost::str(boost::format("【骰子使者】领取奖励：%1%") % boost::algorithm::join(items, "，")), iEdit);

        data = this->Mod_DiceMessenger_Base_tol_get();
        if (data[0].asInt() == Mod_DiceMessenger_Base::SUCCESS) {
            std::vector<std::string> items;
            std::transform(data[1].begin(), data[1].end(), std::back_inserter(items), [this](const Json::Value& x) {
                return boost::str(boost::format("[%1%×%2%]") % db.get_code(version, "Item", x[0].asInt())["text"] % x[1]);
            });
            common::log(boost::str(boost::format("【骰子使者】领取累计点数奖励：%1%") % boost::algorithm::join(items, "，")), iEdit);
        }
    }
}

//============================================================================
// R177 骰子使者面板
// "module":417, "action":0, "request":[], "response":[Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,[Utils.IntUtil,Utils.IntUtil,[Utils.IntUtil,Utils.IntUtil]],[Utils.IntUtil],[Utils.IntUtil,Utils.IntUtil,[Utils.IntUtil,Utils.IntUtil]]]
// DiceMessengerData.as 31:
//     this.info.tol_dot = param1[_loc2_++];
//     this.info.my_gold = param1[_loc2_++];
//     this.info.need_cost = param1[_loc2_++];
//     this.info.all_times = param1[_loc2_++];
//     this.info.today_time = param1[_loc2_++];
//     this.info.dice_award = this.parseDiceAwardList(param1[_loc2_++]);
//     this.info.my_draw_list = this.parseMyDrawList(param1[_loc2_++]);
//     this.info.tol_award = this.parseTotalAwardList(param1[_loc2_++]);
// Example
//     [ 0, 5650, 50, 3, 0, [ [ 10, 1, [ [ 1747, 1200000 ] ] ], [ 10, 2, [ [ 4481, 100 ], [ 6074, 1 ] ] ], [ 7, 3, [ [ 3345, 1 ] ] ] ], [ [ 0 ], [ 0 ], [ 0 ] ], [ [ 400, 0, [ [ 5823, 5 ] ] ], [ 100, 0, [ [ 1740, 50 ] ] ], [ 200, 0, [ [ 2343, 50 ] ] ], [ 800, 0, [ [ 4291, 2 ] ] ], [ 600, 0, [ [ 4954, 2 ] ] ] ] ]
//     [ 6, 5600, 50, 3, 1, [ [ 10, 1, [ [ 1747, 1200000 ] ] ], [ 10, 2, [ [ 4481, 100 ], [ 6074, 1 ] ] ], [ 7, 3, [ [ 3345, 1 ] ] ] ], [ [ 0 ], [ 0 ], [ 0 ] ], [ [ 400, 0, [ [ 5823, 5 ] ] ], [ 100, 0, [ [ 1740, 50 ] ] ], [ 200, 0, [ [ 2343, 50 ] ] ], [ 800, 0, [ [ 4291, 2 ] ] ], [ 600, 0, [ [ 4954, 2 ] ] ] ] ]
//============================================================================
Json::Value sxd_client::Mod_DiceMessenger_Base_get_info() {
    Json::Value data;
    return this->send_and_receive(data, 417, 0);
}

//============================================================================
// R177 掷一下
// "module":417, "action":1, "request":[], "response":[Utils.UByteUtil,[Utils.IntUtil]]
// DiceMessengerData.as 110:
//     this.info.result = param1[_loc2_++];
//     this.info.my_draw_list = this.parseMyDrawList(param1[_loc2_++]);
// Example
//     [ 0, [ [ 1 ], [ 1 ], [ 4 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_DiceMessenger_Base_play_dice() {
    Json::Value data;
    return this->send_and_receive(data, 417, 1);
}

//============================================================================
// R177 领取奖励
// "module":417, "action":2, "request":[], "response":[Utils.UByteUtil,[Utils.IntUtil,Utils.IntUtil],Utils.IntUtil]
// DiceMessengerData.as 128:
//     this.info.result = param1[_loc2_++];
//     this.info.award_list = this.parseAwardList(param1[_loc2_++]);
//     this.info.my_gold = param1[_loc2_++];
// Example
//     [ 0, [ [ 1747, 7200000 ] ], 5600 ]
//============================================================================
Json::Value sxd_client::Mod_DiceMessenger_Base_get_award() {
    Json::Value data;
    return this->send_and_receive(data, 417, 2);
}

//============================================================================
// R177 领取累计点数奖励
// "module":417,"action":3,"request":[],"response":[Utils.UByteUtil,[Utils.IntUtil,Utils.IntUtil]]
// DiceMessengerData.as 142:
//     this.info.result = param1[_loc2_++];
//     this.info.award_list = this.parseAwardList(param1[_loc2_++]);
// Example
//     [ 5, null ]
//============================================================================
Json::Value sxd_client::Mod_DiceMessenger_Base_tol_get() {
    Json::Value data;
    return this->send_and_receive(data, 417, 3);
}


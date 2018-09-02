#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_CoinMission_Base {
public:
    static const int SUCCESS = 4;
};

//============================================================================
// 铜钱副本
//============================================================================
void sxd_client::coin_mission() {
    std::string mission_names[] = { "", "黄金屋", "藏宝阁", "财神殿" };
    int mission_coins[] = { 0, 30, 60, 100 };
    int mission_levels[] = { 0, 100, 160, 220 };

    // get my level
    auto data = this->Mod_Player_Base_get_player_info();
    int my_level = data[1].asInt();

    data = this->Mod_CoinMission_Base_get_coin_mission_info();
    // 从大到小排序
    std::vector<Json::Value> missions;
    std::copy(data[0].begin(), data[0].end(), std::back_inserter(missions));
    std::sort(missions.begin(), missions.end(), [](const Json::Value& x, const Json::Value& y) {return x[0].asInt() > y[0].asInt();});

    for (const Json::Value& mission : missions) {
        int mission_id = mission[0].asInt();
        int flag = mission[1].asInt();
        if (flag)
            continue;
        if (my_level < mission_levels[mission_id])
            continue;
        data = this->Mod_CoinMission_Base_fight(mission_id);
        if (data[0].asInt() == Mod_CoinMission_Base::SUCCESS)
            common::log(boost::str(boost::format("【铜钱副本】挑战 [%1%]，击杀 [%2%]只铜钱小妖，获得 [铜钱×%3%万]") % mission_names[mission_id] % data[1] % (data[1].asInt() * mission_coins[mission_id])), iEdit);
    }
}

//============================================================================
// R179 面板
// "module":374,"action":0,"request":[],"response":[[Utils.ByteUtil,Utils.UByteUtil,Utils.ShortUtil,Utils.ShortUtil]]
// CoinMissionData.as:
//     for each(_loc3_ in param1)
//         oObject.list(_loc3_,_loc4_,["mission_id","flag","kill_count","reset_kill_count"]);
// Example
//     [ [ [ 3, 0, 0, 0 ], [ 1, 0, 0, 0 ], [ 2, 0, 0, 0 ] ] ]
// 挑战后
//     [ [ [ 3, 0, 0, 0 ], [ 1, 1, 30, 0 ], [ 2, 0, 0, 0 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_CoinMission_Base_get_coin_mission_info() {
    Json::Value data;
    return this->send_and_receive(data, 374, 0);
}

//============================================================================
// R179 铜钱小妖阵形
// "module":374,"action":4,"request":[Utils.IntUtil],"response":[[Utils.ByteUtil]]
// Example
//     [ [ [ 2 ], [ 7 ], [ 3 ], [ 1 ], [ 4 ], [ 5 ], [ 9 ] ] ]
//     [ [ [ 9 ], [ 8 ], [ 5 ], [ 4 ], [ 6 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_CoinMission_Base_get_deploys(int mission_id) {
    Json::Value data;
    data.append(mission_id);
    return this->send_and_receive(data, 374, 4);
}

//============================================================================
// R179 挑战
// "module":374,"action":1,"request":[Utils.ByteUtil],"response":[Utils.UByteUtil,Utils.ShortUtil,[...]]
// CoinMissionData.as:
//     this.info.result = param1[_loc2_++];
//     this.info.total_kill_count = param1[_loc2_++];
//     this.info.war_result_list = param1[_loc2_++];
// Example
//     [ 7, 0, null ]
//     [ 4, 30, [...]]
//============================================================================
Json::Value sxd_client::Mod_CoinMission_Base_fight(int mission_id) {
    Json::Value data;
    data.append(mission_id);
    return this->send_and_receive(data, 374, 1);
}

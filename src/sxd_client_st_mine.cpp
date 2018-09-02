#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_StMine_Base {
public:
    static const int SUCCESS = 9;
};

//============================================================================
// R180 矿山系统
//============================================================================
void sxd_client::st_mine() {
    auto data = this->Mod_StMine_Base_get_mine_mountain_info();
    if (data[0].asInt() != Mod_StMine_Base::SUCCESS) {
        common::log(boost::str(boost::format("【矿山系统】打开矿山系统失败，result[%1%]") % data[0]), iEdit);
        return;
    }
    int cur_mountain = data[3].asInt();

    // mountain
    std::vector<Json::Value> mountains;
    std::copy_if(data[4].begin(), data[4].end(), std::back_inserter(mountains), [](const Json::Value& x) {return x[1].asInt() < x[2].asInt();});
    if (!mountains.size())
        return;
    std::sort(mountains.begin(), mountains.end(), [](const Json::Value& x, const Json::Value& y) {return x[2].asInt() < y[2].asInt();});
    if (mountains[0][0].asInt() <= cur_mountain)
        return;
    data = this->Mod_StMine_Base_enter_mine_mountain(mountains[0][0].asInt());
    if (data[0].asInt() != Mod_StMine_Base::SUCCESS) {
        common::log(boost::str(boost::format("【矿山系统】进入 [%1%] 星矿山失败，result[%2%]") % mountains[0][0] % data[0]), iEdit);
        return;
    }

    // hole with nobody
    std::vector<Json::Value> holes;
    std::copy_if(data[22].begin(), data[22].end(), std::back_inserter(holes), [](const Json::Value& x) {return !x[4].asInt();});

    // rob
    this->Mod_StMine_Base_leave_mine_hole();
    this->Mod_StMine_Base_receive_award();
    data = this->Mod_StMine_Base_rob_mine_hole(holes[0][0].asInt(), holes[0][4].asInt());
    if (data[0].asInt() != Mod_StMine_Base::SUCCESS) {
        common::log(boost::str(boost::format("【矿山系统】占领 [%1%] 星矿山失败，result[%2%]") % mountains[0][0] % data[0]), iEdit);
        return;
    }
    common::log(boost::str(boost::format("【矿山系统】占领 [%1%] 星矿山") % mountains[0][0]), iEdit);
}

//============================================================================
// R180 面板
// "module":264,"action":0,"request":[],"response":[Utils.UByteUtil,Utils.ByteUtil,Utils.IntUtil,Utils.ByteUtil,[Utils.ByteUtil,Utils.ByteUtil,Utils.ByteUtil],Utils.UByteUtil,Utils.ByteUtil]
// StMineData.as:
//     this.mineInfo.result = param1[_loc2_++];
//     this.mineInfo.level = param1[_loc2_++];
//     this.mineInfo.total_skill = param1[_loc2_++];
//     this.mineInfo.need_skill = StMineType.getMountainNeedExp(this.mineInfo.level);
//     this.mineInfo.cur_mountain = param1[_loc2_++];
//     this.mineInfo.mine_mountain_info = this.parseMineMountainList(param1[_loc2_++]);
//         oObject.list(_loc3_,_loc4_,["level","used_count","total_count"]);
//     this.mineInfo.lucky_mountain_open_flag = param1[_loc2_++];
//     this.mineInfo.lucky_mountain_flag = param1[_loc2_++];
// Example
// 周一 00:00-20:00 未矿洞修整期，状态是什么?
//     未开始
//     [ 9, 5, 103640049, **0**, [ [ 1, 30, 30 ], [ 2, 20, 20 ], [ 5, 5, 5 ], [ 3, 14, 15 ], [ 4, 10, 10 ] ], 14, 0 ]
//     已开始
//     [ 9, 5, 103668081, **3**, [ [ 1, 29, 30 ], [ 2, 20, 20 ], [ 5, 4, 5 ], [ 3, 15, 15 ], [ 4, 10, 10 ] ], 14, 0 ]
//============================================================================
Json::Value sxd_client::Mod_StMine_Base_get_mine_mountain_info() {
    Json::Value data;
    return this->send_and_receive(data, 264, 0);
}

//============================================================================
// R180 进入矿山
// "module":264,"action":1,"request":[Utils.ByteUtil],"response":[Utils.UByteUtil,Utils.ByteUtil,Utils.IntUtil,Utils.ShortUtil,Utils.ByteUtil,Utils.IntUtil,Utils.UByteUtil,Utils.ShortUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.ShortUtil,Utils.IntUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.StringUtil,Utils.StringUtil,[Utils.ShortUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.UByteUtil,Utils.IntUtil,Utils.StringUtil,Utils.ShortUtil,Utils.StringUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil],Utils.IntUtil]
// StMineData.as:
//     this.mineHolesInfo.result = param1[_loc2_++];
//     this.mineHolesInfo.skill_level = param1[_loc2_++];
//     this.mineHolesInfo.skill_value = param1[_loc2_++];
//     this.mineHolesInfo.need_skill = StMineType.getPersonalSkillNeedExp(this.mineHolesInfo.skill_level);
//     this.mineHolesInfo.remain_rob_count = param1[_loc2_++];
//     this.mineHolesInfo.buy_need_ingot = param1[_loc2_++];
//     this.mineHolesInfo.next_rob_timestamp = param1[_loc2_++];
//     this.mineHolesInfo.player_mine_hole_info = this.parsePlayerMineHole(param1.slice(_loc2_,_loc2_ + 16));
//         _loc2_.status = param1[_loc3_++];
//         _loc2_.hole_id = param1[_loc3_++];
//         _loc2_.rob_over_timestamp = param1[_loc3_++];
//         _loc2_.next_award_timestamp = param1[_loc3_++];
//         _loc2_.protect_over_timestamp = param1[_loc3_++];
//         _loc2_.item_id = param1[_loc3_++];
//         _loc2_.item_count = param1[_loc3_++];
//         _loc2_.need_minutes = param1[_loc3_++];
//         _loc2_.receive_count = param1[_loc3_++];
//         _loc2_.ingot_rob_flag = param1[_loc3_++];
//         _loc2_.daoyuan_rob_flag = param1[_loc3_++];
//         _loc2_.recapture_timestamp = param1[_loc3_++];
//         _loc2_.be_recapture_timestamp = param1[_loc3_++];
//         _loc2_.enemy_role_id = param1[_loc3_++];
//         _loc2_.enemy_nickname = param1[_loc3_++];
//         _loc2_.enemy_servername = param1[_loc3_++];
//     this.mineHolesInfo.mine_hole_info = this.parseMineHoleList(param1[_loc2_ + 16]);
//         oObject.list(_loc3_,_loc4_,["hole_id","lucky_flag","type","status","player_id","nick_name","player_level","server_name","protect_over_timestamp","rob_over_timestamp","receive_count"]);
//     StMineType.feedback_mult = param1[_loc2_ + 17];
// Example
//     [ 9, 4, 23304, **3**, 20, 0,      **0, 0, 0, 0, 0, 2403, 0, 0, 70, 0, 0, 0, 0, 0, "", "",**        [ [ 29, 0, 23, 1, 2977, "\u98d8\u98d8\u8f7b\u4e91.s176", 260, "37wan_s0520", 1529935663, 1530014863, 6 ], [ 61, 0, 8, 1, 2269, "\u60c5\u7fa9\u4e4b\u5dd4\u00b0\u27aa.s156", 258, "duowan_s0152", 1529991967, 1530071167, 60 ], [ 36, 0, 11, 1, 1512, "\u9ad8\u4e39\u67ab", 259, "wodota_s1", 1529935228, 1530014428, 94 ], [ 56, 0, 6, 1, 3821, "\u5341\u4e09.s15", 230, "rising_s033", 1529938262, 1530017462, 90 ],
//        **[ 59, 0, 4, 0, 0, "", 0, "", 0, 0, 0 ]**, [ 12, 0, 11, 1, 3275, "\u8def\u8fc7.s177", 260, "37wan_s0520", 1529935835, 1530015035, 92 ], [ 48, 0, 3, 1, 2925, "\u6b27\u9633\u9038\u5929.s151", 259, "s077", 1529984106, 1530063306, 8 ], [ 67, 0, 21, 1, 3504, "\u4e91\u98de\u4e0d\u7406.s168", 258, "37wan_s0520", 1529939304, 1530012744, 12 ], [ 73, 0, 17, 1, 3360, "\u57cb\u846c*\u79cb.s177", 260, "37wan_s0520", 1529935795, 1530014995, 30 ], [ 14, 0, 2, 1, 7455, "\u590f\u4faf\u4e18\u4e7e.s299", 192, "s0151", 1529937759, 1530016239, 2600000 ], [ 34, 0, 8, 1, 2962, "\u5dc5\u5cf0~\\\u7687\u5c0f.s165", 258, "37wan_s0520", 1529944809, 1530024009, 164 ], [ 63, 0, 13, 1, 3266, "\u7a0b\u666e\u68ee.s151", 258, "s077", 1529936989, 1530016189, 920 ], [ 28, 0, 6, 1, 994, "\u5415\u53cc\u971c", 250, "youxilan_s1", 1529937783, 1530016983, 90 ], [ 7, 0, 11, 1, 3277, "\u9001\u82b1\u75371.s177", 260, "37wan_s0520", 1529936183, 1530015383, 92 ], [ 13, 0, 16, 1, 3329, "\u6211\u662fV5.s175", 260, "37wan_s0520", 1529935654, 1530014854, 30 ] ], 2 ]
//============================================================================
Json::Value sxd_client::Mod_StMine_Base_enter_mine_mountain(int id) {
    Json::Value data;
    data.append(id);
    return this->send_and_receive(data, 264, 1);
}

//============================================================================
// R180 离开
// "module":264,"action":4,"request":[],"response":[Utils.UByteUtil,Utils.ByteUtil,Utils.IntUtil]
// StMineData.as:
//     this.mineHolesInfo.leaveResult = param1[_loc2_++];
//     this.mineHolesInfo.skill_level = param1[_loc2_++];
//     this.mineHolesInfo.skill_value = param1[_loc2_++];
// Example
//     [ 9, 4, 25644 ]
//============================================================================
Json::Value sxd_client::Mod_StMine_Base_leave_mine_hole(){
    Json::Value data;
    return this->send_and_receive(data, 264, 4);
}

//============================================================================
// R180 领取收益
// "module":264,"action":7,"request":[],"response":[Utils.UByteUtil]
// StMineData.as:
//     this.mineHolesInfo.receiveResult = param1[0];
// Example
//     [ 9 ]
//============================================================================
Json::Value sxd_client::Mod_StMine_Base_receive_award() {
    Json::Value data;
    return this->send_and_receive(data, 264, 7);
}

//============================================================================
// R180 抢夺
// "module":264,"action":3,"request":[Utils.ShortUtil,Utils.IntUtil],"response":[Utils.UByteUtil,[Utils.ByteUtil,Utils.IntUtil,[Utils.IntUtil,Utils.StringUtil,[Utils.IntUtil,Utils.IntUtil,Utils.StringUtil,Utils.StringUtil,Utils.IntUtil,Utils.LongUtil,Utils.LongUtil,Utils.ShortUtil,Utils.ByteUtil,Utils.IntUtil,Utils.IntUtil,Utils.ShortUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.ShortUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.IntUtil,[Utils.IntUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.ShortUtil],Utils.ByteUtil,Utils.ShortUtil,[Utils.ShortUtil],[Utils.ShortUtil],Utils.ShortUtil,Utils.ShortUtil,Utils.ShortUtil,[Utils.ShortUtil],[Utils.ShortUtil],Utils.ShortUtil,Utils.ByteUtil,Utils.LongUtil,Utils.ByteUtil,Utils.IntUtil],[Utils.IntUtil,Utils.IntUtil,Utils.StringUtil,Utils.StringUtil,Utils.IntUtil,Utils.LongUtil,Utils.LongUtil,Utils.ShortUtil,Utils.ByteUtil,Utils.IntUtil,Utils.IntUtil,Utils.ShortUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.ShortUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.IntUtil,[Utils.IntUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.ShortUtil],Utils.ByteUtil,Utils.ShortUtil,[Utils.ShortUtil],[Utils.ShortUtil],Utils.ShortUtil,Utils.ShortUtil,Utils.ShortUtil,[Utils.ShortUtil],[Utils.ShortUtil],Utils.ShortUtil,Utils.ByteUtil,Utils.LongUtil,Utils.ByteUtil,Utils.IntUtil],[Utils.IntUtil,Utils.IntUtil,Utils.StringUtil,Utils.StringUtil,Utils.IntUtil,Utils.LongUtil,Utils.LongUtil,Utils.ShortUtil,Utils.ByteUtil,Utils.IntUtil,Utils.IntUtil,Utils.ShortUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.ShortUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.IntUtil,[Utils.IntUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.ShortUtil],Utils.ByteUtil,Utils.ShortUtil,[Utils.ShortUtil],[Utils.ShortUtil],Utils.ShortUtil,Utils.ShortUtil,Utils.ShortUtil,[Utils.ShortUtil],[Utils.ShortUtil],Utils.ShortUtil,Utils.ByteUtil,Utils.LongUtil,Utils.ByteUtil,Utils.IntUtil],[Utils.IntUtil,Utils.IntUtil,Utils.StringUtil,Utils.StringUtil,Utils.IntUtil,Utils.LongUtil,Utils.LongUtil,Utils.ShortUtil,Utils.ByteUtil,Utils.IntUtil,Utils.IntUtil,Utils.ShortUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.ShortUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.IntUtil,[Utils.IntUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.ShortUtil],Utils.ByteUtil,Utils.ShortUtil,[Utils.ShortUtil],[Utils.ShortUtil],Utils.ShortUtil,Utils.ShortUtil,Utils.ShortUtil,[Utils.ShortUtil],[Utils.ShortUtil],Utils.ShortUtil,Utils.ByteUtil,Utils.LongUtil,Utils.ByteUtil,Utils.IntUtil],[Utils.IntUtil],[Utils.IntUtil,Utils.IntUtil],Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.ShortUtil,Utils.IntUtil,Utils.StringUtil,Utils.IntUtil,Utils.IntUtil,[Utils.ByteUtil,Utils.ByteUtil,Utils.ByteUtil],Utils.IntUtil,[Utils.IntUtil,Utils.ByteUtil],Utils.LongUtil,Utils.LongUtil,[Utils.IntUtil],Utils.ByteUtil,[Utils.ByteUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,[Utils.IntUtil,Utils.IntUtil,Utils.ByteUtil,Utils.IntUtil,Utils.IntUtil]]],[[Utils.ByteUtil,Utils.StringUtil,[Utils.UByteUtil,Utils.StringUtil],[Utils.StringUtil],[Utils.IntUtil,Utils.LongUtil,Utils.IntUtil],[Utils.StringUtil],[Utils.StringUtil],[Utils.StringUtil],[Utils.StringUtil],[Utils.StringUtil]],[Utils.ByteUtil,Utils.ByteUtil,Utils.IntUtil,[Utils.StringUtil],[Utils.IntUtil,Utils.LongUtil,Utils.IntUtil]],[Utils.IntUtil,Utils.IntUtil,Utils.ShortUtil,Utils.ByteUtil,[Utils.UByteUtil,Utils.ShortUtil,Utils.ByteUtil],Utils.StringUtil,[Utils.ShortUtil],[Utils.StringUtil],[Utils.StringUtil],[Utils.ShortUtil],[Utils.StringUtil],Utils.StringUtil,[Utils.ShortUtil],[Utils.StringUtil],Utils.StringUtil,[Utils.ShortUtil],[Utils.StringUtil],Utils.StringUtil,[Utils.ShortUtil],[Utils.StringUtil],Utils.StringUtil,[Utils.IntUtil],[Utils.StringUtil],Utils.IntUtil,Utils.ShortUtil,Utils.LongUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.StringUtil,Utils.IntUtil,Utils.UByteUtil,[Utils.IntUtil,Utils.ShortUtil,[Utils.UByteUtil,Utils.ShortUtil],Utils.StringUtil,[Utils.ShortUtil],[Utils.StringUtil],[Utils.ShortUtil],Utils.StringUtil,[Utils.ShortUtil],Utils.StringUtil,[Utils.ShortUtil],Utils.StringUtil,[Utils.ShortUtil],Utils.StringUtil,[Utils.IntUtil],Utils.IntUtil,Utils.IntUtil,Utils.LongUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.IntUtil,Utils.ByteUtil],[Utils.IntUtil,Utils.StringUtil],[Utils.IntUtil,Utils.LongUtil,Utils.IntUtil],[Utils.UByteUtil,Utils.StringUtil]],[Utils.IntUtil,Utils.IntUtil],[Utils.UByteUtil,Utils.StringUtil,[Utils.UByteUtil,Utils.StringUtil],[Utils.StringUtil],[Utils.StringUtil],[Utils.StringUtil],[Utils.StringUtil],[Utils.StringUtil],[Utils.StringUtil]],[Utils.IntUtil,Utils.LongUtil,Utils.IntUtil],[Utils.IntUtil,Utils.LongUtil,Utils.IntUtil]],Utils.ShortUtil],Utils.ShortUtil,Utils.IntUtil,Utils.UByteUtil,Utils.ShortUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.ShortUtil,Utils.IntUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.StringUtil,Utils.StringUtil]
// StMineData.as:
//     this.mineHolesInfo.robResult = param1[_loc2_++];
//     this.mineHolesInfo.war_result = param1[_loc2_++];
//     this.mineHolesInfo.remain_rob_count = param1[_loc2_++];
//     this.mineHolesInfo.next_rob_timestamp = param1[_loc2_++];
//     this.mineHolesInfo.player_mine_hole_info = this.parsePlayerMineHole(param1.slice(_loc2_,_loc2_ + 16));
// Example
//     [ 59, 0 ] --> [ 9, null, 3, 1529229219, 1, 59, 1530099114, 1530021354, 1530019914, 1740, 2, 144, 0, 1, 1, 0, 0, 0, "", "" ]
//============================================================================
Json::Value sxd_client::Mod_StMine_Base_rob_mine_hole(int hole_id, int robbed_player_id) {
    Json::Value data;
    data.append(hole_id);
    data.append(robbed_player_id);
    return this->send_and_receive(data, 264, 3);
}

#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_StUnionActivity_Base {
public:
    static const int SUCCESS = 2;
    static const int BLESS_SUCCESS = 7;
};

void sxd_client::st_union_activity() {
    // tree
    Json::Value data = this->Mod_StUnionActivity_Base_get_st_union_tree_info();
    Json::Value tree_info = data;
    if (tree_info[0].asInt() == 0) {
        common::log("【仙盟之树】未加仙盟");
        return;
    }
    if (tree_info[6].asInt()) {
        data = this->Mod_StUnionActivity_Base_player_get_tree_gift();
        if (data[0].asInt() != Mod_StUnionActivity_Base::SUCCESS) {
            common::log(boost::str(boost::format("【仙盟之树】领取失败，result[%1%]") % data[0]));
            return;
        }
        common::log(boost::str(boost::format("【仙盟之树】领取 [%1%×%2%]") % db.get_code(version, "Gift", tree_info[5][0][1].asInt())["text"] % tree_info[5][0][3]));
    } else if (tree_info[2].asInt() == tree_info[3].asInt()) {
        common::log("【仙盟之树】今日许愿成功");
    } else if (tree_info[5].size() == 3) {
        int index = rand() % 3;
        data = this->Mod_StUnionActivity_Base_choose_wish_item(tree_info[5][index][0].asInt());
        if (data[0].asInt() != Mod_StUnionActivity_Base::SUCCESS) {
            common::log(boost::str(boost::format("【仙盟之树】许愿选择失败，result[%1%]") % data[0]));
            return;
        }
        common::log(boost::str(boost::format("【仙盟之树】许愿选择第 [%1%] 个物品 [%2%]") % (index + 1) % db.get_code(version, "Gift", tree_info[5][index][1].asInt())["text"]));
    }
    // players
    data = this->Mod_StUnionActivity_Base_need_bless_player();
    Json::Value players = data[0];
    for (const auto& player : players) {
        //if (count == 0)
        //    break;
        if (player[4].asInt())
            continue;
        data = this->Mod_StUnionActivity_Base_bless_st_union_player(player[0].asInt());
        if (data[0].asInt() != Mod_StUnionActivity_Base::SUCCESS) {
            common::log(boost::str(boost::format("【仙盟之树】祝福 [%1%] 失败，result[%2%]") % common::utf2gbk(player[1].asString()) % data[0]));
            return;
        }
        common::log(boost::str(boost::format("【仙盟之树】祝福 [%1%]") % common::utf2gbk(player[1].asString())));
    }
}

//============================================================================
// R171 仙盟之树
// {module:179, action:0,
// request:[], response:[Utils.ByteUtil, Utils.IntUtil, Utils.ShortUtil, Utils.ShortUtil, Utils.UByteUtil, [Utils.ShortUtil, Utils.ShortUtil, Utils.IntUtil, Utils.IntUtil, Utils.UByteUtil], Utils.UByteUtil]}
// StUnionActivityController.as 26:
//     oObject.list(_loc_2, _loc_1, ["tree_level", "grow_value", "bless_count", "total_bless_count", "is_today_wish", "wish_item_list", "is_get_award"]);
//     for each (_loc_6 in _loc_1["wish_item_list"])
//         oObject.list(_loc_6, _loc_7, ["wish_id", "award_type", "item_id", "item_count", "is_choosed"]);
// Example
// 未加仙盟
//     [ 0, 0, 0, 0, 1, null, 0 ]
// 未选择
//     [ 6, 0, 0, 5, 1, [ [ 35, 3, 0, 2000000, 1 ], [ 38, 8, 0, 20, 1 ], [ 43, 19, 0, 1, 1 ] ], 0 ]
// 已选择(is_choosed[5][0][4])
//     [ 1, 1750, 0, 5, 1, [ [ 1, 3, 0, 1000000, 0 ] ], 0 ]
// 祝福自己(is_today_wish[4])
//     [ 1, 1760, 1, 5, 0, [ [ 1, 3, 0, 1000000, 0 ] ], 0 ]
// 祝福已满(is_get_award[6])
//     [ 1, 1770, 0, 5, 1, [ [ 1, 3, 0, 1000000, 0 ] ], 1 ]
// 今日许愿成功(bless_count[2]==total_bless_count[3])
//     [ 1, 1840, 5, 5, 0, [ [ 2, 5, 0, 200, 1 ], [ 1, 3, 0, 1000000, 1 ], [ 4, 8, 0, 8, 1 ] ], 0 ]
//============================================================================
Json::Value sxd_client::Mod_StUnionActivity_Base_get_st_union_tree_info() {
    Json::Value data;
    return this->send_and_receive(data, 179, 0);
}

//============================================================================
// R171 盟友
// {module:179, action:3,
// request:[], response:[[Utils.IntUtil, Utils.StringUtil, Utils.StringUtil, Utils.StringUtil, Utils.UByteUtil], Utils.ByteUtil]}
// StUnionActivityController.as 107:
//     var _loc_2:* = _data.stUnionActivityData.needBlessPlayerList[0];
//     var _loc_3:* = _data.stUnionActivityData.needBlessPlayerList[1];
//     for each (_loc_5 in _loc_2)
//         oObject.list(_loc_5, _loc_6, ["st_player_id", "nick_name", "stage_name", "server_name", "is_need_bless"]);
//         _loc_6["remain_award_num"] = _loc_3;
// Example
//     [ [ [ 203964, "\u9ad8\u5343\u5b57.s1", "\u5fc3\u52a8", "s04", 1 ],
//         [ 203963, "\u90ce\u68ee\u4e09.s1", "\u5fc3\u52a8", "s04", 1 ],
//         [ 203960, "\u548c\u8b07\u66b4.s1", "\u5fc3\u52a8", "s04", 1 ],
//         [ 203965, "\u8463\u4e58\u516d.s1", "\u5fc3\u52a8", "s04", 1 ],
//         [ 203962, "\u4f0d\u5c9a\u82b7.s1", "\u5fc3\u52a8", "s04", 0 ],
//         [ 203957, "\u5e84\u606d\u6625.s1", "\u5fc3\u52a8", "s04", 0 ] ], 5 ]
//============================================================================
Json::Value sxd_client::Mod_StUnionActivity_Base_need_bless_player() {
    Json::Value data;
    return this->send_and_receive(data, 179, 3);
}
//============================================================================
// R171 许愿选择
// {module:179, action:1,
// request:[Utils.ShortUtil],
// Example
//     [ 4 ]
// response:[Utils.UByteUtil]}
// StUnionActivityData.as 31:
//     this.result = param1[0];
// Example
//     [ 2 ]
//============================================================================
Json::Value sxd_client::Mod_StUnionActivity_Base_choose_wish_item(int id) {
    Json::Value data;
    data.append(id);
    return this->send_and_receive(data, 179, 1);
}

//============================================================================
// R171 祝福自己
// {module:179, action:2,
// request:[], response:[Utils.UByteUtil]}
// Example
//     [ 7 ]
//============================================================================
Json::Value sxd_client::Mod_StUnionActivity_Base_start_bless() {
    Json::Value data;
    return this->send_and_receive(data, 179, 2);
}

//============================================================================
// R171 祝福盟友
// {module:179, action:4,
// request:[Utils.IntUtil],
// Example
//     [ 203962 ]
// response:[Utils.UByteUtil, Utils.IntUtil]}
// StUnionActivityData.as 49:
//     this.result = param1[0];
//     this.blessUnionCoin = param1[1];
// Example
//     [ 2, 100000 ]
//============================================================================
Json::Value sxd_client::Mod_StUnionActivity_Base_bless_st_union_player(int id) {
    Json::Value data;
    data.append(id);
    return this->send_and_receive(data, 179, 4);
}

//============================================================================
// R171 领取
// {module:179, action:7,
// request:[], response:[Utils.UByteUtil, Utils.IntUtil]}
// StUnionActivityData.as 56:
//     this.result = param1[0];
//     this.ticketNum = param1[1];
// Example
//     [ 2, 0 ]
//============================================================================
Json::Value sxd_client::Mod_StUnionActivity_Base_player_get_tree_gift() {
    Json::Value data;
    return this->send_and_receive(data, 179, 7);
}

class Mod_StUnionTask_Base {
public:
    static const int SUCCESS = 0;
    static const int NOT_IN_UNION = 1;
    static const int NOT_ENOUGH_TICKET = 2;
};

void sxd_client::st_union_task() {
    for (;;) {
        this->Mod_StUnionTask_Base_get_challenge_info();
        Json::Value data = this->Mod_StUnionTask_Base_fight();
        if (data[0].asInt() == Mod_StUnionTask_Base::NOT_ENOUGH_TICKET) {
            common::log("【魔神挑战】挑战券不足");
            return;
        } else if (data[0].asInt() == Mod_StUnionTask_Base::NOT_IN_UNION) {
            common::log("【魔神挑战】未加仙盟");
            return;
        } else if (data[0].asInt() != Mod_StUnionTask_Base::SUCCESS) {
            common::log(boost::str(boost::format("【魔神挑战】挑战失败，result[%1%]") % data[0]));
            return;
        } else
            common::log("【魔神挑战】挑战");
    }
}

//============================================================================
// R171
// 魔神挑战
// {module:337, action:0, request:[], response:[Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, [Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil], Utils.IntUtil, Utils.ShortUtil, [Utils.IntUtil, Utils.IntUtil], Utils.IntUtil]}
// StUnionTaskData.as 29:
//     this.info.ticket_num = param1[_loc_2++];
//     this.info.left_buy_times = param1[_loc_2++];
//     _loc_3.boss_monster_id = param1[_loc_2++];
//     _loc_3.special_pattern_id = param1[_loc_2++];
//     _loc_3.monster_team_list = this.parseMonsterTeamList(param1[_loc_2++]);
//     **this.info.challenge_times = param1[_loc_2++];**
//     this.info.task_type = param1[_loc_2++];
//     this.info.award_list = this.parseAwardList(param1[_loc_2++]);
//     this.info.fight_times = param1[_loc_2++];
// Example
// 初始
//     [ 1, 9, 15070, 31, [ [ 15083, 6, 238, 3 ], [ 15078, 155, 173, 5 ], [ 15070, 75, 235, 8 ], [ 15083, 24, 187, 1 ], [ 15070, 75, 235, 2 ], [ 15070, 75, 398, 9 ], [ 15078, 39, 239, 6 ] ],
//       **2**, 2, [ [ 1747, 3200000 ], [ 2665, 56000000 ], [ 2664, 560 ] ], 0 ]
// 挑战
//     [ 0, 9, 15070, 31, [ [ 15083, 6, 238, 3 ], [ 15078, 155, 173, 5 ], [ 15070, 75, 235, 8 ], [ 15083, 24, 187, 1 ], [ 15070, 75, 235, 2 ], [ 15070, 75, 398, 9 ], [ 15078, 39, 239, 6 ] ],
//       **1**, 2, [ [ 1747, 3200000 ], [ 2665, 56000000 ], [ 2664, 560 ] ], 1 ]
// 挑战
//     [ 0, 9, 15073, 20, [ [ 15076, 155, 427, 7 ], [ 15077, 112, 398, 4 ], [ 15073, 75, 341, 5 ], [ 15076, 155, 243, 8 ], [ 15077, 39, 173, 2 ], [ 15073, 75, 189, 9 ], [ 15073, 75, 398, 6 ] ],
//       **2**, 2, [ [ 1747, 3200000 ], [ 2665, 56000000 ], [ 2664, 560 ] ], 0 ]
//============================================================================
Json::Value sxd_client::Mod_StUnionTask_Base_get_challenge_info() {
    Json::Value data;
    return this->send_and_receive(data, 337, 0);
}

//============================================================================
// R171
// 挑战
// {module:337, action:1,
// request:[], response:[Utils.UByteUtil, [Utils.ByteUtil, Utils.IntUtil, [...
// StUnionTaskData.as 98:
//     this.info.challengeResult = param1[_loc_2++];
//     this.info.war_result_list = param1[_loc_2++];
//     this.info.award_list = this.parseAwardList(param1[_loc_2++]);
// Example
//     [ 0, [ [ 3, -15070, [ [...
//     [ 2, null, null ]
//============================================================================
Json::Value sxd_client::Mod_StUnionTask_Base_fight() {
    Json::Value data;
    return this->send_and_receive(data, 337, 1);
}

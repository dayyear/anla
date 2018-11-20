#include <boost/algorithm/string/join.hpp>
#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_StUnionActivity_Base {
public:
    static const int SUCCESS = 2;
    static const int BLESS_SUCCESS = 7;
};

std::string sxd_client::get_st_union_name() {
    Json::Value data = this->Mod_StTown_Base_get_players();
    auto me = std::find_if(data[0].begin(), data[0].end(), [this](const Json::Value& x) {return x[0].asInt()==player_id;});
    return common::utf2gbk((*me)[12].asString());
}

//============================================================================
// R172 仙盟信息
// {module:178, action:4, request:[], response:[Utils.IntUtil, Utils.StringUtil, Utils.ShortUtil, Utils.ShortUtil, Utils.ShortUtil, Utils.IntUtil, Utils.IntUtil, Utils.StringUtil, Utils.StringUtil, [Utils.IntUtil, Utils.StringUtil, Utils.StringUtil, Utils.StringUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.ShortUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.UByteUtil]]};
// StUnionData.as 147:
//     oObject.list(param1, this._myUnionInfo, ["st_union_id", "name", "level", "rank", "member_count", "exp", "cur_week_exp", "qq_group", "notice"]);
//     for each(_loc2_ in param1[param1.length - 1])
//         oObject.list(_loc2_,_loc3_,["st_player_id","nick_name","stage_name","server_name","level","job_id","role_id","war_power","today_contribute","total_contribute","last_login_timestamp","on_line_flag"]);
// Example
//     [ 507, "\u5ea6\u6708\u5982\u5e74", 3, 126, 7, 1159459, 38927, "", "",
//       [ [ 203964, "\u9ad8\u5343\u5b57.s1", "\u5fc3\u52a8", "s04", 172, 3, 104, 593299, 830, 206345, 1529115051, 10 ],
//         [ 203963, "\u90ce\u68ee\u4e09.s1", "\u5fc3\u52a8", "s04", 173, 3, 104, 635242, 915, 204565, 1529116843, 10 ],
//         [ 203966, "\u7504\u6590\u6590.s1", "\u5fc3\u52a8", "s04", 173, 3, 104, 508312, 830, 91335, 1529117229, 9 ],
//         [ 203960, "\u548c\u8b07\u66b4.s1", "\u5fc3\u52a8", "s04", 172, 3, 103, 526117, 835, 199750, 1529116868, 10 ],
//         [ 203962, "\u4f0d\u4e8c\u4e00.s1", "\u5fc3\u52a8", "s04", 174, 3, 6, 669873, 850, 210280, 1529116832, 10 ],
// *盟主*  [ 203957, "\u5e84\u606d\u6625.s1", "\u5fc3\u52a8", "s04", 183, **1**, 5, 920041, 890, 279560, 1529116821, 10 ],
// *副盟主*[ 24373, "\u5ea6\u65e5\u5982\u5e74.s1", "\u5fc3\u52a8", "s04", 256, **2**, 5, 7876263, 1512, 90989, 1529116909, 10 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_StUnion_Base_get_player_st_union_info() {
    Json::Value data;
    return this->send_and_receive(data, 178, 4);
}

//============================================================================
// R172 仙盟上香
//============================================================================
void sxd_client::st_union_god_incense() {
    Json::Value data = this->Mod_StUnionActivity_Base_st_union_god_incense(1);
    if (data[0].asInt() == Mod_StUnionActivity_Base::SUCCESS)
        common::log("【仙盟上香】给仙盟神像上 [白檀香]", iEdit);
}

//============================================================================
// R172 仙盟上香
// {module:179, action:11, request:[Utils.IntUtil],
//     {1:["白檀香", 0, 5, 10, 10, 0, 100], 2:["苏合香", 30, 50, 100, 30, 1, 0], 3:["天木香", 90, 1000, 1000, 100, 2, 0]};
// response:[Utils.UByteUtil, Utils.ShortUtil, Utils.ShortUtil, Utils.IntUtil]};
//============================================================================
Json::Value sxd_client::Mod_StUnionActivity_Base_st_union_god_incense(int id) {
    Json::Value data;
    data.append(id);
    return this->send_and_receive(data, 179, 11);
}

//============================================================================
// R171 仙盟之树
//============================================================================
void sxd_client::st_union_activity() {

    // tree
    Json::Value data = this->Mod_StUnionActivity_Base_get_st_union_tree_info();
    Json::Value tree_info = data;
    if (tree_info[0].asInt() == 0) {
        common::log("【仙盟之树】未加仙盟", 0);
        return;
    }
    if (tree_info[6].asInt()) {
        data = this->Mod_StUnionActivity_Base_player_get_tree_gift();
        if (data[0].asInt() != Mod_StUnionActivity_Base::SUCCESS) {
            common::log(boost::str(boost::format("【仙盟之树】领取失败，result[%1%]") % data[0]), iEdit);
            return;
        }
        common::log(boost::str(boost::format("【仙盟之树】领取 [%1%×%2%]") % db.get_code(version, "Gift", tree_info[5][0][1].asInt())["text"] % tree_info[5][0][3]), iEdit);
    } else if (tree_info[2].asInt() == tree_info[3].asInt()) {
        common::log("【仙盟之树】今日许愿成功", 0);
    } else if (tree_info[5].size() == 3) {
        int index = rand() % 3;
        data = this->Mod_StUnionActivity_Base_choose_wish_item(tree_info[5][index][0].asInt());
        if (data[0].asInt() != Mod_StUnionActivity_Base::SUCCESS) {
            common::log(boost::str(boost::format("【仙盟之树】许愿选择失败，result[%1%]") % data[0]), iEdit);
            return;
        }
        common::log(boost::str(boost::format("【仙盟之树】许愿选择第 [%1%] 个物品 [%2%]") % (index + 1) % db.get_code(version, "Gift", tree_info[5][index][1].asInt())["text"]), iEdit);
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
            common::log(boost::str(boost::format("【仙盟之树】祝福 [%1%] 失败，result[%2%]") % common::utf2gbk(player[1].asString()) % data[0]), iEdit);
            return;
        }
        common::log(boost::str(boost::format("【仙盟之树】祝福 [%1%]") % common::utf2gbk(player[1].asString())), iEdit);
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
// StUnionType.as:
//     public static const StUnionJobs:Array = [[1,"MengZhu","盟主"],[2,"FumengZhu","副盟主"],[3,"ChengYuan","成员"]];
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
            common::log("【魔神挑战】挑战券不足", 0);
            return;
        } else if (data[0].asInt() == Mod_StUnionTask_Base::NOT_IN_UNION) {
            common::log("【魔神挑战】未加仙盟", 0);
            return;
        } else if (data[0].asInt() != Mod_StUnionTask_Base::SUCCESS) {
            common::log(boost::str(boost::format("【魔神挑战】挑战失败，result[%1%]") % data[0]), iEdit);
            return;
        } else
            common::log("【魔神挑战】挑战", iEdit);
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
// R171 挑战
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

//============================================================================
// 所有仙盟成员
// ["st_player_id","nick_name","stage_name","server_name","level","job_id","role_id","power","on_line_flag"]
//============================================================================
std::vector<Json::Value> sxd_client::get_all_st_union_members() {
    std::vector<Json::Value> all_members;

    //boost::posix_time::ptime now(boost::posix_time::second_clock::local_time());
    std::time_t now = std::time(0);
    std::string file_name = boost::str(boost::format("log\\%1%-%2%.log") % common::to_string(now, "%Y-%m-%d") % this->player_id);

    auto str = common::read_file(file_name);
    if (str.size()) {
        // read all members to file
        Json::Value members;
        std::istringstream(str) >> members;
        for (const auto& member : members)
            all_members.push_back(member);
    } else {
        for (int page = 1;; page++) {
            auto data = Mod_StUnionTask_Base_get_st_union_list(page);
            auto st_unions = data[0];
            int total_page = data[1].asInt();

            for (const auto& st_union : st_unions) {
                int st_union_id = st_union[0].asInt();
                data = this->Mod_StUnionTask_Base_get_st_union_info_simp(st_union_id);
                auto members = data[8];
                for (const auto& member : members)
                    all_members.push_back(member);
            }
            if (page == total_page)
                break;
        }
        // save all members to file
        std::vector<std::string> strs;
        std::transform(all_members.begin(), all_members.end(), std::back_inserter(strs), [this](const Json::Value& x) {
            return boost::str(boost::format("%1%") % x);
        });
        common::write_file(file_name, boost::str(boost::format("[%1%]") % boost::algorithm::join(strs, ", ")));
    }

    return all_members;
}

//============================================================================
// R179 其他仙盟
// "module":178,"action":0,"request":[Utils.StringUtil,Utils.StringUtil,Utils.ShortUtil],"response":[[Utils.IntUtil,Utils.StringUtil,Utils.StringUtil,Utils.ShortUtil,Utils.ShortUtil,Utils.ShortUtil,Utils.IntUtil],Utils.ShortUtil,Utils.UByteUtil]
// StUnionData.as:
//     for each(_loc3_ in param1[0])
//         oObject.list(_loc3_,_loc4_,["st_union_id","name","leader","level","member_count","rank","request_id"]);
//     this._allUnion.total_page = param1[1];
//     this._allUnion.st_union_flag = param1[2];
// Example
//     [ [ [ 18, "\u795e\u4ed9\u9053", "\u98ce\u503e\u5f71\u98d8.s1", 10, 34, 1, 0 ], [ 11, "\u65e0\u9650\u5929\u5730", "\u4efb\u5349\u96c1.s4", 10, 34, 2, 0 ], [ 166, "\u5343\u5c71\u66ae\u96ea", "\u67ef\u6ce2\u5ada.s5", 9, 32, 3, 0 ], [ 22, "\u6b22\u559c\u5c31\u597d", "\u6c5f\u4f5f\u7b71.s6", 9, 33, 4, 0 ], [ 157, "\u718a\u718a\u4e50\u56ed", "\u6797 \u72ec\u6108.s5", 9, 33, 5, 0 ], [ 101, "\u4e3f\u4e36\u8f89\u714c\u5bb6\u56ed", "\u4eba\u7687.s3", 9, 33, 6, 0 ], [ 159, "\u501a\u6a13\u3001\u542c\u98a8", "\u795e\u753b\u2531.s1", 9, 31, 7, 0 ], [ 200, "\u5f00\u5f00\u5fc3\u5fc3", "\u4e07\u5609\u5764.s5", 9, 33, 8, 0 ], [ 23, "\u6df7\u70b9", "\u6c5f\u575a\u6fef.s5", 8, 32, 9, 0 ], [ 29, "\u4e00\u8d77\u73a9", "\u2523\u7687\u723a\u252b.s9", 8, 13, 10, 0 ] ], 46, 0 ]
//============================================================================
Json::Value sxd_client::Mod_StUnionTask_Base_get_st_union_list(int page) {
    Json::Value data;
    data.append("");
    data.append("");
    data.append(page);
    return this->send_and_receive(data, 178, 0);
}

//============================================================================
// R179 查看仙盟
// "module":178,"action":9,"request":[Utils.IntUtil],"response":[Utils.IntUtil,Utils.StringUtil,Utils.StringUtil,Utils.ShortUtil,Utils.IntUtil,Utils.ShortUtil,Utils.ShortUtil,Utils.StringUtil,[Utils.IntUtil,Utils.StringUtil,Utils.StringUtil,Utils.StringUtil,Utils.ShortUtil,Utils.ByteUtil,Utils.ShortUtil,Utils.IntUtil,Utils.UByteUtil]]
// StUnionData.as:
//     oObject.list(param1,this.stUnionBasicData,["st_union_id","faction_name","faction_leader","faction_level","faction_coins","faction_now_count","faction_rank","faction_desc","members"]);
//     for each(_loc2_ in this.stUnionBasicData["members"])
//         oObject.list(_loc2_,_loc3_,["st_player_id","nick_name","stage_name","server_name","level","job_id","role_id","power","on_line_flag"]);
// Example
//     [ 18 ] --> [ 18, "\u795e\u4ed9\u9053", "\u98ce\u503e\u5f71\u98d8.s1", 10, 27721899, 34, 1, "\u503e\u98ce\u98d8\u5f71\u7684\u5bb6\uff01\uff01\uff01", [ [ 6121, "\u98ce\u503e\u5f71\u98d8.s1", "\u5fc3\u52a8", "s04", 256, 1, 1, 1161133, 9 ], [ 6142, "\u98ce\u98d8\u98d8\u98d8.s1", "\u5fc3\u52a8", "s04", 256, 3, 105, 1133876, 9 ], [ 6145, "\u503e\u503e\u503e\u503e.s1", "\u5fc3\u52a8", "s04", 256, 3, 2, 1071601, 9 ], [ 205607, "\u503e\u503e\u5f71\u503e.s1", "\u5fc3\u52a8", "s04", 164, 3, 1, 689309, 9 ], [ 6147, "\u98ce\u98ce\u98ce\u5f71.s1", "\u5fc3\u52a8", "s04", 256, 3, 108, 1149625, 9 ], [ 6148, "\u503e\u98ce\u98ce\u5f71.s1", "\u5fc3\u52a8", "s04", 256, 3, 2, 1205003, 9 ], [ 6141, "\u503e\u503e\u98ce\u5f71.s1", "\u5fc3\u52a8", "s04", 256, 3, 108, 1202868, 9 ], [ 15375, "\u503e\u98d8\u98d8\u5f71.s1", "\u5fc3\u52a8", "s04", 252, 3, 2, 1018172, 9 ], [ 6143, "\u98ce\u98ce\u98ce\u98ce.s1", "\u5fc3\u52a8", "s04", 256, 3, 103, 1132243, 9 ], [ 6122, "\u503e\u5f71\u98ce\u98d8.s1", "\u5fc3\u52a8", "s04", 256, 3, 108, 1184832, 9 ], [ 204594, "\u503e\u503e\u5f71\u98d8.s1", "\u5fc3\u52a8", "s04", 184, 3, 1, 830712, 9 ], [ 6137, "\u98ce\u98ce\u5f71\u503e.s1", "\u5fc3\u52a8", "s04", 256, 3, 2, 1153612, 9 ], [ 204595, "\u503e\u503e\u5f71\u5f71.s1", "\u5fc3\u52a8", "s04", 184, 3, 2, 811326, 9 ], [ 205608, "\u503e\u98ce\u98d8\u503e.s1", "\u5fc3\u52a8", "s04", 163, 3, 2, 690499, 9 ], [ 6144, "\u503e\u98ce\u98d8\u98d8.s1", "\u5fc3\u52a8", "s04", 256, 3, 2, 1096645, 9 ], [ 6115, "\u503e\u98ce\u98d8\u5f71.s1", "\u5fc3\u52a8", "s04", 256, 2, 103, 1209099, 9 ], [ 6133, "\u98ce\u5f71\u503e\u503e.s1", "\u5fc3\u52a8", "s04", 256, 3, 2, 1190129, 9 ], [ 134, "\u70c8\u7130\u6d1b\u795e.s1", "\u5fc3\u52a8", "s04", 256, 2, 1, 4825680, 9 ], [ 6130, "\u98ce\u5f71\u98d8\u503e.s1", "\u5fc3\u52a8", "s04", 256, 3, 1, 1159801, 9 ], [ 6138, "\u98ce\u98ce\u503e\u98d8.s1", "\u5fc3\u52a8", "s04", 256, 3, 1, 1195571, 9 ], [ 6136, "\u98ce\u98ce\u5f71\u5f71.s1", "\u5fc3\u52a8", "s04", 256, 3, 105, 1195554, 9 ], [ 6135, "\u503e\u503e\u98d8\u98d8.s1", "\u5fc3\u52a8", "s04", 256, 3, 2, 1195801, 9 ], [ 6127, "\u98d8\u5f71\u98ce\u503e.s1", "\u5fc3\u52a8", "s04", 256, 3, 108, 1132724, 10 ], [ 6126, "\u5f71\u503e\u98ce\u98d8.s1", "\u5fc3\u52a8", "s04", 256, 3, 108, 1151045, 9 ], [ 6119, "\u503e\u98ce\u5f71\u98d8.s1", "\u5fc3\u52a8", "s04", 256, 3, 105, 1158671, 9 ], [ 6128, "\u98ce\u98d8\u5f71\u503e.s1", "\u5fc3\u52a8", "s04", 256, 3, 2, 1139396, 9 ], [ 6140, "\u98ce\u98ce\u503e\u503e.s1", "\u5fc3\u52a8", "s04", 256, 3, 103, 1179361, 9 ], [ 6124, "\u503e\u5f71\u98d8\u98ce.s1", "\u5fc3\u52a8", "s04", 256, 3, 108, 1137935, 9 ], [ 6117, "\u98d8\u5f71\u503e\u98ce.s1", "\u5fc3\u52a8", "s04", 256, 3, 1, 1154706, 9 ], [ 6134, "\u503e\u98ce\u5f71\u5f71.s1", "\u5fc3\u52a8", "s04", 256, 3, 110, 1146884, 9 ], [ 6129, "\u98ce\u98d8\u503e\u5f71.s1", "\u5fc3\u52a8", "s04", 256, 3, 103, 1182849, 9 ], [ 15387, "\u5f71\u5f71\u5f71\u5f71.s1", "\u5fc3\u52a8", "s04", 251, 3, 2, 1039309, 9 ], [ 6139, "\u98ce\u98ce\u503e\u5f71.s1", "\u5fc3\u52a8", "s04", 256, 3, 108, 1185647, 9 ], [ 6131, "\u503e\u5f71\u98d8\u98d8.s1", "\u5fc3\u52a8", "s04", 256, 3, 105, 1135544, 9 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_StUnionTask_Base_get_st_union_info_simp(int st_union_id) {
    Json::Value data;
    data.append(st_union_id);
    return this->send_and_receive(data, 178, 9);
}

class Mod_StUnion_Base {
public:
    static const int SUCCESS = 2;
    static const int ACCEPT = 11;
};

//============================================================================
// 仙盟审核
//============================================================================
void sxd_client::st_union_approve() {
    auto data = this->Mod_StUnion_Base_get_player_st_union_info();
    auto my_st_union_info = *std::find_if(data[9].begin(), data[9].end(), [this](const Json::Value& x) {return x[0].asInt() == player_id;});
    if (my_st_union_info[5].asInt() == 3)
        return;
    data = this->Mod_StUnion_Base_get_request_join_info();
    auto request_list = data[0];
    for (const auto& request : request_list) {
        int id = request[0].asInt();
        data = this->Mod_StUnion_Base_deal_join_request(id);
        if (data[0].asInt() == Mod_StUnion_Base::SUCCESS)
            common::log(boost::str(boost::format("【仙盟】审核通过 [%1%(%2%级)] 加入") % common::utf2gbk(request[1].asString()) % request[4]), iEdit);
    }
}

//============================================================================
// R180 审核信息
// "module":178,"action":6,"request":[],"response":[[Utils.IntUtil,Utils.StringUtil,Utils.StringUtil,Utils.StringUtil,Utils.ShortUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil]]
// StUnionData.as:
//     for each(_loc2_ in param1[0])
//         oObject.list(_loc2_,_loc3_,["request_id","nick_name","stage_name","server_name","level","war_power","timestamp","role_id"]);
// Example
//     [ [ [ 38287, "\u3053\u3093\u306b\u3061\u306f.s3", "7k7k", "yx567_s0125", 141, 993428, 1531385171, 105 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_StUnion_Base_get_request_join_info() {
    Json::Value data;
    return this->send_and_receive(data, 178, 6);
}

//============================================================================
// R180 审核
// "module":178,"action":7,"request":[Utils.IntUtil,Utils.UByteUtil],"response":[Utils.UByteUtil]
// StUnionData.as:
//     this._dealJoinResult = param1[0];
// Example
//     [ 38287, 11 ] --> [ 2 ]
//============================================================================
Json::Value sxd_client::Mod_StUnion_Base_deal_join_request(int id) {
    Json::Value data;
    data.append(id);
    data.append(Mod_StUnion_Base::ACCEPT);
    return this->send_and_receive(data, 178, 7);
}

class Mod_StUnionAnimal_Base {
public:
    static const int CALL = 0;
    static const int FIGHT = 1;
    static const int KILLED = 2;
    static const int END = 3;
    static const int SUCCESS = 4;
    static const int COIN = 10;
    static const int INGOT = 11;
};

//============================================================================
// 仙盟神兽
//============================================================================
void sxd_client::st_union_nimal() {
    auto data = this->Mod_StUnionAnimal_Base_get_animal_info();
    int status = data[0].asInt();
    int coin_attack_times = data[6].asInt();
    int coin_attack_max_times = data[7].asInt();
    if (status == Mod_StUnionAnimal_Base::FIGHT) {
        while (coin_attack_times < coin_attack_max_times) {
            data = this->Mod_StUnionAnimal_Base_fight(Mod_StUnionAnimal_Base::COIN);
            if (data[0].asInt() != Mod_StUnionAnimal_Base::SUCCESS) {
                common::log(boost::str(boost::format("【仙盟神兽】普通攻击失败，result[%1%]") % data[0]), iEdit);
                return;
            }
            common::log(boost::str(boost::format("【仙盟神兽】普通攻击，对神兽造成 [伤害×%1%]") % data[2]), iEdit);
            coin_attack_times++;
        }
    }
}

//============================================================================
// R181 面板
// "module":386,"action":0,"request":[],"response":[Utils.UByteUtil,[Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil],Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil]
// StUnionAnimalData.as:
//     oObject.list(param1,this.animalInfo,["status","call_list","voted_id","call_deadline_timestamp","called_id","coin_attack_cost","coin_attack_times","coin_attack_max_times","ingot_attack_cost","ingot_attack_times","ingot_attack_max_times"]);
// Example
//     [ 1, [ [ 1, 1, 0, 0, 1809 ], [ 2, 1, 0, 0, 5000 ], [ 3, 1, 0, 0, 5000 ] ], 0, 0, 1, 1000000, 0, 5, 30, 0, 10 ]
//     [ 1, [ [ 1, 1, 0, 0, 1728 ], [ 2, 1, 0, 0, 5000 ], [ 3, 1, 0, 0, 5000 ] ], 0, 0, 1, 1000000, 1, 5, 30, 0, 10 ]
//============================================================================
Json::Value sxd_client::Mod_StUnionAnimal_Base_get_animal_info() {
    Json::Value data;
    return this->send_and_receive(data, 386, 0);
}

//============================================================================
// R181 攻击
// "module":386,"action":3,"request":[Utils.UByteUtil],"response":[Utils.UByteUtil,[Utils.IntUtil,Utils.IntUtil],Utils.IntUtil,Utils.IntUtil]
// StUnionAnimalData.as:
//     oObject.list(param1,this.fightResult,["result","award_list","hurt","left_health"]);
// Example
//     [ 10 ] --> [ 4, [ [ 1743, 7 ] ], 61, 1667 ]
//============================================================================
Json::Value sxd_client::Mod_StUnionAnimal_Base_fight(int type) {
    Json::Value data;
    data.append(type);
    return this->send_and_receive(data, 386, 3);
}


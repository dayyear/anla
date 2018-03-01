#include "sxd_client.h"

//============================================================================
// R170 仙界登录
// {module:94, action:0,
// request:[Utils.StringUtil, Utils.IntUtil, Utils.StringUtil, Utils.IntUtil, Utils.StringUtil],
// View.as 7249
//     _data.call(Mod_StLogin_Base.login, chatConnectLoginBack, [obj.serverName, _ctrl.player.playerInfo.id, _ctrl.player.originNickName, obj.time, obj.passCode], true, 1);
// response:[Utils.UByteUtil, Utils.IntUtil, Utils.IntUtil]}
// StLoginData.as 15:
//     public function login(param1:Array) : void
//     {
//         this.result = param1[0];
//         this.playerId = param1[1];
//         this._data.stFlyingChessActivity.info.myStPlayerId = this.playerId;
//         var _loc_2:* = new Date();
//         var _loc_3:* = _loc_2.getTime() / 1000;
//         if (_loc_2.timezoneOffset != DateTime.timeArea * 60)
//         {
//             _loc_3 = _loc_3 - (_loc_2.timezoneOffset - DateTime.timeArea * 60) * 60;
//         }
//         _data.player.diffWorldServerTime = _loc_3 - param1[2];
//         return;
//     }// end function
//============================================================================
Json::Value sxd_client::Mod_StLogin_Base_login(const std::string& server_name, int player_id, const std::string& nickname, int time, const std::string& pass_code) {
    Json::Value data;
    data.append(server_name);
    data.append(player_id);
    data.append(nickname);
    data.append(time);
    data.append(pass_code);
    return this->send_and_receive(data, 94, 0);
}

//============================================================================
// R170 进入仙界
// - Mod_StTown_Base.enter_town
// {module:95, action:0,
// request:[Utils.IntUtil],
// TownTypeData.as 277:
//     41:["XianJie","仙界",{
// response:[Utils.UByteUtil]}
// - Mod_StTown_Base.notify_enter_town
// {module:95, action:1,
// request:[],
// response:[Utils.IntUtil, Utils.IntUtil, Utils.StringUtil, Utils.ShortUtil, Utils.ShortUtil, Utils.IntUtil, Utils.StringUtil, Utils.StringUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.StringUtil, Utils.UByteUtil, Utils.UByteUtil, Utils.ShortUtil, Utils.IntUtil, Utils.StringUtil, Utils.IntUtil, Utils.ShortUtil, [Utils.UByteUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ShortUtil]]}
// StTownData.as 49:
//     oObject.list(param1, _loc_2, ["player_id", "role_id", "nickname", "position_x", "position_y", "equip_item_id", "stage_name", "server_name", "is_world_war_top", "is_star", "transport", "avatar", "st_union_name", "immortal_flag", "saint_flag", "mount_rune_type_id", "children_role_id", "children_nickname", "children_suit_id", "orange_equipment_fllow_id", "follow_pet_list"]);
//============================================================================
Json::Value sxd_client::Mod_StTown_Base_enter_town(int player_id) {
    Json::Value data;
    data.append(41);
    return this->send_and_receive(data, 95, 0);
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

//============================================================================
// R171 许愿池
// {module:359, action:0,
// request:[], response:[[Utils.IntUtil, Utils.UByteUtil], Utils.ShortUtil, Utils.ByteUtil, Utils.IntUtil, Utils.UByteUtil, Utils.UByteUtil, Utils.ByteUtil]}
// WishPoolData.as 32:
//     oObject.list(param1, this._mineObject, ["wish_pool_award_list", "wish_value", "speedup_times", "speedup_coins", "gotten_flag", "wish_self_flag", "avaliable_wish_other_award_times"]);
//     for each (_loc_4 in this._mineObject.wish_pool_award_list)
//         oObject.list(_loc_4, _loc_3, ["id", "flag"]);
// Example
// 初始
//     [ [ [ 1, 1 ], [ 27, 1 ], [ 8, 1 ], [ 21, 1 ], [ 32, 1 ], [ 31, 1 ], [ 19, 1 ], [ 13, 1 ] ], 0, 3, 500000, 1, 1, 3 ]
// 许愿
//     [ [ [ 1, 1 ], [ 27, 0 ], [ 8, 1 ], [ 21, 0 ], [ 32, 1 ], [ 31, 1 ], [ 19, 0 ], [ 13, 1 ] ], 0, 3, 500000, 1, 1, 3 ]
// 祝福
//     [ [ [ 1, 1 ], [ 27, 0 ], [ 8, 1 ], [ 21, 0 ], [ 32, 1 ], [ 31, 1 ], [ 19, 0 ], [ 13, 1 ] ], 50, 3, 500000, 1, 0, 3 ]
// 祝福满
//     [ [ [ 1, 1 ], [ 26, 1 ], [ 9, 0 ], [ 13, 1 ], [ 14, 0 ], [ 31, 1 ], [ 3, 1 ], [ 19, 0 ] ], 510, 3, 500000, 1, 1, 0 ]
// 领取奖励
//     [ [ [ 33, 1 ], [ 20, 1 ], [ 9, 1 ], [ 5, 1 ], [ 4, 1 ], [ 31, 0 ], [ 3, 0 ], [ 35, 0 ] ], 510, 3, 500000, 0, 1, 0 ]
//============================================================================
Json::Value sxd_client::Mod_WishPool_Base_get_wish_pool_info() {
    Json::Value data;
    return this->send_and_receive(data, 359, 0);
}

//============================================================================
// R171 许愿
// {module:359, action:2,
// request:[[Utils.IntUtil]],
// Example
//     [ [ [ 27 ], [ 21 ], [ 19 ] ] ]
// response:[Utils.UByteUtil]}
// Example
//     [ 2 ]
//============================================================================
Json::Value sxd_client::Mod_WishPool_Base_choose_awards(int ids[], int count) {
    Json::Value data, data1;
    for(int i=0; i<count; i++) {
        Json::Value data2;
        data2.append(ids[i]);
        data1.append(data2);
    }
    data.append(data1);
    return this->send_and_receive(data, 359, 2);
}

//============================================================================
// R171 祝福自己
// {module:359, action:3, request:[], response:[Utils.UByteUtil]}
// Example
//     [ 2 ]
//============================================================================
Json::Value sxd_client::Mod_WishPool_Base_wish_self(){
    Json::Value data;
    return this->send_and_receive(data, 359, 3);
}

//============================================================================
// R171 其他许愿池
// {module:359, action:1,
// request:[], response:[[Utils.IntUtil, Utils.StringUtil, Utils.StringUtil, Utils.StringUtil, [Utils.IntUtil, Utils.UByteUtil], Utils.ShortUtil]]}
// WishPoolData.as 79:
//     for each (_loc_2 in param1[0])
//         oObject.list(_loc_2, _loc_3, ["st_player_id", "nickname", "server_name", "stage_name", "wish_pool_award_list", "wish_value"]);
//         for each (_loc_5 in _loc_3.wish_pool_award_list)
//             oObject.list(_loc_5, _loc_4, ["id", "flag"]);
// Example
// wish_value>=500 为满祝福
//     [ [ [ 200002, "\u9ed1\u591c\u7ff1\u7fd4.s1", "s04", "\u5fc3\u52a8", [ [ 1, 0 ], [ 29, 0 ], [ 34, 1 ], [ 20, 1 ], [ 19, 1 ], [ 17, 1 ], [ 16, 0 ], [ 33, 1 ] ], 500 ], [ 7703, "\u9a6c\u62c9\u6208\u58c1.s2", "yx567_s0125", "7k7k", [ [ 19, 0 ], [ 23, 1 ], [ 9, 1 ], [ 25, 0 ], [ 30, 0 ], [ 1, 1 ], [ 26, 1 ], [ 18, 1 ] ], 520 ], [ 12904, "\u4e3f\u7d14\u4e36\u6c3a\u6c3a.s1", "s04", "\u5fc3\u52a8", [ [ 15, 1 ], [ 18, 0 ], [ 8, 0 ], [ 31, 1 ], [ 20, 1 ], [ 35, 0 ], [ 26, 1 ], [ 29, 1 ] ], 500 ], [ 204904, "\u5f90\u63d0\u83ab.s4", "s04", "\u5fc3\u52a8", [ [ 15, 1 ], [ 23, 1 ], [ 26, 1 ], [ 1, 0 ], [ 18, 0 ], [ 19, 1 ], [ 31, 1 ], [ 16, 0 ] ], 500 ], [ 705, "\u771f\u5b9e.s1", "s04", "\u5fc3\u52a8", [ [ 30, 0 ], [ 14, 0 ], [ 25, 1 ], [ 8, 1 ], [ 16, 0 ], [ 29, 1 ], [ 20, 1 ], [ 13, 1 ] ], 500 ], [ 206708, "\u738b\u4e18\u95ea.s1", "37wan_s0273", "37wan", [ [ 6, 1 ], [ 8, 0 ], [ 22, 0 ], [ 1, 0 ], [ 25, 1 ], [ 19, 1 ], [ 7, 1 ], [ 3, 1 ] ], 500 ], [ 4709, "\u5fc3\u75db\u4e3a\u8c01.s4", "s04", "\u5fc3\u52a8", [ [ 25, 0 ], [ 31, 1 ], [ 3, 1 ], [ 29, 1 ], [ 18, 0 ], [ 22, 0 ], [ 6, 1 ], [ 9, 1 ] ], 500 ], [ 22112, "\u55b5\u5566\u5566.s4", "yx567_s0125", "7k7k", [ [ 17, 0 ], [ 21, 1 ], [ 35, 1 ], [ 18, 1 ], [ 27, 1 ], [ 24, 0 ], [ 22, 0 ], [ 34, 1 ] ], 500 ], [ 1117, "\u2570\u2606\u256e\u8ca1\u795e\u723a.s12", "yaowan_s0152", "\u8981\u73a9", [ [ 11, 1 ], [ 9, 1 ], [ 18, 0 ], [ 33, 1 ], [ 32, 1 ], [ 21, 0 ], [ 20, 0 ], [ 26, 1 ] ], 520 ], [ 200917, "\u590f\u95ea\u817e.s1", "s04", "\u5fc3\u52a8", [ [ 9, 0 ], [ 27, 0 ], [ 15, 1 ], [ 5, 1 ], [ 31, 1 ], [ 17, 1 ], [ 30, 0 ], [ 33, 1 ] ], 500 ], [ 13219, "\u4e0a\u5bab\u84dd\u51cc.s8", "yx567_s0125", "7k7k", [ [ 8, 1 ], [ 27, 0 ], [ 17, 1 ], [ 16, 1 ], [ 14, 0 ], [ 5, 1 ], [ 20, 0 ], [ 28, 1 ] ], 510 ], [ 207021, "\u6881\u66e6\u6708.s1", "s04", "\u5fc3\u52a8", [ [ 32, 1 ], [ 20, 1 ], [ 9, 0 ], [ 17, 1 ], [ 16, 0 ], [ 35, 0 ], [ 18, 1 ], [ 13, 1 ] ], 520 ], [ 322, "\u4eba\u7687.s3", "37wan_s0273", "37wan", [ [ 28, 1 ], [ 7, 0 ], [ 21, 0 ], [ 31, 1 ], [ 4, 1 ], [ 14, 0 ], [ 15, 1 ], [ 32, 1 ] ], 540 ], [ 626, "\u8840\u8759\u8760.s1", "s04", "\u5fc3\u52a8", [ [ 17, 0 ], [ 35, 0 ], [ 1, 1 ], [ 2, 1 ], [ 18, 0 ], [ 31, 1 ], [ 7, 1 ], [ 5, 1 ] ], 500 ], [ 55627, "\u4e00\u5ff5\u5343\u6b87.s1", "s04", "\u5fc3\u52a8", [ [ 16, 1 ], [ 32, 1 ], [ 31, 0 ], [ 19, 1 ], [ 1, 0 ], [ 17, 1 ], [ 14, 0 ], [ 33, 1 ] ], 500 ], [ 1227, "\u82b1\u513f\u7efd\u653e.s9", "yaowan_s0152", "\u8981\u73a9", [ [ 35, 0 ], [ 7, 1 ], [ 13, 0 ], [ 17, 1 ], [ 10, 1 ], [ 28, 1 ], [ 14, 0 ], [ 12, 1 ] ], 500 ], [ 14032, "Star\u4e36z.s4", "s04", "\u5fc3\u52a8", [ [ 19, 1 ], [ 9, 0 ], [ 16, 0 ], [ 7, 1 ], [ 21, 1 ], [ 1, 1 ], [ 34, 0 ], [ 25, 1 ] ], 520 ], [ 14337, "\u9020\u5316\u949f\u795e\u79c0.s3", "37wan_s0273", "37wan", [ [ 3, 1 ], [ 28, 1 ], [ 16, 0 ], [ 18, 0 ], [ 27, 0 ], [ 26, 1 ], [ 32, 1 ], [ 6, 1 ] ], 520 ], [ 8139, "\u5c0f\u841d\u535c.s2", "yx567_s0125", "7k7k", [ [ 27, 0 ], [ 17, 0 ], [ 12, 1 ], [ 28, 0 ], [ 32, 1 ], [ 8, 1 ], [ 31, 1 ], [ 5, 1 ] ], 520 ], [ 9739, "\u6768\u677f\u6028.s2", "yx567_s0125", "7k7k", [ [ 32, 1 ], [ 35, 1 ], [ 18, 1 ], [ 9, 0 ], [ 19, 1 ], [ 34, 0 ], [ 22, 0 ], [ 16, 1 ] ], 140 ], [ 241, "\u82b1\u513f\u7efd\u653e.s11", "yaowan_s0152", "\u8981\u73a9", [ [ 10, 1 ], [ 30, 0 ], [ 4, 1 ], [ 5, 0 ], [ 25, 1 ], [ 32, 1 ], [ 19, 1 ], [ 22, 0 ] ], 500 ], [ 196141, "\u5c9a\u4e91.s6", "yx567_s0125", "7k7k", [ [ 2, 0 ], [ 13, 1 ], [ 16, 0 ], [ 14, 0 ], [ 32, 1 ], [ 26, 1 ], [ 9, 1 ], [ 23, 1 ] ], 520 ], [ 3142, "\u7d2b\u51dd.s1", "s04", "\u5fc3\u52a8", [ [ 10, 1 ], [ 12, 0 ], [ 21, 0 ], [ 25, 1 ], [ 1, 1 ], [ 32, 1 ], [ 24, 1 ], [ 13, 0 ] ], 500 ], [ 206947, "\u6c89\u9999.s5", "s03", "\u5fc3\u52a8", [ [ 33, 1 ], [ 17, 1 ], [ 2, 1 ], [ 22, 0 ], [ 1, 1 ], [ 29, 0 ], [ 16, 0 ], [ 20, 1 ] ], 500 ], [ 11447, "\u6d41\u5929\u7c7b\u661f.s8", "s05", "\u5fc3\u52a8", [ [ 20, 1 ], [ 29, 1 ], [ 3, 1 ], [ 2, 0 ], [ 34, 0 ], [ 27, 1 ], [ 24, 0 ], [ 22, 1 ] ], 500 ], [ 2248, "\u6709\u540d.s7", "37wan_s0273", "37wan", [ [ 31, 1 ], [ 13, 1 ], [ 4, 1 ], [ 35, 1 ], [ 21, 0 ], [ 22, 0 ], [ 30, 0 ], [ 33, 1 ] ], 500 ], [ 5851, "\u67ef\u82e5\u5f64.s15", "95k_s081", "95k", [ [ 28, 1 ], [ 33, 1 ], [ 21, 0 ], [ 22, 1 ], [ 3, 1 ], [ 24, 0 ], [ 17, 0 ], [ 15, 1 ] ], 520 ], [ 203957, "\u5e84\u606d\u6625.s1", "s04", "\u5fc3\u52a8", [ [ 1, 1 ], [ 27, 0 ], [ 8, 1 ], [ 21, 0 ], [ 32, 1 ], [ 31, 1 ], [ 19, 0 ], [ 13, 1 ] ], 80 ], [ 205760, "\u5f02\u754c \u6b8b\u5f71.s6", "s03", "\u5fc3\u52a8", [ [ 35, 0 ], [ 9, 0 ], [ 18, 1 ], [ 29, 1 ], [ 17, 1 ], [ 16, 1 ], [ 19, 0 ], [ 2, 1 ] ], 500 ], [ 207060, "\u53ef\u80fd\u8fd8\u662f\u8c37\u66e6.s1", "s04", "\u5fc3\u52a8", [ [ 16, 1 ], [ 35, 1 ], [ 25, 1 ], [ 13, 0 ], [ 14, 0 ], [ 32, 1 ], [ 29, 1 ], [ 27, 0 ] ], 510 ], [ 7861, "\u5929\u5929\u8bf4\u518d\u89c1.s5", "yx567_s0125", "7k7k", [ [ 9, 0 ], [ 18, 1 ], [ 14, 1 ], [ 2, 0 ], [ 6, 1 ], [ 30, 0 ], [ 16, 1 ], [ 35, 1 ] ], 520 ], [ 862, "\u989c\u82d1.s7", "s04", "\u5fc3\u52a8", [ [ 32, 1 ], [ 5, 1 ], [ 2, 1 ], [ 11, 0 ], [ 26, 1 ], [ 28, 0 ], [ 34, 0 ], [ 10, 1 ] ], 520 ], [ 7765, "\u6d77\u71d5.s2", "yx567_s0125", "7k7k", [ [ 23, 1 ], [ 20, 0 ], [ 32, 1 ], [ 2, 1 ], [ 16, 0 ], [ 17, 1 ], [ 25, 0 ], [ 3, 1 ] ], 510 ], [ 200966, "\u59d0\u73a9\u7684\u5bc2\u5bde.s1", "s04", "\u5fc3\u52a8", [ [ 6, 1 ], [ 18, 1 ], [ 35, 0 ], [ 32, 0 ], [ 25, 1 ], [ 31, 1 ], [ 13, 0 ], [ 4, 1 ] ], 500 ], [ 8366, "\u54c8\u6839\u8fbe\u65af\u4e44.s8", "yx567_s0125", "7k7k", [ [ 31, 1 ], [ 19, 1 ], [ 13, 0 ], [ 21, 0 ], [ 1, 1 ], [ 22, 0 ], [ 25, 1 ], [ 16, 1 ] ], 170 ], [ 2968, "\u82cf\u5a9a\u5a77.s7", "s04", "\u5fc3\u52a8", [ [ 19, 1 ], [ 27, 0 ], [ 15, 1 ], [ 30, 0 ], [ 29, 1 ], [ 26, 1 ], [ 17, 0 ], [ 9, 1 ] ], 500 ], [ 5468, "\u2225:\u8def\u4ebd\u66f1.s1", "s04", "\u5fc3\u52a8", [ [ 18, 1 ], [ 2, 0 ], [ 7, 1 ], [ 14, 0 ], [ 9, 0 ], [ 33, 1 ], [ 35, 1 ], [ 26, 1 ] ], 500 ], [ 7870, "\u51ef\u4e50\u5927\u738b.s1", "yx567_s0125", "7k7k", [ [ 29, 0 ], [ 3, 1 ], [ 27, 0 ], [ 34, 1 ], [ 8, 1 ], [ 25, 1 ], [ 15, 1 ], [ 6, 0 ] ], 520 ], [ 8671, "\u5929\u5e1d.s4", "yx567_s0125", "7k7k", [ [ 19, 1 ], [ 29, 0 ], [ 1, 1 ], [ 18, 1 ], [ 2, 0 ], [ 16, 0 ], [ 33, 1 ], [ 34, 1 ] ], 500 ], [ 5273, "\u65e0\u4eba\u53ca\u4f60.s02", "s02", "\u5fc3\u52a8", [ [ 3, 0 ], [ 5, 1 ], [ 8, 1 ], [ 21, 1 ], [ 26, 1 ], [ 9, 0 ], [ 6, 1 ], [ 27, 0 ] ], 520 ], [ 5376, "\u4e01\u5c27\u83f2.s5", "95k_s081", "95k", [ [ 34, 1 ], [ 22, 1 ], [ 16, 0 ], [ 17, 0 ], [ 20, 1 ], [ 29, 0 ], [ 31, 1 ], [ 25, 1 ] ], 500 ], [ 8186, "1234.s1", "yx567_s0125", "7k7k", [ [ 21, 1 ], [ 7, 1 ], [ 29, 0 ], [ 24, 0 ], [ 34, 1 ], [ 27, 1 ], [ 2, 0 ], [ 4, 1 ] ], 510 ], [ 4088, "\u5a49\u7b90\u537f.s1", "s04", "\u5fc3\u52a8", [ [ 9, 1 ], [ 7, 1 ], [ 8, 1 ], [ 24, 0 ], [ 27, 0 ], [ 35, 0 ], [ 17, 1 ], [ 1, 1 ] ], 500 ], [ 16889, "\u4e1c\u65b9\u5de8\u8fdf.s1", "s04", "\u5fc3\u52a8", [ [ 18, 1 ], [ 13, 1 ], [ 33, 1 ], [ 35, 0 ], [ 9, 0 ], [ 14, 1 ], [ 16, 0 ], [ 34, 1 ] ], 520 ], [ 2889, "\u5eb7\u6f6d\u6c14.s4", "95k_s081", "95k", [ [ 2, 0 ], [ 6, 1 ], [ 29, 1 ], [ 1, 1 ], [ 4, 1 ], [ 27, 0 ], [ 22, 0 ], [ 17, 1 ] ], 520 ], [ 194498, "\u8346\u65e0\u971c.s1", "yx567_s0125", "7k7k", [ [ 24, 1 ], [ 31, 0 ], [ 20, 0 ], [ 29, 1 ], [ 34, 0 ], [ 18, 1 ], [ 8, 1 ], [ 21, 1 ] ], 500 ] ] ]
//         [ 203957, "\u5e84\u606d\u6625.s1", "s04", "\u5fc3\u52a8", [ [ 1, 1 ], [ 27, 0 ], [ 8, 1 ], [ 21, 0 ], [ 32, 1 ], [ 31, 1 ], [ 19, 0 ], [ 13, 1 ] ], 80 ]
//============================================================================
Json::Value sxd_client::Mod_WishPool_Base_get_wish_list(){
    Json::Value data;
    return this->send_and_receive(data, 359, 1);
}

//============================================================================
// R171 祝福他人
// {module:359, action:5,
// request:[Utils.IntUtil],
// Example
//     [ 203957 ]
// response:[Utils.UByteUtil, Utils.IntUtil, Utils.IntUtil]}
// WishPoolData.as 123:
//     this.result = param1[0];
//     _loc_2.st_player_id == param1[1]
//     this.wishOtherAwardCoins = param1[2];
// Example
//     [ 2, 203957, 200000 ]
//============================================================================
Json::Value sxd_client::Mod_WishPool_Base_wish_other(int id){
    Json::Value data;
    data.append(id);
    return this->send_and_receive(data, 359, 5);
}

//============================================================================
// R171 领取
// {module:359, action:6,
// request:[Utils.UByteUtil],
// WishPoolView.as 243:
//     modValue = value == 2 ? (Mod_WishPool_Base.YES) : (Mod_WishPool_Base.NO);
//     _data.call(Mod_WishPool_Base.get_award, get_award_callback, [modValue], true, DataBase.SOCKETSERVERTOWN);
// Example
//     [ 0 ], Mod_WishPool_Base.YES, 奖励翻倍
//     [ 1 ], Mod_WishPool_Base.NO
// response:[Utils.UByteUtil]}
// Example
//     [ 2 ]
//============================================================================
Json::Value sxd_client::Mod_WishPool_Base_get_award(int flag){
    Json::Value data;
    data.append(flag);
    return this->send_and_receive(data, 359, 6);
}

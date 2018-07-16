#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_StPracticeRoom_Base {
public:
    static const int TURE = 0;
    static const int YES = 2;
    static const int NO = 3;
    static const int SUCCESS = 4;
    static const int NOBODY = 11;
};

//============================================================================
// R180 仙界练功房
//============================================================================
void sxd_client::st_practice_room() {
    int room_capcity[] = { 0, 6, 7, 8, 10, 12 };

    // get_rooms
    auto data = this->Mod_StPracticeRoom_Base_get_rooms();
    std::vector<Json::Value> valid_rooms;
    std::copy_if(data[0].begin(), data[0].end(), std::back_inserter(valid_rooms), [room_capcity](const Json::Value& x) {return x[0].asInt() <= 5 && x[1].asInt() < room_capcity[x[0].asInt()];});
    std::sort(valid_rooms.begin(), valid_rooms.end(), [](const Json::Value& x, const Json::Value& y) {return x[0].asInt() > y[0].asInt();});
    int in_practice_flag = data[1].asInt();
    int my_quality = data[2].asInt();

    // enter_room
    data = this->Mod_StPracticeRoom_Base_enter_room();
    if (data[0].asInt() != Mod_StPracticeRoom_Base::SUCCESS) {
        common::log(boost::str(boost::format("【仙界练功房】进入练功房失败，result[%1%]") % data[0]));
        return;
    }
    data = this->Mod_StPracticeRoom_Base_get_room_practice_exp();
    if (data[0].asInt() == Mod_StPracticeRoom_Base::SUCCESS)
        common::log(boost::str(boost::format("【仙界练功房】领取 [经验值×%1%]") % data[1]));

    if (!valid_rooms.size()) {
        // no valid rooms
        if (in_practice_flag == Mod_StPracticeRoom_Base::YES)
            return;
        // sit down free room
        data = this->Mod_StPracticeRoom_Base_sit_down(100, 0);
        if (data[0].asInt() != Mod_StPracticeRoom_Base::SUCCESS) {
            common::log(boost::str(boost::format("【仙界练功房】进入 [普通练功房] 练功失败，result[%1%]") % data[0]));
            return;
        }
        common::log("【仙界练功房】进入 [普通练功房] 练功");
    } else {
        // have valid rooms
        int room_id = valid_rooms[0][0].asInt();
        if (in_practice_flag == Mod_StPracticeRoom_Base::YES && my_quality <= 5 && room_id <= my_quality)
            return;
        // sit down
        data = this->Mod_StPracticeRoom_Base_get_room_info(room_id);
        auto seat_list = data[0];
        if ((int)seat_list.size() == room_capcity[room_id]) {
            common::log(boost::str(boost::format("【仙界练功房】[%1%星练功房] 已满") % room_id));
            return;
        }
        for (int seat_id = 0; seat_id < room_capcity[room_id]; seat_id++) {
            if (std::find_if(seat_list.begin(), seat_list.end(), [seat_id](const Json::Value& x) {return x[0].asInt() == seat_id;}) != seat_list.end())
                continue;
            this->Mod_StPracticeRoom_Base_player_leave_seat();
            data = this->Mod_StPracticeRoom_Base_sit_down(room_id, seat_id);
            if (data[0].asInt() != Mod_StPracticeRoom_Base::SUCCESS) {
                common::log(boost::str(boost::format("【仙界练功房】进入 [%1%星练功房] 练功失败，result[%2%]") % room_id % data[0]));
                return;
            }
            common::log(boost::str(boost::format("【仙界练功房】进入 [%1%星练功房] 练功") % room_id));
            break;
        }
    }
}

//============================================================================
// R180 面板
// "module":117,"action":0,"request":[],"response":[[Utils.ByteUtil,Utils.ShortUtil],Utils.UByteUtil,Utils.ByteUtil]
// StPracticeRoomController.as:
//     oObject.list(_data.stPracticeRoom.room_list,_loc1_,["room_list","in_practice_flag","quality"]);
//     for each(_loc3_ in _loc1_.room_list)
//         oObject.list(_loc3_,_loc4_,["quality","count"]);
// Example
//     [ [ [ 4, 9 ], [ 100, 28 ], [ 5, 10 ], [ 2, 1 ], [ 1, 2 ], [ 3, 3 ] ], 3, 2 ]
//============================================================================
Json::Value sxd_client::Mod_StPracticeRoom_Base_get_rooms() {
    Json::Value data;
    return this->send_and_receive(data, 117, 0);
}

//============================================================================
// R180 enter_room
// "module":117,"action":8,"request":[],"response":[Utils.UByteUtil]
// StPracticeRoomController.as:
//     this.result = param1[0];
// Example
//     [ 4 ]
//============================================================================
Json::Value sxd_client::Mod_StPracticeRoom_Base_enter_room() {
    Json::Value data;
    return this->send_and_receive(data, 117, 8);
}

//============================================================================
// R180 普通练功房
// "module":117,"action":16,"request":[],"response":[Utils.ShortUtil,Utils.ShortUtil,Utils.IntUtil,Utils.IntUtil,Utils.ShortUtil,Utils.ShortUtil,Utils.ByteUtil,Utils.UByteUtil,Utils.ByteUtil]
// StPracticeRoomController.as:
//     oObject.list(_loc2_,_loc1_,["total_page","total_player","practice_time","exp","rob_time","player_page","room_quality","exp_plus_flag","exp_plus_percent"]);
// Example
//     [ 2, 28, 0, 0, 10, 0, 0, 2, 20 ]
//============================================================================
Json::Value sxd_client::Mod_StPracticeRoom_Base_get_free_room_info() {
    Json::Value data;
    return this->send_and_receive(data, 117, 16);
}

//============================================================================
// R180 普通练功房-按页
// "module":117,"action":17,"request":[Utils.ShortUtil],"response":[[Utils.ByteUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.StringUtil,Utils.StringUtil,Utils.StringUtil,Utils.IntUtil,Utils.IntUtil,Utils.UByteUtil]]
// StPracticeRoomController.as:
//     for each(_loc5_ in _loc1_[0])
//         oObject.list(_loc5_,_loc4_,["seat_seq","player_id","role_id","equip_id","nick_name","stage_name","server_name",**"attack_power"**,"level","enemy_flag"]);
// Example
//     [ [ [ 0, 2204, 6, 3787, "\u68a8\u82b1\u6d77\u68e0.s154", "\u591a\u73a9", "duowan_s0152", 3773939, 217, 1 ], [ 1, 3893, 102, 5064, "\u5fd8\u54f2\u5929\u590f.s175", "37wan", "37wan_s0520", 4997820, 245, 1 ], [ 2, 3060, 6, 1801, "\u8f69\u8f95\u7389\u864e.s170", "37wan", "37wan_s0520", 5565436, 259, 1 ], [ 3, 3974, 104, 1877, "\u8f69\u8f95\u7389\u9f99.s170", "37wan", "37wan_s0520", 6030836, 259, 1 ], [ 4, 3814, 201, 4561, "\u534a\u4ed9.s169", "37wan", "37wan_s0520", 5433267, 258, 1 ], [ 5, 2061, 5, 4094, "\u59ec\u8f69\u8f95.s158", "\u591a\u73a9", "duowan_s0152", 4534025, 223, 1 ], [ 6, 3957, 6, 4561, "\u51b0\u6676\u6c34.s16", "\u745e\u661f", "rising_s033", 7593089, 259, 1 ], [ 7, 4203, 5, 4328, "\u7389\u79c0\u99a8.s151", "\u5fc3\u52a8", "s077", 4203332, 255, 1 ], [ 8, 8363, 6, 1872, "\u9646\u4eea\u4fdd.s299", "\u5fc3\u52a8", "s0151", 892615, 144, 1 ], [ 9, 7852, 5, 1872, "\u6613\u5f97\u8fd0.s299", "\u5fc3\u52a8", "s0151", 3161217, 189, 1 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_StPracticeRoom_Base_get_free_room_info_by_page(int page) {
    Json::Value data;
    data.append(page);
    return this->send_and_receive(data, 117, 17);
}

//============================================================================
// R180 星级练功房
// "module":117,"action":1,"request":[Utils.ByteUtil],"response":[[Utils.ByteUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.StringUtil,Utils.StringUtil,Utils.StringUtil,Utils.IntUtil,Utils.IntUtil,Utils.UByteUtil],Utils.IntUtil,Utils.IntUtil,Utils.ShortUtil,Utils.ByteUtil,Utils.UByteUtil,Utils.ByteUtil]
// StPracticeRoomController.as:
//     oObject.list(_loc2_,_loc1_,["seat_list","practice_time","exp","rob_time","room_quality","exp_plus_flag","exp_plus_percent"]);
//     for each(_loc6_ in _loc1_.seat_list)
//         oObject.list(_loc6_,_loc5_,["seat_seq","player_id","role_id","equip_id","nick_name","stage_name","server_name","attack_power","level","enemy_flag"]);
// Example
//     [ 1 ] --> [ [ [ 0, 3350, 2, 2241, "\u503e@\u60c5.s11", "\u745e\u661f", "rising_s033", 3132208, 216, 1 ], [ 3, 3110, 108, 4328, "kkkkkk.s11", "\u745e\u661f", "rising_s033", 4509769, 245, 1 ] ],
//                 43189, 0, 10, 100, 2, 20 ]
//============================================================================
Json::Value sxd_client::Mod_StPracticeRoom_Base_get_room_info(int id) {
    Json::Value data;
    data.append(id);
    return this->send_and_receive(data, 117, 1);
}

//============================================================================
// R180 座位
// "module":117,"action":3,"request":[Utils.ByteUtil,Utils.ByteUtil],"response":[Utils.UByteUtil]
// StPracticeRoomController.as:
//     this.seat_flag = _loc2_[0];
// Example
//     [ 1, 4 ] --> [ 11 ]
//============================================================================
Json::Value sxd_client::Mod_StPracticeRoom_Base_get_seat_info(int room_id, int seat_id) {
    Json::Value data;
    data.append(room_id);
    data.append(seat_id);
    return this->send_and_receive(data, 117, 3);
}

//============================================================================
// R180 经验值标记
// "module":117,"action":7,"request":[],"response":[Utils.UByteUtil]
// StPracticeRoomController.as:
//     return _data.stPracticeRoom.exp_flag;
// Example
//     [ 2 ]
//============================================================================
Json::Value sxd_client::Mod_StPracticeRoom_Base_get_exp_flag() {
    Json::Value data;
    return this->send_and_receive(data, 117, 7);
}

//============================================================================
// R180 离开座位
// "module":117,"action":13,"request":[],"response":[Utils.UByteUtil]
// StPracticeRoomData.as:
//     this.result = param1[0];
// Example
//     [ 4 ]
//============================================================================
Json::Value sxd_client::Mod_StPracticeRoom_Base_player_leave_seat() {
    Json::Value data;
    return this->send_and_receive(data, 117, 13);
}

//============================================================================
// R180 领取经验值
// "module":117,"action":6,"request":[],"response":[Utils.UByteUtil,Utils.IntUtil]
// StPracticeRoomData.as:
//     this.result = param1[0];
//     this.room_exp = param1[1];
// Example
//     [ 4, 5339 ]
//============================================================================
Json::Value sxd_client::Mod_StPracticeRoom_Base_get_room_practice_exp() {
    Json::Value data;
    return this->send_and_receive(data, 117, 6);
}

//============================================================================
// R180 坐下
// "module":117,"action":2,"request":[Utils.ByteUtil,Utils.ByteUtil],"response":[Utils.UByteUtil]
// StPracticeRoomData.as:
//     this.result = param1[0];
// Example
//     [ 1, 4 ] --> [ 4 ]
//============================================================================
Json::Value sxd_client::Mod_StPracticeRoom_Base_sit_down(int room_id, int seat_id) {
    Json::Value data;
    data.append(room_id);
    data.append(seat_id);
    return this->send_and_receive(data, 117, 2);
}

#include <boost/format.hpp>
#include <thread>

#include "common.h"
#include "sxd_client.h"

class Mod_StcLogin_Base {
public:
    static const int OPEN = 0;
};

class Mod_StLogin_Base {
public:
    static const int SUCCESS = 0;
};

class Mod_StTown_Base {
public:
    static const int SUCCESS = 2;
};

int sxd_client::login_super_town(sxd_client* sxd_client_town) {

    // 1. get status
    Json::Value data = sxd_client_town->Mod_StcLogin_Base_get_status();
    if (data[0].asInt() != Mod_StcLogin_Base::OPEN) {
        common::log(boost::str(boost::format("【仙界】入口未开启，status[%1%]") % data[0]), iEdit);
        return 1;
    }
    this->user_id = sxd_client_town->user_id;

    // 2. get login information: host, port, server_name, time, passCode
    data = sxd_client_town->Mod_StcLogin_Base_get_login_info();
    std::string host = data[0].asString();
    std::string port = data[1].asString();
    std::string server_name = data[2].asString();
    int time = data[3].asInt();
    std::string pass_code = data[4].asString();
    // get other information: nickname
    data = sxd_client_town->Mod_Player_Base_get_player_info();
    std::string nickname = data[0].asString();

    // 3. connect
    this->connect(host, port);
    common::log(boost::str(boost::format("【仙界】连接服务器 [%1%:%2%] 成功") % host % port), iEdit);

    // 4. login
    data = this->Mod_StLogin_Base_login(server_name, sxd_client_town->player_id, nickname, time, pass_code);
    if (data[0].asInt() != Mod_StLogin_Base::SUCCESS) {
        common::log(boost::str(boost::format("【仙界】登录失败，result[%1%]") % data[0]), iEdit);
        return 2;
    }
    player_id = data[1].asInt();
    common::log(boost::str(boost::format("【仙界】登录成功，player_id[%1%]") % player_id), iEdit);

    // 5. enter town
    data = this->Mod_StTown_Base_enter_town();
    /*if (data[0].asInt() != Mod_StTown_Base::SUCCESS) {
     common::log(boost::str(boost::format("【仙界】玩家进入 [仙界] 失败，result[%1%]") % data[0]), iEdit);
     return 3;
     }*/
    common::log("【仙界】玩家进入 [仙界]", iEdit);

    // 6. chat
    Json::Value config;
    std::istringstream(db.get_config(user_id.c_str(), "StChat")) >> config;
    std::string message = config[rand() % config.size()].asString();
    this->Mod_Chat_Base_chat_with_players(4, common::gbk2utf(message));

    bLogin = 1;
    return 0;
}

//============================================================================
// R171
// 仙界状态
// {module:96, action:1,
// request:[],
// response:[Utils.UByteUtil, [Utils.IntUtil, Utils.IntUtil, Utils.UByteUtil]]}
// StcLoginData.as 38:
//     oObject.list(param1, this.stcStatusObj, ["status", "close_time_list"]);
// Example
//     [ 0, [ [ 1506736800, 1506765600, 4 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_StcLogin_Base_get_status() {
    Json::Value data;
    return this->send_and_receive(data, 96, 1);
}

//============================================================================
// R170
// 仙界登录信息
// {module:96, action:0,
// request:[],
// response:[Utils.StringUtil, Utils.ShortUtil, Utils.StringUtil, Utils.IntUtil, Utils.StringUtil, Utils.StringUtil]}
// StcLoginData.as 22:
//     public function get_login_info(param1:Array) : void
//     {
//         this.stcLoginObj = new Object();
//         var _loc_2:* = 0;
//         this.stcLoginObj.serverHost = param1[_loc_2++];
//         this.stcLoginObj.port = param1[_loc_2++];
//         this.stcLoginObj.serverName = param1[_loc_2++];
//         this.stcLoginObj.time = param1[_loc_2++];
//         this.stcLoginObj.passCode = param1[_loc_2++];
//         this.stcLoginObj.serverTownName = param1[_loc_2++];
//         this.stcLoginObj.playerId = 32;
//         return;
//     }// end function
// Example
//     [ "8x072.xd.com", 9400, "s04", 1519271240, "04c7653a374a9567e33d4fd3a8f210b9", "super_town_s0" ]
//============================================================================
Json::Value sxd_client::Mod_StcLogin_Base_get_login_info() {
    Json::Value data;
    return this->send_and_receive(data, 96, 0);
}

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
Json::Value sxd_client::Mod_StLogin_Base_login(const std::string& server_name, int player_id_town, const std::string& nickname, int time, const std::string& pass_code) {
    Json::Value data;
    data.append(server_name);
    data.append(player_id_town);
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
Json::Value sxd_client::Mod_StTown_Base_enter_town() {
    Json::Value data;
    data.append(41);
    Json::Value result = this->send_and_receive(data, 95, 0, 95, 1);
    this->x = result[3].asInt();
    this->y = result[4].asInt();
    return result;
}

//============================================================================
// R172 在线玩家信息
// {module:95, action:6, request:[], response:[[Utils.IntUtil, Utils.IntUtil, Utils.StringUtil, Utils.ShortUtil, Utils.ShortUtil, Utils.IntUtil, Utils.StringUtil, Utils.StringUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.StringUtil, Utils.UByteUtil, Utils.UByteUtil, Utils.ShortUtil, Utils.IntUtil, Utils.StringUtil, Utils.IntUtil, Utils.ShortUtil, [Utils.UByteUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ShortUtil]]]};
// StTownData.as 132:
//     param1 = param1[0];
//     for each (_loc_3 in param1)
//         oObject.list(_loc_3, _loc_4, ["player_id", "role_id", "nickname", "position_x", "position_y", "equip_item_id", "stage_name", "server_name", "is_world_war_top", "is_star", "transport", "avatar", "st_union_name", "immortal_flag", "saint_flag", "mount_rune_type_id", "children_role_id", "children_nickname", "children_suit_id", "orange_equipment_fllow_id", "follow_pet_list"]);
//============================================================================
Json::Value sxd_client::Mod_StTown_Base_get_players() {
    Json::Value data;
    return this->send_and_receive(data, 95, 6);
}

void sxd_client::st_town_move_to(int x, int y) {
    this->Mod_StTown_Base_move_to(this->x, this->y, x, y);
    this->Mod_StTown_Base_move_to(x, y, x, y);
    this->x = x;
    this->y = y;
}

Json::Value sxd_client::Mod_StTown_Base_move_to(int x1, int y1, int x2, int y2) {
    Json::Value data;
    data.append(x1);
    data.append(y1);
    data.append(x2);
    data.append(y2);
    return this->send_and_receive(data, 95, 4, 95, 5, [this](const Json::Value& x){return x[0].asInt() == this->player_id;});
}

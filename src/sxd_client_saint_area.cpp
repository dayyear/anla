#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_SaintAreaLogin_Base {
public:
    static const int SUCCESS = 0;
    static const int OPEN = 2;
};

class Mod_SaintAreaTown_Base {
public:
    static const int SUCCESS = 2;
};

int sxd_client::login_saint_area(sxd_client* sxd_client_town) {

    // 1. get status
    Json::Value data = sxd_client_town->Mod_SaintAreaLogin_Base_get_status();
    if (data[0].asInt() != Mod_SaintAreaLogin_Base::OPEN) {
        common::log(boost::str(boost::format("¡¾Ê¥Óò¡¿Èë¿ÚÎ´¿ªÆô£¬status[%1%]") % data[0]));
        return 1;
    }

    // 2. get login information: host, port, server_name, time, passCode
    data = sxd_client_town->Mod_SaintAreaLogin_Base_get_login_info();
    std::string host = data[0].asString();
    std::string port = data[1].asString();
    std::string server_name = data[2].asString();
    int time = data[3].asInt();
    std::string pass_code = data[4].asString();
    if (host.size() == 0) {
        common::log("¡¾Ê¥Óò¡¿Íæ¼ÒÎ´³ÉÊ¥");
        return 2;
    }
    // get other information: nickname
    data = sxd_client_town->Mod_Player_Base_get_player_info();
    std::string nickname = data[0].asString();

    // 3. connect
    this->connect(host, port);
    common::log(boost::str(boost::format("¡¾Ê¥Óò¡¿Á¬½Ó·şÎñÆ÷ [%1%:%2%] ³É¹¦") % host % port));

    // 4. login
    data = this->Mod_SaintAreaLogin_Base_login(server_name, sxd_client_town->player_id, nickname, time, pass_code);
    if (data[0].asInt() != Mod_SaintAreaLogin_Base::SUCCESS) {
        common::log(boost::str(boost::format("¡¾Ê¥Óò¡¿µÇÂ¼Ê§°Ü£¬result[%1%]") % data[0]));
        return 3;
    }
    player_id = data[1].asInt();
    common::log(boost::str(boost::format("¡¾Ê¥Óò¡¿µÇÂ¼³É¹¦£¬player_id[%1%]") % player_id));

    // 5. enter town
    data = this->Mod_SaintAreaTown_Base_enter_town();
    if (data[0].asInt() != Mod_SaintAreaTown_Base::SUCCESS) {
        common::log(boost::str(boost::format("¡¾Ê¥Óò¡¿Íæ¼Ò½øÈë [Ê¥Óò] Ê§°Ü£¬result[%1%]") % data[0]));
        return 4;
    }
    common::log("¡¾Ê¥Óò¡¿Íæ¼Ò½øÈë [Ê¥Óò]");

    return 0;
}

//============================================================================
// R171
// Ê¥Óò×´Ì¬
// {module:293, action:2,
// request:[], response:[Utils.UByteUtil, [Utils.IntUtil, Utils.IntUtil, Utils.UByteUtil]]}
// SaintAreaLoginData.as 66:
//     oObject.list(param1, this.saintAreaStatusObj, ["status", "close_time_list"]);
// Example
//     [ 2, [ [ 1506736800, 1506744000, 6 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_SaintAreaLogin_Base_get_status() {
    Json::Value data;
    return this->send_and_receive(data, 293, 2);
}
//============================================================================
// R171
// Ê¥ÓòµÇÂ¼ĞÅÏ¢
// {module:293, action:1,
// request:[], response:[Utils.StringUtil, Utils.ShortUtil, Utils.StringUtil, Utils.IntUtil, Utils.StringUtil, Utils.StringUtil]}
// SaintAreaLoginData.as 54:
//     this.saintAreaLoginObj.serverHost = param1[_loc_2++];
//     this.saintAreaLoginObj.port = param1[_loc_2++];
//     this.saintAreaLoginObj.serverName = param1[_loc_2++];
//     this.saintAreaLoginObj.time = param1[_loc_2++];
//     this.saintAreaLoginObj.passCode = param1[_loc_2++];
//     this.saintAreaLoginObj.serverTownName = param1[_loc_2++];
// Example
//     [ "8x073.xd.com", 9203, "s04", 1519223759, "cbc143f15b87fc91e82cebd29627418f", "saint_area_s01" ]
//     [ "", 0, "", 0, "", "" ]
//============================================================================
Json::Value sxd_client::Mod_SaintAreaLogin_Base_get_login_info() {
    Json::Value data;
    return this->send_and_receive(data, 293, 1);
}

//============================================================================
// R171
// Ê¥ÓòµÇÂ¼
// {module:293, action:0,
// request:[Utils.StringUtil, Utils.IntUtil, Utils.StringUtil, Utils.IntUtil, Utils.StringUtil],
// View.as 7212:
//     _data.call(Mod_SaintAreaLogin_Base.login, chatSaintAreaConnectLoginBack, [obj.serverName, _ctrl.player.playerInfo.id, _ctrl.player.originNickName, obj.time, obj.passCode], true, DataBase.SOCKETSAINTAREA);
// Example
//     [ "s04", 288299, "\u5ea6\u65e5\u5982\u5e74.s1", 1519221652, "5270f70e9e4ff96fa40089855880f61a" ]
// response:[Utils.UByteUtil, Utils.IntUtil, Utils.IntUtil]}
// SaintAreaLoginData.as 37:
//     this.result = param1[0];
//     this.playerId = param1[1];
// Example
//     [ 0, 584, 1519221653 ]
//============================================================================
Json::Value sxd_client::Mod_SaintAreaLogin_Base_login(const std::string& server_name, int player_id, const std::string& nickname, int time, const std::string& pass_code) {
    Json::Value data;
    data.append(server_name);
    data.append(player_id);
    data.append(nickname);
    data.append(time);
    data.append(pass_code);
    return this->send_and_receive(data, 293, 0);
}

//============================================================================
// R171
// ½øÈëÊ¥Óò
// {module:294, action:0,
// request:[Utils.IntUtil],
// Example
//     [ 78 ]
// response:[Utils.UByteUtil]}
// SaintAreaTownData.as 43:
//     this.enterResult = param1[0];
// Example
//     [ 2 ]
//============================================================================
Json::Value sxd_client::Mod_SaintAreaTown_Base_enter_town() {
    Json::Value data;
    data.append(78);
    return this->send_and_receive(data, 294, 0);
}

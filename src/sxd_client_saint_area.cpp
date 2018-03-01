#include "sxd_client.h"

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

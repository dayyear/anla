#include <thread>
#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_ServerChatRoom_Base {
public:
    static const int UNDO = 4;
    static const int DONE = 5;
    static const int SUCCESS = 9;
    static const int CAN_FEED = 34;
    static const int CAN_ESCORT = 35;
    static const int ESCORTING = 36;
    static const int INGOT_ESCORTING = 37;
    static const int ESCORT_DONE = 38;
    static const int NORMAL = 41;           // 喂养，派遣
    static const int INGOT = 42;            // 一键喂养，高级派遣
};

int sxd_client::login_server_chat(sxd_client* sxd_client_town) {

    // get status
    Json::Value data = sxd_client_town->Mod_ServerChatRoom_Base_get_chat_room_status();
    if (data[0].asInt() != Mod_ServerChatRoom_Base::SUCCESS) {
        common::log(boost::str(boost::format("【全网聊天】入口未开启，status[%1%]") % data[0]));
        return 1;
    }

    // get login information: node, servername, stagename, timestamp, login_code, host, port
    data = sxd_client_town->Mod_ServerChatRoom_Base_get_chat_room_logincode(data[1][0][0].asInt());
    std::string node = data[1].asString();
    std::string servername = data[2].asString();
    std::string stagename = data[3].asString();
    int timestamp = data[4].asInt();
    std::string login_code = data[5].asString();
    std::string host = data[6].asString();
    std::string port = data[7].asString();

    // connect
    this->connect(host, port);
    common::log(boost::str(boost::format("【全网聊天】连接服务器 [%1%:%2%] 成功") % host % port));

    // login
    data = this->Mod_ServerChatRoom_Base_login_chat_room(node, sxd_client_town->player_id, servername, stagename, timestamp, login_code);
    if (data[0].asInt() != Mod_ServerChatRoom_Base::SUCCESS) {
        common::log(boost::str(boost::format("【全网聊天】登录失败，result[%1%]") % data[0]));
        return 2;
    }
    player_id = data[1].asInt();
    common::log(boost::str(boost::format("【全网聊天】登录成功，player_id[%1%]") % player_id));

    // chat
    Json::Value config;
    std::istringstream(db.get_config(user_id.c_str(), "Chat")) >> config;
    std::string message = config[rand() % config.size()].asString();
    this->Mod_ServerChatRoom_Base_chat_with_players(common::gbk2utf(message));
    common::log(boost::str(boost::format("【全网聊天】%1%") % message));

    return 0;
}

//============================================================================
// R171
// 最近加入的聊天室(town)
// {module:336, action:0,
// request:[], response:[Utils.UByteUtil, [Utils.ByteUtil]]}
// ServerChatRoomData.as 69:
//     this.result = param1[0];
//     this.isOpen = this.result == Mod_ServerChatRoom_Base.SUCCESS;
//     for each (_loc_2 in param1[1])
//         this.recentIds.push(_loc_2[0]);
// Example
//     [ 9, null ]
//     [ 9, [ [ 1 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_ServerChatRoom_Base_get_recent_room_list() {
    Json::Value data;
    return this->send_and_receive(data, 336, 0);
}

//============================================================================
// R171
// 聊天室状态(town)
// {module:336, action:1,
// request:[], response:[Utils.UByteUtil, [Utils.ByteUtil, Utils.IntUtil, Utils.UByteUtil]]}
// ServerChatRoomData.as 84:
//     this.result = param1[0];
//     this.isOpen = this.result == Mod_ServerChatRoom_Base.SUCCESS;
//     for each (_loc_2 in param1[1])
//         _loc_3.setData(_loc_2[0], _loc_2[1], _loc_2[2]);
// ServerChatRoomRoomInfo.as 27:
//     this.id = param1;
//     this.count = param2;
//     this.status = param3;
// Example
//     [ 9, [ [ 1, 228, 11 ], [ 2, 42, 11 ], [ 8, 31, 11 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_ServerChatRoom_Base_get_chat_room_status() {
    Json::Value data;
    return this->send_and_receive(data, 336, 1);
}

//============================================================================
// R171
// 聊天室登录信息(town)
// {module:336, action:11,
// request:[Utils.ByteUtil],
// View.as 7158:
//     _data.call(Mod_ServerChatRoom_Base.get_chat_room_logincode, this.onGetLoginCodeCallBack, [this._roomToConnect.id]);
// Example
//     [ 1 ] 闲聊频道1
//     [ 2 ] 神仙斗一斗
//     [ 8 ] 战报分享
// response:[Utils.ByteUtil, Utils.StringUtil, Utils.StringUtil, Utils.StringUtil, Utils.IntUtil, Utils.StringUtil, Utils.StringUtil, Utils.IntUtil]}
// ServerChatRoomData.as 203:
//     oObject.list(param1, _loc_2, ["id", "node", "servername", "stagename", "timestamp", "login_code", "ip", "port"]);
// Example
//     [ 1, "s04@8x096.xd.com", "s04", "verycd", 1519388840, "6452aede170151bc81e1ebe73a3f7fdd", "8x127.xd.com", 8340 ]
//     [ 2, "s04@8x096.xd.com", "s04", "verycd", 1519388840, "5998cdcfc3e26cbaf3320ad7a51bd2ad", "8x162.xd.com", 8531 ]
//     [ 8, "s04@8x096.xd.com", "s04", "verycd", 1519388840, "12b4bafe1fb789b73c2a1cbdbcb40b4e", "8x141.xd.com", 8340 ]
//============================================================================
Json::Value sxd_client::Mod_ServerChatRoom_Base_get_chat_room_logincode(int id) {
    Json::Value data;
    data.append(id);
    return this->send_and_receive(data, 336, 11);
}

//============================================================================
// R171
// 登录(chat_room)
// {module:336, action:12,
// request:[Utils.StringUtil, Utils.IntUtil, Utils.StringUtil, Utils.StringUtil, Utils.IntUtil, Utils.StringUtil],
// View.as 7168:
//     _data.call(Mod_ServerChatRoom_Base.login_chat_room, onLoginChatRoomCallBack,
//         [_roomToConnect.node, _ctrl.player.playerId, _roomToConnect.servername, _roomToConnect.stagename, _roomToConnect.timestamp, _roomToConnect.login_code], true, DataBase.SOCKETSERVERCHATROOM);
// Example
//     [ "s04@8x096.xd.com", 355547, "s04", "verycd", 1519380632, "d1b1e9dd3e7153f826f8e169278e66dd" ]
// response:[Utils.UByteUtil, Utils.IntUtil]}
// ServerChatRoomData.as 210:
//     this.result = param1[0];
//     this.playerId = param1[1];
// Example
//     [ 9, 74784 ]
//============================================================================
Json::Value sxd_client::Mod_ServerChatRoom_Base_login_chat_room(const std::string& node, int player_id, const std::string& servername, const std::string& stagename, int timestamp, const std::string& login_code) {
    Json::Value data;
    data.append(node);
    data.append(player_id);
    data.append(servername);
    data.append(stagename);
    data.append(timestamp);
    data.append(login_code);
    return this->send_and_receive(data, 336, 12);
}

void sxd_client::pet_escort(sxd_client* sxd_client_town) {

    // get nickname
    Json::Value data = sxd_client_town->Mod_Player_Base_get_player_info();
    std::string nickname = data[0].asString();
    // get servername
    data = sxd_client_town->Mod_ServerChatRoom_Base_get_chat_room_status();
    data = sxd_client_town->Mod_ServerChatRoom_Base_get_chat_room_logincode(data[1][0][0].asInt());
    std::string servername = data[2].asString();

    // try 10 times
    for (int i = 0; i < 10; i++) {
        data = this->Mod_ServerChatRoom_Base_get_player_pet_escort_info();
        Json::Value pet_escort_info = data;
        switch (pet_escort_info[2].asInt()) {

        case Mod_ServerChatRoom_Base::CAN_FEED:
            if (pet_escort_info[6].asInt() == Mod_ServerChatRoom_Base::UNDO) {
                data = sxd_client_town->Mod_ServerChatRoom_Base_feed_pet(Mod_ServerChatRoom_Base::NORMAL);
                if (data[0].asInt() != Mod_ServerChatRoom_Base::SUCCESS) {
                    common::log(boost::str(boost::format("【宠物派遣】喂养失败，result[%1%]") % data[0]));
                    return;
                }
                common::log("【宠物派遣】喂养一次");
            }
            std::this_thread::sleep_for(std::chrono::seconds(10));
            data = this->Mod_ServerChatRoom_Base_chat_with_players(boost::str(boost::format("MSG7_%1%_%2%_%3%") % player_id % nickname % servername));
            if (data[1].asInt() != Mod_ServerChatRoom_Base::SUCCESS) {
                common::log(boost::str(boost::format("【宠物派遣】邀请失败，result[%1%]") % data[1]));
                return;
            }
            common::log(boost::str(boost::format("【宠物派遣】邀请 [%1%/10]") % (i + 1)));
            std::this_thread::sleep_for(std::chrono::seconds(10));
            break;

        case Mod_ServerChatRoom_Base::CAN_ESCORT:
            data = sxd_client_town->Mod_ServerChatRoom_Base_escort_pet(Mod_ServerChatRoom_Base::NORMAL);
            if (data[0].asInt() != Mod_ServerChatRoom_Base::SUCCESS) {
                common::log(boost::str(boost::format("【宠物派遣】派遣失败，result[%1%]") % data[0]));
                return;
            }
            common::log("【宠物派遣】派遣");
            return;

        case Mod_ServerChatRoom_Base::ESCORTING:
        case Mod_ServerChatRoom_Base::INGOT_ESCORTING:
            common::log("【宠物派遣】派遣中...");
            return;

        case Mod_ServerChatRoom_Base::ESCORT_DONE:
            if (pet_escort_info[8].asInt() == Mod_ServerChatRoom_Base::UNDO) {
                data = sxd_client_town->Mod_ServerChatRoom_Base_get_pet_escort_award();
                if (data[0].asInt() != Mod_ServerChatRoom_Base::SUCCESS) {
                    common::log(boost::str(boost::format("【宠物派遣】领取失败，result[%1%]") % data[0]));
                    return;
                }
                common::log("【宠物派遣】领取");
            } else {
                common::log("【宠物派遣】今日派遣任务已完成");
            }
            return;

        default:
            common::log(boost::str(boost::format("【宠物派遣】未知状态，status[%1%]") % pet_escort_info[2]));
            return;
        }
    }
}

//============================================================================
// R171
// 派遣宠物(chat_room)
// {module:336, action:40,
// request:[], response:[Utils.IntUtil, Utils.ByteUtil, Utils.UByteUtil, Utils.ByteUtil, Utils.ByteUtil, [Utils.IntUtil, Utils.IntUtil], Utils.UByteUtil, Utils.UByteUtil, Utils.UByteUtil, Utils.IntUtil]}
// ServerChatRoomData.as 548:
//     oObject.list(param1, this.playerPetEscortInfo, ["skill_value", "pet_id", "status", "cur_hungry", "max_hungry", "award_list", "normal_feed_flag", "ingot_feed_flag", "award_flag", "escort_remain_time"]);
// Example
// 初始(status[2]=34, normal_feed_flag[6]=4)
//     [ 0, 62, 34, 0, 2, [ [ 1845, 20 ], [ 5359, 1 ], [ 5362, 3 ], [ 4063, 1 ] ], 4, 4, 4, 0 ]
// 自己喂食(status[2]=34, normal_feed_flag[6]=5)
//     [ 0, 62, 34, 1, 2, [ [ 5277, 12 ], [ 5359, 1 ], [ 5362, 3 ], [ 4063, 1 ] ], 5, 4, 4, 0 ]
// 邀请后(无变化)
//     [ 0, 62, 34, 1, 2, [ [ 5277, 12 ], [ 5359, 1 ], [ 5362, 3 ], [ 4063, 1 ] ], 5, 4, 4, 0 ]
// 饱食度已满，可派遣(status[2]=35)
//     [ 0, 62, 35, 2, 2, [ [ 1845, 20 ], [ 5359, 1 ], [ 5362, 3 ], [ 4063, 1 ] ], 5, 5, 4, 0 ]
// 派遣中(status[2]=36)
//     [ 0, 62, 36, 2, 2, [ [ 5277, 12 ], [ 5359, 1 ], [ 5362, 3 ], [ 4063, 1 ] ], 4, 4, 4, 7112 ]
// 派遣完成，可领取奖励(status[2]=38, award_flag[8]=4)
//     [ 0, 62, 38, 2, 2, [ [ 5277, 12 ], [ 5359, 1 ], [ 5362, 3 ], [ 4063, 1 ] ], 4, 4, 4, 0 ]
// 今日派遣任务已完成(status[2]=38, award_flag[8]=5)
//    [ 40, 62, 38, 2, 2, [ [ 2452, 12 ], [ 5359, 1 ], [ 5362, 3 ], [ 4063, 1 ] ], 4, 4, 5, 0 ]
//============================================================================
Json::Value sxd_client::Mod_ServerChatRoom_Base_get_player_pet_escort_info() {
    Json::Value data;
    return this->send_and_receive(data, 336, 40);
}

//============================================================================
// R171
// 喂食(town)
// {module:336, action:42,
// request:[Utils.UByteUtil],
// Example
//     [ 41 ]
// response:[Utils.UByteUtil, Utils.UByteUtil, Utils.ByteUtil, Utils.ByteUtil]}
// ServerChatRoomData.as 593:
//     this.result = param1[0];
//     this.playerPetEscortInfo.status = param1[1];
//     this.playerPetEscortInfo.cur_hungry = param1[2];
//     this.playerPetEscortInfo.normal_feed_flag = Mod_ServerChatRoom_Base.DONE;
// Example
//     [ 9, 34, 1, 2 ]
//============================================================================
Json::Value sxd_client::Mod_ServerChatRoom_Base_feed_pet(int type) {
    Json::Value data;
    data.append(type);
    return this->send_and_receive(data, 336, 42);
}

//============================================================================
// R171
// 派遣(town)
// {module:336, action:43,
// request:[Utils.UByteUtil],
// Example
//     [ 41 ]
// response:[Utils.UByteUtil, Utils.UByteUtil, Utils.IntUtil, [Utils.IntUtil, Utils.IntUtil]]}
// ServerChatRoomData.as 609:
//     this.result = param1[0];
//     this.playerPetEscortInfo.status = param1[1];
//     this.playerPetEscortInfo.escort_remain_time = param1[2];
//     for each (_loc_3 in param1[3])
//         oObject.list(_loc_3, _loc_4, ["item_id", "count"]);
// Example
//     [ 9, 36, 7200, [ [ 5277, 12 ], [ 5359, 1 ], [ 5362, 3 ], [ 4063, 1 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_ServerChatRoom_Base_escort_pet(int type) {
    Json::Value data;
    data.append(type);
    return this->send_and_receive(data, 336, 43);
}

//============================================================================
// R171
// 邀请(chat_room)
// {module:336, action:20,
// request:[Utils.StringUtil, Utils.StringUtil, Utils.StringUtil],
// ServerChatRoomView.as 155:
//     _data.call(Mod_ServerChatRoom_Base.chat_with_players, this.onSendMsgCallBack, [param1.message, param1.eipNum, param1.eipIndex], true, DataBase.SOCKETSERVERCHATROOM);
// Example
//     [ "MSG7_74656_\u9ad8\u5343\u5b57.s1_s04", "", "" ]
// response:[Utils.IntUtil, Utils.UByteUtil]}
// ServerChatRoomData.as 347:
//     this.result = param1[1];
// Example
//     [ 0, 9 ]
//============================================================================
Json::Value sxd_client::Mod_ServerChatRoom_Base_chat_with_players(const std::string& message, const std::string& eip_num, const std::string& eip_index) {
    Json::Value data;
    data.append(message);
    data.append(eip_num);
    data.append(eip_index);
    return this->send_and_receive(data, 336, 20);
}

//============================================================================
// R171
// 领取(town)
// {module:336, action:44, request:[], response:[Utils.UByteUtil, Utils.IntUtil, [Utils.IntUtil, Utils.IntUtil]]}
// ServerChatRoomData.as 637:
//     oObject.list(param1, this.getPetEscortAwardResult, ["result", "skill_value", "award_list"]);
// Example
//     [ 9, 20, [ [ 2452, 12 ], [ 5359, 1 ], [ 5362, 3 ], [ 4063, 1 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_ServerChatRoom_Base_get_pet_escort_award() {
    Json::Value data;
    return this->send_and_receive(data, 336, 44);
}

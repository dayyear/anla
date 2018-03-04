#include <boost/format.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_Player_Base {
public:
    static const int SUCCEED = 0;
};

class Mod_Town_Base {
public:
    static const int SUCCESS = 41;
};

int sxd_client::login_town(const std::string& web_page) {

    // 1. validation
    boost::smatch match;
    if (!regex_search(web_page, match, boost::regex("\"&player_name=(.*?)\"[\\s\\S]*\"&hash_code=(.*?)\"[\\s\\S]*\"&time=(.*?)\"[\\s\\S]*\"&ip=(.*?)\"[\\s\\S]*\"&port=(.*?)\"[\\s\\S]*\"&server_id=(.*?)\"[\\s\\S]*\"&source=(.*?)\"[\\s\\S]*\"&regdate=(.*?)\"[\\s\\S]*\"&id_card=(.*?)\"[\\s\\S]*\"&open_time=(.*?)\"[\\s\\S]*\"&is_newst=(.*?)\"[\\s\\S]*\"&stage=(.*?)\"[\\s\\S]*\"&client=(.*?)\""))) {
        common::log("请使用登录器重新登录");
        return 1;
    }
    if (match[1].str() != user_id) {
        common::log("user_id不一致");
        return 2;
    }

    // 2. get login information
    std::string hash_code(match[2]);                             // 用于login(0,0)
    std::string time(match[3]);                                  // 用于login(0,0)
    std::string host(match[4]);                                  // 用于socket.Connect()
    std::string port(match[5]);                                  // 用于socket.Connect()
    std::string source(match[7]);                                // 用于login(0,0)
    int regdate = boost::lexical_cast<int>(match[8]);            // 用于login(0,0)
    std::string id_card(match[9]);                               // 用于login(0,0)
    int open_time = boost::lexical_cast<int>(match[10]);         // 用于login(0,0)
    char is_newst = boost::lexical_cast<int>(match[11]);         // 用于login(0,0)
    std::string stage = common::uri_decode(match[12]);           // 用于login(0,0)
    std::string client = common::uri_decode(match[13]);          // 用于login(0,0)

    // 3. connect
    this->connect(host, port);
    common::log(boost::str(boost::format("【登录】连接服务器 [%1%:%2%] 成功") % host % port));

    // 4. login
    Json::Value data = this->Mod_Player_Base_login(user_id, hash_code, time, source, regdate, id_card, open_time, is_newst, stage, client);
    if (data[0].asInt() != Mod_Player_Base::SUCCEED) {
        common::log(boost::str(boost::format("【登录】失败，result[%1%]") % data[0].asInt()));
        return 3;
    }
    player_id = data[1].asInt();
    common::log(boost::str(boost::format("【登录】成功，player_id[%1%]") % player_id));

    // 5. player infomation
    data = this->Mod_Player_Base_get_player_info();
    std::string nickname = data[0].asString();
    int town_map_id = data[9].asInt();
    common::log(boost::str(boost::format("【登录】玩家基本信息，昵称[%1%]，[%2%]级，[VIP%3%]，元宝[%4%]，铜钱[%5%]") % common::utf2gbk(nickname) % data[1] % data[14] % data[2] % data[3]));

    // 6. player contrast infomation
    data = this->Mod_Player_Base_player_info_contrast(player_id);
    common::log(boost::str(boost::format("【登录】玩家排名信息，竞技排名[%1%]，帮派[%2%]，战力[%3%]，声望[%4%]，阅历[%5%]，成就[%6%]，先攻[%7%]，境界[%8%]，鲜花[%9%]，仙令[%10%]") % data[0][0][1] % common::utf2gbk(data[0][0][2].asString()) % data[0][0][3] % data[0][0][4] % data[0][0][5] % data[0][0][6] % data[0][0][7] % data[0][0][8] % data[0][0][9] % data[0][0][10]));

    // 7. enter_town
    data = this->Mod_Town_Base_enter_town(town_map_id);
    if (data[0].asInt() != Mod_Town_Base::SUCCESS) {
        common::log(boost::str(boost::format("【登录】玩家进入 [%1%] 失败，result[%2%]") % db.get_code(version, "Town", town_map_id)["text"] % data[0]));
        return 4;
    }
    common::log(boost::str(boost::format("【登录】玩家进入 [%1%]") % db.get_code(version, "Town", town_map_id)["text"]));

    // 8. chat
    Json::Value config;
    std::istringstream(db.get_config(user_id.c_str(), "Chat")) >> config;
    std::string message = config[rand() % config.size()].asString();
    this->Mod_Chat_Base_chat_with_players(1, common::gbk2utf(message));
    common::log(boost::str(boost::format("【世界聊天】%1%") % message));

    return 0;
}

//============================================================================
// R170
// 登录
// {module:0, action:0,
// request:[Utils.StringUtil, Utils.StringUtil, Utils.StringUtil, Utils.StringUtil, Utils.IntUtil, Utils.StringUtil, Utils.IntUtil, Utils.ByteUtil, Utils.StringUtil, Utils.StringUtil],
// response:[Utils.UByteUtil, Utils.IntUtil, Utils.UByteUtil, Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.ByteUtil, Utils.UByteUtil, Utils.IntUtil]}
// PlayerData.as 205:
//    public function login(param1:Array) : void
//    {
//        this.logined = param1[0];
//        this.playerId = param1[1];
//        this.playerInfo.id = this.playerId;
//        this.enableTime = param1[3] || 0;
//        this.enableTime = DateTime.formatServerTimeNull(this.enableTime);
//        this.enableType = param1[4] || 0;
//        PlayerInfo.bAllActivity = param1[5] == 0;
//        this.isOpenMusic = param1[6] == 1;
//        WarType.WarEffectShowType = param1[7];
//        this.uiSetValue = param1[8];
//        return;
//    }// end function
//============================================================================
Json::Value sxd_client::Mod_Player_Base_login(const std::string& player_name, const std::string& hash_code, const std::string& time, const std::string& source, int regdate, const std::string& id_card, int open_time, char is_newst, const std::string& stage, const std::string& client) {
    Json::Value data;
    data.append(player_name);
    data.append(hash_code);
    data.append(time);
    data.append(source);
    data.append(regdate);
    data.append(id_card);
    data.append(open_time);
    data.append(is_newst);
    data.append(stage);
    data.append(client);
    return this->send_and_receive(data, 0, 0);
}

//============================================================================
// R170
// 玩家基本信息
// {module:0, action:2,
// request:[],
// response:[Utils.StringUtil, Utils.IntUtil, Utils.IntUtil, Utils.LongUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.LongUtil, Utils.LongUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.StringUtil, Utils.StringUtil]}
// PlayerData.as 799:
//     oObject.list(param1, this, ["nickname", "level", "ingot", "coins", "health", "maxHealth", "power", "experience", "maxExperience", "townMapId", "bossMapId", "jihuisuoId", "mounts", "mountsCD", "vipLevel", "avatar", "avatarCD", "mainPlayerRoleId", "mainRoleId", "campId", "townKey", "fame", "fameLevel", "extraPower", "maxExtraPower", "netBarPower", "maxNetBarPower", "tester", "lastPlayVersion", "missionKey", "startAccount", "stLevel", "daoYuan", "exploit", "isHaveTangYuan", "stageName", "full_server_name"]);
//============================================================================
Json::Value sxd_client::Mod_Player_Base_get_player_info() {
    Json::Value data;
    return this->send_and_receive(data, 0, 2);
}

//============================================================================
// R170
// 玩家排名信息
// {module:0, action:48,
// request:[Utils.IntUtil],
// response:[[Utils.IntUtil, Utils.IntUtil, Utils.StringUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil]]}
// PlayerData.as 408:
//     _loc_3.playerId = _loc_2[0];
//     _loc_3.rankIng = _loc_2[1];
//     _loc_3.factionName = _loc_2[2];
//     _loc_3.combat = _loc_2[3];
//     _loc_3.fame = _loc_2[4];
//     _loc_3.skill = _loc_2[5];
//     _loc_3.achievmentPoints = _loc_2[6];
//     _loc_3.firstAttack = _loc_2[7];
//     _loc_3.statePoint = _loc_2[8];
//     _loc_3.flowerCount = _loc_2[9];
//     _loc_3.xianLing = _loc_2[10];
//============================================================================
Json::Value sxd_client::Mod_Player_Base_player_info_contrast(int player_id) {
    Json::Value data;
    data.append(player_id);
    return this->send_and_receive(data, 0, 48);
}

//============================================================================
// R170
// 玩家已开通功能
// {module:0, action:6,
// request:[],
// response:[[Utils.IntUtil, Utils.ByteUtil]]}
// PlayerData.as 1169:
//     private function format_get_player_function(param1:Array) : Array
//     {
//         var _loc_3:* = null;
//         var _loc_4:* = null;
//         param1 = param1[0];
//         var _loc_2:* = [];
//         for (_loc_3 in param1)
//         {
//
//             _loc_4 = {};
//             oObject.list(param1[_loc_3], _loc_4, ["id", "isPlayed"]);
//             _loc_2.push(_loc_4);
//         }
//         return _loc_2;
//     }// end function
//============================================================================
Json::Value sxd_client::Mod_Player_Base_get_player_function() {
    Json::Value data;
    return this->send_and_receive(data, 0, 6);
}

//============================================================================
// R171
// {module:5, action:2,
// request:[Utils.IntUtil],
// response:[Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.StringUtil, Utils.StringUtil, Utils.IntUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ShortUtil, [Utils.StringUtil, Utils.IntUtil, Utils.IntUtil, Utils.LongUtil, Utils.LongUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.ShortUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.UByteUtil, [Utils.IntUtil], [Utils.IntUtil], Utils.UByteUtil, Utils.ShortUtil, Utils.ShortUtil, Utils.ShortUtil, Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.ShortUtil, Utils.ShortUtil, Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.ShortUtil]]}
// RoleMsgData.as 95:
//     oObject.list(param1, _loc_2, ["main_player_role_id", "eqId", "player_id", "sport_ranking", "listener_count", "flower_count", "fame", "faction_name", "max_nickname", "max_flower_count", "player_pet_animal_lv", "player_pet_animal_stage", "sigilId", "become_immortal_level", "role_list"]);
//     for each (_loc_7 in _loc_2.role_list)
//         oObject.list(_loc_7, _loc_6, ["name", "roleId", "playerRoleId", "exp", "expMax", "lv", "jobId", "role_stunt_id", "pShengMing", "pShengMing", "pWuLi", "pJueJi", "pFaShu", "fatePower", "isdeploy", "toTransList", "beTransList", "has_excl_arti", "stId", "goldenDanId", "suit_equip_id"]);
// Example
//     [ 803481, 1335, 355546, 2869, 12, 94, 1895779, "\u5ea6\u6708\u5982\u5e74", "\u7504\u6590\u6590.s1", 93, 9, 6, 0, 0,
//         [ [ "\u5c06\u81e3", 8, 803510, 1565127684, 3087000000, 162, 5, 71, 64560, 64560, 120, 100, 100, 19752, 0, null, null, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ "\u695a\u695a", 9, 803485, 1554773331, 2975400000, 161, 2, 7, 29806, 29806, 60, 60, 50, 0, 2, null, null, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ "\u9b54\u5973\u591c\u9b45", 16, 804682, 198100960, 205898132, 95, 5, 72, 25340, 25340, 90, 125, 50, 0, 1, [ [ 70 ] ], null, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ "\u90ce\u68ee\u4e09.s1", 104, 803481, 1679568120, 3087000000, 162, 3, 181, 391518, 391518, 896, 853, 581, 70923, 0, null, null, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ "\u5c0f\u4ed9\u7ae5", 23, 803561, 1561503932, 3087000000, 162, 6, 16, 31270, 31270, 70, 175, 375, 836, 0, null, null, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ "\u795e\u6768\u622c", 70, 808183, 1679568120, 3087000000, 162, 5, 166, 380420, 380420, 928, 912, 719, 147461, 0, null, [ [ 16 ] ], 3, 2, 0, 0, 5426, 9, 9, 50, 0, 48, 50, 0, 34, 50, 0, 9, 50, 0, 3606, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ "\u5f20\u9ebb\u5b50", 22, 803482, 8777164, 31330000, 86, 2, 70, 21518, 21518, 70, 50, 50, 490, 1, null, null, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ "\u963f\u5bbd", 29, 803537, 1556173550, 3087000000, 162, 1, 74, 43436, 43436, 85, 80, 60, 34, 0, null, null, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_Role_Base_get_role_list(int player_id) {
    Json::Value data;
    data.append(player_id);
    return this->send_and_receive(data, 5, 2);
}

//============================================================================
// R171
// 进入城镇
// {module:1, action:0,
// request:[Utils.IntUtil],
// response:[Utils.UByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, [Utils.UByteUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ShortUtil], Utils.StringUtil, Utils.ShortUtil, Utils.ShortUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.UByteUtil, Utils.ByteUtil, Utils.IntUtil, Utils.StringUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, [Utils.IntUtil], Utils.UByteUtil, [Utils.IntUtil], Utils.UByteUtil, Utils.ShortUtil, Utils.UByteUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.ShortUtil, Utils.StringUtil, Utils.IntUtil, Utils.ShortUtil, Utils.StringUtil, Utils.IntUtil]}
// TownData.as 53:
//     this.result = param1.shift();
//     if (this.result != Mod_Town_Base.SUCCESS)
//     {
//         this.entownList[param1[0]] = param1[0];
//         return;
//     }
//     var _loc_3:* = {};
//     oObject.list(param1, _loc_3, ["player_id", "role_id", "follow_role_id", "follow_pet_list", "nickname", "position_x", "position_y", "transport", "avatar", "camp_id", "equip_item_id", "warState", "practice_status", "is_on_mission_practice", "faction_id", "faction_name", "is_star", "is_world_war_top", "player_pet_animal_lv", "player_pet_animal_stage", "world_faction_war_award", "playable_video_list", "hidden_town_npc_flag", "show_town_npc_list", "is_become_immortal", "suit_equip_id", "is_become_saint", "mount_rune_type_id", "mount_rune_is_show", "card_spirit_id", "card_spirit_nickname", "orange_equipment_follow_id", "children_role_id", "children_nickname", "children_suit_id"]);
// Mod_Town_Base.as 48:
//     public static const SUCCESS:int = 41;
//============================================================================
Json::Value sxd_client::Mod_Town_Base_enter_town(int town_map_id) {
    Json::Value data;
    data.append(town_map_id);
    return this->send_and_receive(data, 1, 0);
}

//============================================================================
// R171
// 获取仙桃信息
// {module:40, action:3,
// request:[],
// response:[Utils.ByteUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ByteUtil]}
// GetPeachController.as 34:
//     _loc_2.peachLv = _loc_1[0];
//     _loc_2.fruitLv = 70 + _loc_2.peachLv * 5;
//     _loc_2.peachNum = _loc_1[1];
//     _loc_2.bAllGet = _loc_1[2] == 1;
//     _loc_2.bCallPeach = _loc_1[3] == 1;
//============================================================================
// Example
//     [ 21, 5, 1, 0 ]
Json::Value sxd_client::Mod_GetPeach_Base_peach_info() {
    Json::Value data;
    return this->send_and_receive(data, 40, 3);
}

//============================================================================
// R171
// 一键摘桃
// {module:40, action:5,
// request:[],
// response:[Utils.UByteUtil, Utils.LongUtil]}
// GetPeachData.as 71:
//     this.batchGetPeachResult = param1[0];
//     this.warExp = param1[1];
// Mod_GetPeach_Base.as 7:
//     public static const SUCCESS:int = 0;
// Example
//     [ 0, 350000000 ]
//============================================================================
Json::Value sxd_client::Mod_GetPeach_Base_batch_get_peach() {
    Json::Value data;
    return this->send_and_receive(data, 40, 5);
}

//============================================================================
// R171
// 药园土地列表
// {module:13, action:0,
// request:[],
// response:[[Utils.IntUtil, Utils.IntUtil, Utils.StringUtil, Utils.StringUtil, Utils.IntUtil, Utils.LongUtil, Utils.LongUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.UByteUtil, Utils.UByteUtil, Utils.ByteUtil], Utils.IntUtil]}
// FarmData.as 258:
//     for each (_loc_2 in param1[0])
//         oObject.list(_loc_2, _loc_6, [
//         0"farmland_id", 1"player_role_id", 2"player_role_sign", 3"player_role_name", 4"player_role_level",
//         5"player_role_experience", 6"max_experience", 7"herbs_id", 8"farmland_time", 9"farmland_level",
//         10"is_plant", 11"herbs_type", 12"isNetBar"]);
// Example
//       0     1      2               3                     4   5       6        7   8      9  10 11 12
// 空土地
//     [ 8328, 0,     "",             "",                   0,  0,      0,       0,  0,     1, 1, 2, 0 ]
// 空土地（红）
//     [ 111186, 0,   "",             "",                   0,  0,      0,       0,  0,     2, 1, 2, 0 ]
// 发财树（可收获）
//     [ 8328, 38729, "JinMingCheng", "\u91d1\u660e\u6210", 46, 51185,  1656640, 31, 0,     1, 0, 3, 0 ]
// 经验树（可收获）
//     [ 8328, 43395, "XiaoXianTong", "\u5c0f\u4ed9\u7ae5", 43, 207634, 1220700, 1,  0,     1, 0, 2, 0 ]
// CD
//     [ 8328, 0,     "",             "",                   0,  0,      0,       0,  28779, 1, 1, 2, 0 ]
//============================================================================
Json::Value sxd_client::Mod_Farm_Base_get_farmlandinfo_list() {
    Json::Value data;
    return this->send_and_receive(data, 13, 0);
}

//============================================================================
// R171
// 收获
// {module:13, action:6,
// request:[Utils.IntUtil, Utils.UByteUtil],
// FarmNewView.as 883:
//     this._data.call(Mod_Farm_Base.harvest, callBack, [intfarmlandId, isDouble ? (Mod_Farm_Base.IS_DOUBLE) : (Mod_Farm_Base.NO_DOUBLE)]);
// Mod_Farm_Base.as 21:
//     public static const IS_DOUBLE:int = 14;
//     public static const NO_DOUBLE:int = 15;
// Example
//
// response:[Utils.UByteUtil, Utils.StringUtil, Utils.StringUtil, Utils.LongUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.UByteUtil]}
// FarmData.as 463:
//     oObject.list(param1, this._objHarvest, ["result", "role_name", "herbs_name", "experience", "level", "coin", "xianling", "farmland_time", "herbs_type"]);
// Mod_Farm_Base.as 15:
//     public static const SUCCESS:int = 8;
// Example
// 收获发财树
//     [ 8, "\u91d1\u660e\u6210", "\u666e\u901a\u53d1\u8d22\u6811", 0, 0, 116400, 0, 0, 3 ]
// 收获经验树
//     [ 8, "\u4f55\u4ed9\u59d1", "\u666e\u901a\u66fc\u9640\u7f57", 79650000, 127, 0, 0, 28800, 2 ]
//============================================================================
Json::Value sxd_client::Mod_Farm_Base_harvest(int land_id, bool is_double) {
    Json::Value data;
    data.append(land_id);
    if (is_double)
        data.append(14);
    else
        data.append(15);
    return this->send_and_receive(data, 13, 6);
}

//============================================================================
// R171
// 获取仙露信息
// {module:13, action:12,
// request:[],
// response:[Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil]}
// FarmData.as 498:
//     oObject.list(param1, this._objBuyCoinTreeCountInfo, ["coin_tree_count", "remain_buy_times", "ingot", "recorve_cd"]);
// Example
//     [ 5, 18, 2, 3228 ]
//============================================================================
Json::Value sxd_client::Mod_Farm_Base_buy_coin_tree_count_info() {
    Json::Value data;
    return this->send_and_receive(data, 13, 12);
}

//============================================================================
// R171
// 获取种植伙伴信息
// {module:13, action:1,
// request:[],
// response:[[Utils.IntUtil, Utils.StringUtil, Utils.StringUtil, Utils.IntUtil, Utils.LongUtil, Utils.LongUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.UByteUtil]]}
// FarmData.as 310:
//     oObject.list(_loc_3, _loc_4, ["player_role_id", "player_role_sign", "player_role_name", "player_role_level", "player_role_experience", "max_experience", "strength", "agile", "intellect", "player_role_state"]);
// Example
//     [ [ [ 781081, "ShenFeiWei", "\u795e\u98de\u536b", 254, 7665338485, 18533600000, 1635, 1347, 1103, 5 ], [ 778995, "ShenHeXianGu", "\u795e\u4f55\u4ed9\u59d1", 245, 7659558033, 17321200000, 650, 715, 1174, 6 ], [ 789569, "XianDouZhanShengFo", "\u4ed9\u6597\u6218\u80dc\u4f5b", 225, 11076515247, 13857200000, 1139, 844, 513, 6 ], [ 792312, "XianXiMenXiaoChui", "\u4ed9\u897f\u95e8\u5c0f\u5439", 210, 306331572, 11259200000, 1088, 947, 636, 6 ], [ 625246, "JuMang", "\u53e5\u8292", 51, 1103834, 2651332, 174, 162, 125, 7 ], [ 625244, "RanKui", "\u71c3\u9b41", 73, 10412992, 14145340, 240, 127, 101, 7 ], [ 622372, "AKuan", "\u963f\u5bbd", 39, 244581, 787306, 115, 96, 85, 7 ], [ 805731, "ShenZhuRong", "\u795e\u795d\u878d", 151, 558415168, 1145800000, 1000, 700, 500, 7 ], [ 668656, "ShenYangJian", "\u795e\u6768\u622c", 202, 6579947569, 9873600000, 680, 575, 537, 7 ], [ 641492, "XianNieXiaoQian", "\u4ed9\u8042\u5c0f\u5029", 92, 95413644, 112146500, 317, 375, 445, 7 ], [ 630550, "XiaoShiMei", "\u5c0f\u5e08\u59b9", 65, 1405730, 8159360, 286, 270, 264, 7 ], [ 622361, "JiangChen", "\u5c06\u81e3", 94, 102630882, 195209840, 181, 141, 103, 7 ], [ 799493, "ZhuBaJie", "\u4ed9\u732a\u516b\u6212", 1, 0, 240, 150, 150, 150, 7 ], [ 623133, "XiaoXianTong", "\u5c0f\u4ed9\u7ae5", 41, 883902, 984272, 106, 118, 167, 7 ], [ 630549, "XianMoWangXingTian", "\u4ed9\u9b54\u738b\u5211\u5929", 175, 38114972, 5197200000, 340, 367, 240, 7 ], [ 630547, "MoNvYeMei", "\u9b54\u5973\u591c\u9b45", 143, 63282744, 583200000, 415, 376, 316, 7 ], [ 649624, "WanYaoHuang", "\u5929\u5996\u661f", 162, 787240553, 3087000000, 521, 383, 271, 7 ], [ 807857, "LingLong", "\u73b2\u73d1", 157, 1748825961, 2058000000, 600, 800, 300, 7 ], [ 622354, "ChuChu", "\u695a\u695a", 27, 82441, 154880, 85, 82, 66, 7 ], [ 657675, "WuDaoJiangJun", "\u4e94\u9053\u5c06\u519b", 95, 183357431, 205898132, 371, 421, 218, 7 ], [ 641494, "MoJiangWuLuo", "\u9b54\u5c06\u6b66\u7f57", 88, 23665909, 35113406, 368, 315, 242, 7 ], [ 787574, "HuoBanFangZiQing", "\u65b9\u5b50\u6674", 200, 1064665923, 9527200000, 715, 785, 1189, 7 ], [ 625381, "TianQi", "\u5929\u6ce3", 68, 2186891, 10094960, 267, 255, 222, 7 ], [ 657674, "XianMengPo", "\u4ed9\u5b5f\u5a46", 156, 174038929, 1983600000, 322, 562, 532, 7 ], [ 622351, "ShenZhangMaZi", "\u795e\u5f20\u9ebb\u5b50", 195, 1135352389, 8661200000, 514, 495, 383, 7 ], [ 676203, "LuoHuWang", "\u7f57\u864e\u738b", 156, 1156550737, 1983600000, 627, 478, 460, 7 ], [ 778348, "XianHuangFeiHu", "\u4ed9\u9ec4\u98de\u864e", 185, 3812902548, 6929200000, 1035, 693, 315, 7 ], [ 622371, "JinMingCheng", "\u91d1\u660e\u6210", 34, 364971, 426560, 65, 55, 60, 7 ], [ 812032, "ShengGongGong", "\u5723\u5171\u5de5", 254, 7665338485, 18533600000, 2314, 1958, 1484, 5 ], [ 783896, "ShengXiongMaoJiuXian", "\u5723\u718a\u732b\u9152\u4ed9", 254, 7665338485, 18533600000, 1455, 1064, 647, 5 ], [ 784781, "ShenShiJiuMei", "\u795e\u5341\u4e5d\u59b9", 254, 7665338485, 18533600000, 1813, 1551, 1124, 5 ], [ 798648, "ShengXuanYuanHuangDi", "\u5723\u8f69\u8f95\u9ec4\u5e1d", 254, 7665338485, 18533600000, 1520, 1350, 891, 5 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_Farm_Base_get_player_roleinfo_list() {
    Json::Value data;
    return this->send_and_receive(data, 13, 1);
}

//============================================================================
// R171
// 种植
// {module:13, action:5,
// request:[Utils.IntUtil, Utils.IntUtil, Utils.UByteUtil, Utils.ByteUtil],
// FarmNewView.as 1255:
//     _data.call(Mod_Farm_Base.plant_herbs, plantCallBack, [aryFieldId[intIndex], intPlayRoleId, intType, herbs_star_level]);
//     herbs_star_level: 1:普通, 2:优秀, 3:精良, 4:传奇, 5:神话
// response:[Utils.UByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.StringUtil, Utils.StringUtil, Utils.IntUtil, Utils.LongUtil, Utils.LongUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.UByteUtil, Utils.UByteUtil, Utils.ByteUtil]}
// FarmData.as 430:
//     oObject.list(param1, this._objPlantHerbs, ["result", "farmland_id", "player_role_id", "player_role_sign", "player_role_name", "player_role_level", "player_role_experience", "max_experience", "herbs_id", "farmland_level", "coin_tree_count", "xianling_tree_count", "is_plant", "herbs_type", "isNetBar"]);
// Example
//     [ 8, 147042, 808834, "ShenYangJian", "\u795e\u6768\u622c", 161, 2302294048, 2975400000, 35, 1, 4, 0, 0, 3, 0 ]
//============================================================================
Json::Value sxd_client::Mod_Farm_Base_plant_herbs(int land_id, int play_role_id, int type, int herbs_star_level) {
    Json::Value data;
    data.append(land_id);
    data.append(play_role_id);
    data.append(type);
    data.append(herbs_star_level);
    return this->send_and_receive(data, 13, 5);
}


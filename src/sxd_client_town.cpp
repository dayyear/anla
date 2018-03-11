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
// R170 登录
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
// R170 玩家基本信息
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
// R170 玩家排名信息
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
// R170 玩家已开启功能
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
// R172 助手
// {module:0, action:42, request:[],
// response:[Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, [Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil], [Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil]]};
// PlayerData.as 1260:
//     oObject.list(list, this.getGameAssistantInfo, ["sports_rank", "combat", "fame", "skill", "role_number", "max_role_number", "day_quest_number", "day_quest_finish", "buy_power_chance", "buy_power_value", "buy_power_ingot", "ingot_rune_chance", "camp_salary", "is_get_camp_salary", "free_fate_chance", "incense_chance", "super_sports_chance", "super_sports_cd_time", "takebible_chance", "takebible_arrival_remain", "send_flower_chance", "free_reset_chance", "coin_tree_chance", "buy_coin_tree_count_chance", "achievement_points", "state_point", "roll_cake_count", "worship_mars_times", "buy_fate_npc_times", "peach_times", "is_can_get_stone", "get_stone_state", "pet_animal_times", "back_times_list", "active_degree_list"]);
//============================================================================
Json::Value sxd_client::Mod_Player_Base_get_game_assistant_info(){
    Json::Value data;
    return this->send_and_receive(data, 0, 42);
}

//============================================================================
// R172 服务器时间
// {module:0, action:22, request:[], response:[Utils.IntUtil]};
//============================================================================
Json::Value sxd_client::Mod_Player_Base_server_time(){
    Json::Value data;
    return this->send_and_receive(data, 0, 22);
}

//============================================================================
// R171 伙伴列表
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
// R171 进入城镇
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


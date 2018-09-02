#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_Faction_Base {
public:
    static const int ACTION_SUCCESS = 2;
    static const int SUCCESS = 32;
    static const int JOIN_SUCCESS = 54;
};

class Mod_LuckyWheel_Base {
public:
    static const int OK = 0;
    static const int SUCCESS = 2;
};

//============================================================================
// R179 °ïÅÉÃû³Æ
//============================================================================
std::string sxd_client::get_faction_name() {
    auto data = this->Mod_Player_Base_player_info_contrast(player_id);
    return common::utf2gbk(data[0][0][2].asString());
}

//============================================================================
// R179 °ïÅÉ¼ÀÉñ
//============================================================================
void sxd_client::faction_god() {
    for (;;) {
        auto data = this->Mod_Faction_Base_faction_god_info();
        int incense_count = data[3].asInt();
        int total_count = data[4].asInt();
        if (incense_count == total_count)
            break;

        data = this->Mod_Faction_Base_incense();
        if (data[0].asInt() != Mod_Faction_Base::SUCCESS) {
            common::log(boost::str(boost::format("¡¾°ïÅÉ¡¿¸ø°ïÅÉÉñÏñÉÏ [°×Ì´Ïã] Ê§°Ü£¬result[%1%]") % data[0]), iEdit);
            return;
        }
        common::log("¡¾°ïÅÉ¡¿¸ø°ïÅÉÉñÏñÉÏ [°×Ì´Ïã]", iEdit);
    }
}

//============================================================================
// R179 Ãæ°å
// "module":10,"action":22,"request":[],"response":[Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.ByteUtil,[Utils.IntUtil,Utils.StringUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil]]
// FactionController.as 432:
//     oObject.list(this._data.faction.godInfo,_loc1_,["faction_god_level","faction_god_exp","require_exp","incense_count","total_count","is_cover"]);
// Example
//     [ 4, 1085, 16000, 0, 1, 0, [ [ 355553, "\u7504\u6590\u6590.s1", 1, 5, 1528810971 ], [ 355531, "\u4f0d\u4e8c\u4e00.s1", 1, 5, 1528810596 ], [ 355547, "\u9ad8\u5343\u5b57.s1", 1, 5, 1528810767 ], [ 355549, "\u8463\u4e58\u516d.s1", 1, 5, 1528810906 ], [ 355548, "\u548c\u8b07\u66b4.s1", 1, 5, 1528810837 ], [ 355553, "\u7504\u6590\u6590.s1", 1, 5, 1528724533 ], [ 355546, "\u90ce\u68ee\u4e09.s1", 1, 5, 1528810675 ], [ 355527, "\u5e84\u606d\u6625.s1", 1, 5, 1528810531 ] ] ]
//     [ 4, 1090, 16000, 1, 1, 0, [ [ 355553, "\u7504\u6590\u6590.s1", 1, 5, 1528810971 ], [ 355531, "\u4f0d\u4e8c\u4e00.s1", 1, 5, 1528810596 ], [ 355547, "\u9ad8\u5343\u5b57.s1", 1, 5, 1528810767 ], [ 355527, "\u5e84\u606d\u6625.s1", 1, 5, 1528866876 ], [ 355549, "\u8463\u4e58\u516d.s1", 1, 5, 1528810906 ], [ 355548, "\u548c\u8b07\u66b4.s1", 1, 5, 1528810837 ], [ 355546, "\u90ce\u68ee\u4e09.s1", 1, 5, 1528810675 ], [ 355527, "\u5e84\u606d\u6625.s1", 1, 5, 1528810531 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_Faction_Base_faction_god_info() {
    Json::Value data;
    return this->send_and_receive(data, 10, 22);
}

//============================================================================
// R179 ÉÏ°×Ì´Ïã
// "module":10,"action":23,"request":[Utils.IntUtil],"response":[Utils.UByteUtil]
// FactionController.as 457:
//     var _loc1_:Object = {"result":this._data.faction.factionIncense};
// Example
//     [ 32 ]
//============================================================================
Json::Value sxd_client::Mod_Faction_Base_incense() {
    Json::Value data;
    data.append(1);
    return this->send_and_receive(data, 10, 23);
}

//============================================================================
// R179 °ïÅÉ¼ªÐÇ¸ßÕÕ
//============================================================================
void sxd_client::faction_roll_cake() {
    for (;;) {
        auto data = this->Mod_Faction_Base_faction_roll_cake_info();
        int remain_times = data[2].asInt();
        if (remain_times == 0)
            break;

        data = this->Mod_Faction_Base_roll_cake();
        if (data[0].asInt() != Mod_Faction_Base::SUCCESS) {
            common::log(boost::str(boost::format("¡¾°ïÅÉ¡¿¼ªÐÇ¸ßÕÕÊ§°Ü£¬msg[%1%]") % data[0]), iEdit);
            return;
        }
        common::log(boost::str(boost::format("¡¾°ïÅÉ¡¿¼ªÐÇ¸ßÕÕ£¬»ñµÃ [»ý·Ö¡Á%1%]£¬½ñÈÕÀÛ¼Æ [»ý·Ö¡Á%2%]") % data[2] % data[3]), iEdit);
    }
}

//============================================================================
// R179 Ãæ°å
// "module":10,"action":45,"request":[],"response":[Utils.IntUtil,Utils.IntUtil,Utils.ByteUtil,Utils.IntUtil,[Utils.IntUtil,Utils.StringUtil,Utils.IntUtil]]
// FactionController.as 679:
//     _loc2_.weekBestScore = _loc1_[0];
//     _loc2_.todayBestScore = _loc1_[1];
//     _loc2_.remainTimes = _loc1_[2];
//     _loc2_.factionScore = _loc1_[3];
//     _loc2_.factionRCInfo = this.renderFactionRollCakeInfo(_loc1_[4]);
// Example
//
//============================================================================
Json::Value sxd_client::Mod_Faction_Base_faction_roll_cake_info() {
    Json::Value data;
    return this->send_and_receive(data, 10, 45);
}

//============================================================================
// R179 ÖÀ÷»×Ó
// "module":10,"action":46,"request":[],"response":[Utils.UByteUtil,Utils.ByteUtil,Utils.IntUtil,Utils.IntUtil,[Utils.ByteUtil],Utils.ByteUtil]
// FactionController.as 712:
//     _loc2_.msg = _loc1_[0];
//     _loc2_.type = _loc1_[1];
//     _loc2_.score = _loc1_[2];
//     _loc2_.todayScore = _loc1_[3];
//     var _loc3_:Array = _loc1_[4];
//     _loc2_.remainTimes = _loc1_[5];
// Example
//
//============================================================================
Json::Value sxd_client::Mod_Faction_Base_roll_cake() {
    Json::Value data;
    return this->send_and_receive(data, 10, 46);
}

//============================================================================
// R179 ÆßÐÇ·âÄ§
//============================================================================
void sxd_client::seal_satan() {
    this->Mod_Faction_Base_seal_satan_member_list();
    auto data = this->Mod_Faction_Base_join_seal_satan();
    if (data[0].asInt() == Mod_Faction_Base::JOIN_SUCCESS)
        common::log("¡¾°ïÅÉ¡¿¼ÓÈë [ÆßÐÇ·âÄ§]", iEdit);
}

//============================================================================
// R179 Ãæ°å
// "module":10,"action":34,"request":[],"response":[[Utils.IntUtil,Utils.StringUtil,Utils.IntUtil,Utils.IntUtil,Utils.UByteUtil]]
//============================================================================
Json::Value sxd_client::Mod_Faction_Base_seal_satan_member_list() {
    Json::Value data;
    return this->send_and_receive(data, 10, 34);
}

//============================================================================
// R179 ¼ÓÈë
// "module":10,"action":28,"request":[],"response":[Utils.UByteUtil]
// FactionController.as 485:
//     var _loc1_:Object = {"result":this._data.faction.joinSealSatan[0]};
// Example
//     [ 54 ]
//============================================================================
Json::Value sxd_client::Mod_Faction_Base_join_seal_satan() {
    Json::Value data;
    return this->send_and_receive(data, 10, 28);
}

//============================================================================
// R179 °ïÅÉ×ªÅÌ
//============================================================================
void sxd_client::faction_lucky_wheel() {
    auto data = this->Mod_LuckyWheel_Base_open_lucky_wheel();
    int rosefinch_number = data[2].asInt();
    while (rosefinch_number) {
        data = this->Mod_LuckyWheel_Base_start_lucky_wheel();
        if (data[0].asInt() != Mod_LuckyWheel_Base::SUCCESS) {
            common::log(boost::str(boost::format("¡¾°ïÅÉ¡¿×ªÅÌ³é½±Ê§°Ü£¬result[%1%]") % data[0]), iEdit);
            return;
        }
        //int type = data[1].asInt();
        rosefinch_number = data[2].asInt();
        //int item_id = data[3].asInt();
        common::log("¡¾°ïÅÉ¡¿×ªÅÌ³é½±", iEdit);
        //common::log(boost::str(boost::format("¡¾°ïÅÉ¡¿×ªÅÌ³é½±£¬»ñµÃ [%1%]") % db.get_code(version, "item", item_id)["text"]), iEdit);
    }
}

//============================================================================
// R179 Ãæ°å
// "module":69,"action":1,"request":[],"response":[Utils.UByteUtil,Utils.IntUtil,Utils.IntUtil]
// LuckyWheelData.as 26:
//     _loc1_.faction_today_con = this._openInfo[1];
//     _loc1_.rosefinch_number = this._openInfo[2];
// Example
//     [ 0, 9628, 12 ]
//============================================================================
Json::Value sxd_client::Mod_LuckyWheel_Base_open_lucky_wheel() {
    Json::Value data;
    return this->send_and_receive(data, 69, 1);
}

//============================================================================
// R179 ³é½±
// "module":69,"action":2,"request":[],"response":[Utils.UByteUtil,Utils.ByteUtil,Utils.IntUtil,Utils.IntUtil]
// LuckyWheelData.as 39:
//     public function get runState() : int
//         return this._runInfo[0];
//     _loc1_.type = this._runInfo[1];
//     _loc1_.rosefinch_number = this._runInfo[2];
//     _loc1_.item_id = this._runInfo[3];
// Example
//     [ 2, 6, 11, 0 ]
//============================================================================
Json::Value sxd_client::Mod_LuckyWheel_Base_start_lucky_wheel() {
    Json::Value data;
    return this->send_and_receive(data, 69, 2);
}

//============================================================================
// R179 ³ÔÏÉÑç
//============================================================================
void sxd_client::faction_join_feast() {
    auto data = this->Mod_Faction_Base_join_feast();
    if (data[0].asInt() == Mod_Faction_Base::SUCCESS) {
        std::vector<std::string> items;
        std::transform(data[1].begin(), data[1].end(), std::back_inserter(items), [this](const Json::Value& x) {
            return boost::str(boost::format("[%1%¡Á%2%]") % db.get_code(version, "Item", x[0].asInt())["text"] % x[1]);
        });
        common::log(boost::str(boost::format("¡¾°ïÅÉ¡¿³ÔÏÉÑç»ñµÃ£º%1%") % boost::algorithm::join(items, "£¬")), iEdit);
    }
}

//============================================================================
// R179 ³ÔÏÉÑç
// "module":10, "action":53,"request":[],"response":[Utils.UByteUtil,[Utils.IntUtil,Utils.IntUtil]]
// FactionData.as 620:
//     this.factionEatInfo.eatResult = param1[_loc2_++];
//     this.factionEatInfo.award_item_list = this.parseFactionEatAwards(param1[_loc2_++]);
//         this.factionEatInfo.feast_type = param1[_loc2_++];
//         this.factionEatInfo.status = param1[_loc2_++];
//============================================================================
Json::Value sxd_client::Mod_Faction_Base_join_feast() {
    Json::Value data;
    return this->send_and_receive(data, 10, 53);
}

//============================================================================
// ÉóºË
//============================================================================
void sxd_client::faction_approve() {
    auto data = this->Mod_Faction_Base_my_faction_info();
    int faction_id = data[0][0][0].asInt();
    int position = data[0][0][11].asInt();
    if (!position)
        return;
    data = this->Mod_Faction_Base_request_list(faction_id);
    auto request_list = data[0];
    for (const auto& request : request_list) {
        int id = request[0].asInt();
        data = this->Mod_Faction_Base_accept_request(id);
        if (data[0].asInt() == Mod_Faction_Base::ACTION_SUCCESS)
            common::log(boost::str(boost::format("¡¾°ïÅÉ¡¿ÉóºËÍ¨¹ý [%1%(%2%¼¶)] ¼ÓÈë") % common::utf2gbk(request[2].asString()) % request[4]), iEdit);
    }

}

//============================================================================
// R180 °ïÅÉÃæ°å
// "module":10,"action":18,"request":[],"response":[[Utils.IntUtil,Utils.StringUtil,Utils.IntUtil,Utils.StringUtil,Utils.ShortUtil,Utils.ShortUtil,Utils.ShortUtil,Utils.IntUtil,Utils.IntUtil,Utils.StringUtil,Utils.StringUtil,Utils.IntUtil,Utils.StringUtil,Utils.IntUtil,Utils.IntUtil]]
// FactionController.as:
//     _loc2_.factionId = param1[0];
//     _loc2_.factionName = param1[1];
//     _loc2_.masterId = param1[2];
//     _loc2_.master = param1[3];
//     _loc2_.level = param1[4];
//     _loc2_.maxPerson = param1[5];
//     _loc2_.minPerson = param1[6];
//     _loc2_.exp = param1[7];
//     _loc2_.requireExp = param1[8];
//     _loc2_.desc = param1[9];
//     _loc2_.notice = param1[10];
//     _loc2_.position = this.renderJob(param1[11]);    °ïÖÐÖ°Î»
//     _loc2_.groupNumber = param1[12];
//     _loc2_.rank = param1[13];
//     _loc2_.nowWeekCon = param1[14];
// FactionController.as:
//     0:FactionControllerLang.Position,        ³ÉÔ±
//     1:FactionControllerLang.Leader,          °ïÖ÷
//     2:FactionControllerLang.DeputyLeader     ¸±°ïÖ÷
// Example
// ³ÉÔ±
//     [ [ [ 15, "\u9b54\u5bab", 17703, "\u5ea6\u65e5\u5982\u5e74.s152", 9, 28, 8, 55352457, 67176000, "", "", 0, "0", 3, 35556 ] ] ]
// °ïÖ÷
//     [ [ [ 15, "\u9b54\u5bab", 17703, "\u5ea6\u65e5\u5982\u5e74.s152", 9, 28, 7, 55352457, 67176000, "", "", 1, "0", 3, 35556 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_Faction_Base_my_faction_info() {
    Json::Value data;
    return this->send_and_receive(data, 10, 18);
}

//============================================================================
// R180 ÉóºËÃæ°å
// "module":10,"action":12,"request":[Utils.IntUtil],"response":[[Utils.IntUtil,Utils.IntUtil,Utils.StringUtil,Utils.IntUtil,Utils.ShortUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil]]
// FactionController.as:
//     _loc6_.applyId = _loc5_[0];
//     _loc6_.playerId = _loc5_[1];
//     _loc6_.playerName = _loc5_[2];
//     _loc6_.showName = _ctrl.player.removeNickNameSuffix(_loc6_.playerName);
//     _loc6_.factionId = _loc5_[3];
//     _loc6_.playerLevel = _loc5_[4];
//     _loc6_.reqTime = TimeChange.timerInfo(DateTime.formatServerTimeNull(_loc5_[5]),_ctrl.player.serverTime);
//     _loc6_.sex = RoleType.getRoleGender(_loc5_[6]);
//     _loc6_.sportRank = _loc5_[7];
// Example
//     [ 15 ] --> [ [ [ 34806, 15936, "\u5367\u9f99.s152", 15, 47, 1531209999, 3, 2230 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_Faction_Base_request_list(int faction_id) {
    Json::Value data;
    data.append(faction_id);
    return this->send_and_receive(data, 10, 12);
}

//============================================================================
// R180 Í¬Òâ
// "module":10,"action":5,"request":[Utils.IntUtil],"response":[Utils.UByteUtil]
// FactionController.as:
//     _loc2_.msg = _loc1_[0];
// Example
//     [ 34806 ] --> [ 2 ]
//============================================================================
Json::Value sxd_client::Mod_Faction_Base_accept_request(int id) {
    Json::Value data;
    data.append(id);
    return this->send_and_receive(data, 10, 5);
}

class Mod_FactionWar_Base {
public:
    static const int ACTION_SUCCESS = 0;
};

//============================================================================
// °ïÅÉÕ½
//============================================================================
void sxd_client::faction_war() {
    auto data = this->Mod_FactionWar_Base_get_faction_war_list();
    int can_ignup = data[0].asInt();
    int is_signup = data[1].asInt();
    if (can_ignup && !is_signup) {
        data = this->Mod_FactionWar_Base_sign_up();
        if (data[0].asInt() != Mod_FactionWar_Base::ACTION_SUCCESS) {
            common::log(boost::str(boost::format("¡¾°ïÅÉ¡¿°ïÅÉÕ½±¨ÃûÊ§°Ü£¬result[%1%]") % data[0]), iEdit);
            return;
        }
        common::log("¡¾°ïÅÉ¡¿°ïÅÉÕ½±¨Ãû", iEdit);
    }
}

//============================================================================
// R181 Ãæ°å
// "module":29,"action":1,"request":[],"response":[Utils.IntUtil,Utils.IntUtil,[Utils.IntUtil,Utils.IntUtil,Utils.ByteUtil,Utils.IntUtil,Utils.IntUtil]]
// FactionWarMapData.as:
//     this.canSignup = param1[0];
//     this.isSignup = param1[1];
//     for each(_loc2_ in param1[2])
//     {
//        if(_loc2_[2] == 1)
//        {
//           this.warId = _loc2_[0];
//           this.warTownId = _loc2_[1];
//           this.signupCoins = _loc2_[3];
//           this.signupNum = _loc2_[4];
//           break;
//        }
//     }
// Example
// ±¨ÃûÇ°£¨°ïÖ÷£©
//     [ 1, 0, [ [ 3, 19, 0, 100000, 0 ], [ 1, 17, 1, 100000, 6 ], [ 2, 18, 0, 100000, 0 ], [ 4, 20, 0, 100000, 0 ] ] ]
// ±¨Ãûºó£¨°ïÖ÷£©
//     [ 1, 1, [ [ 3, 19, 0, 100000, 0 ], [ 1, 17, 1, 100000, 7 ], [ 2, 18, 0, 100000, 0 ], [ 4, 20, 0, 100000, 0 ] ] ]
// ±¨Ãûºó£¨³ÉÔ±£©
//     [ 0, 1, [ [ 3, 19, 0, 100000, 0 ], [ 1, 17, 1, 100000, 7 ], [ 2, 18, 0, 100000, 0 ], [ 4, 20, 0, 100000, 0 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_FactionWar_Base_get_faction_war_list() {
    Json::Value data;
    return this->send_and_receive(data, 29, 1);
}

//============================================================================
// R181 ±¨Ãû
// "module":29,"action":2,"request":[Utils.IntUtil],"response":[Utils.UByteUtil]
// FactionWarSignUpView.as:
//     private function onSignUp() : void {
//         _view.alert.showYesNoMsg(FactionWarSignUpViewLang.AskSignUp,FactionWarSignUpViewLang.Yes,FactionWarSignUpViewLang.No,function(param1:int):void {
//            if(param1 == AlertButtonType.Yes)
//            {
//               sign_up(1);
//            }
//         });
//      }
// FactionWarMapData.as:
//     this.result = param1[0];
// Example
//     [ 1 ] --> [ 0 ]
//============================================================================
Json::Value sxd_client::Mod_FactionWar_Base_sign_up() {
    Json::Value data;
    data.append(1);
    return this->send_and_receive(data, 29, 2);
}

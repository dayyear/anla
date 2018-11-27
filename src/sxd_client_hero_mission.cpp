#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_HeroMission_Base {
public:
    static const int SUCCESS = 3;
    static const int NEW_WAR_REPORT = 9;
    static const int FINISH = 10;
};

//============================================================================
// R177 英雄扫荡
//============================================================================
void sxd_client::hero_mission() {
    // 没有体力了
    Json::Value data = this->Mod_Player_Base_get_player_info();
    if (!data[6].asInt())
        return;
    // read config
    auto config = atoi(db.get_config(user_id.c_str(), "HeroMission").c_str());
    common::log(boost::str(boost::format("【HeroMission】[%1%]") % config), 0);
    // 55:天河镇, ..., 14:扬州城, 13:不归山, 9:雁门郡, 8:玄霄宫, 7:古道城, 4:天剑宗
    int town_ids[] = { 55, 54, 43, 40, 39, 35, 33, 29, 27, 26, 24, 23, 22, 21, 15, 14, 13, 9, 8, 7, 4 };
    for (int town_id : town_ids) {
        if (town_id < config)
            break;
        data = this->Mod_HeroMission_Base_get_hero_mission_list(town_id);
        Json::Value missions = data[0];
        if (std::none_of(missions.begin(), missions.end(), [](const Json::Value& x) {return x[1].asInt()==1 && x[3].asInt()==1;}))
            continue;
        data = this->Mod_HeroMission_Base_start_practice(town_id, 1, 0);
        if (data[0].asInt() != Mod_HeroMission_Base::SUCCESS) {
            common::log(boost::str(boost::format("【英雄扫荡】扫荡失败，msg[%1%]") % data[0]), iEdit);
            return;
        }
        for (;;) {
            data = this->Mod_HeroMission_Base_quickly();
            if (data[0].asInt() != Mod_HeroMission_Base::NEW_WAR_REPORT)
                break;
        }
        common::log(boost::str(boost::format("【英雄扫荡】扫荡英雄副本[%1%]") % db.get_code(version, "Town", town_id)["text"]), iEdit);
        if (data[0].asInt() != Mod_HeroMission_Base::FINISH)
            return;
    }
}

//============================================================================
// R177 英雄扫荡面板
// "module":35, "action":0, "request":[Utils.ShortUtil], "response":[[Utils.IntUtil,Utils.ByteUtil,Utils.UByteUtil,Utils.ByteUtil],Utils.ByteUtil,Utils.ByteUtil,Utils.ShortUtil]
// HeroMissionController.as 43:
//     _loc1_.AllBuyTimes = _loc2_[1];
//     _loc1_.RemainBuyTimes = _loc2_[2];
//     _loc1_.BuyPrice = _loc2_[3];
//     var _loc3_:Array = _loc2_[0];
//         _loc2_.id = param1[0];
//         _loc2_.isCanChallenge = param1[1];
//         _loc2_.rank = this._rankobj[param1[2]];
//         _loc2_.isFinished = param1[3];
// Example
//     [ [ [ 792, 0, 79, 1 ], [ 793, 0, 79, 1 ], [ 794, 0, 79, 1 ], [ 795, 0, 79, 1 ], [ 796, 0, 79, 1 ], [ 797, 0, 79, 1 ], [ 798, 0, 79, 1 ], [ 799, 1, 79, 1 ], [ 800, 1, 79, 1 ] ], 0, 0, 200 ]
//============================================================================
Json::Value sxd_client::Mod_HeroMission_Base_get_hero_mission_list(int town_id) {
    Json::Value data;
    data.append(town_id);
    return this->send_and_receive(data, 35, 0);
}

//============================================================================
// R177 开始扫荡
// "module":35, "action":2, "request":[Utils.ShortUtil,Utils.ByteUtil,Utils.ByteUtil], "response":[Utils.UByteUtil,Utils.IntUtil]
// HeroPracticeView.as 71:
//     _data.call(Mod_HeroMission_Base.start_practice,this.startPracticeCallBack,[this.showTownId,this._autoNum,this.heroType]);
// HeroMissionController.as 87:
//     _loc2_.msg = _loc1_[0];
//     _loc2_.practiceTime = _loc1_[1];
// Example
//     [ 3, 2 ]
//============================================================================
Json::Value sxd_client::Mod_HeroMission_Base_start_practice(int town_id, int auto_num, int hero_type) {
    Json::Value data;
    data.append(town_id);
    data.append(auto_num);
    data.append(hero_type);
    return this->send_and_receive(data, 35, 2);
}

//============================================================================
// R177 英雄扫荡
// "module":35, "action":7, "request":[], "response":[Utils.UByteUtil]
// HeroMissionController.as 212:
//     _loc2_.msg = _loc1_[0];
// HeroMissionController.as 110:
//     _loc2_.type = _loc1_[0];
//     _loc2_.newWarReport = this.renderNewWar(_loc1_[1]);
//     _loc2_.rankReport = this.renderAwardItem(_loc1_[2]);
//     _loc2_.emptyBagNumber = _loc1_[3];
// Example
// Mod_HeroMission_Base.quickly(35,7)
//     [ 3 ]
// Mod_HeroMission_Base.notify(35,4)
//     [ 9, [ [ [ [ 5, 109440 ], [ 29, 109440 ], [ 70, 109440 ] ], null, 200 ] ], [ [ 799, 16637, 0, 443, [ [ 5, 116430 ], [ 29, 116430 ], [ 70, 116430 ] ], [ [ 1385, 1 ] ], 0, 200, 0, 0 ] ], 12 ]
//============================================================================
Json::Value sxd_client::Mod_HeroMission_Base_quickly() {
    Json::Value data;
    return this->send_and_receive(data, 35, 7, 35, 4);
}

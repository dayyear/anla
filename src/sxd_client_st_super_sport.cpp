#include <boost/format.hpp>
#include <boost/date_time/posix_time/conversion.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_StSuperSport_Base {
public:
    static const int GROUP_TIAN_BANG = 1;
    static const int GROUP_DI_BANG = 2;
    static const int GROUP_FINALLY = 3;
    static const int ST_SUPER_SPORT_WAR_CLOSE = 4;
    static const int ST_SUPER_SPORT_WAR_PREPARE = 5;
    static const int ST_SUPER_SPORT_WAR_16 = 6;
    static const int ST_SUPER_SPORT_WAR_16_COMPLETE = 7;
    static const int ST_SUPER_SPORT_WAR_8 = 8;
    static const int ST_SUPER_SPORT_WAR_8_COMPLETE = 9;
    static const int ST_SUPER_SPORT_WAR_4 = 10;
    static const int ST_SUPER_SPORT_WAR_4_COMPLETE = 11;
    static const int ST_SUPER_SPORT_WAR_2 = 12;
    static const int ST_SUPER_SPORT_WAR_2_COMPLETE = 13;
    static const int ST_SUPER_SPORT_WAR_1 = 14;
    static const int ST_SUPER_SPORT_WAR_1_COMPLETE = 15;
    static const int ST_SUPER_SPORT_WAR_0 = 16;
    static const int ST_SUPER_SPORT_WAR_0_COMPLETE = 17;
    static const int ST_SUPER_SPORT_WAR_START = 18;
    static const int POINT_RACE_FIRST_DAY = 21;
    static const int POINT_RACE_SECOND_DAY = 22;
    static const int POINT_RACE_THIRD_DAY = 23;
    static const int POINT_RACE_FOUR_DAY = 24;
    static const int WAR_RACE = 25;
    static const int YES = 26;
    static const int NO = 27;
    static const int SUCCESS = 28;
};

//============================================================================
// R172 排名奖励
//============================================================================
void sxd_client::get_rank_award(sxd_client* sxd_client_town) {
    // get player is_get_award
    Json::Value data = this->Mod_StSuperSport_Base_get_player_st_super_sport();
    if (data[5] == Mod_StSuperSport_Base::YES)
        return;

    // get player level
    data = sxd_client_town->Mod_Player_Base_get_player_info();
    int level = data[1].asInt();

    // get rank award
    data = this->Mod_StSuperSport_Base_get_rank_award(level);
    if (data[0].asInt() != Mod_StSuperSport_Base::SUCCESS) {
        common::log(boost::str(boost::format("【神魔竞技】领取排名奖励失败，result[%1%]") % data[0]));
        return;
    }
    common::log(boost::str(boost::format("【神魔竞技】领取排名奖励，[铜钱×%1%]，[声望×%2%]，[血脉精华×%3%]") % data[1] % data[2] % data[3]));
}

//============================================================================
// R172 神魔竞技基本信息
// {module:115, action:1, request:[], response:[Utils.ShortUtil, Utils.ShortUtil, Utils.ShortUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.UByteUtil, Utils.ShortUtil]};
// StSuperSportData.as 81:
//     oObject.list(param1, this._getPlayerStSuperSport, ["st_super_sport_rank", "continue_win_times", "remain_challenge_times", "cd_time", "clear_cd_ingot", "is_get_award", "today_buy_times"]);
// Example
//
//============================================================================
Json::Value sxd_client::Mod_StSuperSport_Base_get_player_st_super_sport() {
    Json::Value data;
    return this->send_and_receive(data, 115, 1);
}

//============================================================================
// R172 排名奖励
// {module:115, action:9,
// request:[Utils.ShortUtil],
// StSuperSportInfoView.as 930:
//     _data.call(Mod_StSuperSport_Base.get_rank_award, this.onGetAwardCallBack, [_ctrl.player.level], true, 1);
// Example
//
// response:[Utils.UByteUtil, Utils.IntUtil, Utils.ShortUtil, Utils.ShortUtil]};
// StSuperSportData.as 727:
//     oObject.list(param1, this._rankAward, ["result", "award_coin", "award_fame", "award_blood"]);
// Example
//
//============================================================================
Json::Value sxd_client::Mod_StSuperSport_Base_get_rank_award(int level) {
    Json::Value data;
    data.append(level);
    return this->send_and_receive(data, 115, 9);
}

//============================================================================
// R172 神魔大礼
//============================================================================
void sxd_client::get_score_award() {

    // can get score award
    Json::Value data = this->Mod_StSuperSport_Base_can_get_score_award();
    if (data[0].asInt() != Mod_StSuperSport_Base::YES)
        return;

    // player score award information
    data = this->Mod_StSuperSport_Base_player_score_award_info();
    int self_score = data[0].asInt();
    Json::Value award_info = data[1];

    // player get score award
    Json::Value awards;
    std::istringstream("[[1, \"普通宝箱\", 1100, 5, 20, 300000, 500, 2], [2, \"普通宝箱\", 1300, 8, 50, 500000, 600, 2], [3, \"普通宝箱\", 1500, 10, 100, 1000000, 800, 2], [4, \"普通宝箱\", 1800, 20, 200, 1200000, 1000, 2], [5, \"普通宝箱\", 2000, 30, 300, 1500000, 1200, 2], [6, \"青铜宝箱\", 2200, 50, 500, 2000000, 1500, 3], [7, \"白银宝箱\", 2400, 80, 700, 3000000, 1600, 4], [8, \"黄金宝箱\", 2600, 100, 1000, 5000000, 1800, 5]]") >> awards;
    for (const auto& award : awards) {
        if (award[2].asInt() > self_score)
            break;
        int index = award[0].asInt();
        auto award_info_select = std::find_if(award_info.begin(), award_info.end(), [index](const Json::Value& x) {return x[0].asInt()==index;});
        if (award_info_select == award_info.end()) {
            common::log(boost::str(boost::format("【神魔竞技】领取 [神魔大礼] 数据异常，index[%1%]") % index));
            return;
        }
        if ((*award_info_select)[1] == Mod_StSuperSport_Base::YES)
            continue;
        data = this->Mod_StSuperSport_Base_player_get_score_award(index);
        if (data[0].asInt() != Mod_StSuperSport_Base::SUCCESS) {
            common::log(boost::str(boost::format("【神魔竞技】领取 [神魔大礼] 失败，result[%1%]") % data[0]));
            return;
        }
        common::log(boost::str(boost::format("【神魔竞技】领取 [神魔大礼]，[铜钱×%1%]，[声望×%2%]，[血脉精华×%3%]") % award[5] % award[6] % award[3]));
    }
}

//============================================================================
// R172 神魔大礼？
// {module:115, action:11, request:[], response:[Utils.UByteUtil]};
// StSuperSportData.as 757:
//     this._canGetScoreAward = param1[0] == Mod_StSuperSport_Base.YES;
//============================================================================
Json::Value sxd_client::Mod_StSuperSport_Base_can_get_score_award() {
    Json::Value data;
    return this->send_and_receive(data, 115, 11);
}

//============================================================================
// R172 神魔大礼面板
// {module:115, action:12, request:[], response:[Utils.ShortUtil, [Utils.ByteUtil, Utils.UByteUtil]]};
// StSuperSportData.as 771:
//     oObject.list(param1, this._playerScoreAwardInfo, ["self_score", "award_info"]);
//     for each (_loc_3 in this._playerScoreAwardInfo.award_info)
//         oObject.list(_loc_3, _loc_4, ["award_id", "is_get"]);
// Example
//     [ 1302, [ [ 1, 26 ], [ 5, 27 ], [ 7, 27 ], [ 8, 27 ], [ 4, 27 ], [ 2, 27 ], [ 3, 27 ], [ 6, 27 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_StSuperSport_Base_player_score_award_info() {
    Json::Value data;
    return this->send_and_receive(data, 115, 12);
}

//============================================================================
// R172 领取
// {module:115, action:13, request:[Utils.ByteUtil], response:[Utils.UByteUtil]};
// StSuperSportData.as 791:
//     this._playerGetScoreAwardResult = param1[0];
//============================================================================
Json::Value sxd_client::Mod_StSuperSport_Base_player_get_score_award(int index) {
    Json::Value data;
    data.append(index);
    return this->send_and_receive(data, 115, 13);
}

//============================================================================
// R172 积分赛
//============================================================================
void sxd_client::point_race(sxd_client* sxd_client_town) {

    // get status
    Json::Value data = this->Mod_StSuperSport_Base_get_st_super_sport_status();
    if (data[0].asInt() < Mod_StSuperSport_Base::POINT_RACE_FIRST_DAY || data[0].asInt() > Mod_StSuperSport_Base::POINT_RACE_FOUR_DAY)
        return;
    common::log("【神魔竞技】今日 [积分赛]");

    // get player information
    data = this->Mod_StSuperSport_Base_get_player_st_super_sport();
    common::log(boost::str(boost::format("【神魔竞技】排名 [%1%]，今日还可挑战 [%2%] 次") % data[0] % data[2]));
    if (data[2].asInt() == 0)
        return;
    // cd
    if (data[3].asInt()) {
        common::log("【神魔竞技】CD中...");
        return;
    }

    // challenge list
    data = this->Mod_StSuperSport_Base_challenge_list();
    // 1. sort
    std::vector<Json::Value> challenge_players;
    std::copy(data[0].begin(), data[0].end(), std::back_inserter(challenge_players));
    std::sort(challenge_players.begin(), challenge_players.end(), [](const Json::Value& x, const Json::Value& y) {return x[5].asInt()<y[5].asInt();});
    // 2. filter(AM)
    data = sxd_client_town->Mod_Player_Base_server_time();
    auto server_time = boost::posix_time::from_time_t(data[0].asInt());
    int hour = (std::stoi(common::to_string(server_time, "%H")) + 8) % 24;
    if (hour < 12) {
        //challenge_players.assign(challenge_players.begin(), challenge_players.begin() + 2);
        challenge_players.pop_back();
        challenge_players.pop_back();
        challenge_players.pop_back();
    }
    // 3. filter(is_challenge==Mod_StSuperSport_Base::NO)
    std::vector<Json::Value> challenge_players_valid;
    std::copy_if(challenge_players.begin(), challenge_players.end(), std::back_inserter(challenge_players_valid), [](const Json::Value& x) {return x[6].asInt()==Mod_StSuperSport_Base::NO;});
    if (challenge_players_valid.size() == 0)
        return;

    // challenge
    for (const auto& player : challenge_players_valid) {
        data = this->Mod_StSuperSport_Base_challenge(player[0].asInt());
        if (data[0].asInt() != Mod_StSuperSport_Base::SUCCESS) {
            common::log(boost::str(boost::format("【神魔竞技】挑战 [%1%(%2%)] 失败，result[%3%]") % common::utf2gbk(player[2].asString()) % player[1] % data[0]));
            return;
        }
        if (data[8].asInt()) {
            common::log(boost::str(boost::format("【神魔竞技】挑战 [%1%(%2%)]，战败") % common::utf2gbk(player[2].asString()) % player[1]));
            return;
        }
        common::log(boost::str(boost::format("【神魔竞技】挑战 [%1%(%2%)]，战胜") % common::utf2gbk(player[2].asString()) % player[1]));
    }
}

//============================================================================
// R172 积分赛|神魔大战
// {module:115, action:0, request:[], response:[Utils.UByteUtil]};
// Example
//     积分赛：   [ 21-24 ]
//     神魔大战：[ 25 ]
//============================================================================
Json::Value sxd_client::Mod_StSuperSport_Base_get_st_super_sport_status() {
    Json::Value data;
    return this->send_and_receive(data, 115, 0);
}

//============================================================================
// R172 挑战对手信息
// {module:115, action:2, request:[], response:[[Utils.ByteUtil, Utils.IntUtil, Utils.StringUtil, Utils.ShortUtil, Utils.ShortUtil, Utils.IntUtil, Utils.UByteUtil, Utils.StringUtil, Utils.StringUtil, Utils.ShortUtil], Utils.IntUtil]};
// StSuperSportData.as 110:
//     oObject.list(param1, this._challengeList, ["challenge_players", "next_refresh_time"]);
//     for each (_loc_3 in this._challengeList.challenge_players)
//         oObject.list(_loc_3, _loc_4, ["sequence_id", "st_player_id", "nickname", "role_id", "player_level", "attack_power", "is_challenge", "stage_name", "server_name", "equip_item_id"]);
// Example
//
//============================================================================
Json::Value sxd_client::Mod_StSuperSport_Base_challenge_list() {
    Json::Value data;
    return this->send_and_receive(data, 115, 2);
}

//============================================================================
// R172 挑战
// {module:115, action:3,
// request:[Utils.ByteUtil],
// Example
//
// response:[Utils.UByteUtil, Utils.IntUtil, Utils.ByteUtil, Utils.ShortUtil, [Utils.ByteUtil, Utils.IntUtil, [Utils.IntUtil, Utils.StringUtil, [Utils.IntUtil, Utils.IntUtil, Utils.StringUtil, Utils.StringUtil, Utils.IntUtil, Utils.LongUtil, Utils.LongUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.IntUtil, [Utils.IntUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ShortUtil], Utils.ByteUtil, Utils.ShortUtil, [Utils.ShortUtil], [Utils.ShortUtil], Utils.ShortUtil, Utils.ShortUtil, Utils.ShortUtil, [Utils.ShortUtil], [Utils.ShortUtil], Utils.ShortUtil, Utils.ByteUtil, Utils.LongUtil, Utils.ByteUtil, Utils.IntUtil], [Utils.IntUtil, Utils.IntUtil, Utils.StringUtil, Utils.StringUtil, Utils.IntUtil, Utils.LongUtil, Utils.LongUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.IntUtil, [Utils.IntUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ShortUtil], Utils.ByteUtil, Utils.ShortUtil, [Utils.ShortUtil], [Utils.ShortUtil], Utils.ShortUtil, Utils.ShortUtil, Utils.ShortUtil, [Utils.ShortUtil], [Utils.ShortUtil], Utils.ShortUtil, Utils.ByteUtil, Utils.LongUtil, Utils.ByteUtil, Utils.IntUtil], [Utils.IntUtil, Utils.IntUtil, Utils.StringUtil, Utils.StringUtil, Utils.IntUtil, Utils.LongUtil, Utils.LongUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.IntUtil, [Utils.IntUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ShortUtil], Utils.ByteUtil, Utils.ShortUtil, [Utils.ShortUtil], [Utils.ShortUtil], Utils.ShortUtil, Utils.ShortUtil, Utils.ShortUtil, [Utils.ShortUtil], [Utils.ShortUtil], Utils.ShortUtil, Utils.ByteUtil, Utils.LongUtil, Utils.ByteUtil, Utils.IntUtil], [Utils.IntUtil, Utils.IntUtil, Utils.StringUtil, Utils.StringUtil, Utils.IntUtil, Utils.LongUtil, Utils.LongUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.IntUtil, [Utils.IntUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ShortUtil], Utils.ByteUtil, Utils.ShortUtil, [Utils.ShortUtil], [Utils.ShortUtil], Utils.ShortUtil, Utils.ShortUtil, Utils.ShortUtil, [Utils.ShortUtil], [Utils.ShortUtil], Utils.ShortUtil, Utils.ByteUtil, Utils.LongUtil, Utils.ByteUtil, Utils.IntUtil], [Utils.IntUtil], [Utils.IntUtil, Utils.IntUtil], Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.ShortUtil, Utils.IntUtil, Utils.StringUtil, Utils.IntUtil, Utils.IntUtil, [Utils.ByteUtil, Utils.ByteUtil, Utils.ByteUtil], Utils.IntUtil, [Utils.IntUtil, Utils.ByteUtil], Utils.LongUtil, Utils.LongUtil, [Utils.IntUtil], Utils.ByteUtil, [Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, [Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil]]], [[Utils.ByteUtil, Utils.StringUtil, [Utils.UByteUtil, Utils.StringUtil], [Utils.StringUtil], [Utils.IntUtil, Utils.LongUtil, Utils.IntUtil], [Utils.StringUtil], [Utils.StringUtil], [Utils.StringUtil], [Utils.StringUtil], [Utils.StringUtil]], [Utils.ByteUtil, Utils.ByteUtil, Utils.IntUtil, [Utils.StringUtil], [Utils.IntUtil, Utils.LongUtil, Utils.IntUtil]], [Utils.IntUtil, Utils.IntUtil, Utils.ShortUtil, Utils.ByteUtil, [Utils.UByteUtil, Utils.ShortUtil, Utils.ByteUtil], Utils.StringUtil, [Utils.ShortUtil], [Utils.StringUtil], [Utils.StringUtil], [Utils.ShortUtil], [Utils.StringUtil], Utils.StringUtil, [Utils.ShortUtil], [Utils.StringUtil], Utils.StringUtil, [Utils.ShortUtil], [Utils.StringUtil], Utils.StringUtil, [Utils.ShortUtil], [Utils.StringUtil], Utils.StringUtil, [Utils.IntUtil], [Utils.StringUtil], Utils.IntUtil, Utils.ShortUtil, Utils.LongUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.StringUtil, Utils.IntUtil, Utils.UByteUtil, [Utils.IntUtil, Utils.ShortUtil, [Utils.UByteUtil, Utils.ShortUtil], Utils.StringUtil, [Utils.ShortUtil], [Utils.StringUtil], [Utils.ShortUtil], Utils.StringUtil, [Utils.ShortUtil], Utils.StringUtil, [Utils.ShortUtil], Utils.StringUtil, [Utils.ShortUtil], Utils.StringUtil, [Utils.IntUtil], Utils.IntUtil, Utils.IntUtil, Utils.LongUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.IntUtil, Utils.ByteUtil], [Utils.IntUtil, Utils.StringUtil], [Utils.IntUtil, Utils.LongUtil, Utils.IntUtil], [Utils.UByteUtil, Utils.StringUtil]], [Utils.IntUtil, Utils.IntUtil], [Utils.UByteUtil, Utils.StringUtil, [Utils.UByteUtil, Utils.StringUtil], [Utils.StringUtil], [Utils.StringUtil], [Utils.StringUtil], [Utils.StringUtil], [Utils.StringUtil], [Utils.StringUtil]], [Utils.IntUtil, Utils.LongUtil, Utils.IntUtil], [Utils.IntUtil, Utils.LongUtil, Utils.IntUtil]], Utils.ShortUtil], Utils.ShortUtil, Utils.ShortUtil, Utils.ShortUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.StringUtil, Utils.StringUtil, Utils.ShortUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, [Utils.IntUtil, Utils.IntUtil]]};
// StSuperSportData.as 147:
//     oObject.list(param1, this._challengeResult, ["result", "award_coin", "award_fame", "award_score", "war_result", "st_super_sport_rank", "cur_score", "remain_challenge_times", "cd_time", "clear_cd_ingot", "award_blood_essence", "stage_name", "server_name", "continue_win_times", "award_dao_yuan", "award_gun", "scrapId", "scrapNum", "awardItemId", "awardCount", "war_report_id", "award_item_list"]);
// Example
//
//============================================================================
Json::Value sxd_client::Mod_StSuperSport_Base_challenge(int index) {
    Json::Value data;
    data.append(index);
    return this->send_and_receive(data, 115, 3);
}

//============================================================================
// R172 神魔大战
//============================================================================
void sxd_client::war_race(sxd_client* sxd_client_town) {
    // get status
    Json::Value data = this->Mod_StSuperSport_Base_get_st_super_sport_status();
    if (data[0].asInt() != Mod_StSuperSport_Base::WAR_RACE)
        return;
    common::log("【神魔竞技】今日 [神魔大战]");

    // get race step
    data = this->Mod_StSuperSport_Base_get_race_step();
    if (data[10].asInt()) {
        common::log("【神魔竞技】已下注");
        return;
    }
    int race_step = data[0].asInt();
    // 5, 7, 9 ,11, 13, 15
    if ((race_step - Mod_StSuperSport_Base::ST_SUPER_SPORT_WAR_PREPARE) % 2)
        return;
    if (race_step < Mod_StSuperSport_Base::ST_SUPER_SPORT_WAR_PREPARE)
        return;
    if (race_step > Mod_StSuperSport_Base::ST_SUPER_SPORT_WAR_1_COMPLETE)
        return;

    // get race list
    std::vector<Json::Value> players_all, players_top16, players_top8, players_top4, players_top2, players_top1;
    // 神族
    data = this->Mod_StSuperSport_Base_get_race_list(Mod_StSuperSport_Base::GROUP_TIAN_BANG);
    for (const auto& player : data[1]) {
        Json::Value temp1, temp2;
        for (int i = 0; i < 14; i++)
            temp1.append(player[i]);
        players_all.push_back(temp1);
        for (int i = 14; i < 28; i++)
            temp2.append(player[i]);
        players_all.push_back(temp2);
    }
    for (const auto& player : data[3]) {
        int player_id = player[0].asInt();
        players_top16.push_back(*std::find_if(players_all.begin(), players_all.end(), [player_id](const Json::Value& x) {return x[0].asInt()==player_id;}));
    }
    for (const auto& player : data[4]) {
        int player_id = player[0].asInt();
        players_top8.push_back(*std::find_if(players_all.begin(), players_all.end(), [player_id](const Json::Value& x) {return x[0].asInt()==player_id;}));
    }
    for (const auto& player : data[5]) {
        int player_id = player[0].asInt();
        players_top4.push_back(*std::find_if(players_all.begin(), players_all.end(), [player_id](const Json::Value& x) {return x[0].asInt()==player_id;}));
    }
    for (const auto& player : data[6]) {
        int player_id = player[0].asInt();
        players_top2.push_back(*std::find_if(players_all.begin(), players_all.end(), [player_id](const Json::Value& x) {return x[0].asInt()==player_id;}));
    }
    {
        int player_id = data[7].asInt();
        players_top1.push_back(*std::find_if(players_all.begin(), players_all.end(), [player_id](const Json::Value& x) {return x[0].asInt()==player_id;}));
    }

    // 魔族
    data = this->Mod_StSuperSport_Base_get_race_list(Mod_StSuperSport_Base::GROUP_DI_BANG);
    for (const auto& player : data[1]) {
        Json::Value temp1, temp2;
        for (int i = 0; i < 14; i++)
            temp1.append(player[i]);
        players_all.push_back(temp1);
        for (int i = 14; i < 28; i++)
            temp2.append(player[i]);
        players_all.push_back(temp2);
    }
    for (const auto& player : data[3]) {
        int player_id = player[0].asInt();
        players_top16.push_back(*std::find_if(players_all.begin(), players_all.end(), [player_id](const Json::Value& x) {return x[0].asInt()==player_id;}));
    }
    for (const auto& player : data[4]) {
        int player_id = player[0].asInt();
        players_top8.push_back(*std::find_if(players_all.begin(), players_all.end(), [player_id](const Json::Value& x) {return x[0].asInt()==player_id;}));
    }
    for (const auto& player : data[5]) {
        int player_id = player[0].asInt();
        players_top4.push_back(*std::find_if(players_all.begin(), players_all.end(), [player_id](const Json::Value& x) {return x[0].asInt()==player_id;}));
    }
    for (const auto& player : data[6]) {
        int player_id = player[0].asInt();
        players_top2.push_back(*std::find_if(players_all.begin(), players_all.end(), [player_id](const Json::Value& x) {return x[0].asInt()==player_id;}));
    }
    {
        int player_id = data[7].asInt();
        players_top1.push_back(*std::find_if(players_all.begin(), players_all.end(), [player_id](const Json::Value& x) {return x[0].asInt()==player_id;}));
    }

    /*for (const auto& player : players_all)
     common::log(boost::str(boost::format("【神魔竞技】[%1%(%2%)]，[战力%3%]") % common::utf2gbk(player[1].asString()) % player[0] % player[3]));
     common::log("【16强】");
     for (const auto& player : players_top16)
     common::log(boost::str(boost::format("【神魔竞技】[%1%(%2%)]，[战力%3%]") % common::utf2gbk(player[1].asString()) % player[0] % player[3]));
     common::log("【8强】");
     for (const auto& player : players_top8)
     common::log(boost::str(boost::format("【神魔竞技】[%1%(%2%)]，[战力%3%]") % common::utf2gbk(player[1].asString()) % player[0] % player[3]));
     common::log("【4强】");
     for (const auto& player : players_top4)
     common::log(boost::str(boost::format("【神魔竞技】[%1%(%2%)]，[战力%3%]") % common::utf2gbk(player[1].asString()) % player[0] % player[3]));
     common::log("【2强】");
     for (const auto& player : players_top2)
     common::log(boost::str(boost::format("【神魔竞技】[%1%(%2%)]，[战力%3%]") % common::utf2gbk(player[1].asString()) % player[0] % player[3]));
     common::log("【1强】");
     for (const auto& player : players_top1)
     common::log(boost::str(boost::format("【神魔竞技】[%1%(%2%)]，[战力%3%]") % common::utf2gbk(player[1].asString()) % player[0] % player[3]));*/

    // select
    std::vector<Json::Value>::iterator player_select;
    switch (race_step) {
    case Mod_StSuperSport_Base::ST_SUPER_SPORT_WAR_PREPARE:
        std::sort(players_all.begin(), players_all.end(), [](const Json::Value& x, const Json::Value& y) {return x[3].asInt()>y[3].asInt();});
        player_select = players_all.begin();
        break;
    case Mod_StSuperSport_Base::ST_SUPER_SPORT_WAR_16_COMPLETE:
        std::sort(players_top16.begin(), players_top16.end(), [](const Json::Value& x, const Json::Value& y) {return x[3].asInt()>y[3].asInt();});
        player_select = players_top16.begin();
        break;
    case Mod_StSuperSport_Base::ST_SUPER_SPORT_WAR_8_COMPLETE:
        std::sort(players_top8.begin(), players_top8.end(), [](const Json::Value& x, const Json::Value& y) {return x[3].asInt()>y[3].asInt();});
        player_select = players_top8.begin();
        break;
    case Mod_StSuperSport_Base::ST_SUPER_SPORT_WAR_4_COMPLETE:
        std::sort(players_top4.begin(), players_top4.end(), [](const Json::Value& x, const Json::Value& y) {return x[3].asInt()>y[3].asInt();});
        player_select = players_top4.begin();
        break;
    case Mod_StSuperSport_Base::ST_SUPER_SPORT_WAR_2_COMPLETE:
        std::sort(players_top2.begin(), players_top2.end(), [](const Json::Value& x, const Json::Value& y) {return x[3].asInt()>y[3].asInt();});
        player_select = players_top2.begin();
        break;
    case Mod_StSuperSport_Base::ST_SUPER_SPORT_WAR_1_COMPLETE:
        std::sort(players_top1.begin(), players_top1.end(), [](const Json::Value& x, const Json::Value& y) {return x[3].asInt()>y[3].asInt();});
        player_select = players_top1.begin();
        break;
    default:
        // impossible
        break;
    }

    // bet
    int player_id_select = (*player_select)[0].asInt();
    std::string player_name_select = common::utf2gbk((*player_select)[1].asString());
    data = this->Mod_StSuperSport_Base_bet(player_id_select);
    if (data[0].asInt() != Mod_StSuperSport_Base::SUCCESS) {
        common::log(boost::str(boost::format("【神魔竞技】下注 [%1%(%2%)] 失败，result[%3%]") % player_name_select % player_id_select % data[0]));
        return;
    }
    common::log(boost::str(boost::format("【神魔竞技】下注 [%1%(%2%)]") % player_name_select % player_id_select));
}

//============================================================================
// R172 神魔大战比赛阶段
// {module:115, action:30, request:[],
// response:[Utils.UByteUtil, Utils.UByteUtil, Utils.IntUtil, Utils.IntUtil, [Utils.UByteUtil, Utils.IntUtil], Utils.IntUtil, Utils.IntUtil, Utils.UByteUtil, Utils.StringUtil, [Utils.StringUtil, Utils.StringUtil], Utils.IntUtil]};
// StSuperSportData.as 273:
//     oObject.list(param1, _loc_2, ["race_step", "player_step", "war_round", "player_number", "time_list", "cup_race_time_step", "apply_id", "group", "war_zone", "server_list", "bet_player_id"]);
// Example
//     准备：[5,6,0,0,[[6,1511773200],[8,1511776800],[10,1511780400],[12,1511784000],[14,1511787600],[16,1511791200]],600,203957,1,"",[],0]
//     16强：[6,6,0,64,[[6,1511773200],[8,1511776800],[10,1511780400],[12,1511784000],[14,1511787600],[16,1511791200]],600,203957,1,"",[],5417]
//     16强完：[7,6,0,0,[[6,1511773200],[8,1511776800],[10,1511780400],[12,1511784000],[14,1511787600],[16,1511791200]],600,203957,1,"",[],5417]
//============================================================================
Json::Value sxd_client::Mod_StSuperSport_Base_get_race_step() {
    Json::Value data;
    return this->send_and_receive(data, 115, 30);
}

//============================================================================
// R172 神魔大战选手
// {module:115, action:32, request:[Utils.UByteUtil], response:[Utils.UByteUtil, [Utils.IntUtil, Utils.StringUtil, Utils.ShortUtil, Utils.IntUtil, Utils.IntUtil, Utils.ShortUtil, Utils.StringUtil, Utils.StringUtil, Utils.LongUtil, [Utils.ShortUtil, Utils.ShortUtil], Utils.StringUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.UByteUtil, Utils.IntUtil, Utils.StringUtil, Utils.ShortUtil, Utils.IntUtil, Utils.IntUtil, Utils.ShortUtil, Utils.StringUtil, Utils.StringUtil, Utils.LongUtil, [Utils.ShortUtil, Utils.ShortUtil], Utils.StringUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.UByteUtil], [Utils.IntUtil, Utils.IntUtil, Utils.StringUtil], [Utils.IntUtil], [Utils.IntUtil], [Utils.IntUtil], [Utils.IntUtil], Utils.IntUtil, [Utils.IntUtil]]};
// StSuperSportData.as 273:
//     oObject.list(param1, _loc_2, ["race_step", "player_step", "war_round", "player_number", "time_list", "cup_race_time_step", "apply_id", "group", "war_zone", "server_list", "bet_player_id"]);
// Example
//     [1,[[24373,"度日如年.s1",104,7186843,0,250,"心动","s04",2000000,[],"",0,0,20,6395,"金麟龙.s8",100,3890726,0,217,"37wan","37wan_s0273",0,[],"",0,0,20],[4074,"问道.s1",104,3526116,0,250,"心动","s04",0,[],"",0,0,20,204132,"轩辕豹.s1",100,882214,0,149,"心动","s04",0,[],"",0,0,20],[197708,"蓝牙国家.s1",5,7010203,0,250,"心动","s04",0,[],"",0,0,20,4554,"让艳晨爱上我.s1",104,782668,0,125,"心动","s04",0,[],"",0,0,20],[205960,"唐蕊卉.s1",6,767188,0,126,"心动","s04",0,[],"",0,0,20,203964,"高千字.s1",104,478252,0,152,"心动","s04",0,[],"",0,0,20],[5417,"海欲.s6",104,8865013,0,210,"37wan","37wan_s0273",131500000,[],"",0,0,20,205955,"尹晴萱.s3",2,1301225,0,146,"心动","s02",0,[],"",0,0,20],[2917,"硅步万里.s10",100,3044919,0,250,"要玩","yaowan_s0152",0,[],"",0,0,20,14792,"候博.s4",1,1070376,0,142,"心动","s04",0,[],"",0,0,20],[201283,"闻人飞云.s1",2,5300471,0,246,"37wan","37wan_s0273",0,[],"",0,0,20,6508,"金麟龙.s7",5,3223408,0,217,"37wan","37wan_s0273",0,[],"",0,0,20],[203725,"绝色倾城.s1",102,3108479,0,204,"心动","s04",0,[],"",0,0,20,2874,"石红.s10",104,3125595,0,197,"要玩","yaowan_s0152",0,[],"",0,0,20],[8536,"☆弑神者☆.s1",102,5394715,0,250,"7k7k","yx567_s0125",0,[],"",0,0,20,26581,"无可奈何.s1",100,5061826,0,223,"心动","s04",0,[],"",0,0,20],[203966,"甄斐斐.s1",104,397770,0,154,"心动","s04",0,[],"",0,0,20,6511,"金麟龙.s6",1,3319714,0,217,"37wan","37wan_s0273",0,[],"",0,0,20],[7804,"万里挑一.s5",6,5821585,0,205,"7k7k","yx567_s0125",0,[],"",0,0,20,17756,"诸葛义锋.s1",100,1188761,0,162,"7k7k","yx567_s0125",0,[],"",0,0,20],[203962,"伍岚芷.s1",110,516879,0,156,"心动","s04",0,[],"",0,0,20,11083,"强大的蜗牛.s1",104,3392974,0,211,"心动","s04",0,[],"",0,0,20],[10243,"梦幻小紫晶.s1",102,5175558,0,236,"7k7k","yx567_s0125",0,[],"",0,0,20,203957,"庄恭春.s1",5,753066,0,167,"心动","s04",0,[],"",0,0,20],[2112,"牛牛.s3",106,4209741,0,250,"6711","6711_s0160",0,[],"",0,0,20,14969,"lx1026.s1",5,907332,0,134,"心动","s04",0,[],"",0,0,20],[200547,"哥玩地寂寞.s1",104,7018813,0,250,"心动","s04",0,[],"",0,0,20,206601,"白字钦.s1",3,1033570,0,146,"心动","s04",0,[],"",0,0,20],[2833,"春桃1.s4",100,4518472,0,232,"95k","95k_s081",0,[],"",0,0,20,203960,"和謇暴.s1",3,356303,0,151,"心动","s04",0,[],"",0,0,20]],[],[],[],[],[],0,[[2000000]]]
//     [2,[[684,"超级邪剑仙.s4",100,5567983,0,237,"95k","95k_s081",0,[],"",0,0,20,206932,"袁载狱.s1",1,582194,0,110,"37wan","37wan_s0273",0,[],"",0,0,20],[9167,"骨灰玩家丶.s1",2,4571859,0,226,"心动","s04",0,[],"",0,0,20,3270,"奶油糖兔死妈.s4",1,1586823,0,149,"心动","s04",0,[],"",0,0,20],[8147,"李嘯龙.s5",106,7380024,0,250,"7k7k","yx567_s0125",0,[],"",0,0,20,2364,"好无聊.s10",104,5408628,0,220,"要玩","yaowan_s0152",0,[],"",0,0,20],[207059,"舒衣萦.s1",6,397035,0,89,"心动","s04",0,[],"",0,0,20,205754,"诸葛贤圣.s3",102,1169741,0,154,"心动","s02",0,[],"",0,0,20],[201992,"夜凉诚.s1",100,5817810,0,220,"心动","s04",0,[],"",0,0,20,2934,"司马婷娆.s10",106,2390827,0,192,"要玩","yaowan_s0152",0,[],"",0,0,20],[203965,"董乘六.s1",107,391110,0,153,"心动","s04",0,[],"",0,0,20,2224,"马妹汁.s8",102,4312194,0,246,"心动","s05",0,[],"",0,0,20],[200966,"姐玩的寂寞.s1",102,6079973,0,250,"心动","s04",0,[],"",0,0,20,206708,"王丘闪.s1",100,857972,0,130,"37wan","37wan_s0273",0,[],"",0,0,20],[1000,"徐御茂.s1",100,3854562,0,215,"心动","s04",0,[],"",0,0,20,206808,"纪衣妙.s2",109,549625,0,106,"37wan","37wan_s0273",0,[],"",0,0,20],[1237,"江门古天乐.s1",100,7209889,0,217,"心动","s04",5000000,[],"",0,0,20,206177,"欧阳柏岚.s4",6,1393017,0,159,"37wan","37wan_s0273",0,[],"",0,0,20],[22112,"凌风箭羽.s4",108,1221336,0,161,"7k7k","yx567_s0125",0,[],"",0,0,20,7975,"拳王1号.s5",104,4116792,0,250,"7k7k","yx567_s0125",0,[],"",0,0,20],[2187,"圣⊙何仙姑.s3",206,7817505,0,231,"56","56_s024",0,[],"",0,0,20,12618,"云渠青.s9",1,880587,0,142,"心动","s05",0,[],"",0,0,20],[204599,"吕悟茂.s4",104,2477404,0,195,"37wan","37wan_s0273",0,[],"",0,0,20,2821,"黄驹炼.s4",102,3964554,0,222,"95k","95k_s081",0,[],"",0,0,20],[3212,"毕柳薰.s1",106,6497897,0,249,"心动","s04",0,[],"",0,0,20,205700,"欧阳千伟.s1",5,1118576,0,153,"心动","s04",0,[],"",0,0,20],[10585,"苏苏.s3",6,4199984,0,209,"心动","s02",0,[],"",0,0,19,190989,"吕洞宾.s10",6,5313525,0,243,"95k","95k_s081",0,[],"",0,0,20],[8474,"无敌铁砂掌.s1",102,6885750,0,250,"7k7k","yx567_s0125",0,[],"",0,0,20,3426,"阿珂.s9",6,5119910,0,250,"心动","s05",0,[],"",0,0,20],[9890,"霸气剑皇.s2",5,1205008,0,158,"7k7k","yx567_s0125",0,[],"",0,0,20,203963,"郎森三.s1",104,475050,0,153,"心动","s04",0,[],"",0,0,20]],[],[],[],[],[],0,[[2000000]]]
//     [3,[],[[2,243822,"2017112401"]],[],[],[],[],0,[]]
//============================================================================
Json::Value sxd_client::Mod_StSuperSport_Base_get_race_list(int group) {
    Json::Value data;
    data.append(group);
    return this->send_and_receive(data, 115, 32);
}

//============================================================================
// R172 神魔大战下注
// {module:115, action:37,
// request:[Utils.IntUtil],
// Example
//     [5417] "海欲.s6"
// response:[Utils.UByteUtil, Utils.IntUtil]};
// StSuperSportData.as 593:
//     this.result = param1[0];
//     var _loc_2:* = param1[1];
//     this.info.isBet = _loc_2 > 0;
//     this.info.betPlayerId = _loc_2;
// Example
//     [28,5417]
//============================================================================
Json::Value sxd_client::Mod_StSuperSport_Base_bet(int id) {
    Json::Value data;
    data.append(id);
    return this->send_and_receive(data, 115, 37);
}


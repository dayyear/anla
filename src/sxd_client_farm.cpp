#include <thread>
#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_Farm_Base {
public:
    static const int NO_PLANT = 1;
    static const int EXP_HERBS = 2;
    static const int COIN_HERBS = 3;
    static const int XIANLING_HERBS = 4;
    static const int WAR = 5;
    static const int INVITE = 6;
    static const int NOINVITE = 7;
    static const int SUCCESS = 8;
};

void sxd_client::harvest() {
    Json::Value data = this->Mod_Farm_Base_get_farmlandinfo_list();
    Json::Value lands = data[0];
    for (const auto& land : lands) {
        if (land[1].asInt()) {
            // 收获容易卡
            /*this->Mod_Farm_Base_harvest(land[0].asInt());
             common::log("【药园】收获", iEdit);
             std::this_thread::sleep_for(std::chrono::seconds(3));*/
            data = this->Mod_Farm_Base_harvest(land[0].asInt());
            if (data[0].asInt() == Mod_Farm_Base::SUCCESS)
                common::log(boost::str(boost::format("【药园】[%1%] 收获 [%2%]，获得铜钱[%3%]，经验值[%4%]") % common::utf2gbk(data[1].asString()) % common::utf2gbk(data[2].asString()) % data[5] % data[3]), iEdit);
            else
                common::log(boost::str(boost::format("【药园】收获失败，result[%1%]") % data[0]), iEdit);
        }
    }
}

void sxd_client::plant() {
    Json::Value data = this->Mod_Farm_Base_get_farmlandinfo_list();
    std::vector<Json::Value> lands;
    std::copy_if(data[0].begin(), data[0].end(), std::back_inserter(lands), [](const Json::Value& x) {return !x[8].asInt() && x[10].asInt() == Mod_Farm_Base::NO_PLANT;});
    if (!lands.size())
        return;
    // 最优土地
    Json::Value land_max = *std::max_element(lands.begin(), lands.end(), [](const Json::Value& x1, const Json::Value& x2) {return x1[9].asInt()<x2[9].asInt();});
    int land_max_id = land_max[0].asInt();

    // 获取种植伙伴信息
    data = this->Mod_Farm_Base_get_player_roleinfo_list();
    std::vector<Json::Value> player_roles;
    std::copy_if(data[0].begin(), data[0].end(), std::back_inserter(player_roles), [](const Json::Value& x) {return x[9].asInt() == Mod_Farm_Base::WAR || x[9].asInt() == Mod_Farm_Base::INVITE;});
    if (!player_roles.size())
        return;
    // 最优伙伴
    Json::Value player_role_max = *std::max_element(player_roles.begin(), player_roles.end(), [](const Json::Value& x1, const Json::Value& x2) {return x1[3].asInt() < x2[3].asInt();});
    int player_role_max_id = player_role_max[0].asInt();
    Json::Value player_role_min = *std::max_element(player_roles.begin(), player_roles.end(), [](const Json::Value& x1, const Json::Value& x2) {return x1[3].asInt() > x2[3].asInt();});
    int player_role_min_id = player_role_min[0].asInt();

    for (;;) {
        data = this->Mod_Farm_Base_buy_coin_tree_count_info();
        if (data[0].asInt() == 0) {
            common::log("【药园】仙露已用完", 0);
            break;
        }
        data = this->Mod_Farm_Base_plant_herbs(land_max_id, player_role_max_id, Mod_Farm_Base::COIN_HERBS, 1);
        if (data[0].asInt() == Mod_Farm_Base::SUCCESS) {
            common::log(boost::str(boost::format("【药园】[%1%] 种植 [普通发财树]") % common::utf2gbk(data[4].asString())), iEdit);
            // 收获容易卡
            /*this->Mod_Farm_Base_harvest(land_max_id);
             common::log("【药园】收获", iEdit);
             std::this_thread::sleep_for(std::chrono::seconds(3));*/
            data = this->Mod_Farm_Base_harvest(land_max_id);
            if (data[0].asInt() == Mod_Farm_Base::SUCCESS)
                common::log(boost::str(boost::format("【药园】[%1%] 收获 [%2%]，获得铜钱[%3%]") % common::utf2gbk(data[1].asString()) % common::utf2gbk(data[2].asString()) % data[5]), iEdit);
            else {
                common::log(boost::str(boost::format("【药园】收获失败，result[%1%]") % data[0]), iEdit);
                return;
            }
        } else {
            common::log(boost::str(boost::format("【药园】种植失败，result[%1%]") % data[0]), iEdit);
            return;
        }
    }

    // 伙伴满级则不种植经验树
    data = this->Mod_Player_Base_get_player_info();
    int level = data[1].asInt();
    if (player_role_min[3].asInt() == level)
        return;

    for (const auto& land : lands) {
        data = this->Mod_Farm_Base_plant_herbs(land[0].asInt(), player_role_min_id, Mod_Farm_Base::EXP_HERBS, 1);
        if (data[0].asInt() != Mod_Farm_Base::SUCCESS) {
            common::log(boost::str(boost::format("【药园】种植失败，result[%1%]") % data[0]), iEdit);
            return;
        }
        common::log(boost::str(boost::format("【药园】[%1%] 种植 [普通经验树]") % common::utf2gbk(data[4].asString())), iEdit);
    }
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


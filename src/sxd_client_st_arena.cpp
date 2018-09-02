#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_StArena_Base {
public:
    static const int SCORE_RACE = 0;
    static const int SCORE_RACE_COMPLETE = 1;
    static const int SUCCESS = 13;
    static const int NOT_ENOUGH_EXPLOIT = 19;
};

void sxd_client::st_arena() {
    try {
        // get race step
        Json::Value data = this->Mod_StArena_Base_get_race_step();
        if (data[0].asInt() == Mod_StArena_Base::SCORE_RACE_COMPLETE) {
            common::log("【仙界竞技场】今日 [争霸赛]", 0);
            return;
        } else if (data[0].asInt() == Mod_StArena_Base::SCORE_RACE) {
            common::log("【仙界竞技场】今日 [积分赛]", 0);

            // open st arena
            data = this->Mod_StArena_Base_open_st_arena();
            common::log(boost::str(boost::format("【仙界竞技场】我的积分 [%1%]，积分奖励 [%2%荣誉]，今日还可挑战 [%3%] 次") % data[0] % data[2] % data[1]), 0);
            if (data[1].asInt() == 0)
                return;

            // sort
            Json::Value challenge_list = data[5];       // 挑战者
            std::vector<Json::Value> challenge_vector;  // 挑战者排序
            std::copy(challenge_list.begin(), challenge_list.end(), std::back_inserter(challenge_vector));
            std::sort(challenge_vector.begin(), challenge_vector.end(), [](Json::Value& x, Json::Value& y) {return x[1].asInt()<y[1].asInt();});

            // challenge
            data = this->Mod_StArena_Base_challenge(challenge_vector[0][0].asInt());
            if (data[0].asInt() != Mod_StArena_Base::SUCCESS) {
                common::log(boost::str(boost::format("【仙界竞技场】挑战失败，result[%1%]") % data[0]), iEdit);
                return;
            }
            std::string challenge_name = common::utf2gbk(data[1][0][2][1][1].asString());
            if (data[3].asInt())
                common::log(boost::str(boost::format("【仙界竞技场】挑战 [%1%]，战胜获得积分 [%2%]") % challenge_name % data[3]), iEdit);
            else {
                common::log(boost::str(boost::format("【仙界竞技场】挑战 [%1%]，战败") % challenge_name), iEdit);

                // refresh_player_list 容易卡住
                /*data = this->Mod_StArena_Base_refresh_player_list();
                 if (data[0] != Mod_StArena_Base::SUCCESS)
                 common::log(boost::str(boost::format("【仙界竞技场】换一批失败，result[%1%]") % data[0]), iEdit);
                 else
                 common::log("【仙界竞技场】换一批", iEdit);*/
            }
        } else
            common::log(boost::str(boost::format("【仙界竞技场】数据异常，race_step[%1%]") % data[0]), iEdit);
    } catch (const std::exception& ex) {
        common::log(boost::str(boost::format("发现错误(st arena)：%1%") % ex.what()));
    }
}

//============================================================================
// R172 积分赛|争霸赛
// {module:214, action:15, request:[],
// response:[Utils.UByteUtil, [Utils.UByteUtil, Utils.IntUtil], Utils.IntUtil]};
// StArenaData.as 242:
//     oObject.list(param1, _loc_2, ["race_step", "time_list", "cup_race_time_step"]);
// Example
//     [ 0, [ [ 3, 1515920400 ], [ 5, 1515924000 ], [ 7, 1515927600 ], [ 9, 1515931200 ], [ 11, 1515934800 ] ], 600 ]
//============================================================================
Json::Value sxd_client::Mod_StArena_Base_get_race_step() {
    Json::Value data;
    return this->send_and_receive(data, 214, 15);
}

//============================================================================
// R172 仙界竞技场操作面板
// {module:214, action:0, request:[],
// response:[Utils.IntUtil, Utils.ShortUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, [Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.ShortUtil], [Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.StringUtil, Utils.IntUtil, Utils.StringUtil, Utils.IntUtil, Utils.IntUtil, Utils.StringUtil, Utils.IntUtil, Utils.ByteUtil], [Utils.IntUtil, Utils.StringUtil, Utils.StringUtil, Utils.StringUtil, Utils.IntUtil, Utils.IntUtil]]};
// StArenaData.as 37:
//     oObject.list(param1, _loc_2, ["integral", "reduceChallengeCount", "awardFeats", "awardGetTime", "reduceRefreshTime", "challengeList", "reportList", "integralRankList"]);
//     for each (_loc_3 in _loc_2["challengeList"])
//         oObject.list(_loc_3, _loc_4, ["st_player_id", "integral", "role_id", "level"]);
// Example
//     [ 1320, 5, 62, 1520438400, 1520348883, [ [ 2692, 1364, 103, 201 ], [ 5650, 1324, 5, 189 ], [ 7637, 1318, 104, 175 ], [ 8184, 1322, 102, 175 ], [ 8782, 1315, 6, 189 ] ], [ [ 420217, 1, 203957, "\u5e84\u606d\u6625.s1", 4134, "\u5c18\u4e16.s1", 4134, 0, "2018030201", 1520348823, 1 ], [ 420041, 1, 203957, "\u5e84\u606d\u6625.s1", 4134, "\u5c18\u4e16.s1", 4134, 0, "2018030201", 1520343437, 1 ], [ 420040, 1, 203957, "\u5e84\u606d\u6625.s1", 8782, "\u5bc6\u96ea\u513f.s1", 8782, 0, "2018030201", 1520343425, 1 ], [ 419741, 1, 203957, "\u5e84\u606d\u6625.s1", 8782, "\u5bc6\u96ea\u513f.s1", 8782, 0, "2018030201", 1520325183, 1 ], [ 419740, 1, 203957, "\u5e84\u606d\u6625.s1", 752, "\u611b\u00a7\u8207\u00a7\u6068.s3", 752, 0, "2018030201", 1520325174, 1 ] ], [ [ 1238, "\u98ce\u4e4b\u5f71.s8", "\u5fc3\u52a8", "s05", 7830, 763 ], [ 1884, "\u4e01\u8377\u854a.s1", "\u5fc3\u52a8", "s04", 7780, 738 ], [ 408, "\u88ab\u5996\u602a\u6293\u8d70\u4e86.s8", "\u5fc3\u52a8", "s05", 7738, 723 ], [ 728, "\u9648\u4f1f.s1", "\u5fc3\u52a8", "s04", 7713, 716 ], [ 291, "\u9053\u65e0\u6daf.s4", "\u5fc3\u52a8", "s04", 7686, 713 ], [ 1284, "\u6714\u98ce.s5", "37wan", "37wan_s0273", 7633, 703 ], [ 39, "\u5609\u5609.s2", "\u8981\u73a9", "yaowan_s0152", 7528, 692 ], [ 1117, "\u2570\u2606\u256e\u8ca1\u795e\u723a.s12", "\u8981\u73a9", "yaowan_s0152", 7525, 692 ], [ 89, "\u6c5f\u575a\u6fef.s5", "95k", "95k_s081", 7518, 691 ], [ 216, "\u5b59\u950b\u9ea6.s3", "95k", "95k_s081", 7473, 687 ], [ 297, "\u4e1c\u65b9\u5802\u8fea.s6", "95k", "95k_s081", 7278, 657 ], [ 322, "\u4eba\u7687.s3", "37wan", "37wan_s0273", 6822, 612 ], [ 946, "\u4e2d\u56fd\u529f\u592b.s3", "\u5fc3\u52a8", "s02", 6803, 610 ], [ 2133, "\u7d2b\u84310226.s1", "\u5fc3\u52a8", "s04", 6802, 610 ], [ 165, "\u6b27\u9633\u5a77.s7", "37wan", "37wan_s0273", 6778, 607 ], [ 105, "\u6734 \u667a \u598d.s1", "\u5fc3\u52a8", "s04", 6756, 605 ], [ 1474, "\u4e50\u9738\u9738.s1", "\u5fc3\u52a8", "s04", 6755, 605 ], [ 3829, "\u67ef\u6ce2\u5ada.s5", "6711", "6711_s0160", 6750, 605 ], [ 2625, "\u728e.s7", "\u5fc3\u52a8", "s04", 6747, 604 ], [ 3323, "\u5929\u6daf.\u6d6a\u5b50.s1", "37wan", "37wan_s0273", 6745, 604 ], [ 2036, "\u4e00\u4e86\u767e\u4e86.s3", "\u8981\u73a9", "yaowan_s0152", 6485, 578 ], [ 3882, "\u2605\u6613\u6c34\u5bd2\u2605.s9", "\u5fc3\u52a8", "s05", 6462, 576 ], [ 1033, "\u795e\u753b\u252e.s1", "\u5fc3\u52a8", "s04", 6399, 569 ], [ 7687, "\u65b9\u7b2c\u96c9.s7", "7k7k", "yx567_s0125", 6299, 559 ], [ 3776, "\u725b\u4f73\u8335.s1", "95k", "95k_s081", 6237, 553 ], [ 2151, "zc.s3", "6711", "6711_s0160", 6224, 552 ], [ 8206, "\u521d\u543b\u611f\u89c9.s5", "7k7k", "yx567_s0125", 6210, 551 ], [ 7752, "\u597d\u5403\u662f\u79cd\u75c5.s1", "7k7k", "yx567_s0125", 6124, 542 ], [ 7864, "\u6e05\u98ce\u8475.s3", "7k7k", "yx567_s0125", 6122, 542 ], [ 7673, "\u6c5f\u4f5f\u7b71.s6", "7k7k", "yx567_s0125", 6096, 539 ], [ 1561, "\u6728\u6728\u5200.s4", "\u8981\u73a9", "yaowan_s0152", 6078, 537 ], [ 8072, "\u9648\u5efa\u9f99.s5", "7k7k", "yx567_s0125", 6027, 532 ], [ 203232, "\u561f\u561f\u565c~~~.s1", "37wan", "37wan_s0273", 5991, 529 ], [ 895, "\u566c\u9b42\u72c2\u50b2.s6", "56", "56_s024", 5817, 511 ], [ 8190, "\u73a9\u6b7b\u5fc3\u52a8.s2", "7k7k", "yx567_s0125", 5774, 507 ], [ 7682, "\u548c\u7f8e\u5c9a.s3", "7k7k", "yx567_s0125", 5768, 506 ], [ 2988, "-\u706b\u9e92\u9e9f-.s4", "6711", "6711_s0160", 5747, 504 ], [ 7732, "\u4e00\u5c81\u4e00\u67af\u8363.s1", "7k7k", "yx567_s0125", 5746, 504 ], [ 979, "\u5fa1\u98ce.s1", "56", "56_s01", 5738, 503 ], [ 1888, "\u5411\u524d\u884c\u300a\u300b.s5", "6711", "6711_s0160", 5719, 501 ], [ 2012, "\u5929\u874e.s1", "\u5fc3\u52a8", "s04", 5708, 500 ], [ 61, "\u9f8d\u9b42\u7948\u5929.s6", "95k", "95k_s081", 5679, 497 ], [ 2941, "\u5f64\u5f64\u5b9d\u8d1d.s1", "\u5fc3\u52a8", "s04", 5660, 496 ], [ 955, "\u51cc\u83f2\u6021.s6", "37wan", "37wan_s0273", 5658, 495 ], [ 1432, "\u6b87\u60c5.s5", "\u5fc3\u52a8", "s03", 5566, 486 ], [ 160, "\u6ca7\u6d77\u5c0f\u6708.s8", "37wan", "37wan_s0273", 5550, 485 ], [ 1537, "MSN\u4e09\u53c9\u621f.s3", "56", "56_s024", 5524, 482 ], [ 3686, "\u4eae\u56db\u5c11.s6", "\u5fc3\u52a8", "s03", 5421, 472 ], [ 209, "mhw.s17", "95k", "95k_s081", 5411, 471 ], [ 1627, "\u5723\u2103\u4e5d\u5929\u7384\u5973.s7", "\u5fc3\u52a8", "s04", 5405, 470 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_StArena_Base_open_st_arena() {
    Json::Value data;
    return this->send_and_receive(data, 214, 0);
}

//============================================================================
// R172 挑战
// {module:214, action:2,
// request:[Utils.IntUtil, Utils.IntUtil],
// response:[Utils.UByteUtil, [Utils.ByteUtil, Utils.IntUtil, [Utils.IntUtil, Utils.StringUtil, [Utils.IntUtil, Utils.IntUtil, Utils.StringUtil, Utils.StringUtil, Utils.IntUtil, Utils.LongUtil, Utils.LongUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.IntUtil, [Utils.IntUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ShortUtil], Utils.ByteUtil, Utils.ShortUtil, [Utils.ShortUtil], [Utils.ShortUtil], Utils.ShortUtil, Utils.ShortUtil, Utils.ShortUtil, [Utils.ShortUtil], [Utils.ShortUtil], Utils.ShortUtil, Utils.ByteUtil, Utils.LongUtil, Utils.ByteUtil, Utils.IntUtil], [Utils.IntUtil, Utils.IntUtil, Utils.StringUtil, Utils.StringUtil, Utils.IntUtil, Utils.LongUtil, Utils.LongUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.IntUtil, [Utils.IntUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ShortUtil], Utils.ByteUtil, Utils.ShortUtil, [Utils.ShortUtil], [Utils.ShortUtil], Utils.ShortUtil, Utils.ShortUtil, Utils.ShortUtil, [Utils.ShortUtil], [Utils.ShortUtil], Utils.ShortUtil, Utils.ByteUtil, Utils.LongUtil, Utils.ByteUtil, Utils.IntUtil], [Utils.IntUtil, Utils.IntUtil, Utils.StringUtil, Utils.StringUtil, Utils.IntUtil, Utils.LongUtil, Utils.LongUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.IntUtil, [Utils.IntUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ShortUtil], Utils.ByteUtil, Utils.ShortUtil, [Utils.ShortUtil], [Utils.ShortUtil], Utils.ShortUtil, Utils.ShortUtil, Utils.ShortUtil, [Utils.ShortUtil], [Utils.ShortUtil], Utils.ShortUtil, Utils.ByteUtil, Utils.LongUtil, Utils.ByteUtil, Utils.IntUtil], [Utils.IntUtil, Utils.IntUtil, Utils.StringUtil, Utils.StringUtil, Utils.IntUtil, Utils.LongUtil, Utils.LongUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.IntUtil, [Utils.IntUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ShortUtil], Utils.ByteUtil, Utils.ShortUtil, [Utils.ShortUtil], [Utils.ShortUtil], Utils.ShortUtil, Utils.ShortUtil, Utils.ShortUtil, [Utils.ShortUtil], [Utils.ShortUtil], Utils.ShortUtil, Utils.ByteUtil, Utils.LongUtil, Utils.ByteUtil, Utils.IntUtil], [Utils.IntUtil], [Utils.IntUtil, Utils.IntUtil], Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.ShortUtil, Utils.IntUtil, Utils.StringUtil, Utils.IntUtil, Utils.IntUtil, [Utils.ByteUtil, Utils.ByteUtil, Utils.ByteUtil], Utils.IntUtil, [Utils.IntUtil, Utils.ByteUtil], Utils.LongUtil, Utils.LongUtil, [Utils.IntUtil], Utils.ByteUtil, [Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, [Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil]]], [[Utils.ByteUtil, Utils.StringUtil, [Utils.UByteUtil, Utils.StringUtil], [Utils.StringUtil], [Utils.IntUtil, Utils.LongUtil, Utils.IntUtil], [Utils.StringUtil], [Utils.StringUtil], [Utils.StringUtil], [Utils.StringUtil], [Utils.StringUtil]], [Utils.ByteUtil, Utils.ByteUtil, Utils.IntUtil, [Utils.StringUtil], [Utils.IntUtil, Utils.LongUtil, Utils.IntUtil]], [Utils.IntUtil, Utils.IntUtil, Utils.ShortUtil, Utils.ByteUtil, [Utils.UByteUtil, Utils.ShortUtil, Utils.ByteUtil], Utils.StringUtil, [Utils.ShortUtil], [Utils.StringUtil], [Utils.StringUtil], [Utils.ShortUtil], [Utils.StringUtil], Utils.StringUtil, [Utils.ShortUtil], [Utils.StringUtil], Utils.StringUtil, [Utils.ShortUtil], [Utils.StringUtil], Utils.StringUtil, [Utils.ShortUtil], [Utils.StringUtil], Utils.StringUtil, [Utils.IntUtil], [Utils.StringUtil], Utils.IntUtil, Utils.ShortUtil, Utils.LongUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.StringUtil, Utils.IntUtil, Utils.UByteUtil, [Utils.IntUtil, Utils.ShortUtil, [Utils.UByteUtil, Utils.ShortUtil], Utils.StringUtil, [Utils.ShortUtil], [Utils.StringUtil], [Utils.ShortUtil], Utils.StringUtil, [Utils.ShortUtil], Utils.StringUtil, [Utils.ShortUtil], Utils.StringUtil, [Utils.ShortUtil], Utils.StringUtil, [Utils.IntUtil], Utils.IntUtil, Utils.IntUtil, Utils.LongUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.IntUtil, Utils.ByteUtil], [Utils.IntUtil, Utils.StringUtil], [Utils.IntUtil, Utils.LongUtil, Utils.IntUtil], [Utils.UByteUtil, Utils.StringUtil]], [Utils.IntUtil, Utils.IntUtil], [Utils.UByteUtil, Utils.StringUtil, [Utils.UByteUtil, Utils.StringUtil], [Utils.StringUtil], [Utils.StringUtil], [Utils.StringUtil], [Utils.StringUtil], [Utils.StringUtil], [Utils.StringUtil]], [Utils.IntUtil, Utils.LongUtil, Utils.IntUtil], [Utils.IntUtil, Utils.LongUtil, Utils.IntUtil]], Utils.ShortUtil], Utils.IntUtil, Utils.IntUtil, Utils.StringUtil, Utils.StringUtil, Utils.ShortUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, [Utils.IntUtil, Utils.IntUtil]]};
// StArenaData.as 123:
//     oObject.list(param1, _loc_2, ["result", "warResult", "challengeAwardFeats", "challengeAddIntegral", "defenseStageName", "defenseServerName", "reduceChallengeCount", "item_id", "number", "award_st_snatch_treasure_scrap_id", "scrap_num", "award_item_list"]);
// Example
//     [ 13, [ [ 3, 8782, [ [ 203957, "\u5e84\u606d\u6625.s1", [ [ 807429, 70, "ShenYangJian", "\u795e\u6768\u622c", 5, 455131, 455131, 175, 1, 100, 135, 166, 0, 10, 6, 1, 0, 0, null, 0, 0, [ [ 0 ], [ 0 ] ], null, 0, 0, 0, null, null, 0, 0, 0, 0, 0 ], [ 803448, 5, "FeiYuNan", "\u98de\u7fbd\u7537", 3, 485734, 485734, 175, 2, 100, 115, 184, 0, 10, 0, 0, 1, 0, null, 0, 0, [ [ 0 ], [ 0 ] ], null, 0, 0, 0, null, null, 0, 0, 0, 0, 0 ], [ 812186, 111, "XianHeXianGu", "\u4ed9\u4f55\u4ed9\u59d1", 6, 372480, 372480, 174, 4, 100, 90, 213, 0, 10, 35, 0, 0, 0, null, 0, 0, [ [ 0 ], [ 0 ] ], null, 0, 0, 0, null, null, 0, 0, 0, 0, 0 ], [ 803458, 8, "JiangChen", "\u5c06\u81e3", 5, 69820, 69820, 175, 5, 100, 75, 71, 0, 10, 0, 0, 0, 0, null, 0, 0, [ [ 0 ], [ 0 ] ], null, 0, 0, 0, null, null, 0, 0, 0, 0, 0 ], [ 815340, 82, "FeiWei", "\u98de\u536b", 3, 85462, 85462, 169, 7, 100, 50, 160, 0, 10, 19, 0, 0, 0, null, 0, 0, [ [ 0 ], [ 0 ] ], null, 0, 0, 0, null, null, 0, 0, 0, 0, 0 ] ], [ [ 803518, 29, "AKuan", "\u963f\u5bbd", 1, 58888, 58888, 175, 1, 100, 75, 74, 0, 10, 0, 0, 0, 0, null, 0, 0, [ [ 0 ], [ 0 ] ], null, 0, 0, 0, null, null, 0, 0, 0, 0, 0 ] ], null, null, null, null, 803448, 1440, 5, 175, 27567, "", 10, 1, [ [ 17, 1, 5 ], [ 21, 1, 4 ], [ 9, 6, 3 ], [ 13, 1, 2 ], [ 54, 2, 1 ] ], 0, null, 0, 0, null, 0, null ], [ 8782, "\u5bc6\u96ea\u513f.s1", [ [ -2862842, 208, "ShenFeiWei", "\u795e\u98de\u536b", 3, 435610, 435610, 189, 1, 100, 105, 369, 0, 10, 25, 1, 0, 0, null, 0, 0, [ [ 0 ], [ 0 ] ], null, 0, 0, 0, null, null, 0, 0, 0, 0, 0 ], [ -418, 6, "FeiYuNv", "\u98de\u7fbd\u5973", 3, 701388, 701388, 189, 3, 100, 140, 184, 0, 10, 67, 0, 1, 298, null, 0, 0, [ [ 0 ], [ 0 ] ], [ [ 19 ] ], 0, 0, 1, [ [ 64 ] ], null, 0, 0, 0, 0, 0 ], [ -1606459, 70, "YangJianXiaoTianQuan", "\u795e\u6768\u622c", 5, 363534, 363534, 182, 5, 100, 105, 166, 0, 10, 115, 1, 0, 0, null, 0, 2, [ [ 0 ], [ 0 ] ], null, 0, 0, 0, null, null, 0, 0, 0, 0, 0 ], [ -2915668, 132, "ShenDouZhanShengFo", "\u795e\u6597\u6218\u80dc\u4f5b", 5, 518934, 518934, 186, 6, 100, 110, 250, 0, 10, 16, 1, 0, 0, null, 0, 0, [ [ 0 ], [ 0 ] ], [ [ 19 ] ], 0, 0, 0, null, null, 0, 0, 0, 0, 0 ], [ -2851873, 231, "ShenHeXianGu", "\u795e\u4f55\u4ed9\u59d1", 6, 287556, 287556, 182, 8, 100, 105, 381, 0, 10, 39, 1, 0, 0, null, 0, 0, [ [ 0 ], [ 0 ] ], null, 0, 0, 0, null, null, 0, 0, 0, 0, 0 ] ], [ [ -2923933, 145, "ShenXiongMaoJiuXian", "\u795e\u718a\u732b\u9152\u4ed9", 1, 296634, 296634, 176, 5, 100, 100, 285, 0, 10, 101, 0, 0, 0, null, 0, 0, [ [ 0 ], [ 0 ] ], null, 0, 0, 0, null, null, 0, 0, 0, 0, 0 ] ], null, null, null, null, -418, 1308, 6, 189, 125038, "", 10, 2, [ [ 9, 6, 5 ], [ 6, 5, 4 ], [ 41, 2, 3 ], [ 21, 4, 2 ], [ 13, 7, 1 ] ], 0, null, 0, 0, null, 0, null ] ], [ [ [ [ 1, "13|45", null, null, [ [ 803518, 58888, 75 ], [ 807429, 455131, 90 ], [ 803448, 485734, 70 ], [ 812186, 372480, 45 ], [ 803458, 69820, 30 ], [ 815340, 85462, 5 ], [ -2923933, 296634, 100 ], [ -2862842, 435610, 105 ], [ -418, 701388, 140 ], [ -1606459, 363534, 105 ], [ -2915668, 518934, 110 ], [ -2851873, 287556, 105 ] ], null, null, null, null, null ], [ 0, "54|807429|803448|812186|803458|815340", null, null, [ [ 803518, 58888, 75 ], [ 807429, 455131, 90 ], [ 803448, 485734, 70 ], [ 812186, 372480, 45 ], [ 803458, 69820, 30 ], [ 815340, 85462, 5 ], [ -2923933, 296634, 100 ], [ -2862842, 435610, 105 ], [ -418, 701388, 140 ], [ -1606459, 363534, 105 ], [ -2915668, 518934, 110 ], [ -2851873, 287556, 105 ] ], null, null, null, null, null ] ], null, [ [ 8782, -2862842, 369, 0, null, "", null, null, null, null, null, "", null, null, "", null, null, "", null, null, "", null, null, 115, 0, 435610, 0, 0, 0, 203957, 1, 0, "", 108903, 67, [ [ 807429, 0, null, "", null, null, null, "", null, "", null, "", null, "", null, 90, 221065, 234066, 1, 0, 1, 807429, 8 ], [ 803448, 0, null, "", null, null, null, "", null, "", null, "", null, "", null, 70, 52989, 432745, 1, 1, 0, 803448, 8 ] ], [ [ 10075, "807429|386864|807429|803448" ] ], [ [ 807429, 0, 40 ], [ 803448, 432745, 20 ], [ 812186, 372480, 45 ], [ 803458, 69820, 30 ], [ 815340, 85462, 5 ], [ -2862842, 435610, 115 ], [ -418, 701388, 140 ], [ -1606459, 363534, 105 ], [ -2915668, 518934, 110 ], [ -2851873, 287556, 105 ], [ 803518, 58888, 75 ] ], null ], [ 203957, 803518, 0, 0, null, "", null, null, null, null, null, "", null, null, "", null, null, "", null, null, "", null, null, 100, 0, 58888, 0, 0, 0, 8782, 0, 0, "", 0, 63, [ [ -2862842, 0, null, "", null, null, null, "", null, "", null, "", null, "", null, 140, 1655, 433955, 1, 0, 0, -2862842, 9 ] ], null, [ [ 803518, 58888, 100 ], [ 803448, 432745, 20 ], [ 812186, 372480, 45 ], [ 803458, 69820, 30 ], [ 815340, 85462, 5 ], [ -2862842, 433955, 140 ], [ -418, 701388, 140 ], [ -1606459, 363534, 105 ], [ -2915668, 518934, 110 ], [ -2851873, 287556, 105 ] ], null ], [ 8782, -418, 184, 0, [ [ 61, 298, 2 ] ], "", null, null, null, null, null, "", null, null, "", null, null, "", null, null, "", null, null, 75, 0, 701388, 0, 0, 0, 203957, 0, 0, "", 0, 67, [ [ 803458, 0, [ [ 61, 184 ] ], "", null, null, null, "", null, "", null, "", null, "", null, 30, 465958, 0, 1, 0, 1, 803458, 8 ], [ 815340, 0, [ [ 61, 184 ] ], "", [ [ 19 ] ], null, null, "", null, "", null, "", null, "", null, 5, 465166, 0, 1, 0, 1, 815340, 8 ], [ 812186, 0, [ [ 61, 184 ] ], "", null, null, null, "", null, "", null, "", null, "", null, 45, 231721, 140759, 1, 0, 0, 812186, 8 ] ], null, [ [ 803518, 58888, 100 ], [ 803448, 432745, 20 ], [ 812186, 140759, 45 ], [ 803458, 0, 30 ], [ 815340, 0, 5 ], [ -2862842, 433955, 140 ], [ -418, 701388, 75 ], [ -1606459, 363534, 105 ], [ -2915668, 518934, 110 ], [ -2851873, 287556, 105 ] ], null ], [ 8782, -418, 0, 0, [ [ 61, 298, 2 ] ], "67|70138", null, null, null, null, null, "", null, null, "", null, null, "", null, null, "", null, null, 100, 6, 701388, 0, 0, 0, 203957, 0, 0, "", 0, 63, [ [ 803448, 0, [ [ 61, 369 ] ], "", null, null, null, "", null, "", null, "", null, "", null, 30, 153647, 279098, 1, 0, 0, 803448, 7 ] ], null, [ [ 803518, 58888, 100 ], [ 803448, 279098, 30 ], [ 812186, 140759, 45 ], [ -2862842, 433955, 140 ], [ -418, 701388, 100 ], [ -1606459, 363534, 105 ], [ -2915668, 518934, 110 ], [ -2851873, 287556, 105 ] ], null ], [ 203957, 803448, 0, 0, null, "", null, null, null, null, null, "", null, null, "", null, null, "", null, null, "", null, null, 80, 0, 279098, 0, 0, 0, 8782, 0, 0, "", 0, 63, [ [ -1606459, 0, null, "", [ [ 67 ] ], null, null, "", null, "", null, "", null, "", null, 130, 32772, 330762, 1, 0, 1, -1606459, 9 ] ], null, [ [ 803518, 58888, 100 ], [ 803448, 279098, 80 ], [ 812186, 140759, 45 ], [ -2862842, 433955, 140 ], [ -418, 701388, 100 ], [ -1606459, 330762, 130 ], [ -2915668, 518934, 110 ], [ -2851873, 287556, 105 ] ], null ], [ 8782, -1606459, 166, 4, null, "115", [ [ 67 ], [ 115 ] ], null, null, null, null, "", null, null, "", null, null, "", null, null, "", null, null, 0, 0, 363534, 0, 0, 0, 203957, 1, 0, "", 90884, 67, [ [ 803448, 0, null, "", null, null, null, "", null, "", null, "", null, "", null, 80, 196239, 82859, 1, 0, 1, 803448, 6 ] ], null, [ [ 803518, 58888, 100 ], [ 803448, 82859, 80 ], [ 812186, 140759, 45 ], [ -2862842, 433955, 140 ], [ -418, 701388, 100 ], [ -1606459, 363534, 0 ], [ -2915668, 518934, 110 ], [ -2851873, 287556, 105 ] ], null ], [ 8782, -1606459, 166, 4, null, "", [ [ 115 ], [ 67 ] ], null, null, null, null, "", null, null, "", null, null, "", null, null, "", null, null, 25, 1, 363534, 0, 0, 0, 203957, 1, 0, "", 0, 63, [ [ 803448, 0, null, "", null, null, null, "", null, "", null, "", null, "", null, 105, 213110, 0, 1, 0, 1, 803448, 5 ] ], null, [ [ 803518, 58888, 100 ], [ 803448, 0, 105 ], [ 812186, 140759, 45 ], [ -2862842, 433955, 140 ], [ -418, 701388, 100 ], [ -1606459, 363534, 25 ], [ -2915668, 518934, 110 ], [ -2851873, 287556, 105 ] ], null ], [ 203957, 812186, 0, 0, null, "", null, null, null, null, null, "", null, null, "", null, null, "", null, null, "", null, null, 80, 0, 140759, 0, 0, 0, 8782, 0, 0, "", 0, 66, null, null, [ [ 803518, 58888, 100 ], [ 812186, 140759, 80 ], [ -2862842, 433955, 140 ], [ -418, 701388, 100 ], [ -1606459, 363534, 25 ], [ -2915668, 518934, 110 ], [ -2851873, 287556, 105 ] ], null ], [ 8782, -2915668, 250, 5, null, "", [ [ 67 ] ], null, null, null, null, "", null, null, "", null, null, "", null, null, "", null, null, 0, 0, 518934, 0, 0, 0, 203957, 1, 0, "", 129734, 67, [ [ 803518, 0, null, "", null, null, null, "", null, "", null, "", null, "", null, 110, 51321, 7567, 1, 1, 0, 803518, 9 ], [ 812186, 0, null, "", null, null, null, "", null, "", null, "", null, "", null, 80, 311275, 0, 1, 0, 1, 812186, 7 ] ], null, [ [ 803518, 7567, 110 ], [ 812186, 0, 80 ], [ -2862842, 433955, 140 ], [ -418, 701388, 100 ], [ -1606459, 363534, 25 ], [ -2915668, 518934, 0 ], [ -2851873, 287556, 105 ] ], null ], [ 8782, -2915668, 250, 5, null, "", [ [ 67 ] ], null, null, null, null, "", null, null, "", null, null, "", null, null, "", null, null, 0, 3, 518934, 0, 0, 0, 203957, 1, 0, "", 0, 67, [ [ 803518, 0, null, "", null, null, null, "", null, "", null, "", null, "", null, 110, 122489, 0, 1, 0, 1, 803518, 8 ] ], null, [ [ 803518, 0, 110 ], [ -2862842, 433955, 140 ], [ -418, 701388, 100 ], [ -1606459, 363534, 25 ], [ -2915668, 518934, 0 ], [ -2851873, 287556, 105 ] ], null ] ], [ [ 8782, 2871844 ], [ 203957, 34427 ] ], null, null, null ] ], 0 ] ], 6, 0, "7k7k", "yx567_s0125", 4, 0, 0, 0, 0, null ]
//============================================================================
Json::Value sxd_client::Mod_StArena_Base_challenge(int player_id) {
    Json::Value data;
    data.append(player_id);
    data.append(0);
    return this->send_and_receive(data, 214, 2);
}

//============================================================================
// R172 换一批
// {module:214, action:1, request:[],
// response:[Utils.UByteUtil, [Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.ShortUtil], Utils.IntUtil]};
// StArenaData.as 111:
//     oObject.list(param1, _loc_2, ["result", "challengeList", "reduceRefreshTime"]);
// Example
//     [ 13, [ [ 3651, 1339, 108, 195 ], [ 9549, 1299, 100, 173 ], [ 111860, 1302, 1, 176 ], [ 113175, 1298, 104, 174 ], [ 200077, 1300, 5, 153 ] ], 1520428157 ]
//============================================================================
Json::Value sxd_client::Mod_StArena_Base_refresh_player_list() {
    Json::Value data;
    return this->send_and_receive(data, 214, 1);
}

void sxd_client::exploit_shop() {
    try {
        Json::Value data = this->Mod_StArena_Base_exploit_shop_item_list();
        Json::Value shop_item_list = data[0];
        int good_id = 2;
        auto good = std::find_if(shop_item_list.begin(), shop_item_list.end(), [good_id](const Json::Value& x) {return x[0].asInt()==good_id;});
        if (good == shop_item_list.end()) {
            common::log("【荣誉商店】商品不存在", iEdit);
            return;
        }
        if ((*good)[1].asInt()) {
            data = this->Mod_StArena_Base_buy_exploit_shop_item(good_id, (*good)[1].asInt());
            if (data[0].asInt() == Mod_StArena_Base::NOT_ENOUGH_EXPLOIT)
                common::log("【荣誉商店】购买 [内丹] 失败，荣誉不足", 0);
            else if (data[0].asInt() != Mod_StArena_Base::SUCCESS)
                common::log(boost::str(boost::format("【荣誉商店】购买 [内丹] 失败，result[%1%]") % data[0]), iEdit);
            else
                common::log(boost::str(boost::format("【荣誉商店】购买 [内丹×%1%]") % (*good)[1]), iEdit);
        }
    } catch (const std::exception& ex) {
        common::log(boost::str(boost::format("发现错误(exploit shop)：%1%") % ex.what()));
    }
}

//============================================================================
// R172 荣誉商店
// {module:214, action:5, request:[],
// response:[[Utils.ByteUtil, Utils.ShortUtil]]};
// StArenaData.as 177:
//     oObject.list(_loc_3, _loc_2, ["good_id", "good_count"]);
// Example
//     [ [ [ 4, 50 ], [ 6, 10 ], [ 5, 10 ], [ 2, 10 ], [ 1, 10 ], [ 3, 10 ], [ 7, 50 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_StArena_Base_exploit_shop_item_list() {
    Json::Value data;
    return this->send_and_receive(data, 214, 5);
}

//============================================================================
// R172 购买
// {module:214, action:6,
// request:[Utils.ByteUtil, Utils.ShortUtil],
// Example
//     [ 2, 10 ]
// response:[Utils.UByteUtil]};
// Example
//     [ 13 ]
//============================================================================
Json::Value sxd_client::Mod_StArena_Base_buy_exploit_shop_item(int id, int count) {
    Json::Value data;
    data.append(id);
    data.append(count);
    return this->send_and_receive(data, 214, 6);
}

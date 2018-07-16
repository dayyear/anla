#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_SuperSport_Base {
public:
    static const int SUCCESS = 0;
};

void sxd_client::super_sport() {
    for (;;) {
        Json::Value data = this->Mod_SuperSport_Base_open_super_sport();
        if (data[4].asInt() == 0) {
            common::log("【竞技场】挑战次数已用完", 0);
            return;
        }
        if (data[11].asInt()) {
            common::log("【竞技场】CD中...");
            return;
        }
        Json::Value person = data[12][0];
        data = this->Mod_SuperSport_Base_start_challenge(person[0].asInt());
        if (data[0].asInt() != Mod_SuperSport_Base::SUCCESS) {
            common::log(boost::str(boost::format("【竞技场】挑战失败，result[%1%]") % data[0]));
            return;
        }
        if (data[2].asInt()) {
            common::log(boost::str(boost::format("【竞技场】挑战 [%1%]，战败") % common::utf2gbk(person[4].asString())));
            return;
        }
        common::log(boost::str(boost::format("【竞技场】挑战 [%1%]，战胜") % common::utf2gbk(person[4].asString())));
    }
}

//============================================================================
// R171
// 竞技场
// {module:28, action:0, request:[],
// response:[Utils.IntUtil, Utils.ShortUtil, Utils.IntUtil, Utils.ShortUtil, Utils.ShortUtil, Utils.ShortUtil, Utils.ShortUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, [Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.ShortUtil, Utils.StringUtil, Utils.IntUtil, Utils.IntUtil], [Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.StringUtil, Utils.IntUtil, Utils.StringUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.StringUtil, Utils.IntUtil]]}
// SuperSportController.as 39:
//     _loc_3.mainData.myRank = _loc_2[0];
//     _loc_3.mainData.winCount = _loc_2[1];
//     _loc_3.mainData.fame = _loc_2[2];
//     _loc_3.mainData.fameName = FameLevel.getFameName(_loc_2[3]);
//     **_loc_3.ChallengeNum.remainChallengeTimes = _loc_2[4];**
//     _loc_3.ChallengeNum.remainBuyTimes = _loc_2[5];
//     _loc_3.ChallengeNum.nextBuyPrice = _loc_2[6];
//     _loc_3.awardInfo.awardCoin = _loc_2[7];
//     _loc_3.awardInfo.awardFame = _loc_2[8];
//     _loc_3.awardInfo.awardTime = DateTime.formatServerTimeNull(_loc_2[9]) - _loc_4;
//     _loc_3.openTimer = DateTime.formatServerTimeNull(_loc_2[10]) - _loc_4;
//     **_loc_3.cdTimer = _loc_2[11];**
//     **_loc_3.challengePersonList = this.renderChallengePersonList(_loc_2[12]);**
//     _loc_3.sportReportList = this.renderSportReportList(_loc_2[13]);
//     _loc_3.winCountData.winId = _loc_2[14];
//     _loc_3.winCountData.winName = _loc_2[15];
//     _loc_3.winCountData.winTimesTop = _loc_2[16];
// Example
//     [ 1292, 0, 2585578, 42, **5**, 0, 2, 70800, 0, 1519695049,
//       1519436149, **0**, **[ [ 1267, 332878, 2, 163, "\u60dc\u8776.s1", 73300, 0 ], [ 1242, 170272, 108, 161, "\u5316\u96e8.s7", 75800, 0 ], [ 1217, 236268, 104, 159, "\u2661 \u82b7 \u67d2.s1", 78300, 0 ], [ 1192, 295629, 100, 162, "\u5584\u7f18.s1", 80800, 0 ], [ 1167, 166947, 110, 165, "\u591c\u6708.s7", 83300, 0 ] ]**, [ [ 13086621, 1, 355527, "\u5e84\u606d\u6625.s1", 332878, "\u60dc\u8776.s1", 332878, 1292, 1292, "2018021201", 1519484832 ], [ 13086618, 1, 355527, "\u5e84\u606d\u6625.s1", 332878, "\u60dc\u8776.s1", 332878, 1292, 1292, "2018021201", 1519484800 ], [ 13086616, 1, 355527, "\u5e84\u606d\u6625.s1", 332878, "\u60dc\u8776.s1", 332878, 1292, 1292, "2018021201", 1519484767 ], [ 13086615, 1, 355527, "\u5e84\u606d\u6625.s1", 332878, "\u60dc\u8776.s1", 332878, 1292, 1292, "2018021201", 1519484734 ], [ 13086614, 1, 355527, "\u5e84\u606d\u6625.s1", 332878, "\u60dc\u8776.s1", 332878, 1292, 1292, "2018021201", 1519484701 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_SuperSport_Base_open_super_sport() {
    Json::Value data;
    return this->send_and_receive(data, 28, 0);
}

//============================================================================
// R171
// 挑战
// {module:28, action:2,
// request:[Utils.IntUtil],
// Example
//     [ 1267 ]
// response:[Utils.UByteUtil, Utils.ShortUtil, Utils.IntUtil, ...]
// SuperSportController.as 166:
//     _loc_2.result = _loc_1[0];
//     _loc_2.msg = _loc_1[0];
//     _loc_2.remain = _loc_1[1];
//     _loc_2.cdTimer = _loc_1[2];
// Example
//     [ 0, 4, 0, ...]
//============================================================================
Json::Value sxd_client::Mod_SuperSport_Base_start_challenge(int id) {
    Json::Value data;
    data.append(id);
    return this->send_and_receive(data, 28, 2);
}

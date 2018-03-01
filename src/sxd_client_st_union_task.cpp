#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_StUnionTask_Base {
public:
    static const int SUCCESS = 0;
    static const int NOT_IN_UNION = 1;
    static const int NOT_ENOUGH_TICKET = 2;
};

void sxd_client::st_union_task() {
    for (;;) {
        this->Mod_StUnionTask_Base_get_challenge_info();
        Json::Value data = this->Mod_StUnionTask_Base_fight();
        if (data[0].asInt() == Mod_StUnionTask_Base::NOT_ENOUGH_TICKET) {
            common::log("【魔神挑战】挑战券不足");
            return;
        } else if (data[0].asInt() == Mod_StUnionTask_Base::NOT_IN_UNION) {
            common::log("【魔神挑战】未加仙盟");
            return;
        } else if (data[0].asInt() != Mod_StUnionTask_Base::SUCCESS) {
            common::log(boost::str(boost::format("【魔神挑战】挑战失败，result[%1%]") % data[0]));
            return;
        } else
            common::log("【魔神挑战】挑战");
    }
}

//============================================================================
// R171
// 魔神挑战
// {module:337, action:0, request:[], response:[Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, [Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil], Utils.IntUtil, Utils.ShortUtil, [Utils.IntUtil, Utils.IntUtil], Utils.IntUtil]}
// StUnionTaskData.as 29:
//     this.info.ticket_num = param1[_loc_2++];
//     this.info.left_buy_times = param1[_loc_2++];
//     _loc_3.boss_monster_id = param1[_loc_2++];
//     _loc_3.special_pattern_id = param1[_loc_2++];
//     _loc_3.monster_team_list = this.parseMonsterTeamList(param1[_loc_2++]);
//     **this.info.challenge_times = param1[_loc_2++];**
//     this.info.task_type = param1[_loc_2++];
//     this.info.award_list = this.parseAwardList(param1[_loc_2++]);
//     this.info.fight_times = param1[_loc_2++];
// Example
// 初始
//     [ 1, 9, 15070, 31, [ [ 15083, 6, 238, 3 ], [ 15078, 155, 173, 5 ], [ 15070, 75, 235, 8 ], [ 15083, 24, 187, 1 ], [ 15070, 75, 235, 2 ], [ 15070, 75, 398, 9 ], [ 15078, 39, 239, 6 ] ],
//       **2**, 2, [ [ 1747, 3200000 ], [ 2665, 56000000 ], [ 2664, 560 ] ], 0 ]
// 挑战
//     [ 0, 9, 15070, 31, [ [ 15083, 6, 238, 3 ], [ 15078, 155, 173, 5 ], [ 15070, 75, 235, 8 ], [ 15083, 24, 187, 1 ], [ 15070, 75, 235, 2 ], [ 15070, 75, 398, 9 ], [ 15078, 39, 239, 6 ] ],
//       **1**, 2, [ [ 1747, 3200000 ], [ 2665, 56000000 ], [ 2664, 560 ] ], 1 ]
// 挑战
//     [ 0, 9, 15073, 20, [ [ 15076, 155, 427, 7 ], [ 15077, 112, 398, 4 ], [ 15073, 75, 341, 5 ], [ 15076, 155, 243, 8 ], [ 15077, 39, 173, 2 ], [ 15073, 75, 189, 9 ], [ 15073, 75, 398, 6 ] ],
//       **2**, 2, [ [ 1747, 3200000 ], [ 2665, 56000000 ], [ 2664, 560 ] ], 0 ]
//============================================================================
Json::Value sxd_client::Mod_StUnionTask_Base_get_challenge_info() {
    Json::Value data;
    return this->send_and_receive(data, 337, 0);
}

//============================================================================
// R171
// 挑战
// {module:337, action:1,
// request:[], response:[Utils.UByteUtil, [Utils.ByteUtil, Utils.IntUtil, [...
// StUnionTaskData.as 98:
//     this.info.challengeResult = param1[_loc_2++];
//     this.info.war_result_list = param1[_loc_2++];
//     this.info.award_list = this.parseAwardList(param1[_loc_2++]);
// Example
//     [ 0, [ [ 3, -15070, [ [...
//     [ 2, null, null ]
//============================================================================
Json::Value sxd_client::Mod_StUnionTask_Base_fight() {
    Json::Value data;
    return this->send_and_receive(data, 337, 1);
}

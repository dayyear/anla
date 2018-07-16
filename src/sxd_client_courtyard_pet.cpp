#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_CourtyardPet_Base {
public:
    static const int NORMAL = 0;
    static const int INGOT = 1;
    static const int SUCCESS = 2;
    static const int YES = 6;
    static const int NO = 7;
    static const int INIT = 15;
    static const int RUNNING = 16;
    static const int WINNER = 17;
    static const int LOSER = 18;
};

void sxd_client::courtyard_pet() {
    Json::Value data = this->Mod_CourtyardPet_Base_get_player_info();
    int count = data[0].asInt();
    while (count) {
        data = this->Mod_CourtyardPet_Base_call(Mod_CourtyardPet_Base::NORMAL);
        if (data[0].asInt() != Mod_CourtyardPet_Base::SUCCESS) {
            common::log(boost::str(boost::format("【宠物】灵符召唤失败，result[%1%]") % data[0]));
            return;
        }
        common::log("【宠物】灵符召唤");
        count--;
    }
    common::log("【宠物】灵符已用完", 0);
}

//============================================================================
// R171
// 宠物房
// {module:216, action:0, request:[],
// response:[Utils.ShortUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.ByteUtil]}
// CourtyardPetData.as 48:
//     oObject.list(param1, _loc_2, ["normal_call_count", "normal_total_count", "ingot_call_count", "ingot_total_count", "ingot_cost", "lucky_count", "total_count"]);
// Example
//     [ 10, 10, 20, 20, 50, 2, 20 ]
//     [ 9, 10, 20, 20, 50, 2, 20 ]
//============================================================================
Json::Value sxd_client::Mod_CourtyardPet_Base_get_player_info() {
    Json::Value data;
    return this->send_and_receive(data, 216, 0);
}

//============================================================================
// R171
// 灵符召唤
// {module:216, action:1,
// request:[Utils.UByteUtil],
// Example
//     [ 0 ]
// response:[Utils.UByteUtil, Utils.ShortUtil, Utils.UByteUtil]};
// CourtyardPetData.as 61:
//     oObject.list(param1, this._callResult, ["result", "courtyard_pet_id", "new_pet_flag"]);
// Example
//     [ 2, 10, 7 ]
//============================================================================
Json::Value sxd_client::Mod_CourtyardPet_Base_call(int type) {
    Json::Value data;
    data.append(type);
    return this->send_and_receive(data, 216, 1);
}

//============================================================================
// 宠物任务
//============================================================================
void sxd_client::courtyard_pet_quest() {
    auto data = this->Mod_CourtyardPet_Base_get_quest_list();
    std::vector<Json::Value> quest_list_can_award;
    // award_flag == Mod_CourtyardPet_Base::YES(10)
    std::copy_if(data[2].begin(), data[2].end(), std::back_inserter(quest_list_can_award), [](const Json::Value& x) {return x[10].asInt() == Mod_CourtyardPet_Base::YES;});
    for (const auto& quest : quest_list_can_award) {
        int quest_id = quest[0].asInt();
        data = this->Mod_CourtyardPet_Base_get_award(quest_id);
        if (data[0].asInt() != Mod_CourtyardPet_Base::SUCCESS) {
            common::log(boost::str(boost::format("【宠物任务】领取奖励失败，result[%1%]") % data[0]));
            return;
        }
        common::log("【宠物任务】领取奖励");
    }

    for (;;) {
        data = this->Mod_CourtyardPet_Base_get_quest_list();
        int usable_count = data[1].asInt();
        int usablePetCount = data[3].asInt();
        if (!usable_count || !usablePetCount)
            return;

        int quest_running_count = std::count_if(data[2].begin(), data[2].end(), [](const Json::Value& x) {return x[2].asInt() == Mod_CourtyardPet_Base::RUNNING;});
        std::vector<Json::Value> quest_list_valueable;
        if (quest_running_count)
            // (x-1)*(x-13) <= 0, qtype = 15(INIT), victory_rate >= 10
            std::copy_if(data[2].begin(), data[2].end(), std::back_inserter(quest_list_valueable), [](const Json::Value& x) {return (x[0].asInt()-1)*(x[0].asInt()-13) <= 0 && x[2].asInt() == Mod_CourtyardPet_Base::INIT && x[12].asInt() >= 10;});
        else
            // (x-1)*(x-13)*(x-23)*(x-25)*(x-29)*(x-45) <= 0, qtype = 15(INIT), victory_rate >= 10
            std::copy_if(data[2].begin(), data[2].end(), std::back_inserter(quest_list_valueable), [](const Json::Value& x) {return (x[0].asInt()-1)*(x[0].asInt()-13)*(x[0].asInt()-23)*(x[0].asInt()-25)*(x[0].asInt()-29)*(x[0].asInt()-45) <= 0 && x[2].asInt() == Mod_CourtyardPet_Base::INIT && x[12].asInt() >= 10;});
        if (!quest_list_valueable.size())
            return;
        std::sort(quest_list_valueable.begin(), quest_list_valueable.end(), [](const Json::Value& x, const Json::Value& y) {return x[0].asInt() < y[0].asInt();});
        auto quest = quest_list_valueable[0];
        int quest_id = quest[0].asInt();
        //int pet_list[3];
        std::vector<int> pet_list;
        std::transform(quest[7].begin(), quest[7].end(), std::back_inserter(pet_list), [](const Json::Value& x) {return x[0].asInt();});
        data = this->Mod_CourtyardPet_Base_accept_quest(quest_id, pet_list.data(), pet_list.size());
        if (data[0].asInt() != Mod_CourtyardPet_Base::SUCCESS) {
            common::log(boost::str(boost::format("【宠物任务】开始任务失败，result[%1%]") % data[0]));
            return;
        }
        common::log("【宠物任务】开始任务");
    }
}

//============================================================================
// R181
//============================================================================
Json::Value sxd_client::Mod_CourtyardPet_Base_get_courtyard_pet_list() {
    Json::Value data;
    return this->send_and_receive(data, 000, 000);
}

//============================================================================
// R181 任务列表
// "module":216,"action":5,"request":[],"response":[Utils.UByteUtil,Utils.ByteUtil,[Utils.IntUtil,[Utils.ShortUtil,Utils.ShortUtil,Utils.ShortUtil,[Utils.ShortUtil]],Utils.UByteUtil,Utils.UByteUtil,Utils.ByteUtil,Utils.IntUtil,Utils.UByteUtil,[Utils.ShortUtil],[Utils.ShortUtil,Utils.IntUtil],[Utils.ShortUtil],Utils.UByteUtil,[Utils.IntUtil,Utils.IntUtil,Utils.IntUtil],Utils.IntUtil],Utils.ByteUtil,Utils.ByteUtil,Utils.ByteUtil]
// CourtyardPetData.as:
//     this.get_quest_list_result = param1[0];
//     this._taskInfo.usable_count = param1[1];
//     this._taskInfo.parseData(param1[2],this._info);
//         _loc4_.quest_id = _loc3_[0];
//         _loc4_.monster_list = _loc3_[1];
//         **_loc4_.qtype = _loc3_[2];**
//         **_loc4_.complete_flag = _loc3_[3];**
//         _loc4_.cost_hours = _loc3_[4];
//         _loc4_.seconds = _loc3_[5];
//         _loc4_.normal_flag = _loc3_[6];
//         _loc4_.pet_list = this.parsePetList(_loc3_[7],param2);
//         _loc4_.perfect_pet_list = this.parsePerfectPetList(_loc3_[8],param2);
//         _loc4_.actual_pet_list = this.parsePetList(_loc3_[9],param2);
//         **_loc4_.award_flag = _loc3_[10];**
//         _loc4_.award_list = _loc3_[11];
//         _loc4_.victory_rate = _loc3_[12];
//     this._taskInfo.usablePetCount = param1[3];
//     this._taskInfo.totalPetCount = param1[4];
//     this._taskInfo.is_can_set = param1[5];
// Example
//     [ 2, 10, [ [ 13, [ [ 13, 7, 4, [ [ 5 ], [ 8 ] ] ], [ 13, 10, 3, [ [ 7 ], [ 1 ] ] ] ], 15, 15, 7,
//                           0, 19, [ [ 25 ], [ 35 ] ], null, null, 7, [ [ 3426, 190, 0 ], [ 1740, 1, 0 ], [ 1845, 6, 0 ] ], 21 ],
//                [ 38, [ [ 38, 4, 3, [ [ 3 ] ] ], [ 38, 5, 2, [ [ 3 ], [ 4 ] ] ], [ 38, 3, 2, [ [ 3 ], [ 1 ] ] ] ], 15, 15, 7,
//                           0, 19, [ [ 68 ], [ 17 ], [ 25 ] ], null, null, 7, [ [ 3426, 100, 0 ], [ 3281, 1, 0 ], [ 3417, 1, 0 ] ], 80 ],
//                [  5, [ [ 5, 9, 3, [ [ 5 ], [ 6 ] ] ], [ 5, 12, 3, [ [ 5 ], [ 6 ] ] ], [ 5, 9, 4, [ [ 5 ], [ 7 ] ] ] ], **15**, **15**, 10,
//                          0, 19, [ [ 61 ], [ 68 ], [ 25 ] ], null, null, 7, [ [ 3426, 220, 0 ], [ 2263, 3, 0 ], [ 1845, 8, 0 ] ], 28 ], [ 42, [ [ 42, 6, 3, [ [ 6 ], [ 4 ] ] ] ], 15, 15, 6, 0, 19, [ [ 68 ] ], null, null, 7, [ [ 3426, 70, 0 ], [ 2403, 50, 0 ], [ 2452, 3, 0 ] ], 80 ], [ 46, [ [ 46, 1, 2, [ [ 2 ] ] ], [ 46, 1, 2, [ [ 4 ] ] ] ], 15, 15, 5, 0, 19, [ [ 68 ], [ 17 ] ], null, null, 7, [ [ 3426, 50, 0 ], [ 1743, 3, 0 ] ], 97 ], [ 14, [ [ 14, 1, 2, [ [ 1 ] ] ], [ 14, 1, 1, [ [ 2 ] ] ] ], 15, 15, 5, 0, 19, [ [ 25 ], [ 68 ] ], null, null, 7, [ [ 3426, 50, 0 ], [ 2452, 2, 0 ] ], 100 ], [ 7, [ [ 7, 11, 3, [ [ 3 ], [ 8 ] ] ], [ 7, 12, 3, [ [ 3 ], [ 4 ] ] ] ], 15, 15, 11, 0, 19, [ [ 61 ], [ 68 ] ], null, null, 7, [ [ 3426, 170, 0 ], [ 2395, 2, 0 ], [ 1845, 6, 0 ] ], 40 ], [ 51, [ [ 51, 1, 2, [ [ 1 ] ] ] ], 15, 15, 5, 0, 19, [ [ 25 ] ], null, null, 7, [ [ 3426, 50, 0 ], [ 2452, 1, 0 ] ], 99 ], [ 29, [ [ 29, 2, 2, [ [ 4 ], [ 6 ] ] ], [ 29, 1, 2, [ [ 6 ], [ 4 ] ] ] ], 15, 15, 8, 0, 19, [ [ 68 ], [ 17 ] ], null, null, 7, [ [ 3426, 100, 0 ], [ 2197, 1, 0 ], [ 2452, 3, 0 ] ], 88 ], [ 26, [ [ 26, 1, 1, [ [ 1 ] ] ] ], 15, 15, 6, 0, 19, [ [ 25 ] ], null, null, 7, [ [ 3426, 50, 0 ], [ 1743, 5, 0 ] ], 100 ], [ 1, [ [ 1, 20, 4, [ [ 3 ], [ 19 ], [ 8 ] ] ], [ 1, 19, 4, [ [ 2 ], [ 3 ], [ 19 ] ] ], [ 1, 14, 3, [ [ 1 ], [ 7 ], [ 19 ] ] ] ], 15, 15, 10, 0, 19, [ [ 68 ], [ 61 ], [ 25 ] ], null, null, 7, [ [ 3426, 300, 0 ], [ 2395, 3, 0 ], [ 1747, 2040000, 0 ], [ 3417, 3, 0 ] ], 0 ], [ 10, [ [ 10, 9, 3, [ [ 7 ], [ 2 ] ] ], [ 10, 11, 4, [ [ 6 ], [ 8 ] ] ], [ 10, 11, 3, [ [ 6 ], [ 3 ] ] ] ], 15, 15, 8, 0, 19, [ [ 35 ], [ 68 ], [ 61 ] ], null, null, 7, [ [ 3426, 190, 0 ], [ 2263, 3, 0 ], [ 347, 8, 0 ] ], 34 ], [ 20, [ [ 20, 1, 2, [ [ 2 ] ] ], [ 20, 1, 2, [ [ 2 ] ] ] ], 15, 15, 4, 0, 19, [ [ 54 ], [ 68 ] ], null, null, 7, [ [ 3426, 50, 0 ], [ 1845, 4, 0 ] ], 93 ], [ 22, [ [ 22, 1, 1, [ [ 2 ] ] ] ], 15, 15, 4, 0, 19, [ [ 68 ] ], null, null, 7, [ [ 3426, 50, 0 ], [ 1743, 5, 0 ] ], 100 ], [ 36, [ [ 36, 4, 3, [ [ 3 ], [ 4 ] ] ], [ 36, 4, 3, [ [ 2 ] ] ] ], 15, 15, 5, 0, 19, [ [ 68 ], [ 54 ] ], null, null, 7, [ [ 3426, 110, 0 ], [ 2452, 4, 0 ], [ 2197, 1, 0 ] ], 73 ], [ 6, [ [ 6, 7, 3, [ [ 1 ], [ 2 ] ] ], [ 6, 7, 3, [ [ 8 ], [ 7 ] ] ], [ 6, 10, 3, [ [ 4 ], [ 7 ] ] ] ], 15, 15, 7, 0, 19, [ [ 25 ], [ 68 ], [ 35 ] ], null, null, 7, [ [ 3426, 230, 0 ], [ 3050, 8, 0 ], [ 3277, 1, 0 ] ], 35 ], [ 16, [ [ 16, 1, 2, [ [ 2 ] ] ] ], 15, 15, 6, 0, 19, [ [ 68 ] ], null, null, 7, [ [ 3426, 50, 0 ], [ 1845, 5, 0 ] ], 93 ], [ 44, [ [ 44, 1, 1, [ [ 2 ], [ 3 ] ] ], [ 44, 1, 2, [ [ 6 ] ] ], [ 44, 4, 1, [ [ 4 ] ] ] ], 15, 15, 6, 0, 19, [ [ 61 ], [ 68 ], [ 17 ] ], null, null, 7, [ [ 3426, 90, 0 ], [ 2197, 1, 0 ], [ 3277, 1, 0 ] ], 96 ], [ 40, [ [ 40, 5, 3, [ [ 2 ] ] ], [ 40, 7, 2, [ [ 2 ] ] ], [ 40, 5, 2, [ [ 2 ] ] ] ], 15, 15, 6, 0, 19, [ [ 68 ], [ 54 ], [ 17 ] ], null, null, 7, [ [ 3426, 70, 0 ], [ 3050, 5, 0 ], [ 2197, 1, 0 ] ], 63 ], [ 15, [ [ 15, 1, 1, [ [ 4 ] ] ], [ 15, 1, 2, [ [ 1 ] ] ] ], 15, 15, 5, 0, 19, [ [ 17 ], [ 25 ] ], null, null, 7, [ [ 3426, 50, 0 ], [ 1747, 510000, 0 ] ], 100 ] ],
//       20, 20, 0 ]
// 开始任务后，[qtype,2] 15 --> 16
//     [ 2,  9, [ [ 13, [ [ 13, 7, 4, [ [ 5 ], [ 8 ] ] ], [ 13, 10, 3, [ [ 7 ], [ 1 ] ] ] ], 15, 15, 7, 0, 19, [ [ 54 ], [ 35 ] ], [ [ 25, 35999 ] ], null, 7, [ [ 3426, 190, 0 ], [ 1740, 1, 0 ], [ 1845, 6, 0 ] ], 39 ],
//                [ 38, [ [ 38, 4, 3, [ [ 3 ] ] ], [ 38, 5, 2, [ [ 3 ], [ 4 ] ] ], [ 38, 3, 2, [ [ 3 ], [ 1 ] ] ] ], 15, 15, 7, 0, 19, [ [ 54 ], [ 17 ], [ 20 ] ], [ [ 68, 35999 ], [ 25, 35999 ] ], null, 7, [ [ 3426, 100, 0 ], [ 3281, 1, 0 ], [ 3417, 1, 0 ] ], 65 ],
//                [  5, [ [ 5, 9, 3, [ [ 5 ], [ 6 ] ] ], [ 5, 12, 3, [ [ 5 ], [ 6 ] ] ], [ 5, 9, 4, [ [ 5 ], [ 7 ] ] ] ], **16**, **15**, 10, 35999, 19, [ [ 17 ], [ 54 ], [ 35 ] ], [ [ 61, 35999 ], [ 68, 35999 ], [ 25, 35999 ] ], [ [ 61 ], [ 68 ], [ 25 ] ], 7, [ [ 3426, 220, 0 ], [ 2263, 3, 0 ], [ 1845, 8, 0 ] ], 28 ], [ 42, [ [ 42, 6, 3, [ [ 6 ], [ 4 ] ] ] ], 15, 15, 6, 0, 19, [ [ 17 ] ], [ [ 68, 35999 ] ], null, 7, [ [ 3426, 70, 0 ], [ 2403, 50, 0 ], [ 2452, 3, 0 ] ], 52 ], [ 46, [ [ 46, 1, 2, [ [ 2 ] ] ], [ 46, 1, 2, [ [ 4 ] ] ] ], 15, 15, 5, 0, 19, [ [ 54 ], [ 17 ] ], [ [ 68, 35999 ] ], null, 7, [ [ 3426, 50, 0 ], [ 1743, 3, 0 ] ], 97 ], [ 14, [ [ 14, 1, 2, [ [ 1 ] ] ], [ 14, 1, 1, [ [ 2 ] ] ] ], 15, 15, 5, 0, 19, [ [ 54 ], [ 21 ] ], [ [ 25, 35999 ], [ 68, 35999 ] ], null, 7, [ [ 3426, 50, 0 ], [ 2452, 2, 0 ] ], 93 ], [ 7, [ [ 7, 11, 3, [ [ 3 ], [ 8 ] ] ], [ 7, 12, 3, [ [ 3 ], [ 4 ] ] ] ], 15, 15, 11, 0, 19, [ [ 54 ], [ 17 ] ], [ [ 61, 35999 ], [ 68, 35999 ] ], null, 7, [ [ 3426, 170, 0 ], [ 2395, 2, 0 ], [ 1845, 6, 0 ] ], 28 ], [ 51, [ [ 51, 1, 2, [ [ 1 ] ] ] ], 15, 15, 5, 0, 19, [ [ 54 ] ], [ [ 25, 35999 ] ], null, 7, [ [ 3426, 50, 0 ], [ 2452, 1, 0 ] ], 93 ], [ 29, [ [ 29, 2, 2, [ [ 4 ], [ 6 ] ] ], [ 29, 1, 2, [ [ 6 ], [ 4 ] ] ] ], 15, 15, 8, 0, 19, [ [ 17 ], [ 35 ] ], [ [ 68, 35999 ] ], null, 7, [ [ 3426, 100, 0 ], [ 2197, 1, 0 ], [ 2452, 3, 0 ] ], 77 ], [ 26, [ [ 26, 1, 1, [ [ 1 ] ] ] ], 15, 15, 6, 0, 19, [ [ 54 ] ], [ [ 25, 35999 ] ], null, 7, [ [ 3426, 50, 0 ], [ 1743, 5, 0 ] ], 100 ], [ 1, [ [ 1, 20, 4, [ [ 3 ], [ 19 ], [ 8 ] ] ], [ 1, 19, 4, [ [ 2 ], [ 3 ], [ 19 ] ] ], [ 1, 14, 3, [ [ 1 ], [ 7 ], [ 19 ] ] ] ], 15, 15, 10, 0, 19, [ [ 54 ], [ 21 ], [ 35 ] ], [ [ 68, 35999 ], [ 61, 35999 ], [ 25, 35999 ] ], null, 7, [ [ 3426, 300, 0 ], [ 2395, 3, 0 ], [ 1747, 2040000, 0 ], [ 3417, 3, 0 ] ], 0 ], [ 10, [ [ 10, 9, 3, [ [ 7 ], [ 2 ] ] ], [ 10, 11, 4, [ [ 6 ], [ 8 ] ] ], [ 10, 11, 3, [ [ 6 ], [ 3 ] ] ] ], 15, 15, 8, 0, 19, [ [ 35 ], [ 54 ], [ 34 ] ], [ [ 68, 35999 ], [ 61, 35999 ] ], null, 7, [ [ 3426, 190, 0 ], [ 2263, 3, 0 ], [ 347, 8, 0 ] ], 15 ], [ 20, [ [ 20, 1, 2, [ [ 2 ] ] ], [ 20, 1, 2, [ [ 2 ] ] ] ], 15, 15, 4, 0, 19, [ [ 19 ], [ 54 ] ], [ [ 68, 35999 ] ], null, 7, [ [ 3426, 50, 0 ], [ 1845, 4, 0 ] ], 87 ], [ 22, [ [ 22, 1, 1, [ [ 2 ] ] ] ], 15, 15, 4, 0, 19, [ [ 54 ] ], [ [ 68, 35999 ] ], null, 7, [ [ 3426, 50, 0 ], [ 1743, 5, 0 ] ], 100 ], [ 36, [ [ 36, 4, 3, [ [ 3 ], [ 4 ] ] ], [ 36, 4, 3, [ [ 2 ] ] ] ], 15, 15, 5, 0, 19, [ [ 17 ], [ 54 ] ], [ [ 68, 35999 ] ], null, 7, [ [ 3426, 110, 0 ], [ 2452, 4, 0 ], [ 2197, 1, 0 ] ], 61 ], [ 6, [ [ 6, 7, 3, [ [ 1 ], [ 2 ] ] ], [ 6, 7, 3, [ [ 8 ], [ 7 ] ] ], [ 6, 10, 3, [ [ 4 ], [ 7 ] ] ] ], 15, 15, 7, 0, 19, [ [ 21 ], [ 54 ], [ 35 ] ], [ [ 25, 35999 ], [ 68, 35999 ] ], null, 7, [ [ 3426, 230, 0 ], [ 3050, 8, 0 ], [ 3277, 1, 0 ] ], 27 ], [ 16, [ [ 16, 1, 2, [ [ 2 ] ] ] ], 15, 15, 6, 0, 19, [ [ 54 ] ], [ [ 68, 35999 ] ], null, 7, [ [ 3426, 50, 0 ], [ 1845, 5, 0 ] ], 93 ], [ 44, [ [ 44, 1, 1, [ [ 2 ], [ 3 ] ] ], [ 44, 1, 2, [ [ 6 ] ] ], [ 44, 4, 1, [ [ 4 ] ] ] ], 15, 15, 6, 0, 19, [ [ 21 ], [ 54 ], [ 17 ] ], [ [ 61, 35999 ], [ 68, 35999 ] ], null, 7, [ [ 3426, 90, 0 ], [ 2197, 1, 0 ], [ 3277, 1, 0 ] ], 81 ], [ 40, [ [ 40, 5, 3, [ [ 2 ] ] ], [ 40, 7, 2, [ [ 2 ] ] ], [ 40, 5, 2, [ [ 2 ] ] ] ], 15, 15, 6, 0, 19, [ [ 54 ], [ 19 ], [ 17 ] ], [ [ 68, 35999 ] ], null, 7, [ [ 3426, 70, 0 ], [ 3050, 5, 0 ], [ 2197, 1, 0 ] ], 57 ], [ 15, [ [ 15, 1, 1, [ [ 4 ] ] ], [ 15, 1, 2, [ [ 1 ] ] ] ], 15, 15, 5, 0, 19, [ [ 17 ], [ 54 ] ], [ [ 25, 35999 ] ], null, 7, [ [ 3426, 50, 0 ], [ 1747, 510000, 0 ] ], 100 ] ],
//       17, 20, 0 ]
// 任务完成后，是怎样的状态
//                [ 15, [ [ 15, 1, 1, [ [ 1 ] ] ] ], **16**, **17**, 4,
//                           0, 19, [ [ 60 ] ], [ [ 25, 0 ] ], [ [ 25 ] ], **6**, [ [ 3426, 50, 0 ], [ 1845, 4, 0 ] ], 100 ]
//============================================================================
Json::Value sxd_client::Mod_CourtyardPet_Base_get_quest_list() {
    Json::Value data;
    return this->send_and_receive(data, 216, 5);
}

//============================================================================
// R181
//============================================================================
Json::Value sxd_client::Mod_CourtyardPet_Base_get_courtyard_normal_pet_list() {
    Json::Value data;
    return this->send_and_receive(data, 000, 000);
}

//============================================================================
// R181
//============================================================================
Json::Value sxd_client::Mod_CourtyardPet_Base_reload() {
    Json::Value data;
    return this->send_and_receive(data, 000, 000);
}

//============================================================================
// R181 开始任务
// "module":216,"action":7,"request":[Utils.ShortUtil,[Utils.ShortUtil]],"response":[Utils.UByteUtil]
// CourtyardPetData.as:
//     this._acceptQuest.result = param1[0];
// Example
//     [ 5, [ [ 61 ], [ 68 ], [ 25 ] ] ] --> [ 2 ]
//============================================================================
Json::Value sxd_client::Mod_CourtyardPet_Base_accept_quest(int quest_id, int pet_list[], int count) {
    Json::Value data, data1;
    data.append(quest_id);
    for (int i = 0; i < count; i++) {
        Json::Value data2;
        data2.append(pet_list[i]);
        data1.append(data2);
    }
    data.append(data1);
    return this->send_and_receive(data, 216, 7);
}

//============================================================================
// R181 领取奖励
// "module":216,"action":8,"request":[Utils.ShortUtil],"response":[Utils.UByteUtil]
// CourtyardPetData.as:
//     this.getAwardResult = param1[0];
// Example
//
//============================================================================
Json::Value sxd_client::Mod_CourtyardPet_Base_get_award(int quest_id) {
    Json::Value data;
    data.append(quest_id);
    return this->send_and_receive(data, 216, 8);
}

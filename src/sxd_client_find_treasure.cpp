#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_Memoirs_Base {
public:
    static const int OPEN = 145;  // -111
};

class Mod_FindTreasure_Base {
public:
    static const int SUCCESS = 2;
};

//============================================================================
// Áú¹¬Ì½±¦
//============================================================================
void sxd_client::find_treasure() {
    auto data = this->Mod_Memoirs_Base_is_activity_open();
    auto boards = data[0];
    for (auto& board : boards) {
        auto& board_info = board[0];
        auto board_type = board[1].asInt();
        /* 151: ¶ÀÁ¢, ALONE */
        if (board_type == 151) {
            for (auto& activity : board_info) {
                auto activity_type = activity[0].asInt();
                auto is_open = activity[1].asInt();
                /* 155: Áú¹¬Ì½±¦, find_treasure */
                if (activity_type == 155 && is_open == Mod_Memoirs_Base::OPEN) {
                    /* begin */
                    common::log(common::sprintf("¡¾Áú¹¬Ì½±¦¡¿¿ªÆôÊ±¼ä´Ó [%s] ÖÁ [%s]", common::to_string(activity[2].asInt(), "%Y%m%d").c_str(), common::to_string(activity[3].asInt(), "%Y%m%d").c_str()), iEdit);
                    data = this->Mod_FindTreasure_Base_open_find_treasure();
                    int free_count = data[0].asInt();
                    while (free_count) {
                        data = this->Mod_FindTreasure_Base_find_treasure(0);
                        if (data[0].asInt() != Mod_FindTreasure_Base::SUCCESS) {
                            common::log(common::sprintf("¡¾Áú¹¬Ì½±¦¡¿Ì½Ë÷Ê§°Ü£¬result[%d]", data[0]), iEdit);
                            break;
                        }
                        free_count = data[2].asInt();
                        int score = data[4].asInt();
                        common::log(common::sprintf("¡¾Áú¹¬Ì½±¦¡¿Ì½Ë÷£¬ÎÒµÄ»ı·Ö [%d]£¬»¹Ê£ [%d] ´Î", score, free_count), iEdit);
                    }
                    int ids[] = { 50, 200, 500, 800, 1200, 1500, 2000, 2500, 3000, 3500 };
                    for (int id : ids) {
                        data = this->Mod_FindTreasure_Base_exchange_score_award(id);
                        if (data[0].asInt() == Mod_FindTreasure_Base::SUCCESS)
                            common::log(common::sprintf("¡¾Áú¹¬Ì½±¦¡¿»ı·Ö´ïµ½ [%d]£¬ÁìÈ¡½±Àø", id), iEdit);
                    }
                    /* end */
                }
            }
            break;
        }
    }

}

//============================================================================
// Áú¹¬Ì½±¦-Ãæ°å
// "module":188,"action":1,"request":[],"response":[Utils.ByteUtil,Utils.ByteUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,[Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.ByteUtil],Utils.ByteUtil,Utils.ByteUtil,Utils.ByteUtil]
// FindTreasureData.as:
//     this.info.free_count = param1[_loc2_++];
//     this.info.ingot_count = param1[_loc2_++];
//     this.info.score = param1[_loc2_++];
//     this.info.find_need_ingot = param1[_loc2_++];
//     this.info.lock_need_ingot = param1[_loc2_++];
//     this.info.reset_need_ingot = param1[_loc2_++];
//     this.info.reset_get_score = param1[_loc2_++];
//     this.info.refresh_time = param1[_loc2_++];
//     this.info.award_list = param1[_loc2_++];
//     this.info.extra_find_count = param1[_loc2_++];
//     this.info.extra_find_need_count = param1[_loc2_++];
//     this.info.extra_free_need_count = param1[_loc2_++];
// Example
//     [ 5(free_count), 125, 0, 30, 20, 20, 20, 14400, [ [ 12, 4254, 1, 1, 0, 0 ], [ 13, 35, 1, 9, 0, 1 ], [ 12, 5090, 1, 5, 0, 0 ], [ 12, 2405, 1, 3, 0, 1 ], [ 19, 0, 50, 6, 0, 1 ], [ 15, 7, 3, 2, 0, 0 ], [ 1, 0, 1000000, 8, 0, 1 ], [ 12, 5092, 1, 4, 0, 0 ], [ 12, 4279, 1, 7, 0, 0 ] ], 0, 100, 10 ]
//============================================================================
Json::Value sxd_client::Mod_FindTreasure_Base_open_find_treasure() {
    Json::Value data;
    return this->send_and_receive(data, 188, 1);
}

//============================================================================
// Áú¹¬Ì½±¦-Ì½Ë÷
// "module":188,"action":2,"request":[Utils.ByteUtil],"response":[Utils.UByteUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.IntUtil,Utils.ByteUtil,Utils.UByteUtil,Utils.ByteUtil]
// FindTreasureData.as:
//     this.info.resultCode = param1[_loc2_++];
//     if(this.info.resultCode == Mod_FindTreasure_Base.SUCCESS)
//     {
//        this.info.random_grid = param1[_loc2_++];
//        this.info.free_count = param1[_loc2_++];
//        this.info.ingot_count = param1[_loc2_++];
//        this.info.score = param1[_loc2_++];
//        this.info.extra_find_count = param1[_loc2_++];
//        this.info.extra_award_flag = param1[_loc2_++];
//        this.info.extra_free_need_count = param1[_loc2_++];
//     }
// Example
//     [ 0(not lock) ] -> [ 2(Mod_FindTreasure_Base::SUCCESS), 8, 4(free_count), 125, 20(score), 0, 11, 10 ]
//============================================================================
Json::Value sxd_client::Mod_FindTreasure_Base_find_treasure(int is_lock) {
    Json::Value data;
    data.append(is_lock);
    return this->send_and_receive(data, 188, 2);
}

//============================================================================
// Áú¹¬Ì½±¦-ÁìÈ¡»ı·Ö½±Àø
// Example
//     [ 50 ] -> [ 2(Mod_FindTreasure_Base::SUCCESS), 1, 0, 800000, 0 ]
//============================================================================
Json::Value sxd_client::Mod_FindTreasure_Base_exchange_score_award(int id) {
    Json::Value data;
    data.append(id);
    return this->send_and_receive(data, 188, 5);
}


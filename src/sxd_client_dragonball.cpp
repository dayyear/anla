#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_Dragonball_Base {
public:
    static const int SUCCESS = 0;
    static const int NORMAL = 4;
    static const int NOT_AUTO = 7;
    static const int PLAY = 14;
};

//============================================================================
// R180 ÉñÁúÉÏ¹©
//============================================================================
void sxd_client::dragonball() {

    std::string dragonball_names[] = { "", "", "", "½ð¼×ÁúÁÛ", "", "", "ÁúÐ¥¾ÅÌì", "Ä¾Ò¶ÁúÆø", "Õ½ÁúÒ»Ð¥", "ÇàÁú°ÂÒå", "ÌìÍâ·ÉÁú", "º³µØÁúºð", "ºÚÁúÍÌÊÉ", "ÁúÍÌÌìÏÂ", "°ÔÌìÁúÍþ", "±©ÁÒÁú»ê", "ÁúÆø³åÌì", "¾£¼¬Áú´Ì", "ÊÈÑª¿ñÁú", "ÁÒÑæÁú×¦", "ÉýÁúËé¿Õ", "·ÉÁúÔÚÌì", "Áú¾íÌìî¸", "Ð¡»ðÁúÖé", "ÖÐ»ðÁúÖé", "´ó»ðÁúÖé", "ÁÒ»ðÁúÖé", "ºÚÁúÖé", "°µÁúÖé", "Ó°ÁúÖé", "Ä§ÁúÖé", "ÑÖÁúÖé", "Ð°ÁúÖé", "¹íÁúÖé", "ÄõÁúÖé", "ÁúÖéËéÆ¬", "ÉñÁúËéÆ¬", "³¬¼¶»ðÁúÖé", "ÁúÕ½ÌìÏÂ", "¿ºÁúÓÐ»Ú", "ÉñÁú¼ûÊ×", "Áú»¢·çÔÆ", "ÁúÌÚ»¢Ô¾", "·ÉÁú³ËÔÆ", "ÉñÁúÖ®Å­", "ÕæÁúÕðÌì", "ÁúÖ®¿ñÐ¥", "»¢¾áÁúÅÌ", "Ä§Áú¿ñÐ¥", "ÉñÁú°ÚÎ²", "¾ÅÁú»¤Ìå", "²ÔÁúÖðÈÕ", "ÁúÕðÌìÍþ", "¾ÅÏöÁúÒ÷", "Ñ×Áú×ªÊÀ", "Ê¥Áú±Ó»¤", "¼ûÁúÔÚÌï" };

    this->Mod_Dragonball_Base_get_dragonball_skill_data();
    for (;;) {
        auto data = this->Mod_Dragonball_Base_get_tmp_dragonball_basic_info();
        int remain_normal_count = data[4].asInt();
        if (!remain_normal_count)
            return;

        // ÆÕÍ¨ÉÏ¹©
        data = this->Mod_Dragonball_Base_sacrifice(Mod_Dragonball_Base::NORMAL, Mod_Dragonball_Base::NOT_AUTO);
        if (data[0].asInt() != Mod_Dragonball_Base::SUCCESS){
            common::log(boost::str(boost::format("¡¾ÉñÁúÉÏ¹©¡¿ÆÕÍ¨ÉÏ¹©Ê§°Ü£¬result[%1%]") % data[0]), iEdit);
            return;
        }
        else
            common::log(boost::str(boost::format("¡¾ÉñÁúÉÏ¹©¡¿ÆÕÍ¨ÉÏ¹©£¬»ñµÃ [%1%]") % dragonball_names[data[1][0][1].asInt()]), iEdit);

        // Âô³ö»òÊ°È¡
        data = this->Mod_Dragonball_Base_get_tmp_dragonball_basic_info();
        auto tmp_grid_d_b_list = data[6];
        for (const auto& tmp_grid_d_b : tmp_grid_d_b_list) {
            int id = tmp_grid_d_b[0].asInt();
            if (tmp_grid_d_b[1].asInt() >= 27 && tmp_grid_d_b[1].asInt() <= 34) {
                data = this->Mod_Dragonball_Base_sell_dragonball(&id, 1);
                if (data[0].asInt() != Mod_Dragonball_Base::SUCCESS) {
                    common::log(boost::str(boost::format("¡¾ÉñÁúÉÏ¹©¡¿Âô³ö [%1%] Ê§°Ü£¬result[%2%]") % dragonball_names[tmp_grid_d_b[1].asInt()] % data[0]), iEdit);
                    return;
                }
                common::log(boost::str(boost::format("¡¾ÉñÁúÉÏ¹©¡¿Âô³ö [%1%]") % dragonball_names[tmp_grid_d_b[1].asInt()]), iEdit);
            } else {
                data = this->Mod_Dragonball_Base_collect_dragonball(&id, 1);
                if (data[0].asInt() != Mod_Dragonball_Base::SUCCESS) {
                    common::log(boost::str(boost::format("¡¾ÉñÁúÉÏ¹©¡¿Ê°È¡ [%1%] Ê§°Ü£¬result[%2%]") % dragonball_names[tmp_grid_d_b[1].asInt()] % data[0]), iEdit);
                    return;
                }
                common::log(boost::str(boost::format("¡¾ÉñÁúÉÏ¹©¡¿Ê°È¡ [%1%]") % dragonball_names[tmp_grid_d_b[1].asInt()]), iEdit);
            }
        }
    }
}

//============================================================================
// R180 Ãæ°å
// "module":111,"action":10,"request":[],"response":[Utils.ShortUtil,Utils.IntUtil,Utils.IntUtil,Utils.ShortUtil,Utils.ShortUtil,Utils.IntUtil]
// DragonBallData.as:
//     oObject.list(param1,this.dragonball_skill_data,["skill_level","skill_exp","scrap","luck","max_luck","gold_scrap"]);
// Example
//     [ 1, 0, 45, 100, 80, 0 ]
//============================================================================
Json::Value sxd_client::Mod_Dragonball_Base_get_dragonball_skill_data() {
    Json::Value data;
    return this->send_and_receive(data, 111, 10);
}

//============================================================================
// R180 ÉÏ¹©¸ñ×Ó
// "module":111,"action":0,"request":[],"response":[Utils.ShortUtil,Utils.ShortUtil,Utils.ByteUtil,Utils.ShortUtil,Utils.ShortUtil,Utils.ShortUtil,[Utils.IntUtil,Utils.ShortUtil,Utils.ByteUtil]]
// DragonBallData.as:
//     oObject.list(param1,this.dragon_altar_info,["normal_count","high_level_count","buff_id","buff_time","remain_normal_count","remain_high_level_count","tmp_grid_d_b_list"]);
//     for each(_loc2_ in this.dragon_altar_info["tmp_grid_d_b_list"])
//         oObject.list(_loc2_,_loc3_,["player_d_b_id","d_b_id","lv"]);
// Example
//     [ 0, 0, 0, 0, 20, 1, null ]
//     [ 1, 0, 7, 300, 19, 1, [ [ 3518917, 11, 1 ] ] ]
//     [ 2, 0, 7, 300, 18, 1, [ [ 3518918, 31, 1 ], [ 3518917, 11, 1 ] ] ]
//     [ 2, 0, 7, 293, 18, 1, null ]
//============================================================================
Json::Value sxd_client::Mod_Dragonball_Base_get_tmp_dragonball_basic_info() {
    Json::Value data;
    return this->send_and_receive(data, 111, 0);
}

//============================================================================
// R180 ÉÏ¹©
// "module":111,"action":2,"request":[Utils.UByteUtil,Utils.UByteUtil],"response":[Utils.UByteUtil,[Utils.IntUtil,Utils.ShortUtil,Utils.ByteUtil],Utils.UByteUtil,Utils.ShortUtil,Utils.IntUtil,Utils.IntUtil]
// DragonBallData.as:
//     this.result = param1[0];
//     for each(_loc2_ in param1[1])
//         oObject.list(_loc2_,_loc3_,["player_d_b_id","d_b_id","lv"]);
//     this.isPlayAnimation = param1[2] == Mod_Dragonball_Base.PLAY;
//     this.dragonball_skill_data.skill_level = param1[3];
//     this.dragonball_skill_data.skill_exp = param1[4];
//     this.chip = param1[5];
// Example
//     [ 4, 7 ] --> [ 0, [ [ 3518917, 11, 1 ] ], 14, 1, 5, 45 ]
//     [ 4, 7 ] --> [ 0, [ [ 3518918, 31, 1 ] ], 14, 1, 12, 45 ]
//============================================================================
Json::Value sxd_client::Mod_Dragonball_Base_sacrifice(int mode, int auuto) {
    Json::Value data;
    data.append(mode);
    data.append(auuto);
    return this->send_and_receive(data, 111, 2);
}

//============================================================================
// R180 Ò»¼üÊ°È¡
// "module":111,"action":1,"request":[[Utils.IntUtil]],"response":[Utils.UByteUtil,[Utils.IntUtil]]
// DragonBallData.as:
//     this.result = param1[0];
//     for each(_loc2_ in param1[1])
//         oObject.list(_loc2_,_loc3_,["player_d_b_id","d_b_id"]);
// Example
//     [ [ [ 3518917 ] ] ] --> [ 0, [ [ 3518917 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_Dragonball_Base_collect_dragonball(int ids[], int count) {
    Json::Value data, data1;
    for (int i = 0; i < count; i++) {
        Json::Value data2;
        data2.append(ids[i]);
        data1.append(data2);
    }
    data.append(data1);
    return this->send_and_receive(data, 111, 1);
}

//============================================================================
// R180 Ò»¼üÂô³ö
// "module":111,"action":7,"request":[[Utils.IntUtil]],"response":[Utils.UByteUtil]
// DragonBallData.as:
//     this.result = param1[0];
// Example
//     [ [ [ 3518918 ] ] ] --> [ 0 ]
//============================================================================
Json::Value sxd_client::Mod_Dragonball_Base_sell_dragonball(int ids[], int count) {
    Json::Value data, data1;
    for (int i = 0; i < count; i++) {
        Json::Value data2;
        data2.append(ids[i]);
        data1.append(data2);
    }
    data.append(data1);
    return this->send_and_receive(data, 111, 7);
}

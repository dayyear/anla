#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_Library_Base {
public:
    static const int YES = 0;
    static const int NO = 1;
    static const int SUCCESS = 2;
};

class Mod_FindImmortal_Base {
public:
    static const int SUCCESS = 0;
};

//============================================================================
// ²Ø¾­¸ó
//============================================================================
void sxd_client::library() {
    // ÊÇ·ñ¿ªÍ¨FindImmortal(70, Ï²´ÓÌì½µ)¹¦ÄÜ
    auto data = this->Mod_Player_Base_get_player_function();
    if (std::find_if(data[0].begin(), data[0].end(), [](const Json::Value& x) {return x[0].asInt() == 70;}) != data[0].end()) {
        data = this->Mod_Player_Base_player_info_contrast(player_id);
        int fame = data[0][0][4].asInt();
        int xian_ling = data[0][0][10].asInt();
        // ÏÉÁîÐ¡ÓÚ2000ÔòÓÃÉùÍû¶Ò»»
        if (fame >= (1200200) && xian_ling < 2000) {
            int exchange_xian_ling = (fame - 1200000) / 200;
            data = this->Mod_FindImmortal_Base_exchange_xian_ling(exchange_xian_ling);
            if (data[0].asInt() == Mod_FindImmortal_Base::SUCCESS)
                common::log(boost::str(boost::format("¡¾²Ø¾­¸ó¡¿ÏûºÄ [ÉùÍû¡Á%1%] ¶Ò»» [ÏÉÁî¡Á%2%]") % fame % exchange_xian_ling), iEdit);
        }
    }

    std::string jobs[] = { "", "ÎäÊ¥", "½£Áé", "·ÉÓð", "", "½«ÐÇ", "ÊõÊ¿" };
    data = this->Mod_Library_Base_get_library_info();
    auto role_job_info_list = data[0];
    bool is_activate_upgrade = data[4].asInt() == Mod_Library_Base::YES;
    for (int job_id = 1; job_id <= (is_activate_upgrade ? 6 : 3); ++job_id) {
        if (job_id == 4)
            continue;
        if (!is_activate_upgrade && std::find_if(role_job_info_list.begin(), role_job_info_list.end(), [job_id](const Json::Value& x) {return x[0].asInt() == job_id && x[1].asInt() == 180;}) != role_job_info_list.end())
            continue;


        //this->Mod_Library_Base_up_library_level(job_id);
        data = this->Mod_Library_Base_up_library_level(job_id);
        if (data[0].asInt() == Mod_Library_Base::SUCCESS)
            common::log(boost::str(boost::format("¡¾²Ø¾­¸ó¡¿Éý¼¶ [%1%] ¾­Âö") % jobs[job_id]), iEdit);
    }
}

//============================================================================
// 2019030101 ÉùÍû¶Ò»»ÏÉÁî
// "module":68,"action":6,"request":[Utils.IntUtil],"response":[Utils.UByteUtil]
// Example
//     [ 1 ] -> [ 0(Mod_FindImmortal_Base::SUCCESS) ]
//============================================================================
Json::Value sxd_client::Mod_FindImmortal_Base_exchange_xian_ling(int num) {
    Json::Value data;
    data.append(num);
    return this->send_and_receive(data, 68, 6);
}

//============================================================================
// 2019030101 ²Ø¾­¸óÃæ°å
// "module":85,"action":1,"request":[],"response":[[Utils.ByteUtil,Utils.ShortUtil],Utils.ShortUtil,Utils.ShortUtil,Utils.ShortUtil,Utils.UByteUtil]
// LibraryData.as:
//     for each(_loc2_ in param1[0])
//         this._role_job_info_list.push({
//             "role_job_id":_loc2_[0],
//             "job_library_level":_loc2_[1]
//         });
//     this._cur_role_id = param1[1];
//     this._cur_role_stunt_id = param1[2];
//     this._study_stunt_type_id = param1[3];
//     this._is_activate_upgrade = param1[4] == Mod_Library_Base.YES;
// Example
//     [ [ [ 3(·ÉÓð), 149 ], [ 1(ÎäÊ¥), 140 ], [ 2(½£Áé), 149 ] ], 5(·ÉÓðÄÐ), 184(Çî×·ÃÍ´ò), 117, 1(Mod_Library_Base.NO) ]
//     [ [ [ 1(ÎäÊ¥), 242(´øÂöÆßÐÇ¶þ½×) ], [ 6(ÊõÊ¿), 240(´øÂöÁùºÏÊ®½×) ], [ 5(½«ÐÇ), 240(´øÂöÁùºÏÊ®½×) ], [ 2(½£Áé), 243(´øÂöÆßÐÇÈý½×) ], [ 3(·ÉÓð), 253(´øÂö°ËØÔÈý½×) ] ], 6, 209, 322, 0(Mod_Library_Base.YES) ]
//     [ [ [ 3(·ÉÓð), 151(¶½ÂöÆßÐÇÒ»½×) ], [ 1(ÎäÊ¥), 143(¶½ÂöÁùºÏÈý½×) ], [ 2(½£Áé), 151(¶½ÂöÆßÐÇÒ»½×) ] ], 5, 184, 117, 1(Mod_Library_Base.NO) ]
//============================================================================
Json::Value sxd_client::Mod_Library_Base_get_library_info() {
    Json::Value data;
    return this->send_and_receive(data, 85, 1);
}

//============================================================================
// 2019030101 ²Ø¾­¸ó¼¤»î
// "module":85,"action":4,"request":[],"response":[Utils.UByteUtil]
// LibraryData.as:
//     this.result = param1[0];
// Example
//     [ 2(Mod_Library_Base::SUCCESS) ]
//============================================================================
Json::Value sxd_client::Mod_Library_Base_activate_library_upgrade() {
    Json::Value data;
    return this->send_and_receive(data, 85, 4);
}

//============================================================================
// 2019030101 ²Ø¾­¸ó×ªÖ°Éý¼¶
// "module":85,"action":2,"request":[Utils.ByteUtil],"response":[Utils.UByteUtil]
// Example
//     [ 1(ÎäÊ¥) ] -> [ 2(Mod_Library_Base::SUCCESS) ]
//============================================================================
Json::Value sxd_client::Mod_Library_Base_up_library_level(int job_id) {
    Json::Value data;
    data.append(job_id);
    //return this->send_frame(data, 85, 2);
    return this->send_and_receive(data, 85, 2);
}

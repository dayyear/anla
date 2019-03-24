#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_ThreeAnniversarySignIn_Base {
public:
    static const int YES = 0;
    static const int SUCCESS = 27;
};

void sxd_client::sign_in() {
    if (!this->get_empty_pack_num())
        return;

    Json::Value data = this->Mod_ThreeAnniversarySignIn_Base_get_sign_in_status();
    if (data[0].asInt() != Mod_ThreeAnniversarySignIn_Base::YES)
        common::log(boost::str(boost::format("【新年签到】活动未开启，isActivity[%1%]") % data[0]), 0);
    else {
        if (data[1].asInt() != Mod_ThreeAnniversarySignIn_Base::YES)
            common::log(boost::str(boost::format("【新年签到】活动不可参与，isCanJoin[%1%]") % data[1]), 0);
        else {
            data = this->Mod_ThreeAnniversarySignIn_Base_get_player_sign_in_info();
            if (data[3].asInt() == Mod_ThreeAnniversarySignIn_Base::YES)
                common::log("【新年签到】今日已签到", 0);
            else {
                data = this->Mod_ThreeAnniversarySignIn_Base_player_sign_in();
                if (data[0].asInt() == Mod_ThreeAnniversarySignIn_Base::SUCCESS)
                    common::log("【新年签到】今日签到成功", iEdit);
                else
                    common::log(boost::str(boost::format("【新年签到】今日签到失败，result[%1%]") % data[0]), iEdit);
            }
        }
    }
}

//============================================================================
// R171
// 状态
// {module:151, action:1,
// request:[],
// response:[Utils.UByteUtil, Utils.UByteUtil]}
// ThreeAnniversarySignInData.as 356:
//     this.isActivity = param1[0] == Mod_ThreeAnniversarySignIn_Base.YES;
//     this.isCanJoin = param1[1] == Mod_ThreeAnniversarySignIn_Base.YES;
//============================================================================
Json::Value sxd_client::Mod_ThreeAnniversarySignIn_Base_get_sign_in_status() {
    Json::Value data;
    return this->send_and_receive(data, 151, 1);
}

//============================================================================
// R171
// 信息
// {module:151, action:0,
// request:[],
// response:[Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.UByteUtil, [Utils.IntUtil, Utils.UByteUtil, Utils.IntUtil], Utils.ByteUtil, Utils.ByteUtil, Utils.ByteUtil, [Utils.ByteUtil, Utils.UByteUtil, [Utils.UByteUtil, Utils.IntUtil, Utils.IntUtil]]]}
// ThreeAnniversarySignInData.as 34:
//     oObject.list(param1, this.getSignInInfo, ["activity_start_date", "activity_end_date", "now_date", "is_today_signed_in", "signed_in_data_list", "total_signed_in_count", "repair_sign_in_count", "tea_eggs_count", "tea_eggs_exchange_list"]);
// Example
// 签到前后对比
//     [ 1515427200, 1523116800, 1518883200, 1, [ [ 1518537600, 17, 5 ], [ 1466956800, 9, 1 ], [ 1484150400, 9, 1 ], [ 1483718400, 9, 1 ], [ 1482076800, 20, 30 ], [ 1466524800, 2, 500000 ], [ 1501689600, 9, 1 ], [ 1484928000, 2, 2000000 ], [ 1483459200, 36, 1 ], [ 1501776000, 15, 1 ], [ 1486483200, 2, 2000000 ], [ 1484755200, 2, 2000000 ], [ 1467302400, 20, 5 ], [ 1466784000, 15, 1 ], [ 1486051200, 15, 1 ], [ 1500048000, 20, 15 ], [ 1467043200, 36, 1 ], [ 1467475200, 10, 50 ], [ 1486915200, 15, 1 ], [ 1467129600, 35, 1 ], [ 1484582400, 15, 1 ], [ 1487433600, 20, 10 ], [ 1484496000, 20, 20 ], [ 1466870400, 9, 1 ], [ 1487779200, 16, 5 ], [ 1482163200, 20, 30 ], [ 1483632000, 20, 30 ], [ 1466611200, 20, 20 ], [ 1499270400, 2, 2000000 ], [ 1466438400, 15, 1 ], [ 1501603200, 15, 1 ], [ 1487692800, 4, 1 ], [ 1467388800, 20, 15 ] ], 1, 39, 0, [ [ 5, 22, [ [ 2, 0, 5000000 ], [ 24, 2016, 2 ] ] ], [ 10, 22, [ [ 24, 4236, 1 ], [ 24, 1944, 2 ] ] ], [ 15, 22, [ [ 24, 4235, 1 ], [ 3, 0, 10 ] ] ], [ 20, 22, [ [ 24, 4234, 1 ], [ 3, 0, 20 ] ] ] ] ]
//     [ 1515427200, 1523116800, 1518883200, 0, [ [ 1518537600, 17, 5 ], [ 1466956800, 9, 1 ], [ 1484150400, 9, 1 ], [ 1483718400, 9, 1 ], [ 1482076800, 20, 30 ], [ 1466524800, 2, 500000 ], [ 1501689600, 9, 1 ], [ 1484928000, 2, 2000000 ], [ 1483459200, 36, 1 ], [ 1501776000, 15, 1 ], [ 1486483200, 2, 2000000 ], [ 1484755200, 2, 2000000 ], [ 1467302400, 20, 5 ], [ 1518883200, 15, 1 ], [ 1466784000, 15, 1 ], [ 1486051200, 15, 1 ], [ 1500048000, 20, 15 ], [ 1467043200, 36, 1 ], [ 1467475200, 10, 50 ], [ 1486915200, 15, 1 ], [ 1467129600, 35, 1 ], [ 1484582400, 15, 1 ], [ 1487433600, 20, 10 ], [ 1484496000, 20, 20 ], [ 1466870400, 9, 1 ], [ 1487779200, 16, 5 ], [ 1482163200, 20, 30 ], [ 1483632000, 20, 30 ], [ 1466611200, 20, 20 ], [ 1499270400, 2, 2000000 ], [ 1466438400, 15, 1 ], [ 1501603200, 15, 1 ], [ 1487692800, 4, 1 ], [ 1467388800, 20, 15 ] ], 2, 39, 1, [ [ 5, 22, [ [ 2, 0, 5000000 ], [ 24, 2016, 2 ] ] ], [ 10, 22, [ [ 24, 4236, 1 ], [ 24, 1944, 2 ] ] ], [ 15, 22, [ [ 24, 4235, 1 ], [ 3, 0, 10 ] ] ], [ 20, 22, [ [ 24, 4234, 1 ], [ 3, 0, 20 ] ] ] ] ]
//============================================================================
Json::Value sxd_client::Mod_ThreeAnniversarySignIn_Base_get_player_sign_in_info() {
    Json::Value data;
    return this->send_and_receive(data, 151, 0);
}

//============================================================================
// R171
// 签到
// {module:151, action:2,
// request:[Utils.ByteUtil],
// 0,签到 1,补签
// response:[Utils.UByteUtil, Utils.UByteUtil, [Utils.IntUtil, Utils.UByteUtil, Utils.IntUtil], Utils.ByteUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, [Utils.ByteUtil, Utils.UByteUtil, [Utils.UByteUtil, Utils.IntUtil, Utils.IntUtil]]]}
// ThreeAnniversarySignInData.as 372:
//     oObject.list(param1, this.playerSignIn, ["result", "is_today_signed_in", "signed_in_data_list", "total_signed_in_count", "repair_sign_in_count", "signed_in", "item_id", "tea_eggs_count", "tea_eggs_exchange_list"]);
// Example
//     [ 25, 0, [ [ 1518537600, 17, 5 ], [ 1466956800, 9, 1 ], [ 1484150400, 9, 1 ], [ 1483718400, 9, 1 ], [ 1482076800, 20, 30 ], [ 1466524800, 2, 500000 ], [ 1501689600, 9, 1 ], [ 1484928000, 2, 2000000 ], [ 1483459200, 36, 1 ], [ 1501776000, 15, 1 ], [ 1486483200, 2, 2000000 ], [ 1484755200, 2, 2000000 ], [ 1467302400, 20, 5 ], [ 1518883200, 15, 1 ], [ 1466784000, 15, 1 ], [ 1486051200, 15, 1 ], [ 1500048000, 20, 15 ], [ 1467043200, 36, 1 ], [ 1467475200, 10, 50 ], [ 1486915200, 15, 1 ], [ 1467129600, 35, 1 ], [ 1484582400, 15, 1 ], [ 1487433600, 20, 10 ], [ 1484496000, 20, 20 ], [ 1466870400, 9, 1 ], [ 1487779200, 16, 5 ], [ 1482163200, 20, 30 ], [ 1483632000, 20, 30 ], [ 1466611200, 20, 20 ], [ 1499270400, 2, 2000000 ], [ 1466438400, 15, 1 ], [ 1501603200, 15, 1 ], [ 1487692800, 4, 1 ], [ 1467388800, 20, 15 ] ], 2, 39, 1518883200, 0, 1, [ [ 5, 22, [ [ 2, 0, 5000000 ], [ 24, 2016, 2 ] ] ], [ 10, 22, [ [ 24, 4236, 1 ], [ 24, 1944, 2 ] ] ], [ 15, 22, [ [ 24, 4235, 1 ], [ 3, 0, 10 ] ] ], [ 20, 22, [ [ 24, 4234, 1 ], [ 3, 0, 20 ] ] ] ] ]
//============================================================================
Json::Value sxd_client::Mod_ThreeAnniversarySignIn_Base_player_sign_in() {
    Json::Value data;
    data.append(0);
    return this->send_and_receive(data, 151, 2);
}

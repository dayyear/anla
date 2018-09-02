#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_DunhuangTreasure_Base {
public:
    static const int TONG = 6;
    static const int SUCCESS = 7;
};

//============================================================================
// 敦煌秘宝
//============================================================================
void sxd_client::dunhuang_treasure() {
    auto data = this->Mod_DunhuangTreasure_Base_get_dunhuang_treasure_info();
    auto gift_list = data[7];
    for (const auto& gift : gift_list) {
        int type = gift[0].asInt();
        int open_times = gift[3].asInt();
        if (type == Mod_DunhuangTreasure_Base::TONG && !open_times) {
            data = this->Mod_DunhuangTreasure_Base_open_treasure(type);
            if (data[0].asInt() == Mod_DunhuangTreasure_Base::SUCCESS)
                common::log(boost::str(boost::format("【敦煌秘宝】开启 [玉珠机关]，获得 [积分×%1%]") % data[2]), iEdit);
        }
    }
}

//============================================================================
// R181 面板
// "module":356,"action":0,"request":[],"response":[[Utils.IntUtil,Utils.IntUtil,Utils.IntUtil],Utils.UByteUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,[Utils.UByteUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil]]
// DunHuangTreasureData.as:
//     oObject.list(param1,this._info,["award_list","prop_type","tong_coin_times","tong_coin_cost","score","open_times","max_open_times","gift_list"]);
//     for each(_loc3_ in this._info.award_list)
//         oObject.list(_loc3_,_loc4_,["award_id","item_id","item_num"]);
//     for each(_loc3_ in this._info.gift_list)
//         oObject.list(_loc3_,_loc4_,["type","gift_item_id","gift_num","open_times"]);
// Example
//     [ [ [ 25, 5104, 1 ], [ 28, 1444, 2 ], [ 13, 5117, 5 ], [ 38, 5522, 1 ], [ 33, 5397, 1 ], [ 18, 1747, 4500000 ], [ 31, 2395, 5 ], [ 11, 5398, 1 ] ], 3, 4, 0, 0, 0, 50,
//       [ [ 4, 4982, 3, 0 ], [ 5, 4982, 2, 0 ], [ **6**, 5104, 2, **0** ] ] ]
//     [ [ [ 36, 2263, 8 ], [ 25, 5104, 1 ], [ 32, 5156, 5 ], [ 6, 1740, 20 ], [ 9, 347, 50 ], [ 22, 3050, 25 ], [ 23, 4519, 25 ], [ 37, 5520, 1 ] ], 3, 3, 5000000, 30, 1, 50,
//       [ [ 4, 4982, 3, 0 ], [ 5, 4982, 2, 0 ], [ ****6, 5104, 2, **1** ] ] ]
//============================================================================
Json::Value sxd_client::Mod_DunhuangTreasure_Base_get_dunhuang_treasure_info() {
    Json::Value data;
    return this->send_and_receive(data, 356, 0);
}

//============================================================================
// R181 打开机关
// "module":356,"action":1,"request":[Utils.UByteUtil],"response":[Utils.UByteUtil,[Utils.IntUtil,Utils.IntUtil,Utils.IntUtil],Utils.IntUtil]
// DunHuangTreasureData.as:
//     oObject.list(param1,this.result,["result","select_list","add_score"]);
//     for each(_loc3_ in this.result.select_list)
//         oObject.list(_loc3_,_loc4_,["award_id","item_id","item_num"]);
// Example
//     [ 6 ] --> [ 7, [ [ 31, 2395, 5 ], [ 11, 5398, 1 ], [ 25, 5104, 1 ] ], 30 ]
//============================================================================
Json::Value sxd_client::Mod_DunhuangTreasure_Base_open_treasure(int type) {
    Json::Value data;
    data.append(type);
    return this->send_and_receive(data, 356, 1);
}

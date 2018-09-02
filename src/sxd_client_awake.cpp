#include <map>
#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_Awake_Base {
public:
    static const int SUCCESS = 5;
    static const int MORTAL = 16;   // 人间
    static const int REGULUS = 17;  // 轩辕
    static const int ANCIENT = 18;  // 上古
    static const int ONCE = 19;
    static const int MULTI = 20;
};

void sxd_client::awake() {
    std::map<int, std::string> forbidden_map = { { 16, "人间" }, { 17, "轩辕" }, { 18, "上古" } };
    Json::Value data = this->Mod_Awake_Base_player_forbidden_book_info();
    Json::Value items = data[0];
    for (const auto& item : items) {
        if (item[1].asInt() == Mod_Awake_Base::ONCE && item[2].asInt() == 0 && item[3].asInt() == 0) {
            data = this->Mod_Awake_Base_explore_forbidden_book(item[0].asInt(), Mod_Awake_Base::ONCE);
            if (data[0].asInt() == Mod_Awake_Base::SUCCESS)
                common::log(boost::str(boost::format("【觉醒】探索 [%1%] 禁地之书") % forbidden_map[item[0].asInt()]), iEdit);
            else
                common::log(boost::str(boost::format("【觉醒】探索 [%1%] 禁地之书失败，result[%2%]") % forbidden_map[item[0].asInt()] % data[0]), iEdit);
        }
    }
}

//============================================================================
// R173 禁地之书
// "module":199, "action":11, "request":[],
// "response":[[Utils.UByteUtil,Utils.UByteUtil,Utils.IntUtil,Utils.IntUtil,Utils.ShortUtil,Utils.ShortUtil,Utils.IntUtil],Utils.IntUtil]
// AwakeData.as 123:
//     for each(_loc4_ in param1[0])
//         oObject.list(_loc4_,_loc3_,["forbidden_type","explore_type","cost_coin","cost_ingot","cur_number","total_number","next_free_time"]);
// Example
//     [ [ [ 17, 19, 0, 0,   0, 21, 1523030400 ],
//         [ 16, 19, 0, 0,   0, 21, 1523030400 ],
//         [ 16, 20, 0, 100, 0, 20, 0 ],
//         [ 18, 19, 0, 30,  0, 20, 1523030400 ],
//         [ 18, 20, 0, 300, 0, 20, 0 ],
//         [ 17, 20, 0, 200, 0, 20, 0 ] ], 0 ]
//     [ [ [ 17, 19, 0, 0,   0, 21, 1523116800 ],
//         [ 16, 19, 0, 0,   0, 21, 1523116800 ],
//         [ 16, 20, 0, 100, 0, 20, 0 ],
//         [ 18, 19, 0, 0,   0, 21, 1523203200 ],
//         [ 18, 20, 0, 300, 0, 20, 0 ],
//         [ 17, 20, 0, 200, 0, 20, 0 ] ], 0 ]
//============================================================================
Json::Value sxd_client::Mod_Awake_Base_player_forbidden_book_info() {
    Json::Value data;
    return this->send_and_receive(data, 199, 11);
}

//============================================================================
// R173 探索
// "module":199, "action":12,
// "request":[Utils.UByteUtil,Utils.UByteUtil],
// AwakeForbiddenBookView.as 209:
//     this.explore_forbidden_book(forbiddenType,timesType,this.exploreForbiddenBookCallBack);
// Example
//
// "response":[Utils.UByteUtil,[Utils.IntUtil,Utils.ShortUtil,Utils.ShortUtil,Utils.ShortUtil,Utils.IntUtil,Utils.ShortUtil]]
// AwakeData.as 145:
//     this._exploreForbiddenBookObj.result = param1[0];
//     for each(_loc4_ in param1[1])
//         oObject.list(_loc4_,_loc3_,["player_awake_id","item_id","grid_id","level","resolve_price","number"]);
// Example
//
//============================================================================
Json::Value sxd_client::Mod_Awake_Base_explore_forbidden_book(int forbidden_type, int times_type) {
    Json::Value data;
    data.append(forbidden_type);
    data.append(times_type);
    return this->send_and_receive(data, 199, 12);
}


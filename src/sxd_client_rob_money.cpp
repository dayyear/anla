#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_RobMoney_Base {
public:
    static const int NOT_SEARCH = 0;
    static const int SEARCHED = 1;
    static const int SUCCESS = 5;
};

//============================================================================
// R179 ½ÙïÚ
//============================================================================
void sxd_client::rob_money() {
    for (;;) {
        auto data = this->Mod_RobMoney_Base_get_rob_money_info();
        int rob_status = data[0].asInt();
        int free_rob_times = data[2].asInt();
        if (rob_status == Mod_RobMoney_Base::SEARCHED) {
            data = this->Mod_RobMoney_Base_rob();
            if (data[0].asInt() != Mod_RobMoney_Base::SUCCESS) {
                common::log(boost::str(boost::format("¡¾½ÙïÚ¡¿½ÙïÚÊ§°Ü£¬result[%1%]") % data[0]), iEdit);
                return;
            }
            std::vector<std::string> items;
            std::transform(data[1].begin(), data[1].end(), std::back_inserter(items), [this](const Json::Value& x) {
                return boost::str(boost::format("[%1%¡Á%2%]") % db.get_code(version, "Item", x[0].asInt())["text"] % x[1]);
            });
            common::log(boost::str(boost::format("¡¾½ÙïÚ¡¿½ÙïÚ»ñµÃ£º%1%") % boost::algorithm::join(items, "£¬")), iEdit);
        } else {
            if (free_rob_times == 0)
                break;
            data = this->Mod_RobMoney_Base_search();
            if (data[1].asInt() != Mod_RobMoney_Base::SUCCESS) {
                common::log(boost::str(boost::format("¡¾½ÙïÚ¡¿¿±²ìÊ§°Ü£¬result[%1%]") % data[1]), iEdit);
                return;
            }
            common::log("¡¾½ÙïÚ¡¿¿±²ì", iEdit);
        }
    }
}

//============================================================================
// R179 Ãæ°å
// "module":314,"action":0,"request":[],"response":[Utils.UByteUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,[Utils.IntUtil,Utils.UByteUtil,Utils.IntUtil,Utils.IntUtil],Utils.StringUtil,Utils.IntUtil,Utils.IntUtil]
// RobMoneyData.as 52:
//     oObject.list(param1,this.robMoneyObject,["rob_status","total_rob_times","free_rob_times","left_buy_times","max_buy_times","ship_list","nickname","had_rob_times","max_rob_times"]);
// Example
//     [ 0, 1, 1, 3, 3, [ [ 558404, 4, 70006, 0 ], [ 558412, 2, 5573, 0 ], [ 558406, 3, 55189, 0 ], [ 558405, 4, 66211, 0 ], [ 558403, 2, 77166, 0 ], [ 558398, 2, 118195, 0 ], [ 558397, 3, 128844, 0 ], [ 558402, 2, 88833, 0 ], [ 558408, 4, 41269, 0 ], [ 558407, 4, 47781, 0 ], [ 558411, 4, 13099, 0 ], [ 558409, 4, 34015, 0 ], [ 558400, 3, 105333, 0 ], [ 558401, 4, 93885, 0 ], [ 558410, 4, 26053, 0 ], [ 558399, 4, 110988, 0 ] ], "", 354, 50 ]
//============================================================================
Json::Value sxd_client::Mod_RobMoney_Base_get_rob_money_info() {
    Json::Value data;
    return this->send_and_receive(data, 314, 0);
}

//============================================================================
// R179 ¿±²ì
// "module":314,"action":1,"request":[],"response":[[Utils.IntUtil],Utils.UByteUtil]
// RobMoneyData.as 80:
//     oObject.list(param1,this.searchObject,["ship_id_list","result"]);
// Example
//     [ [ [ 558403 ] ], 5 ]
//============================================================================
Json::Value sxd_client::Mod_RobMoney_Base_search() {
    Json::Value data;
    return this->send_and_receive(data, 314, 1);
}

//============================================================================
// R179 ½ÙïÚ
// "module":314,"action":2,"request":[],"response":[Utils.UByteUtil,[Utils.IntUtil,Utils.IntUtil]]
// RobMoneyData.as 88:
//     this.result = param1[0];
//     for each(_loc3_ in param1[1])
//         oObject.list(_loc3_,_loc2_,["item_id","item_count"]);
// Example
//     [ 5, [ [ 1747, 5000000 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_RobMoney_Base_rob() {
    Json::Value data;
    return this->send_and_receive(data, 314, 2);
}

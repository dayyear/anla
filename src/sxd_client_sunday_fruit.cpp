#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_SundayFruit_Base {
public:
    static const int YES = 0;
    static const int SUCCESS = 2;
};

//============================================================================
// 周末水果机
//============================================================================
void sxd_client::sunday_fruit() {
    auto data = this->Mod_SundayFruit_Base_active_status();
    if (data[0].asInt() != Mod_SundayFruit_Base::YES)
        return;

    for (;;) {
        data = this->Mod_SundayFruit_Base_sunday_fruit_info();
        if (!data[0].asInt())
            break;
        data = this->Mod_SundayFruit_Base_draw();
        if (data[0].asInt() != Mod_SundayFruit_Base::SUCCESS) {
            common::log(boost::str(boost::format("【周末水果机】转一次失败，result[%1%]") % data[0]));
            return;
        }
        common::log("【周末水果机】转一次");
        /*std::vector<std::string> items;
        std::transform(data[2].begin(), data[2].end(), std::back_inserter(items), [this](const Json::Value& x) {
            return boost::str(boost::format("[%1%×%2%]") % db.get_code(version, "Item", x[1].asInt())["text"] % x[2]);
        });
        common::log(boost::str(boost::format("【周末水果机】转一次，获得 %1%") % boost::algorithm::join(items, "，")));*/
    }

}

//============================================================================
// R179 状态
// "module":242,"action":0,"request":[],"response":[Utils.UByteUtil,Utils.IntUtil,Utils.IntUtil]
// SundayFruitData.as:
//     this.sundayFruitInfo.status = param1[0];
//     this.sundayFruitInfo.start_time = param1[1];
//     this.sundayFruitInfo.end_time = param1[2];
// Example
//     [ 0, 1529078400, 1529251199 ]
//============================================================================
Json::Value sxd_client::Mod_SundayFruit_Base_active_status() {
    Json::Value data;
    return this->send_and_receive(data, 242, 0);
}

//============================================================================
// R179 面板
// "module":242,"action":1,"request":[],"response":[Utils.IntUtil,[Utils.IntUtil,Utils.IntUtil,Utils.IntUtil]]
// SundayFruitData.as:
//     this.sundayFruitInfo.remain_draw_times = param1[0];
//     this.sundayFruitInfo.award_list = this.parseAwardList(param1[1]);
// Example
//     [ 10, [ [ 9, 0, 80 ], [ 1, 0, 1000000 ], [ 16, 13, 1 ], [ 12, 347, 8 ], [ 27, 60, 2 ], [ 12, 347, 5 ], [ 27, 69, 1 ], [ 5, 0, 60000 ], [ 7, 0, 5 ], [ 7, 0, 3 ], [ 12, 347, 10 ], [ 5, 0, 30000 ], [ 3, 0, 100 ], [ 1, 0, 1200000 ], [ 27, 37, 2 ], [ 15, 2, 1 ], [ 35, 0, 5 ], [ 1, 0, 600000 ], [ 12, 1705, 20 ], [ 1, 0, 800000 ], [ 12, 3399, 2 ], [ 15, 1, 1 ], [ 9, 0, 120 ], [ 27, 12, 2 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_SundayFruit_Base_sunday_fruit_info() {
    Json::Value data;
    return this->send_and_receive(data, 242, 1);
}

//============================================================================
// R179 转一次
// "module":242,"action":2,"request":[],"response":[Utils.UByteUtil,[Utils.ByteUtil],[Utils.IntUtil,Utils.IntUtil,Utils.IntUtil]]
// SundayFruitData.as:
//     this.sundayFruitInfo.result = param1[0];
//     this.sundayFruitInfo.draw_fruit_list = param1[1];
//     this.sundayFruitInfo.draw_award_list = this.parseAwardList(param1[2]);
//     for each(_loc3_ in param1)
//         oObject.list(_loc3_,_loc4_,["award_type","award_id","award_number"]);
// Example
//     [ 2, [ [ 2 ], [ 3 ], [ 5 ] ], [ [ 27, 12, 2 ], [ 15, 2, 1 ], [ 3, 0, 100 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_SundayFruit_Base_draw() {
    Json::Value data;
    return this->send_and_receive(data, 242, 2);
}

#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_StLuckySuperNumber_Base {
public:
    static const int SUCCESS = 0;
};

//============================================================================
// R179 幸运大比拼
//============================================================================
void sxd_client::lucky_super_number() {
    for (;;) {
        auto data = this->Mod_StLuckySuperNumber_Base_get_lucky_super_number_info();
        int remain_draw_times = data[1].asInt();
        if (remain_draw_times == 0)
            break;

        auto my_lucky_number_list = data[2];
        for (const Json::Value& my_lucky_number : my_lucky_number_list) {
            int position = my_lucky_number[0].asInt();
            int number = my_lucky_number[1].asInt();
            if (number)
                continue;

            data = this->Mod_StLuckySuperNumber_Base_draw(position);
            if (data[0].asInt() != Mod_StLuckySuperNumber_Base::SUCCESS) {
                //common::log(boost::str(boost::format("【幸运大比拼】抽取失败，result[%1%]") % data[0]));
                return;
            }
            common::log(boost::str(boost::format("【幸运大比拼】抽取第%1%个数字 [%2%]") % data[1] % data[2]));
            break;
        }
    }
}

//============================================================================
// R179 面板
// "module":311,"action":0,"request":[],"response":[Utils.IntUtil,Utils.IntUtil,[Utils.ByteUtil,Utils.ByteUtil,Utils.ByteUtil],Utils.IntUtil]
// StLuckySuperNumberData.as 33:
//     oObject.list(param1,this.stLuckySuperNumberObject,["active_value","remain_draw_times","my_lucky_number_list","lucky_number","lucky_number_award_status"]);
//     for each(_loc5_ in this.stLuckySuperNumberObject.my_lucky_number_list)
//         oObject.list(_loc5_,_loc3_,["position","number","is_draw"]);
// Example
//     [ 865, 4, [ [ 1, 0, 0 ], [ 2, 0, 0 ], [ 3, 0, 0 ], [ 4, 0, 0 ], [ 5, 0, 0 ] ], 4 ]
//     [ 865, 3, [ [ 1, 7, 1 ], [ 2, 0, 0 ], [ 3, 0, 0 ], [ 4, 0, 0 ], [ 5, 0, 0 ] ], 4 ]
//     [ 865, 2, [ [ 1, 7, 1 ], [ 2, 4, 1 ], [ 3, 0, 0 ], [ 4, 0, 0 ], [ 5, 0, 0 ] ], 4 ]
//============================================================================
Json::Value sxd_client::Mod_StLuckySuperNumber_Base_get_lucky_super_number_info() {
    Json::Value data;
    return this->send_and_receive(data, 311, 0);
}

//============================================================================
// R179 抽取
// "module":311,"action":1,"request":[Utils.ByteUtil],"response":[Utils.UByteUtil,Utils.ByteUtil,Utils.ByteUtil]
// StLuckySuperNumberData.as 76:
//     oObject.list(param1,this.drawObject,["result","position","number"]);
// Example
//     [ 0, 1, 7 ]
//     [ 0, 2, 4 ]
//============================================================================
Json::Value sxd_client::Mod_StLuckySuperNumber_Base_draw(int position) {
    Json::Value data;
    data.append(position);
    return this->send_and_receive(data, 311, 1);
}

#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

void sxd_client::furniture_effect() {
    Json::Value data = this->Mod_MarryHome_Base_batch_get_furniture_effect();
    if (data[0].size())
        common::log("【家园】每日领取家园奖励");
}

//============================================================================
// R172 家园奖励
// {module:210, action:18, request:[],
// response:[[Utils.IntUtil, Utils.IntUtil]]};
// MarryHomeData.as 306:
//     for each (_loc_2 in param1[0])
//         oObject.list(_loc_2, _loc_3, ["item_id", "effect"]);
//============================================================================
Json::Value sxd_client::Mod_MarryHome_Base_batch_get_furniture_effect() {
    Json::Value data;
    return this->send_and_receive(data, 210, 18);
}

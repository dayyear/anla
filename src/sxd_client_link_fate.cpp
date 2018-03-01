#include "sxd_client.h"

//============================================================================
// R171
// 结缘宝箱
// {module:232, action:30, request:[],
// response:[[Utils.IntUtil, Utils.IntUtil], Utils.ShortUtil, Utils.ShortUtil, Utils.ShortUtil]}
// LinkFateController.as 18:
//     var _loc_1:* = _data.linkFate.get_link_fate_box_list[0];
//     var _loc_2:* = [];
//     for each (_loc_4 in _loc_1)
//     {
//         _loc_3 = {};
//         oObject.list(_loc_4, _loc_3, ["item_id", "item_count"]);
//     }
//     public function get buyGoldenBoxCount() : int
//     {
//         return _data.linkFate.get_link_fate_box_list[1];
//     }// end function
//     public function get buyGoldenBoxDiscountCount() : int
//     {
//         return _data.linkFate.get_link_fate_box_list[3];
//     }// end function
//     public function get buyGoldenBoxDiscountRate() : int
//     {
//         return _data.linkFate.get_link_fate_box_list[2];
//     }// end function
// Example
//     [ [ [ 3937, 11 ], [ 3938, 0 ], [ 3940, 0 ], [ 3941, 0 ], [ 3942, 0 ] ], 20, 90, 0 ]
//============================================================================
Json::Value sxd_client::Mod_LinkFate_Base_get_link_fate_box() {
    Json::Value data;
    return this->send_and_receive(data, 232, 30);
}

//============================================================================
// R171
// 缘石背包
// {module:232, action:10, request:[],
// response:[[Utils.IntUtil, Utils.ShortUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.IntUtil, Utils.ShortUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.ShortUtil]]}
// LinkFateData.as 105:
//     this.linkFateInfo.warehouseList = this.parseStone(param1[0]);
//     oObject.list(_loc_6, _loc_7, ["player_link_fate_stone_id", "item_id", "grid_id", "level", "experience", "supernatural_id", "health", "attack", "defense", "stunt_attack", "stunt_defense", "magic_attack", "magic_defense", "hit", "block", "dodge", "critical", "break_block", "break_critical", "kill", "protect", "advanced_flag"]);
// Example
//     [ [ [ 5962510, 3960, 2, 1, 0, 19, 140000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 5987850, 3954, 1, 9, 7030, 0, 0, 0, 450000, 0, 0, 0, 0, 0, 0, 0, 1800, 1800, 0, 0, 0, 0 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_LinkFate_Base_get_player_link_fate_stone_pack() {
    Json::Value data;
    return this->send_and_receive(data, 232, 10);
}

//============================================================================
// R171
// 十连开
// {module:232, action:34,
// request:[Utils.IntUtil],
// Example
//     [ 3937 ]
// response:[Utils.UByteUtil, [Utils.IntUtil, Utils.ShortUtil, Utils.ShortUtil]]}
// LinkFateData.as 290:
//     this.linkFateTenOpenInfo.result = param1[0];
// Example
//     [ 5, [ [ 0, 3963, 0 ], [ 0, 3943, 0 ], [ 3938, 3963, 0 ], [ 0, 3963, 0 ], [ 0, 3963, 0 ], [ 0, 3963, 0 ], [ 0, 3963, 0 ], [ 3938, 3963, 0 ], [ 3938, 3963, 0 ], [ 3938, 3963, 0 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_LinkFate_Base_one_key_open_box(int id) {
    Json::Value data;
    data.append(id);
    return this->send_and_receive(data, 232, 34);
}

//============================================================================
// R171
// 一键吞噬
// {module:232, action:13,
// request:[Utils.ByteUtil],
// Example
//     [ 0 ]
// response:[Utils.UByteUtil]}
// Example
//     [ 5 ]
//============================================================================
Json::Value sxd_client::Mod_LinkFate_Base_auto_merge_link_fate_stone() {
    Json::Value data;
    data.append(0);
    return this->send_and_receive(data, 232, 13);
}

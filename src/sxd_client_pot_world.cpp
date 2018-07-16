#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_PotWorld_Base {
public:
    static const int YES = 3;
    static const int NO = 4;
    static const int SUCCESS = 5;
};

//============================================================================
// ∫¯÷–ΩÁ
//============================================================================
void sxd_client::pot_world() {
    // √Ê∞Â
    auto data = this->Mod_PotWorld_Base_get_info();
    auto building_list = data[4];
    // ≤÷ø‚
    data = this->Mod_PotWorld_Base_get_pack_info();
    auto pack_list = data[1];
    // ≤÷ø‚ŒÔ∆∑Õ≥º∆
    std::map<int, int> items_my;
    for (const auto& item : pack_list) {
        int item_id = item[0].asInt();
        int item_count = item[2].asInt();
        if (items_my.find(item_id) != items_my.end())
            items_my[item_id] += item_count;
        else
            items_my[item_id] = item_count;
    }
    // show
    //for (const auto& item : items_my)
    //    common::log(boost::str(boost::format("°æ∫¯÷–ΩÁ°ø[%1%°¡%2%]") % db.get_code(version, "Item", item.first)["text"] % item.second));

    // 1:≤›“©…ÃµÍ, 2:æ´∆«…ÃµÍ
    for (int building_id = 1; building_id <= 2; building_id++) {
        if (building_list[building_id][4].asInt()) {
            data = this->Mod_PotWorld_Base_get_store_info(building_id);
            int building_level = data[1].asInt();
            auto goods_list = data[3];
            for (const auto& good : goods_list) {
                int item_id = good[0].asInt();
                int buy_num = good[1].asInt();
                int cd_time = good[4].asInt();
                int is_lock = good[5].asInt();
                // lock
                if (!is_lock)
                    continue;
                // count_my
                int count_my = 0;
                if (items_my.find(item_id) != items_my.end())
                    count_my = items_my[item_id];
                // buy conditions
                while (buy_num < building_level && count_my < 60) {
                    // server_time
                    data = this->Mod_Player_Base_server_time();
                    int server_time = data[0].asInt();
                    if (server_time < cd_time)
                        break;
                    // buy
                    data = this->Mod_PotWorld_Base_buy_item(item_id);
                    if (data[0].asInt() != Mod_PotWorld_Base::SUCCESS) {
                        common::log(boost::str(boost::format("°æ∫¯÷–ΩÁ°øπ∫¬Ú [%1%]  ß∞‹£¨result[%2%]") % db.get_code(version, "Item", item_id)["text"] % data[0]));
                        return;
                    }
                    common::log(boost::str(boost::format("°æ∫¯÷–ΩÁ°øπ∫¬Ú [%1%]") % db.get_code(version, "Item", item_id)["text"]));
                    buy_num++;
                    count_my++;
                }
            }
        }
    }

    // ◊£∏£¡∂ªØ
    for (;;) {
        // server_time
        data = this->Mod_Player_Base_server_time();
        int server_time = data[0].asInt();
        // fu_valid
        data = this->Mod_PotWorld_Base_get_fu_info();
        std::vector<Json::Value> fu_valid;
        std::copy_if(data[0].begin(), data[0].end(), std::back_inserter(fu_valid), [server_time](const Json::Value& x) {
            int cost_time = x[2].asInt();
            int is_lock = x[3].asInt();
            int forever_flag = x[5].asInt();
            if (!is_lock || forever_flag == Mod_PotWorld_Base::YES || server_time < cost_time) return 0;
            auto merge_item_list = x[4];
            for (const auto& merge_item : merge_item_list) {
                int awrad_num = merge_item[1].asInt();
                int now_num = merge_item[2].asInt();
                if (awrad_num > now_num) return 0;
            }
            return 1;
        });
        if (!fu_valid.size())
            break;

        int item_id = fu_valid[rand() % fu_valid.size()][0].asInt();
        data = this->Mod_PotWorld_Base_merge_item(item_id);
        if (data[0].asInt() != Mod_PotWorld_Base::SUCCESS) {
            common::log(boost::str(boost::format("°æ∫¯÷–ΩÁ°ø◊£∏£¡∂ªØ [%1%]  ß∞‹£¨result[%2%]") % db.get_code(version, "Item", item_id)["text"] % data[0]));
            return;
        }
        common::log(boost::str(boost::format("°æ∫¯÷–ΩÁ°ø◊£∏£¡∂ªØ [%1%]") % db.get_code(version, "Item", item_id)["text"]));
    }
}

//============================================================================
// R181
// "module":235,"action":1,"request":[],"response":[Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,[Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.UByteUtil]]
// PotWorldData.as:
//     oObject.list(param1,this._getInfo,["level","cur_exp","max_exp","anima","building_list"]);
//     for each(_loc4_ in this._getInfo.building_list)
//        oObject.list(_loc4_,_loc2_,["building_id","level","need_level","unlock_anima","is_lock"]);
// building_id 1:≤›“©…ÃµÍ, 2:æ´∆«…ÃµÍ, 3:◊£∏£¡∂ªØ, 4:≤÷ø‚, 5:”¿æ√◊£∏£
// Example
//     [ 1, 0, 100, 6200,     [ [ 3, 1, 0, 0, 1 ], [ 1, 0, 3, 500, 0 ], [ 2, 1, 0, 0, 1 ], [ 5, 0, 30, 10000, 0 ], [ 4, 1, 0, 0, 1 ] ] ]
// æ´∆«…ÃµÍ…˝º∂
//     [ 1, 50, 100, 7275,    [ [ 3, 1, 0, 0, 1 ], [ 1, 0, 3, 500, 0 ], [ 2, 2, 0, 0, 1 ], [ 5, 0, 30, 10000, 0 ], [ 4, 1, 0, 0, 1 ] ] ]
// ¬˙º∂
//     [ 50, 3000, 0, 591935, [ [ 3, 1, 0, 0, 1 ], [ 1, 3, 3, 500, 1 ], [ 2, 3, 0, 0, 1 ], [ 5, 1, 30, 10000, 1 ], [ 4, 3, 0, 0, 1 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_PotWorld_Base_get_info() {
    Json::Value data;
    return this->send_and_receive(data, 235, 1);
}

//============================================================================
// R181 1Œ™≤›“©…ÃµÍ£¨2Œ™æ´∆«…ÃµÍ
// "module":235,"action":2,"request":[Utils.IntUtil],"response":[Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,[Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.UByteUtil,Utils.IntUtil,Utils.IntUtil]]
// PotWorldData.as:
//     this.shopInfo.anima = param1[0];
//     this.shopInfo.building_level = param1[1];
//     this.shopInfo.pot_level = param1[2];
//     this.shopInfo.goods_list = this.parseGoods(param1[3]);
//         oObject.list(_loc3_,_loc4_,["item_id","buy_num","pot_level","anima","cd_time","is_lock","unlock_num","max_cd_time"]);
// Example
// ¬˙º∂
//     [ 1 ] --> [ 591935, 3, 50, [ [ 4007, 0, 10, 50, 1500032975, 1, 1200, 172800 ], [ 4002, 0, 3, 15, 1499968173, 1, 0, 108000 ], [ 4003, 0, 8, 40, 1499896178, 1, 1000, 36000 ], [ 4005, 0, 3, 15, 1499903372, 1, 0, 43200 ], [ 4006, 0, 5, 30, 1499946574, 1, 600, 86400 ], [ 4004, 0, 8, 40, 1499914177, 1, 1000, 54000 ] ] ]
//     [ 2 ] --> [ 591935, 3, 50, [ [ 3979, 0, 2, 10, 1499896166, 1, 100, 36000 ], [ 3976, 0, 1, 10, 1499932165, 1, 0, 72000 ], [ 3980, 0, 5, 30, 1499932169, 1, 600, 72000 ], [ 3975, 0, 1, 5, 1499896164, 1, 0, 36000 ], [ 4001, 0, 10, 50, 1499914168, 1, 1200, 54000 ] ] ]
// ∑«¬˙º∂
//     [ 2 ] --> [ 7275, 2, 1, [ [ 3979, 0, 2, 10, 36000, 0, 100, 36000 ], [ 3976, 0, 1, 10, 72000, **1**, 0, 72000 ], [ 3980, 0, 5, 30, 72000, 0, 600, 72000 ], [ 3975, 0, 1, 5, 36000, **1**, 0, 36000 ], [ 4001, 0, 10, 50, 54000, 0, 1200, 54000 ] ] ]
//     3975 ∞◊…´æß∆«£¨3976 ¿∂…´æß∆«
//============================================================================
Json::Value sxd_client::Mod_PotWorld_Base_get_store_info(int building_id) {
    Json::Value data;
    data.append(building_id);
    return this->send_and_receive(data, 235, 2);
}

//============================================================================
// R181 ¬Ú
// "module":235,"action":9,"request":[Utils.IntUtil],"response":[Utils.UByteUtil,Utils.IntUtil]
// PotWorldData.as:
//     this.shopInfo.result = param1[0];
//     this.shopInfo.anima = param1[1];
// Example
//     [ 3975 ] --> [ 5, 591930 ]
//============================================================================
Json::Value sxd_client::Mod_PotWorld_Base_buy_item(int item_id) {
    Json::Value data;
    data.append(item_id);
    return this->send_and_receive(data, 235, 9);
}

//============================================================================
// R181 ”¿æ√◊£∏£
// "module":235,"action":17,"request":[],"response":[[Utils.IntUtil,Utils.UByteUtil,Utils.UByteUtil,Utils.IntUtil,Utils.IntUtil]]
// PotWorldData.as:
//     this.foreverBlessInfo.fu_list = this.parseForeverFuList(param1[0]);
//         oObject.list(_loc4_,_loc3_,["fu_id","is_lock","forever_flag","forever_need_anima","level"]);
// Example
//     [ [ [ 3985, 4, 3, 500, 3 ], [ 3993, 4, 3, 8000, 3 ], [ 3987, 4, 3, 3000, 3 ], [ 3978, 4, 3, 1000, 3 ], [ 3983, 4, 3, 1500, 3 ], [ 3990, 4, 3, 8000, 3 ], [ 3994, 4, 3, 12000, 3 ], [ 3977, 4, 3, 500, 3 ], [ 3991, 4, 3, 12000, 3 ], [ 3989, 4, 3, 4000, 3 ], [ 3982, 4, 3, 1500, 3 ], [ 3992, 4, 3, 8000, 3 ], [ 3986, 4, 3, 3000, 3 ], [ 3984, 4, 3, 1000, 3 ], [ 3988, 4, 3, 8000, 3 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_PotWorld_Base_get_forever_zhu_fu_list() {
    Json::Value data;
    return this->send_and_receive(data, 235, 17);
}

//============================================================================
// R181 ◊£∏£¡∂ªØ
// "module":235,"action":4,"request":[],"response":[[Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.UByteUtil,[Utils.IntUtil,Utils.IntUtil,Utils.LongUtil],Utils.UByteUtil]]
// PotWorldData.as:
//     for each(_loc5_ in param1[0])
//         oObject.list(_loc5_,_loc2_,["item_id","level","cost_time","is_lock","merge_item_list","forever_flag"]);
//         for each(_loc6_ in _loc2_.merge_item_list)
//             oObject.list(_loc6_,_loc3_,["item_id","awrad_num","now_num"]);
// Example
//     [ [ [ 3988, 1, 0, 0, [ [ 4001, 2, 0 ], [ 4006, 1, 0 ], [ 4005, 2, 0 ] ], 4 ],
//         [ 3984, 1, 0, 1, [ [ 3976, 2, 0 ] ], 4 ],
//         [ 3986, 1, 0, 0, [ [ 3979, 2, 0 ], [ 3976, 1, 0 ], [ 4002, 1, 0 ] ], 4 ],
//         [ 3992, 1, 0, 0, [ [ 3976, 2, 0 ], [ 4005, 2, 0 ], [ 4007, 2, 0 ] ], 4 ],
//         [ 3982, 1, 0, 1, [ [ 3975, 1, 0 ], [ 3976, 2, 0 ] ], 4 ],
//         [ 3989, 1, 0, 0, [ [ 4006, 1, 0 ], [ 3980, 1, 0 ], [ 3975, 1, 0 ] ], 4 ],
//         [ 3991, 1, 0, 0, [ [ 4001, 2, 0 ], [ 4007, 1, 0 ], [ 4004, 2, 0 ] ], 4 ],
//         [ 3977, 1, 0, 1, [ [ 3976, 1, 0 ] ], 4 ],
//         [ 3994, 1, 0, 0, [ [ 3980, 2, 0 ], [ 4003, 2, 0 ], [ 4004, 2, 0 ], [ 3979, 2, 0 ] ], 4 ],
//         [ 3990, 1, 0, 0, [ [ 4003, 2, 0 ], [ 3980, 1, 0 ], [ 4004, 1, 0 ] ], 4 ],
//         [ 3983, 1, 0, 0, [ [ 3975, 1, 0 ], [ 3976, 1, 0 ], [ 3979, 1, 0 ] ], 4 ],
//         [ 3978, 1, 0, 1, [ [ 3976, 1, 0 ], [ 3975, 2, 0 ] ], 4 ],
//         [ 3987, 1, 0, 0, [ [ 3975, 1, 0 ], [ 4005, 2, 0 ], [ 3976, 1, 0 ] ], 4 ],
//         [ 3993, 1, 0, 0, [ [ 3980, 2, 0 ], [ 4002, 2, 0 ], [ 4001, 1, 0 ] ], 4 ],
//         [ 3985, 1, 0, 1, [ [ 3975, 2, 0 ] ], 4 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_PotWorld_Base_get_fu_info() {
    Json::Value data;
    return this->send_and_receive(data, 235, 4);
}

//============================================================================
// R181 ¡∂ªØ
// "module":235,"action":10,"request":[Utils.IntUtil],"response":[Utils.UByteUtil]
// PotWorldData.as:
//     oObject.list(param1,this._mergeItem,["result"]);
// Example
//     [ 3978 ] --> [ 5 ]
//============================================================================
Json::Value sxd_client::Mod_PotWorld_Base_merge_item(int item_id) {
    Json::Value data;
    data.append(item_id);
    return this->send_and_receive(data, 235, 10);
}

//============================================================================
// R181 ≤÷ø‚
// "module":235,"action":6,"request":[],"response":[Utils.IntUtil,[Utils.IntUtil,Utils.IntUtil,Utils.IntUtil],Utils.IntUtil,Utils.IntUtil,Utils.IntUtil]
// PotWorldData.as:
//     this.packInfo.need_anima = param1[0];
//     this.packInfo.pack_list = this.parsePack(param1[1]);
//         oObject.list(_loc3_,_loc4_,["item_id","grid","number"]);
//     this.packInfo.anima = param1[2];
//     this.packInfo.building_level = param1[3];
//     this.packInfo.pot_level = param1[4];
// Example
// –Ë“™5000œ…÷µ…˝º∂≤÷ø‚
//     [ 5000, null, 6200, 1, 1 ]
// Œﬁ–Ë…˝º∂≤÷ø‚
//     [ 0, [ [ 4001, 95, 6 ], [ 4007, 24, 10 ], [ 4006, 42, 10 ], [ 4001, 8, 10 ], [ 4002, 80, 10 ], [ 4006, 91, 4 ], [ 4004, 62, 10 ], [ 4006, 39, 10 ], [ 4005, 56, 10 ], [ 4001, 1, 10 ], [ 4002, 78, 10 ], [ 3979, 14, 10 ], [ 3979, 93, 9 ], [ 4007, 25, 10 ], [ 3980, 9, 10 ], [ 3979, 16, 10 ], [ 3979, 11, 10 ], [ 4004, 98, 3 ], [ 4005, 58, 10 ], [ 4005, 53, 10 ], [ 4006, 45, 10 ], [ 4007, 32, 10 ], [ 3975, 88, 10 ], [ 4005, 61, 10 ], [ 4001, 6, 10 ], [ 4007, 33, 10 ], [ 4003, 70, 10 ], [ 4007, 29, 10 ], [ 3980, 92, 9 ], [ 3979, 15, 10 ], [ 4001, 5, 10 ], [ 4005, 90, 5 ], [ 4003, 73, 10 ], [ 4003, 71, 10 ], [ 4003, 68, 10 ], [ 3979, 13, 10 ], [ 4006, 44, 10 ], [ 4003, 74, 10 ], [ 3979, 18, 10 ], [ 4006, 48, 10 ], [ 4007, 31, 10 ], [ 4004, 63, 10 ], [ 4002, 82, 10 ], [ 4004, 65, 10 ], [ 4003, 75, 10 ], [ 4005, 54, 10 ], [ 4002, 87, 7 ], [ 3976, 96, 3 ], [ 4005, 60, 10 ], [ 4006, 46, 10 ], [ 4002, 81, 10 ], [ 4006, 38, 10 ], [ 4003, 72, 10 ], [ 4003, 69, 10 ], [ 4002, 77, 10 ], [ 4005, 52, 10 ], [ 4001, 7, 10 ], [ 4003, 66, 10 ], [ 4006, 37, 10 ], [ 4002, 79, 10 ], [ 3979, 10, 10 ], [ 4007, 97, 1 ], [ 4001, 4, 10 ], [ 4007, 30, 10 ], [ 3979, 22, 10 ], [ 4006, 41, 10 ], [ 4002, 84, 10 ], [ 4003, 67, 10 ], [ 4007, 34, 10 ], [ 4002, 85, 10 ], [ 3979, 21, 10 ], [ 3979, 17, 10 ], [ 4006, 43, 10 ], [ 4006, 47, 10 ], [ 4003, 99, 2 ], [ 4002, 86, 10 ], [ 4005, 55, 10 ], [ 3976, 89, 10 ], [ 3979, 12, 10 ], [ 4007, 35, 10 ], [ 4005, 59, 10 ], [ 4005, 57, 10 ], [ 4005, 51, 10 ], [ 3975, 94, 5 ], [ 4001, 2, 10 ], [ 4001, 3, 10 ], [ 4007, 36, 10 ], [ 4004, 64, 10 ], [ 4005, 50, 10 ], [ 4006, 40, 10 ], [ 4007, 26, 10 ], [ 3979, 20, 10 ], [ 4002, 76, 10 ], [ 3979, 19, 10 ], [ 4007, 27, 10 ], [ 4006, 49, 10 ], [ 4007, 28, 10 ], [ 4002, 83, 10 ], [ 3976, 23, 10 ] ], 591935, 3, 50 ]
//============================================================================
Json::Value sxd_client::Mod_PotWorld_Base_get_pack_info() {
    Json::Value data;
    return this->send_and_receive(data, 235, 6);
}


#include <vector>
#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_LuckyStore_Base {
public:
    static const int SUCCESS = 0;
    static const int LUCKY_YES = 8;
    static const int LUCKY_NO = 9;
    static const int LUCKY_SUCCESS = 10;
    static const int ShenMiShangRen = 101;
};

void sxd_client::lucky_shop(const std::string& player_name) {
    // read config
    Json::Value config1, config2;
    std::map<int, int> items_config1, items_config2;
    std::istringstream(db.get_config(player_name.c_str(), "LuckyStore1")) >> config1;
    std::istringstream(db.get_config(player_name.c_str(), "LuckyStore2")) >> config2;
    // 商品配置
    for (const auto& item : config1)
        items_config1[item[0].asInt()] = item[1].asInt();
    for (const auto& item : items_config1)
        common::log(boost::str(boost::format("【LuckyStore1】[%1%(%2%)×%3%]，%4%") % db.get_lucky_shop_item(version, item.first)["item_name"] % item.first % item.second % db.get_lucky_shop_item(version, item.first)["item_comment"]), 0);
    // 材料配置
    for (const auto& item : config2)
        items_config2[item[0].asInt()] = item[1].asInt();
    for (const auto& item : items_config2)
        common::log(boost::str(boost::format("【LuckyStore2】[%1%(%2%)×%3%]，%4%") % db.get_code(version, "Item", item.first)["text"] % item.first % item.second % db.get_code(version, "Item", item.first)["comment"]), 0);
    // 1. pack
    std::map<int, int> items_my;
    Json::Value data = this->Mod_Item_Base_get_player_pack_item_list();
    Json::Value items = data[2];
    for (const auto& item : items) {
        int item_id = item[1].asInt();
        int item_count = item[5].asInt();
        if (items_my.find(item_id) != items_my.end())
            items_my[item_id] += item_count;
        else
            items_my[item_id] = item_count;
    }
    // 2. warehouse
    data = this->Mod_Item_Base_get_player_warehouse_grids();
    items = data[3];
    for (const auto& item : items) {
        int item_id = item[1].asInt();
        int item_count = item[5].asInt();
        if (items_my.find(item_id) != items_my.end())
            items_my[item_id] += item_count;
        else
            items_my[item_id] = item_count;
    }
    // 3. home warehouse
    data = this->Mod_Item_Base_get_player_home_warehouse_grids();
    items = data[2];
    for (const auto& item : items) {
        int item_id = item[1].asInt();
        int item_count = item[5].asInt();
        if (items_my.find(item_id) != items_my.end())
            items_my[item_id] += item_count;
        else
            items_my[item_id] = item_count;
    }
    //for (const auto& item : items_my)
    //    common::log(boost::str(boost::format("【物品】[%1%(%2%)×%3%]") % db.get_code(version, "Item", item.first)["text"] % item.first % item.second));

    // lucky store
    data = this->Mod_LuckyStore_Base_get_lucky_store_item_list();
    Json::Value lucky_store_items = data[0];
    // show
    for (const auto& lucky_store_item : lucky_store_items) {
        int item_id = lucky_store_item[0].asInt();
        int lucky_store_id = lucky_store_item[1].asInt();
        int buy_flag = lucky_store_item[2].asInt();
        if (buy_flag == Mod_LuckyStore_Base::LUCKY_YES)
            continue;
        if (lucky_store_id) {
            auto item = db.get_lucky_shop_item(version, lucky_store_id);
            common::log(boost::str(boost::format("【神秘商人】[%1%×%2%]，售价[铜钱×%3%]，[元宝×%4%]") % item["item_name"] % item["count"] % item["coin"] % item["ingot"]), 0);
        } else {
            data = this->Mod_Item_Base_get_item_basical_infos(&item_id, 1);
            auto item = db.get_code(version, "Item", item_id);
            common::log(boost::str(boost::format("【神秘商人】[%1%×99]，售价[铜钱×%2%]，[元宝×0]") % item["text"] % (data[0][0][3].asInt() * 99)), 0);
        }
    }
    // buy
    for (const auto& lucky_store_item : lucky_store_items) {
        int item_id = lucky_store_item[0].asInt();
        int lucky_store_id = lucky_store_item[1].asInt();
        int buy_flag = lucky_store_item[2].asInt();
        if (buy_flag == Mod_LuckyStore_Base::LUCKY_YES)
            continue;
        // count_my
        int count_my = 0;
        if (items_my.find(item_id) != items_my.end())
            count_my = items_my[item_id];
        // count_config
        int count_config = 0;
        if (lucky_store_id) { // 商品
            if (items_config1.find(lucky_store_id) != items_config1.end())
                count_config = items_config1[lucky_store_id];
        } else {             // 材料
            if (items_config2.find(item_id) != items_config2.end())
                count_config = items_config2[item_id];
        }
        // enough
        if (count_config <= count_my)
            continue;
        // buy
        data = this->Mod_LuckyStore_Base_buy_lucky_store_item(Mod_LuckyStore_Base::ShenMiShangRen, item_id, lucky_store_id);
        if (data[0].asInt() != Mod_LuckyStore_Base::LUCKY_SUCCESS) {
            common::log(boost::str(boost::format("【神秘商人】购买 [%1%] 失败，result[%2%]") % db.get_code(version, "Item", item_id)["text"] % data[0]));
            return;
        }
        common::log(boost::str(boost::format("【神秘商人】购买 [%1%]") % db.get_code(version, "Item", item_id)["text"]));
    }
}

void sxd_client::black_shop(const std::string& player_name) {
    // read config
    Json::Value config;
    std::istringstream(db.get_config(player_name.c_str(), "BlackShop")) >> config;
    for (const auto& item : config)
        common::log(boost::str(boost::format("【BlackShop】[%1%(%2%)]") % db.get_code(version, "Item", item.asInt())["text"] % item), 0);
    // black_shop
    Json::Value data = this->Mod_LuckyStore_Base_black_shop_item_list();
    Json::Value black_shop_items = data[0];
    for (const auto& black_shop_item : black_shop_items) {
        if (black_shop_item[7].asInt())
            continue;
        if (black_shop_item[3].asInt())
            continue;
        int blcak_shop_id = black_shop_item[0].asInt();
        int item_id = black_shop_item[1].asInt();
        std::string item_name = db.get_code(version, "Item", item_id)["text"];
        if (std::find_if(config.begin(), config.end(), [item_id](const Json::Value& x) {return x.asInt()==item_id;}) == config.end())
            continue;
        // buy
        data = this->Mod_LuckyStore_Base_buy_black_shop_item(blcak_shop_id);
        if (data[0].asInt() != Mod_LuckyStore_Base::SUCCESS) {
            common::log(boost::str(boost::format("【珍奇异宝】购买 [%1%] 失败，result[%2%]") % item_name % data[0]));
            return;
        }
        common::log(boost::str(boost::format("【珍奇异宝】购买 [%1%]") % item_name));
    }
}

//============================================================================
// R171
// 神秘商人
// {module:87, action:5, request:[],
// response:[[Utils.IntUtil, Utils.IntUtil, Utils.UByteUtil, Utils.UByteUtil], Utils.ShortUtil]};
// LuckyShopData.as 273:
//     oObject.list(_loc_4, _loc_2, ["item_id", "lucky_store_id", "buy_flag", "is_fresh"]);
// Example
//     [ [ [ 4099, 164, 9, 9 ], [ 2427, 151, 9, 9 ], [ 86, 0, 9, 9 ], [ 2615, 155, 9, 9 ], [ 347, 50, 9, 9 ], [ 253, 0, 9, 9 ], [ 9, 0, 9, 9 ], [ 257, 0, 9, 9 ], [ 420, 3, 9, 9 ], [ 2615, 148, 9, 9 ], [ 1378, 0, 9, 9 ], [ 387, 1, 9, 9 ] ], 1447 ]
// 购买真元(4099)和初级五行之灵(2427)后
//     [ [ [ 4099, 164, 8, 9 ], [ 2427, 151, 8, 9 ], [ 86, 0, 9, 9 ], [ 2615, 155, 9, 9 ], [ 347, 50, 9, 9 ], [ 253, 0, 9, 9 ], [ 9, 0, 9, 9 ], [ 257, 0, 9, 9 ], [ 420, 3, 9, 9 ], [ 2615, 148, 9, 9 ], [ 1378, 0, 9, 9 ], [ 387, 1, 9, 9 ] ], 1442 ]
//============================================================================
Json::Value sxd_client::Mod_LuckyStore_Base_get_lucky_store_item_list() {
    Json::Value data;
    return this->send_and_receive(data, 87, 5);
}

//============================================================================
// R171
// 买
// {module:87, action:6,
// request:[Utils.IntUtil, Utils.IntUtil, Utils.IntUtil],
// SuperLuckyShopView.as 209:
//     _data.call(Mod_LuckyStore_Base.buy_lucky_store_item, buyClickCallback, [intNpcID, this._superLuckyShop.intItemId, this._superLuckyShop.intLuckyStoreId]);
// Example
//     [ 196, 4099, 164 ]
//     [ 196, 2427, 151 ]
//     天剑宗, "ShenMiShangRen":[101,1170,360,173]
//     卧龙城, "ShenMiShangRen":[196,1800,366,0],
// response:[Utils.UByteUtil]};
// Example
//     [ 10 ]
//============================================================================
Json::Value sxd_client::Mod_LuckyStore_Base_buy_lucky_store_item(int npc_id, int item_id, int lucky_store_id) {
    Json::Value data;
    data.append(npc_id);
    data.append(item_id);
    data.append(lucky_store_id);
    return this->send_and_receive(data, 87, 6);
}

//============================================================================
// R171
// 珍奇异宝
// {module:87, action:10, request:[],
// response:[[Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.ByteUtil]]}
// LuckyShopData.as 355:
//     for each (_loc_3 in param1[0])
//         oObject.list(_loc_3, _loc_2, ["blcak_shop_award_id", "item_id", "award_number", "ingot", "coin", "skill", "daoyuan", "is_buy", "is_fresh"]);
// Example
//     [ [ [ 11, 3319, 1, 300, 0, 0, 0, 0, 0 ],
//         [ 7, 1411, 10, 0, 28000000, 0, 0, 0, 0 ],
//         [ 24, 3339, 1, 100, 0, 0, 0, 0, 0 ],
//         [ 2, 2263, 20, 0, 35000000, 0, 0, 0, 0 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_LuckyStore_Base_black_shop_item_list() {
    Json::Value data;
    return this->send_and_receive(data, 87, 10);
}

//============================================================================
// R171
// 买
// {module:87, action:11, request:[Utils.IntUtil],
// Example
//    [ 7 ]
//    [ 2 ]
// response:[Utils.UByteUtil]};
// Example
//    [ 0 ]
//============================================================================
Json::Value sxd_client::Mod_LuckyStore_Base_buy_black_shop_item(int black_shop_id) {
    Json::Value data;
    data.append(black_shop_id);
    return this->send_and_receive(data, 87, 11);
}

//============================================================================
// R171
// 碎片秘藏
// {module:87, action:13, request:[], response:[[Utils.UByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil]]}
// Example
//     [ [ [ 25, 3826, 1, 4, 0, 10, 1, 35, 100 ], [ 26, 1862, 1, 2, 0, 10, 1, 34, 50 ], [ 27, 3834, 1, 8, 0, 10, 1, 148, 500 ], [ 28, 2343, 1, 2, 0, 10, 1, 172, 0 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_LuckyStore_Base_scrap_item_list() {
    Json::Value data;
    return this->send_and_receive(data, 87, 13);
}

//============================================================================
// R171
// 缘魂宝鉴
// {module:87, action:15, request:[], response:[[Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.UByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil]]};
// Example
//     [ [ [ 1, 46, 1, 32, 0, 300000, 0, 10, 1, 2, 50 ], [ 3, 36, 1, 33, 0, 600000, 0, 10, 1, 10, 50 ], [ 5, 9, 1, 34, 8, 0, 0, 10, 1, 0, 50 ], [ 7, 50, 1, 35, 35, 0, 0, 10, 1, 0, 50 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_LuckyStore_Base_link_soul_item_list() {
    Json::Value data;
    return this->send_and_receive(data, 87, 15);
}

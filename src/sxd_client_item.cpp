#include <vector>
#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_Item_Base {
public:
    static const int ACTION_SUCCESS = 20;
    static const int MATERIAL_NOT_ENOUGH = 22;
    static const int NOT_ENOUGH_FATE_GRID = 46;
};

class Mod_GiftItemPack_Base {
public:
    static const int SUCCESS = 4;
};

//============================================================================
// 获取背包中空格子数量
//============================================================================
int sxd_client::get_empty_pack_num() {
    auto data = this->Mod_Item_Base_get_player_pack_item_list();
    return data[0].asInt() - data[2].size();
}

void sxd_client::item_reel() {
    // read config
    Json::Value config;
    std::istringstream(db.get_config(user_id.c_str(), "ItemReel")) >> config;
    // show
    //for (const auto& item : config)
    //    common::log(boost::str(boost::format("【ItemReel】[%1%]") % db.get_code(version, "Item", item.asInt())["text"]), 0);
    // pack
    Json::Value data = this->Mod_Item_Base_get_player_pack_item_list();
    Json::Value items = data[2];
    // reel
    for (const auto& item : items) {
        int player_item_id = item[0].asInt();
        int item_id = item[1].asInt();
        std::string item_name = db.get_code(version, "Item", item_id)["text"];
        int item_count = item[5].asInt();
        if (std::find_if(config.begin(), config.end(), [item_id](const Json::Value& x) {return x.asInt()==item_id;}) == config.end())
            continue;
        for (int i = 0; i < item_count; i++) {
            data = this->Mod_Item_Base_player_use_grid_reel(player_item_id);
            if (data[0].asInt() == Mod_Item_Base::ACTION_SUCCESS)
                common::log(boost::str(boost::format("【背包】合成 [%1%]") % item_name), iEdit);
            else if (data[0].asInt() == Mod_Item_Base::MATERIAL_NOT_ENOUGH) {
                //common::log(boost::str(boost::format("【背包】合成 [%1%] 失败，材料不足") % item_name), iEdit);
                break;
            } else {
                common::log(boost::str(boost::format("【背包】合成 [%1%] 失败，result[%2%]") % item_name % data[0]), 0);
                break;
            }
        }
    }
}

void sxd_client::item_use() {
    // read config
    Json::Value config;
    std::istringstream(db.get_config(user_id.c_str(), "ItemUse")) >> config;
    for (const auto& item : config)
        common::log(boost::str(boost::format("【ItemUse】[%1%]") % item.asString()), 0);
    // pack
    Json::Value data = this->Mod_Item_Base_get_player_pack_item_list();
    Json::Value items = data[2];
    // use
    for (const auto& item : items) {
        int item_id = item[1].asInt();
        int box_id = item[2].asInt();
        std::string item_name = db.get_code(version, "Item", item_id)["text"];
        int item_count = item[5].asInt();
        if (std::find_if(config.begin(), config.end(), [item_name](const Json::Value& x) {return item_name.find(x.asString())!=std::string::npos;}) == config.end())
            continue;
        for (int i = 0; i < item_count; i++) {
            data = this->Mod_Item_Base_player_use_grid_item(box_id);
            if (data[0].asInt() == Mod_Item_Base::ACTION_SUCCESS)
                common::log(boost::str(boost::format("【背包】使用 [%1%]") % item_name), iEdit);
            else if (data[0].asInt() == Mod_Item_Base::NOT_ENOUGH_FATE_GRID) {
                common::log(boost::str(boost::format("【背包】使用 [%1%] 失败，命格背包满") % item_name), iEdit);
                break;
            } else {
                common::log(boost::str(boost::format("【背包】使用 [%1%]失败，result[%2%]") % item_name % data[0]), 0);
                break;
            }
        }
    }
    for (const auto& item : items) {
        int item_id = item[1].asInt();
        int box_id = item[2].asInt();
        std::string item_name = db.get_code(version, "Item", item_id)["text"];

        data = this->Mod_GiftItemPack_Base_put_item_to_package(box_id);
        if (data[0].asInt() == Mod_GiftItemPack_Base::SUCCESS)
            common::log(boost::str(boost::format("【背包】 [%1%] 转移至礼包仓库") % item_name), iEdit);
        data = this->Mod_Item_Base_move_pack_grid_item_to_supergift_warehouse(box_id);
        if (data[0].asInt() == Mod_Item_Base::ACTION_SUCCESS)
            common::log(boost::str(boost::format("【背包】 [%1%] 转移至礼包仓库") % item_name), iEdit);
    }
}

void sxd_client::item_sell() {
    // read config
    Json::Value config;
    std::istringstream(db.get_config(user_id.c_str(), "ItemSell")) >> config;
    for (const auto& item : config)
        common::log(boost::str(boost::format("【ItemSell】[%1%]") % db.get_code(version, "Item", item.asInt())["text"]), 0);
    // pack
    Json::Value data = this->Mod_Item_Base_get_player_pack_item_list();
    Json::Value items = data[2];
    // 卖掉配置的物品
    for (const auto& item : items) {
        int item_id = item[1].asInt();
        int box_id = item[2].asInt();
        std::string item_name = db.get_code(version, "Item", item_id)["text"];
        if (std::find_if(config.begin(), config.end(), [item_id](const Json::Value& x) {return x.asInt()==item_id;}) == config.end())
            continue;
        data = this->Mod_Item_Base_player_sell_item(box_id);
        if (data[0].asInt() != Mod_Item_Base::ACTION_SUCCESS)
            common::log(boost::str(boost::format("【背包】出售 [%1%]失败，result[%2%]") % item_name % data[0]), iEdit);
        else
            common::log(boost::str(boost::format("【背包】出售 [%1%]") % item_name), iEdit);
    }

    // 准备items_config2
    Json::Value config2;
    std::map<int, int> items_config2;
    std::istringstream(db.get_config(user_id.c_str(), "LuckyStore2")) >> config2;
    for (const auto& item : config2)
        items_config2[item[0].asInt()] = item[1].asInt();
    // 准备equips
    data = this->Mod_Role_Base_get_role_list(player_id);
    auto roles = data[14];
    data = this->Mod_Item_Base_get_all_player_item_infos();
    std::vector<Json::Value> equips;
    std::copy_if(data[0].begin(), data[0].end(), std::back_inserter(equips), [](const Json::Value& x) {return x[16].asInt();});
    // 卖掉无用的材料
    for (const auto& item : items) {
        int item_id = item[1].asInt();
        int box_id = item[2].asInt();
        std::string item_name = db.get_code(version, "Item", item_id)["text"];
        // 1. 必须facture_reel中存在
        std::ostringstream where_clause;
        where_clause << "version='" << version << "' and item_id=" << item_id;
        auto items = db.get_records("facture_reel", where_clause.str());
        if (!items.size())
            continue;
        // 2. 必须是装备制作材料
        where_clause.str("");
        where_clause << "version='" << version << "' and type='Item' and value='" << item_id << "' and comment like '%" << common::gbk2utf("装备") << "%'";
        items = db.get_records("code", where_clause.str());
        if (!items.size())
            continue;
        // 3. 不能在LuckyStore2中存在
        if (items_config2.find(item_id) != items_config2.end())
            continue;
        // 4. 不能是有用的装备制作材料
        bool can_sell = true;
        for (const auto& equip : equips) {
            int player_role_id = equip[16].asInt();
            if (std::find_if(roles.begin(), roles.end(), [player_role_id](const Json::Value& role) {return role[2].asInt() == player_role_id;}) == roles.end())
                continue;
            auto materials = db.get_facture_reel(version, equip[1].asInt());
            if (std::find_if(materials.begin(), materials.end(), [item_id](mss& material) {return atoi(material["item_id"].c_str()) == item_id;}) != materials.end())
                can_sell = false;
        }
        // 5. 卖吧
        if (can_sell) {
            data = this->Mod_Item_Base_player_sell_item(box_id);
            if (data[0].asInt() != Mod_Item_Base::ACTION_SUCCESS)
                common::log(boost::str(boost::format("【背包】自动出售无用材料 [%1%]失败，result[%2%]") % item_name % data[0]), iEdit);
            else
                common::log(boost::str(boost::format("【背包】自动出售无用材料 [%1%]") % item_name), iEdit);
        }
    }
}

//============================================================================
// R171 背包
// {module:2, action:41,
// request:[], response:[Utils.ShortUtil, Utils.IntUtil, [Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.ShortUtil, Utils.ShortUtil]]}
// ItemData.as 238:
//     this.resetGrids(BoxGrid.idPack, param1[0], this.gridsPack);
//     this.ingot_pack = param1[1];
//     for each (_loc_2 in param1[2])
//         this.updateGrids(_loc_2);
// Example
//   total_count    id,       item_id, pos, x, price, num,... (pos从1开始)
//     [ 48, 0, [ [ 47931395, 448,     18,  1, 51,    18, 0, 0, 0, 0, 0, 0, 0, 0, 0 ],
//                [ 49593887, 251,     6,   1, 84,    99, 0, 0, 0, 0, 0, 0, 0, 0, 0 ],
//                [ 49593426, 257,     11,  1, 60,    5, 0, 0, 0, 0, 0, 0, 0, 0, 0 ],... ] ]
//============================================================================
Json::Value sxd_client::Mod_Item_Base_get_player_pack_item_list() {
    Json::Value data;
    return this->send_and_receive(data, 2, 41);
}

//============================================================================
// R171 仓库
// {module:2, action:42,
// request:[Utils.IntUtil],
// Example
//     [ 0 ]
// response:[Utils.UByteUtil, Utils.ShortUtil, Utils.IntUtil, [Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.ShortUtil, Utils.ShortUtil]]}
// ItemData.as 251:
//     this.result = param1[0];
//     this.resetGrids(BoxGrid.idWareHouse, param1[1], this.gridsWarehouse);
//     this.ingot_warehouse = param1[2];
//     for each (_loc_2 in param1[3])
//         this.updateGrids(_loc_2);
// Example
//       total_count    id,       item_id, pos, x, price, num,... (pos从101开始)
//     [ 20, 54, 0, [ [ 48245243, 1614,    137, 1, 320,   99, 0, 0, 0, 0, 0, 0, 0, 0, 0 ],
//                    [ 48096779, 1615,    139, 1, 320,   41, 0, 0, 0, 0, 0, 0, 0, 0, 0 ],
//                    [ 46661486, 3782,    153, 1, 0,     6, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], ...] ]
//============================================================================
Json::Value sxd_client::Mod_Item_Base_get_player_warehouse_grids() {
    Json::Value data;
    data.append(0);
    return this->send_and_receive(data, 2, 42);
}

//============================================================================
// R171 家园仓库
// {module:2, action:135,
// request:[], response:[Utils.UByteUtil, Utils.ShortUtil, [Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.ShortUtil, Utils.ShortUtil]]};
// ItemData.as 269:
//     this.result = param1[0];
//     this.resetGrids(BoxGrid.idWareHouse_Home, param1[1], this.gridsWarehouse_Home);
//     for each (_loc_2 in param1[2])
//         this.updateGrids(_loc_2);
// Example
//    total_count    id,       item_id, pos, x, price, num,... (pos从301开始)
//     [ 20, 54, [ [ 49382333, 221,     303, 1, 11,    84, 0, 0, 0, 0, 0, 0, 0, 0, 0 ],
//                 [ 49403943, 9,       301, 1, 84,    66, 0, 0, 0, 0, 0, 0, 0, 0, 0 ],
//                 [ 49408090, 261,     350, 1, 64,    99, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], ... ] ]
//============================================================================
Json::Value sxd_client::Mod_Item_Base_get_player_home_warehouse_grids() {
    Json::Value data;
    return this->send_and_receive(data, 2, 135);
}

//============================================================================
// R171 物品基本信息
// {module:2, action:1,
// request:[[Utils.IntUtil]],
// response:[[Utils.IntUtil, Utils.IntUtil, Utils.ShortUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.StringUtil]]};
//============================================================================
Json::Value sxd_client::Mod_Item_Base_get_item_basical_infos(int ids[], int count) {
    Json::Value data, data1;
    for (int i = 0; i < count; i++) {
        Json::Value data2;
        data2.append(ids[i]);
        data1.append(data2);
    }
    data.append(data1);
    return this->send_and_receive(data, 2, 1);
}

//============================================================================
// R171 卷轴
// {module:2, action:21,
// request:[Utils.IntUtil],
// Example
//     [ 407 ]
// response:[Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, [Utils.IntUtil, Utils.IntUtil, Utils.StringUtil, Utils.IntUtil, Utils.IntUtil], Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.ByteUtil]};
// Example               item_id, need,                                   have
//     [ 407, 482, 1, 1, [ [ 263, 20, "\u5929\u5883(6)\u6389\u843d", 560, 69 ],
//                         [ 241, 36, "\u7384\u9759\u5bfa(8)\u6389\u843d", 523, 0 ],
//                         [ 219, 60, "\u5929\u5883(2)\u6389\u843d", 552, 2 ],
//                         [ 235, 60, "\u706b\u7130\u5c71(8)\u6389\u843d", 508, 0 ],
//                         [ 234, 40, "\u4e1c\u701b(2)\u6389\u843d", 567, 128 ] ], 0, 0, 0, 0, 0 ]
//============================================================================
Json::Value sxd_client::Mod_Item_Base_get_facture_reel_data(int item_id) {
    Json::Value data;
    data.append(item_id);
    return this->send_and_receive(data, 2, 21);
}

//============================================================================
// R171 合成
// {module:2, action:27,
// request:[Utils.IntUtil, Utils.IntUtil, Utils.IntUtil],
// Example
//     [ 49688979, 0, 0 ]
// response:[Utils.UByteUtil, Utils.IntUtil, Utils.IntUtil, [Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.ShortUtil, Utils.ShortUtil]]};
// Example
//     [ 20, 380, 6, [ [ 49283478, 380, 23, 1, 5, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 49688979, 985, 1, 1, 1070, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 47399569, 257, 342, 1, 60, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 49396895, 257, 343, 1, 60, 99, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 49688421, 257, 16, 1, 60, 99, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 49408201, 257, 341, 1, 60, 93, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 46858983, 254, 338, 1, 55, 93, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_Item_Base_player_use_grid_reel(int player_item_id) {
    Json::Value data;
    data.append(player_item_id);
    data.append(0);
    data.append(0);
    return this->send_and_receive(data, 2, 27);
}

//============================================================================
// R171 批量使用
// {module:2, action:130,
// request:[Utils.IntUtil],
// RoleMsgView.as 2853:
//     _data.call(Mod_Item_Base.large_use_grid_item, this.largeUseingBack, [this._clickBoxPack.playerItemId]);
// response:[Utils.UByteUtil, [Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.ShortUtil, Utils.ShortUtil]]};
// ItemData.as 2017:
//     this.result = param1[0];
//============================================================================
Json::Value sxd_client::Mod_Item_Base_large_use_grid_item(int player_item_id) {
    Json::Value data;
    data.append(player_item_id);
    return this->send_and_receive(data, 2, 130);
}

//============================================================================
// R171 打开
// {module:2, action:53,
// request:[Utils.ShortUtil],
// RoleMsgView.as 3329:
//     _data.call(Mod_Item_Base.player_use_grid_item, player_use_grid_item_back, [box1.id]);
// response:[Utils.UByteUtil, [Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.ShortUtil, Utils.ShortUtil]]};
// ItemData.as 758:
//     this.result = param1[0];
//============================================================================
Json::Value sxd_client::Mod_Item_Base_player_use_grid_item(int box_id) {
    Json::Value data;
    data.append(box_id);
    return this->send_and_receive(data, 2, 53);
}

//============================================================================
// R171 出售
// {module:2, action:73,
// request:[Utils.ShortUtil],
// RoleMsgView.as 3196:
//     private function player_sell_item(param1:BoxGrid) : void
//         _data.call(Mod_Item_Base.player_sell_item, this.player_sell_item_back, [param1.id]);
// response:[Utils.UByteUtil, [Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.ShortUtil, Utils.ShortUtil], [Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil]]};
// ItemData.as 511:
//     this.result = param1[0];
//============================================================================
Json::Value sxd_client::Mod_Item_Base_player_sell_item(int box_id) {
    Json::Value data;
    data.append(box_id);
    return this->send_and_receive(data, 2, 73);
}

//============================================================================
// R190 获取玩家所有物品
// "module":2, "action":3,
// "request":[],
// "response":[[Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.ByteUtil,Utils.IntUtil,Utils.IntUtil,Utils.ByteUtil,Utils.ShortUtil,Utils.ShortUtil,Utils.ByteUtil,Utils.IntUtil,Utils.IntUtil]]
// ItemData.as 1396:
//     for each(_loc2_ in param1[0])
//         oObject.list(_loc2_,_loc3_,["player_item_id","item_id","grid_id","upgrade_level","price","number","is_using","gold_oil_id","end_time","god_equip_level","god_equip_percent","god_equip_wat_id","jewel_equip_level","zodiac_stone_addition","open_light_first_attack","jewel_upgrade_level","player_role_id","coolTime"]);
// Example
//     [ [ [ 7390019, 871, 205, 1, 3153, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59529, 0 ], [ 7389938, 1488, 112, 1, 1, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 7390030, 819, 203, 1, 3153, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59530, 0 ], [ 7390540, 253, 106, 1, 53, 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 7388672, 5822, 101, 1, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 7390332, 252, 105, 1, 47, 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 7390414, 252, 109, 1, 47, 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 7390018, 835, 204, 1, 3153, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59529, 0 ], [ 7393851, 246, 8, 1, 43, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 7390323, 249, 103, 1, 51, 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 7390538, 262, 111, 1, 69, 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 7393848, 950, 6, 1, 50019, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 7392748, 940, 2, 1, 4691, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 7393331, 255, 5, 1, 55, 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 7390025, 846, 202, 1, 3153, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59530, 0 ], [ 7391524, 257, 116, 1, 60, 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 7388663, 809, 201, 1, 3153, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59527, 0 ], [ 7390895, 254, 113, 1, 55, 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 7390028, 871, 205, 1, 3153, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59530, 0 ], [ 7390029, 852, 206, 1, 3153, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59530, 0 ], [ 7390023, 835, 204, 1, 3153, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59528, 0 ], [ 7390021, 852, 206, 1, 3153, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59528, 0 ], [ 7390020, 852, 206, 1, 3153, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59529, 0 ], [ 7388657, 820, 202, 22, 10529, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59528, 0 ], [ 7390744, 253, 107, 1, 53, 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 7390762, 257, 110, 1, 60, 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 7393856, 252, 9, 1, 47, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 7390293, 246, 104, 1, 43, 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 7390959, 261, 114, 1, 64, 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 7390022, 871, 205, 1, 3153, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59528, 0 ], [ 7388668, 819, 203, 22, 17904, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59529, 0 ], [ 7389997, 249, 102, 1, 51, 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 7388660, 835, 204, 1, 3153, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59527, 0 ], [ 7391403, 246, 115, 1, 43, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 7390017, 809, 201, 1, 3153, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59529, 0 ], [ 7390024, 809, 201, 1, 3153, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59528, 0 ], [ 7393774, 1488, 7, 1, 1, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 7390443, 246, 108, 1, 43, 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 7388662, 852, 206, 1, 3153, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59527, 0 ], [ 7394023, 255, 10, 1, 55, 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 7390027, 835, 204, 1, 3153, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59530, 0 ], [ 7388665, 820, 202, 22, 10529, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59529, 0 ], [ 7393225, 3782, 3, 1, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 7393325, 5822, 4, 1, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 7388661, 819, 203, 22, 17904, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59527, 0 ], [ 7388837, 819, 203, 22, 17904, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59528, 0 ], [ 7390026, 809, 201, 1, 3153, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59530, 0 ], [ 7388656, 822, 202, 22, 10529, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59527, 0 ], [ 7389165, 1411, 1, 1, 74, 54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 7388664, 871, 205, 1, 3153, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59527, 0 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_Item_Base_get_all_player_item_infos() {
    Json::Value data;
    return this->send_and_receive(data, 2, 3);
}

//============================================================================
// 礼包转移到仓库1
// "module":161,"action":2,"request":[Utils.ShortUtil],"response":[Utils.UByteUtil]
// Example
//     [ 22(box_id) ] -> [ 4(Mod_GiftItemPack_Base::SUCCESS) ]
//============================================================================
Json::Value sxd_client::Mod_GiftItemPack_Base_put_item_to_package(int box_id) {
    Json::Value data;
    data.append(box_id);
    return this->send_and_receive(data, 161, 2);
}

//============================================================================
// 礼包转移到仓库2
// "module":2,"action":154,"request":[Utils.ShortUtil,Utils.ShortUtil],
// Example
//     [ 17(box_id), 0 ] -> [ 20(Mod_Item_Base::ACTION_SUCCESS), [ [ 0, 0, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 8920208, 5147, 1007, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_Item_Base_move_pack_grid_item_to_supergift_warehouse(int box_id) {
    Json::Value data;
    data.append(box_id);
    data.append(0);
    return this->send_and_receive(data, 2, 154);
}


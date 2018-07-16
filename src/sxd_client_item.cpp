#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_Item_Base {
public:
    static const int ACTION_SUCCESS = 20;
    static const int MATERIAL_NOT_ENOUGH = 22;
    static const int NOT_ENOUGH_FATE_GRID = 46;
};

void sxd_client::item_reel() {
    // read config
    Json::Value config;
    std::istringstream(db.get_config(user_id.c_str(), "ItemReel")) >> config;
    for (const auto& item : config)
        common::log(boost::str(boost::format("【ItemReel】[%1%]") % db.get_code(version, "Item", item.asInt())["text"]), 0);
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
                common::log(boost::str(boost::format("【背包】合成 [%1%]") % item_name));
            else if (data[0].asInt() == Mod_Item_Base::MATERIAL_NOT_ENOUGH) {
                //common::log(boost::str(boost::format("【背包】合成 [%1%] 失败，材料不足") % item_name));
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
                common::log(boost::str(boost::format("【背包】使用 [%1%]") % item_name));
            else if (data[0].asInt() == Mod_Item_Base::NOT_ENOUGH_FATE_GRID) {
                common::log(boost::str(boost::format("【背包】使用 [%1%] 失败，命格背包满") % item_name));
                break;
            } else {
                common::log(boost::str(boost::format("【背包】使用 [%1%]失败，result[%2%]") % item_name % data[0]));
                break;
            }
        }
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
    // sell
    for (const auto& item : items) {
        int item_id = item[1].asInt();
        int box_id = item[2].asInt();
        std::string item_name = db.get_code(version, "Item", item_id)["text"];
        if (std::find_if(config.begin(), config.end(), [item_id](const Json::Value& x) {return x.asInt()==item_id;}) == config.end())
            continue;
        data = this->Mod_Item_Base_player_sell_item(box_id);
        if (data[0].asInt() != Mod_Item_Base::ACTION_SUCCESS)
            common::log(boost::str(boost::format("【背包】出售 [%1%]失败，result[%2%]") % item_name % data[0]));
        else
            common::log(boost::str(boost::format("【背包】出售 [%1%]") % item_name));
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

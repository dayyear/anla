#include <fstream>
#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_HideTreasureMap_Base {
public:
    static const int SUCCEED = 0;
    static const int NOT_PLACE = 6;
    static const int COIN = 7;
};

//============================================================================
// ²Ø±¦Í¼
//============================================================================
void sxd_client::hide_treasure_map() {
    this->hide_treasure_map_search();
    for (;;) {
        Json::Value data = this->Mod_HideTreasureMap_Base_buy_item_info();
        int coin_limit = data[2].asInt();
        int coin_count = data[3].asInt();
        if (coin_count == coin_limit)
            return;
        data = this->Mod_HideTreasureMap_Base_buy_item();
        if (data[0].asInt() != Mod_HideTreasureMap_Base::SUCCEED) {
            common::log(boost::str(boost::format("¡¾²Ø±¦Í¼¡¿¹ºÂò [²Ø±¦Í¼] Ê§°Ü£¬result[%1%]") % data[0]), 0);
            return;
        }
        common::log("¡¾²Ø±¦Í¼¡¿¹ºÂò [²Ø±¦Í¼¡Á1]", iEdit);
        this->hide_treasure_map_search();
    }

}

void sxd_client::hide_treasure_map_search() {
    // ActivityGameAwardTypeData.as
    std::map<int, int> maps = { { 1, 1 }, { 2, 1 }, { 3, 1 }, { 4, 1 }, { 5, 1 }, { 6, 2 }, { 7, 2 }, { 8, 2 }, { 9, 2 }, { 10, 2 }, { 11, 3 }, { 12, 3 }, { 13, 3 }, { 14, 3 }, { 15, 3 }, { 16, 3 }, { 17, 3 }, { 18, 4 }, { 19, 4 }, { 20, 4 }, { 21, 4 }, { 22, 4 }, { 23, 7 }, { 24, 7 }, { 25, 7 }, { 26, 7 }, { 27, 8 }, { 28, 8 }, { 29, 8 }, { 30, 8 }, { 31, 8 }, { 32, 9 }, { 33, 9 }, { 34, 9 }, { 35, 9 }, { 36, 9 }, { 37, 9 }, { 38, 13 }, { 39, 13 }, { 40, 13 }, { 41, 13 }, { 42, 13 }, { 43, 14 }, { 44, 14 }, { 45, 14 }, { 46, 14 }, { 47, 14 }, { 48, 15 }, { 49, 15 }, { 50, 15 }, { 51, 15 }, { 52, 15 }, { 53, 21 }, { 54, 21 }, { 55, 21 }, { 56, 21 }, { 57, 21 }, { 58, 22 }, { 59, 22 }, { 60, 75 }, { 61, 75 }, { 62, 75 }, { 63, 75 }, { 64, 75 }, { 65, 22 }, { 66, 22 }, { 67, 22 }, { 69, 23 }, { 70, 23 }, { 71, 23 }, { 72, 23 }, { 73, 23 }, { 74, 24 }, { 75, 24 }, { 76, 24 }, { 77, 24 }, { 78, 24 }, { 84, 27 }, { 85, 27 }, { 86, 27 }, { 87, 27 }, { 88, 27 }, { 89, 29 }, { 90, 29 }, { 91, 29 }, { 92, 29 }, { 93, 29 }, { 94, 33 }, { 95, 33 }, { 96, 33 }, { 97, 33 }, { 98, 33 }, { 99, 35 }, { 100, 35 }, { 101, 35 }, { 102, 35 }, { 103, 35 }, { 104, 39 }, { 105, 39 }, { 106, 39 }, { 107, 39 }, { 108, 39 }, { 109, 40 }, { 110, 40 }, { 111, 40 }, { 112, 40 }, { 113, 40 }, { 114, 43 }, { 115, 43 }, { 116, 43 }, { 117, 43 }, { 118, 43 }, { 119, 54 }, { 120, 54 }, { 121, 54 }, { 122, 54 }, { 123, 54 }, { 124, 55 }, { 125, 55 }, { 126, 55 }, { 127, 55 }, { 128, 55 }, { 129, 56 }, { 130, 56 }, { 131, 56 }, { 132, 56 }, { 133, 56 }, { 134, 58 }, { 135, 58 }, { 136, 58 }, { 137, 58 }, { 138, 58 }, { 139, 62 }, { 140, 62 }, { 141, 62 }, { 142, 62 }, { 143, 62 }, { 144, 65 }, { 145, 65 }, { 146, 65 }, { 147, 65 }, { 148, 65 }, { 149, 68 }, { 150, 68 }, { 151, 68 }, { 152, 68 }, { 153, 68 }, { 154, 70 }, { 155, 70 }, { 156, 70 }, { 157, 70 }, { 158, 70 }, { 159, 73 }, { 160, 73 }, { 161, 73 }, { 162, 73 }, { 163, 73 } };
    for (;;) {
        Json::Value data = this->Mod_Item_Base_get_player_pack_item_list();
        Json::Value items = data[2];
        // 4872:ÆÕÍ¨²Ø±¦Í¼
        auto map = std::find_if(items.begin(), items.end(), [](const Json::Value& x) {return x[1].asInt()==4872;});
        if (map == items.end())
            return;
        int box_id = (*map)[2].asInt();

        data = this->Mod_HideTreasureMap_Base_player_use_grid_item(box_id);
        if (data[0].asInt() != Mod_HideTreasureMap_Base::SUCCEED) {
            common::log(boost::str(boost::format("¡¾²Ø±¦Í¼¡¿´ò¿ª [²Ø±¦Í¼] Ê§°Ü£¬result[%1%]") % data[0]), iEdit);
            return;
        }
        int treasure_map_item_id = data[1].asInt();
        int town_map_id = maps[treasure_map_item_id];
        this->Mod_Town_Base_enter_town(town_map_id);

        bool found = false;
        for (int x = 140; x <= 2500 && !found; x += 20) {
            for (int y = 100; y <= 100 && !found; y += 20) {
                data = this->Mod_HideTreasureMap_Base_start_grub(box_id, town_map_id, x, y);
                if (data[0].asInt() == Mod_HideTreasureMap_Base::SUCCEED) {
                    common::log("¡¾²Ø±¦Í¼¡¿ÍÚµ½ [±¦²Ø¡Á1]", iEdit);
                    common::log(boost::str(boost::format("¡¾²Ø±¦Í¼¡¿[treasure=%1%]£¬[town=%2%]£¬[x=%3%]£¬[y=%4%]") % treasure_map_item_id % town_map_id % x % y), iEdit);
                    std::ofstream ofile("²Ø±¦Í¼.txt", std::ios::binary | std::ios::out | std::ios::app);
                    ofile << boost::format("%1%, %2%, %3%, %4%") % treasure_map_item_id % town_map_id % x % y << std::endl;
                    ofile.close();
                    found = true;
                }
            }
        }
        if (!found) {
            common::log(boost::str(boost::format("¡¾²Ø±¦Í¼¡¿ÍÚ±¦²ØÊ§°Ü£¬[treasure_map_item_id=%1%]") % treasure_map_item_id), iEdit);
            return;
        }
    }
}

//============================================================================
// R177 ²Ø±¦Í¼Ãæ°å
// "module":287, "action":3, "request":[], "response":[Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil]
// HideTreasureMapData.as 135:
//     oObject.list(param1,this.buyInfo,["ingot_limit","ingot_count","coin_limit","coin_count"]);
// Example
//     [ 5, 0, 5, 0 ]
//============================================================================
Json::Value sxd_client::Mod_HideTreasureMap_Base_buy_item_info() {
    Json::Value data;
    return this->send_and_receive(data, 287, 3);
}

//============================================================================
// R177 Í­Ç®¹ºÂò
// "module":287, "action":2, "request":[Utils.UByteUtil], "response":[Utils.UByteUtil,Utils.IntUtil,Utils.IntUtil]
// HideTreasureMapData.as 123:
//     this.result = param1[0];
//     this.buyInfo.ingot_count = param1[1];
//     this.buyInfo.coin_count = param1[2];
// Example
//     [ 7 ]
//     [ 0, 0, 1 ]
//============================================================================
Json::Value sxd_client::Mod_HideTreasureMap_Base_buy_item() {
    Json::Value data;
    data.append(Mod_HideTreasureMap_Base::COIN);
    return this->send_and_receive(data, 287, 2);
}

//============================================================================
// R177 ´ò¿ª²Ø±¦Í¼
// "module":287, "action":0, "request":[Utils.IntUtil], "response":[Utils.UByteUtil,Utils.IntUtil]
// HideTreasureMapData.as 33:
//     this.result = param1[0];
//     this.treasureMapItemId = param1[1];
// Example
//     [ 0, 30 ]
//============================================================================
Json::Value sxd_client::Mod_HideTreasureMap_Base_player_use_grid_item(int box_id) {
    Json::Value data;
    data.append(box_id);
    return this->send_and_receive(data, 287, 0);
}

//============================================================================
// R177 ÍÚ±¦
// "module":287, "action":1, "request":[Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil], "response":[Utils.UByteUtil,[Utils.IntUtil,Utils.IntUtil,Utils.IntUtil],Utils.IntUtil,Utils.IntUtil,Utils.IntUtil]
// HideTreasureMapData.as 47:
//     oObject.list(param1,this.startGrubData,["result","award_list","award_type_id","item_id","item_count"]);
// Example
//     [ 0, [ [ 12, 347, 50 ], [ 12, 347, 50 ], [ 7, 1740, 5 ], [ 12, 5087, 1 ], [ 12, 5087, 1 ], [ 12, 347, 10 ], [ 12, 6074, 1 ], [ 27, 55, 1 ], [ 7, 1740, 3 ], [ 12, 4481, 200 ], [ 12, 5088, 1 ], [ 7, 1740, 5 ], [ 12, 5089, 1 ], [ 12, 347, 50 ], [ 12, 4481, 200 ], [ 27, 32, 1 ], [ 12, 347, 10 ], [ 7, 1740, 20 ] ], 12, 4481, 200 ]
//     [ 6, null, 0, 0, 0 ]
//============================================================================
Json::Value sxd_client::Mod_HideTreasureMap_Base_start_grub(int box_id, int town_map_id, int x, int y) {
    Json::Value data;
    data.append(box_id);
    data.append(town_map_id);
    data.append(x);
    data.append(y);
    return this->send_and_receive(data, 287, 1);
}


#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_ChaosEquipment_Base {
public:
    static const int SUCCESS = 0;
};

class Mod_SpaceFind_Base {
public:
    static const int NORMAL = 0;
    static const int USEINGOT = 1;
    static const int SUCCESS = 2;
};

void sxd_client::space_find() {
    Json::Value data = this->Mod_SpaceFind_Base_open_space_find();
    int count = data[0].asInt();
    common::log(boost::str(boost::format("°æªÏ„Á–Èø’°øΩÒ»’ªπø…◊•≤∂ [%1%] ¥Œ") % count));
    for (int i = 0; i < count; i++) {
        data = this->Mod_SpaceFind_Base_do_space_find(Mod_SpaceFind_Base::NORMAL);
        if (data[0] != Mod_SpaceFind_Base::SUCCESS) {
            common::log(boost::str(boost::format("°æªÏ„Á–Èø’°ø—∞’““Ï ﬁ ß∞‹£¨result[%1%]") % data[0]));
            break;
        }
        common::log(boost::str(boost::format("°æªÏ„Á–Èø’°ø—∞’““Ï ﬁ£¨∑¢œ÷ [%1%]") % db.get_code(version, "Item", data[1].asInt())["text"]));
        data = this->Mod_SpaceFind_Base_get_space_find();
        if (data[0] != Mod_SpaceFind_Base::SUCCESS) {
            common::log(boost::str(boost::format("°æªÏ„Á–Èø’°ø◊•≤∂“Ï ﬁ ß∞‹£¨result[%1%]") % data[0]));
            break;
        }
        std::ostringstream oss;
        for (const auto& item : data[1])
            oss << "[" << item[1] << "] ∏ˆ [" << db.get_code(version, "Item", item[0].asInt())["text"] << "]£¨";
        common::log(boost::str(boost::format("°æªÏ„Á–Èø’°ø◊•≤∂“Ï ﬁ£¨ªÒµ√ %1%") % oss.str().substr(0, oss.str().size() - 2)));
    }
}

//============================================================================
// R171
// ªÏ„Á–Èø’
// {module:123, action:0,
// request:[], response:[Utils.ShortUtil, Utils.IntUtil, Utils.ShortUtil, Utils.StringUtil, Utils.ShortUtil, Utils.IntUtil, Utils.UByteUtil]}
// SpaceFindController.as 20:
//     oObject.list(_loc_2, _loc_1, ["count", "item_id", "golden", "be_app_nickname", "can_buy_count", "integral", "useType"]);
// Example
//     [ 2, 0, 5, "", 20, 0, 0 ]
//============================================================================
Json::Value sxd_client::Mod_SpaceFind_Base_open_space_find() {
    Json::Value data;
    return this->send_and_receive(data, 123, 0);
}

//============================================================================
// R171
// —∞’““Ï ﬁ
// {module:123, action:1,
// request:[Utils.UByteUtil],
// Example
//     [ 0 ]
// response:[Utils.UByteUtil, Utils.IntUtil, Utils.ShortUtil, Utils.IntUtil, Utils.UByteUtil]}
// SpaceFindController.as 34:
//     oObject.list(_loc_2, _loc_1, ["result", "item_id", "golden", "integral", "useType"]);
// Example
//     [ 2, 1824, 5, 0, 0 ]
//============================================================================
Json::Value sxd_client::Mod_SpaceFind_Base_do_space_find(int type) {
    Json::Value data;
    data.append(type);
    return this->send_and_receive(data, 123, 1);
}
//============================================================================
// R171
// ◊•≤∂
// {module:123, action:2,
// request:[], response:[Utils.UByteUtil, [Utils.IntUtil, Utils.ShortUtil], Utils.ShortUtil, Utils.ShortUtil]}
// SpaceFindController.as 44:
//     oObject.list(_loc_2, _loc_1, ["result", "find_goods", "count", "can_buy_count"]);
// Example
//     [ 2, [ [ 1845, 1 ], [ 1854, 1 ] ], 1, 20 ]
//============================================================================
Json::Value sxd_client::Mod_SpaceFind_Base_get_space_find() {
    Json::Value data;
    return this->send_and_receive(data, 123, 2);
}

void sxd_client::chaos_equipment() {
    Json::Value data = this->Mod_ChaosEquipment_Base_get_pack_chaos_monster_list();
    common::log(boost::str(boost::format("°æªÏ„Á–Èø’°ø¡È±¶÷∆◊˜£∫[%1%] ∏ˆ [¡È“∫]") % data[0]), 0);
    std::ostringstream oss;
    for (const auto& item : data[1])
        oss << "[" << db.get_code(version, "Item", item[1].asInt())["text"] << "]£¨";
    common::log(boost::str(boost::format("°æªÏ„Á–Èø’°ø“Ï ﬁ±≥∞¸£∫%1%") % oss.str().substr(0, oss.str().size() - 2)), 0);
    oss.str("");
    for (const auto& item : data[2])
        oss << "[" << db.get_code(version, "Item", item[1].asInt())["text"] << "]£¨";
    common::log(boost::str(boost::format("°æªÏ„Á–Èø’°ø¡È±¶±≥∞¸£∫%1%") % oss.str().substr(0, oss.str().size() - 2)), 0);
    oss.str("");
    for (const auto& item : data[3])
        oss << "[" << item[1] << "] ∏ˆ [" << db.get_code(version, "Item", item[0].asInt())["text"] << "]£¨";
    common::log(boost::str(boost::format("°æªÏ„Á–Èø’°ø“Ï ﬁÕºº¯£∫%1%") % oss.str().substr(0, oss.str().size() - 2)), 0);

    Json::Value scraps = data[3];
    for (unsigned i = 0; i < scraps.size(); i++) {
        mss scrap_item = db.get_code(version, "Item", scraps[i][0].asInt());
        std::string scrap_comment = scrap_item["comment"];
        std::string scrap_name = scrap_item["text"];
        mss monster_item = db.get_code(version, "Item", common::gbk2utf(scrap_name.substr(0, scrap_name.size() - 4))); // »•µÙÀÈ∆¨
        int monster_id = boost::lexical_cast<int>(monster_item["value"]);
        std::string monster_name = monster_item["text"];
        // green and blue
        if ((scrap_comment.find("00ff00") != std::string::npos && scraps[i][1].asInt() >= 10) || (scrap_comment.find("00b7ee") != std::string::npos && scraps[i][1].asInt() >= 20)) {
            // ∫œ≥…
            data = this->Mod_ChaosEquipment_Base_make_chaos_monster(monster_id);
            if (data[0].asInt() != Mod_ChaosEquipment_Base::SUCCESS) {
                common::log(boost::str(boost::format("°æªÏ„Á–Èø’°ø∫œ≥… [%1%]  ß∞‹") % monster_name));
                break;
            }
            common::log(boost::str(boost::format("°æªÏ„Á–Èø’°ø∫œ≥… [%1%]") % monster_name));
            // ∑÷Ω‚
            data = this->Mod_ChaosEquipment_Base_resolve_player_chaos_monster(data[1][0][0].asInt());
            if (data[0].asInt() != Mod_ChaosEquipment_Base::SUCCESS) {
                common::log(boost::str(boost::format("°æªÏ„Á–Èø’°ø∑÷Ω‚ [%1%]  ß∞‹") % monster_name));
                break;
            }
            common::log(boost::str(boost::format("°æªÏ„Á–Èø’°ø∑÷Ω‚ [%1%]") % monster_name));
            // update scraps
            data = this->Mod_ChaosEquipment_Base_get_pack_chaos_monster_list();
            scraps = data[3];
            scrap_item = db.get_code(version, "Item", scraps[i][0].asInt());
            scrap_comment = scrap_item["comment"];
            if ((scrap_comment.find("00ff00") != std::string::npos && scraps[i][1].asInt() >= 10) || (scrap_comment.find("00b7ee") != std::string::npos && scraps[i][1].asInt() >= 20))
                i--;
        }
    }
}

//============================================================================
// R171
// ªÏ„Á“Ï ﬁ
// {module:124, action:0,
// request:[], response:[Utils.IntUtil, [Utils.IntUtil, Utils.ShortUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, [Utils.IntUtil, Utils.ShortUtil, Utils.ShortUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil]], [Utils.IntUtil, Utils.ShortUtil, Utils.ShortUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil], [Utils.ShortUtil, Utils.IntUtil]]}
// ChaosEquipmentData.as 35:
//     this._chaosEquipmentInfo.chaos_ling_ye = param1[0];      // ªÏ„Á¡È“∫
//     this._chaosEquipmentInfo.chaosMonsterExt(param1[1]);     // “Ï ﬁ±≥∞¸
//     this._chaosEquipmentInfo.chaosItemExt(param1[2]);        // ¡È±¶±≥∞¸
//     this._chaosEquipmentInfo.chaosScrapInfoExt(param1[3]);   // “Ï ﬁÕºº¯
// ChaosEquipmentInfo.as 94:
//     oObject.list(_loc_2, _loc_3, ["player_chaos_monster_id", "item_id", "grid_id", "potential", "break_count_1", "break_count_2", "number", "price", "player_role_id", "eat_count", "three_percent", "four_percent", "item_info"]);
// ChaosEquipmentInfo.as 208:
//     oObject.list(_loc_2, _loc_3, ["player_chaos_item_id", "item_id", "grid_id", "age_level", "number", "price", "player_chaos_monster_id"]);
// ChaosEquipmentInfo.as 993:
//     oObject.list(_loc_2, _loc_3, ["item_id", "number"]);
// Example
//     [ 1605, [ [ 133162, 1824, 3, 1, 0, 0, 1, 0, 0, 0, 0, 0, null ], [ 133163, 1825, 4, 1, 0, 0, 1, 0, 0, 0, 0, 0, null ], [ 112970, 1827, 2, 2, 0, 0, 1, 0, 0, 0, 0, 0, null ], [ 112434, 1824, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, null ] ],
//             [ [ 282844, 1842, 1, 600, 1, 0, 0 ] ],
//             [ [ 1854, 93 ], [ 1855, 89 ], [ 1856, 89 ], [ 1857, 47 ], [ 1858, 34 ], [ 1859, 35 ], [ 1860, 29 ], [ 1861, 29 ], [ 1862, 20 ], [ 1863, 3 ], [ 1864, 4 ], [ 1865, 2 ], [ 3697, 0 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_ChaosEquipment_Base_get_pack_chaos_monster_list() {
    Json::Value data;
    return this->send_and_receive(data, 124, 0);
}

//============================================================================
// R171
// ∫œ≥…
// {module:124, action:8,
// request:[Utils.ShortUtil],
// Example
//     [ 1824 ]
// response:[Utils.UByteUtil, [Utils.IntUtil, Utils.ShortUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil], [Utils.ShortUtil, Utils.IntUtil]]}
// ChaosEquipmentData.as 121:
//     this.makeChaosMonsterResult = param1[0];
//     this._chaosEquipmentInfo.chaosMonsterExt(param1[1]);     // “Ï ﬁ±≥∞¸(‘ˆº”)
//     this._chaosEquipmentInfo.chaosScrapInfoExt(param1[2]);   // “Ï ﬁÕºº¯(À¢–¬)
// Example
//     [ 0, [ [ 133164, 1824, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0 ] ], [ [ 1854, 83 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_ChaosEquipment_Base_make_chaos_monster(int id) {
    Json::Value data;
    data.append(id);
    return this->send_and_receive(data, 124, 8);
}

//============================================================================
// R171
// ∑÷Ω‚
// {module:124, action:13,
// request:[Utils.IntUtil],
// Example
//     [ 112434 ]
// response:[Utils.UByteUtil, [Utils.ShortUtil, Utils.IntUtil], [Utils.IntUtil, Utils.ShortUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil]]}
// ChaosEquipmentData.as 151:
//     this.playerResolveChaosResult = param1[0];
//     this._chaosEquipmentInfo.chaosScrapInfoExt(param1[1]);   // “Ï ﬁÕºº¯(À¢–¬)
//     this._chaosEquipmentInfo.chaosMonsterExt(param1[2]);     // “Ï ﬁ±≥∞¸(ºı…Ÿ)
// Example
//     [ 0, [ [ 1857, 48 ] ], [ [ 112434, 1824, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_ChaosEquipment_Base_resolve_player_chaos_monster(int id) {
    Json::Value data;
    data.append(id);
    return this->send_and_receive(data, 124, 13);
}


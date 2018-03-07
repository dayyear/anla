#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_WishPool_Base {
public:
    static const int YES = 0;
    static const int NO = 1;
    static const int SUCCESS = 2;
};

void sxd_client::wish_pool() {
    Json::Value data = this->Mod_WishPool_Base_get_wish_pool_info();
    Json::Value wish_pool_info = data;
    common::log(boost::str(boost::format("°æ–Ì‘∏≥ÿ°ø[◊£∏£÷µ°¡%1%]") % wish_pool_info[1]));
    int wish_count = 0;
    for (const auto& item : wish_pool_info[0])
        if (item[1].asInt() == Mod_WishPool_Base::YES)
            wish_count++;
    if (wish_count == 0) {
        int choose_index[] = { 0, 1, 2 };
        int choose_id[3];
        for (int i = 0; i < 3; i++)
            choose_id[i] = wish_pool_info[0][choose_index[i]][0].asInt();
        data = this->Mod_WishPool_Base_choose_awards(choose_id, 3);
        if (data[0].asInt() != Mod_WishPool_Base::SUCCESS) {
            common::log(boost::str(boost::format("°æ–Ì‘∏≥ÿ°ø–Ì‘∏ ß∞‹£¨result[%1%]") % data[0]));
            return;
        }
        common::log("°æ–Ì‘∏≥ÿ°ø–Ì‘∏");
    }

    if (wish_pool_info[1] < 500) {
        if (wish_pool_info[5] == Mod_WishPool_Base::NO) {
            data = this->Mod_WishPool_Base_wish_self();
            if (data[0].asInt() != Mod_WishPool_Base::SUCCESS) {
                common::log(boost::str(boost::format("°æ–Ì‘∏≥ÿ°ø◊£∏£◊‘º∫ ß∞‹£¨result[%1%]") % data[0]));
                return;
            }
            common::log("°æ–Ì‘∏≥ÿ°ø◊£∏£◊‘º∫");
        }
    } else {
        if (wish_pool_info[4] == Mod_WishPool_Base::NO) {
            data = this->Mod_WishPool_Base_get_award(Mod_WishPool_Base::NO);
            if (data[0].asInt() != Mod_WishPool_Base::SUCCESS) {
                common::log(boost::str(boost::format("°æ–Ì‘∏≥ÿ°ø¡Ï»°Ω±¿¯ ß∞‹£¨result[%1%]") % data[0]));
                return;
            }
            common::log("°æ–Ì‘∏≥ÿ°ø¡Ï»°Ω±¿¯");
        } else
            common::log("°æ–Ì‘∏≥ÿ°øΩ±¿¯“—¡Ï»°");
    }

    int count = wish_pool_info[6].asInt();
    // ”≈œ»∏¯◊‘º∫»À◊£∏£
    data = this->Mod_WishPool_Base_get_wish_list();
    Json::Value players = data[0];
    for (const auto& player : players) {
        if (player[5].asInt() >= 500)
            continue;
        std::string name = player[1].asString();
        // ºÏ≤‚◊‘º∫»À
        if (db.get_records("user", ("name='" + name + "'").c_str()).size() == 0)
            continue;
        data = this->Mod_WishPool_Base_wish_other(player[0].asInt());
        if (data[0].asInt() != Mod_WishPool_Base::SUCCESS) {
            common::log(boost::str(boost::format("°æ–Ì‘∏≥ÿ°ø◊£∏£ [%1%]  ß∞‹£¨result[%2%]") % common::utf2gbk(name) % data[0]));
            return;
        }
        common::log(boost::str(boost::format("°æ–Ì‘∏≥ÿ°ø◊£∏£ [%1%]") % common::utf2gbk(name)));
        count--;
    }
    // ∏¯ƒ∞…˙»À◊£∏£
    data = this->Mod_WishPool_Base_get_wish_list();
    players = data[0];
    for (const auto& player : players) {
        if (count <= 0)
            break;
        if (player[5].asInt() >= 500)
            continue;
        std::string name = player[1].asString();
        data = this->Mod_WishPool_Base_wish_other(player[0].asInt());
        if (data[0].asInt() != Mod_WishPool_Base::SUCCESS) {
            common::log(boost::str(boost::format("°æ–Ì‘∏≥ÿ°ø◊£∏£ [%1%]  ß∞‹£¨result[%2%]") % common::utf2gbk(name) % data[0]));
            return;
        }
        common::log(boost::str(boost::format("°æ–Ì‘∏≥ÿ°ø◊£∏£ [%1%]") % common::utf2gbk(name)));
        count--;
    }
}

//============================================================================
// R171 –Ì‘∏≥ÿ
// {module:359, action:0,
// request:[], response:[[Utils.IntUtil, Utils.UByteUtil], Utils.ShortUtil, Utils.ByteUtil, Utils.IntUtil, Utils.UByteUtil, Utils.UByteUtil, Utils.ByteUtil]}
// WishPoolData.as 32:
//     oObject.list(param1, this._mineObject, ["wish_pool_award_list", "wish_value", "speedup_times", "speedup_coins", "gotten_flag", "wish_self_flag", "avaliable_wish_other_award_times"]);
//     for each (_loc_4 in this._mineObject.wish_pool_award_list)
//         oObject.list(_loc_4, _loc_3, ["id", "flag"]);
// Example
// ≥ı º
//     [ [ [ 1, 1 ], [ 27, 1 ], [ 8, 1 ], [ 21, 1 ], [ 32, 1 ], [ 31, 1 ], [ 19, 1 ], [ 13, 1 ] ], 0, 3, 500000, 1, 1, 3 ]
// –Ì‘∏
//     [ [ [ 1, 1 ], [ 27, 0 ], [ 8, 1 ], [ 21, 0 ], [ 32, 1 ], [ 31, 1 ], [ 19, 0 ], [ 13, 1 ] ], 0, 3, 500000, 1, 1, 3 ]
// ◊£∏£
//     [ [ [ 1, 1 ], [ 27, 0 ], [ 8, 1 ], [ 21, 0 ], [ 32, 1 ], [ 31, 1 ], [ 19, 0 ], [ 13, 1 ] ], 50, 3, 500000, 1, 0, 3 ]
// ◊£∏£¬˙
//     [ [ [ 1, 1 ], [ 26, 1 ], [ 9, 0 ], [ 13, 1 ], [ 14, 0 ], [ 31, 1 ], [ 3, 1 ], [ 19, 0 ] ], 510, 3, 500000, 1, 1, 0 ]
// ¡Ï»°Ω±¿¯
//     [ [ [ 33, 1 ], [ 20, 1 ], [ 9, 1 ], [ 5, 1 ], [ 4, 1 ], [ 31, 0 ], [ 3, 0 ], [ 35, 0 ] ], 510, 3, 500000, 0, 1, 0 ]
//============================================================================
Json::Value sxd_client::Mod_WishPool_Base_get_wish_pool_info() {
    Json::Value data;
    return this->send_and_receive(data, 359, 0);
}

//============================================================================
// R171 –Ì‘∏
// {module:359, action:2,
// request:[[Utils.IntUtil]],
// Example
//     [ [ [ 27 ], [ 21 ], [ 19 ] ] ]
// response:[Utils.UByteUtil]}
// Example
//     [ 2 ]
//============================================================================
Json::Value sxd_client::Mod_WishPool_Base_choose_awards(int ids[], int count) {
    Json::Value data, data1;
    for (int i = 0; i < count; i++) {
        Json::Value data2;
        data2.append(ids[i]);
        data1.append(data2);
    }
    data.append(data1);
    return this->send_and_receive(data, 359, 2);
}

//============================================================================
// R171 ◊£∏£◊‘º∫
// {module:359, action:3, request:[], response:[Utils.UByteUtil]}
// Example
//     [ 2 ]
//============================================================================
Json::Value sxd_client::Mod_WishPool_Base_wish_self() {
    Json::Value data;
    return this->send_and_receive(data, 359, 3);
}

//============================================================================
// R171 ∆‰À˚–Ì‘∏≥ÿ
// {module:359, action:1,
// request:[], response:[[Utils.IntUtil, Utils.StringUtil, Utils.StringUtil, Utils.StringUtil, [Utils.IntUtil, Utils.UByteUtil], Utils.ShortUtil]]}
// WishPoolData.as 79:
//     for each (_loc_2 in param1[0])
//         oObject.list(_loc_2, _loc_3, ["st_player_id", "nickname", "server_name", "stage_name", "wish_pool_award_list", "wish_value"]);
//         for each (_loc_5 in _loc_3.wish_pool_award_list)
//             oObject.list(_loc_5, _loc_4, ["id", "flag"]);
// Example
// wish_value>=500 Œ™¬˙◊£∏£
//     [ [ [ 200002, "\u9ed1\u591c\u7ff1\u7fd4.s1", "s04", "\u5fc3\u52a8", [ [ 1, 0 ], [ 29, 0 ], [ 34, 1 ], [ 20, 1 ], [ 19, 1 ], [ 17, 1 ], [ 16, 0 ], [ 33, 1 ] ], 500 ], [ 7703, "\u9a6c\u62c9\u6208\u58c1.s2", "yx567_s0125", "7k7k", [ [ 19, 0 ], [ 23, 1 ], [ 9, 1 ], [ 25, 0 ], [ 30, 0 ], [ 1, 1 ], [ 26, 1 ], [ 18, 1 ] ], 520 ], [ 12904, "\u4e3f\u7d14\u4e36\u6c3a\u6c3a.s1", "s04", "\u5fc3\u52a8", [ [ 15, 1 ], [ 18, 0 ], [ 8, 0 ], [ 31, 1 ], [ 20, 1 ], [ 35, 0 ], [ 26, 1 ], [ 29, 1 ] ], 500 ], [ 204904, "\u5f90\u63d0\u83ab.s4", "s04", "\u5fc3\u52a8", [ [ 15, 1 ], [ 23, 1 ], [ 26, 1 ], [ 1, 0 ], [ 18, 0 ], [ 19, 1 ], [ 31, 1 ], [ 16, 0 ] ], 500 ], [ 705, "\u771f\u5b9e.s1", "s04", "\u5fc3\u52a8", [ [ 30, 0 ], [ 14, 0 ], [ 25, 1 ], [ 8, 1 ], [ 16, 0 ], [ 29, 1 ], [ 20, 1 ], [ 13, 1 ] ], 500 ], [ 206708, "\u738b\u4e18\u95ea.s1", "37wan_s0273", "37wan", [ [ 6, 1 ], [ 8, 0 ], [ 22, 0 ], [ 1, 0 ], [ 25, 1 ], [ 19, 1 ], [ 7, 1 ], [ 3, 1 ] ], 500 ], [ 4709, "\u5fc3\u75db\u4e3a\u8c01.s4", "s04", "\u5fc3\u52a8", [ [ 25, 0 ], [ 31, 1 ], [ 3, 1 ], [ 29, 1 ], [ 18, 0 ], [ 22, 0 ], [ 6, 1 ], [ 9, 1 ] ], 500 ], [ 22112, "\u55b5\u5566\u5566.s4", "yx567_s0125", "7k7k", [ [ 17, 0 ], [ 21, 1 ], [ 35, 1 ], [ 18, 1 ], [ 27, 1 ], [ 24, 0 ], [ 22, 0 ], [ 34, 1 ] ], 500 ], [ 1117, "\u2570\u2606\u256e\u8ca1\u795e\u723a.s12", "yaowan_s0152", "\u8981\u73a9", [ [ 11, 1 ], [ 9, 1 ], [ 18, 0 ], [ 33, 1 ], [ 32, 1 ], [ 21, 0 ], [ 20, 0 ], [ 26, 1 ] ], 520 ], [ 200917, "\u590f\u95ea\u817e.s1", "s04", "\u5fc3\u52a8", [ [ 9, 0 ], [ 27, 0 ], [ 15, 1 ], [ 5, 1 ], [ 31, 1 ], [ 17, 1 ], [ 30, 0 ], [ 33, 1 ] ], 500 ], [ 13219, "\u4e0a\u5bab\u84dd\u51cc.s8", "yx567_s0125", "7k7k", [ [ 8, 1 ], [ 27, 0 ], [ 17, 1 ], [ 16, 1 ], [ 14, 0 ], [ 5, 1 ], [ 20, 0 ], [ 28, 1 ] ], 510 ], [ 207021, "\u6881\u66e6\u6708.s1", "s04", "\u5fc3\u52a8", [ [ 32, 1 ], [ 20, 1 ], [ 9, 0 ], [ 17, 1 ], [ 16, 0 ], [ 35, 0 ], [ 18, 1 ], [ 13, 1 ] ], 520 ], [ 322, "\u4eba\u7687.s3", "37wan_s0273", "37wan", [ [ 28, 1 ], [ 7, 0 ], [ 21, 0 ], [ 31, 1 ], [ 4, 1 ], [ 14, 0 ], [ 15, 1 ], [ 32, 1 ] ], 540 ], [ 626, "\u8840\u8759\u8760.s1", "s04", "\u5fc3\u52a8", [ [ 17, 0 ], [ 35, 0 ], [ 1, 1 ], [ 2, 1 ], [ 18, 0 ], [ 31, 1 ], [ 7, 1 ], [ 5, 1 ] ], 500 ], [ 55627, "\u4e00\u5ff5\u5343\u6b87.s1", "s04", "\u5fc3\u52a8", [ [ 16, 1 ], [ 32, 1 ], [ 31, 0 ], [ 19, 1 ], [ 1, 0 ], [ 17, 1 ], [ 14, 0 ], [ 33, 1 ] ], 500 ], [ 1227, "\u82b1\u513f\u7efd\u653e.s9", "yaowan_s0152", "\u8981\u73a9", [ [ 35, 0 ], [ 7, 1 ], [ 13, 0 ], [ 17, 1 ], [ 10, 1 ], [ 28, 1 ], [ 14, 0 ], [ 12, 1 ] ], 500 ], [ 14032, "Star\u4e36z.s4", "s04", "\u5fc3\u52a8", [ [ 19, 1 ], [ 9, 0 ], [ 16, 0 ], [ 7, 1 ], [ 21, 1 ], [ 1, 1 ], [ 34, 0 ], [ 25, 1 ] ], 520 ], [ 14337, "\u9020\u5316\u949f\u795e\u79c0.s3", "37wan_s0273", "37wan", [ [ 3, 1 ], [ 28, 1 ], [ 16, 0 ], [ 18, 0 ], [ 27, 0 ], [ 26, 1 ], [ 32, 1 ], [ 6, 1 ] ], 520 ], [ 8139, "\u5c0f\u841d\u535c.s2", "yx567_s0125", "7k7k", [ [ 27, 0 ], [ 17, 0 ], [ 12, 1 ], [ 28, 0 ], [ 32, 1 ], [ 8, 1 ], [ 31, 1 ], [ 5, 1 ] ], 520 ], [ 9739, "\u6768\u677f\u6028.s2", "yx567_s0125", "7k7k", [ [ 32, 1 ], [ 35, 1 ], [ 18, 1 ], [ 9, 0 ], [ 19, 1 ], [ 34, 0 ], [ 22, 0 ], [ 16, 1 ] ], 140 ], [ 241, "\u82b1\u513f\u7efd\u653e.s11", "yaowan_s0152", "\u8981\u73a9", [ [ 10, 1 ], [ 30, 0 ], [ 4, 1 ], [ 5, 0 ], [ 25, 1 ], [ 32, 1 ], [ 19, 1 ], [ 22, 0 ] ], 500 ], [ 196141, "\u5c9a\u4e91.s6", "yx567_s0125", "7k7k", [ [ 2, 0 ], [ 13, 1 ], [ 16, 0 ], [ 14, 0 ], [ 32, 1 ], [ 26, 1 ], [ 9, 1 ], [ 23, 1 ] ], 520 ], [ 3142, "\u7d2b\u51dd.s1", "s04", "\u5fc3\u52a8", [ [ 10, 1 ], [ 12, 0 ], [ 21, 0 ], [ 25, 1 ], [ 1, 1 ], [ 32, 1 ], [ 24, 1 ], [ 13, 0 ] ], 500 ], [ 206947, "\u6c89\u9999.s5", "s03", "\u5fc3\u52a8", [ [ 33, 1 ], [ 17, 1 ], [ 2, 1 ], [ 22, 0 ], [ 1, 1 ], [ 29, 0 ], [ 16, 0 ], [ 20, 1 ] ], 500 ], [ 11447, "\u6d41\u5929\u7c7b\u661f.s8", "s05", "\u5fc3\u52a8", [ [ 20, 1 ], [ 29, 1 ], [ 3, 1 ], [ 2, 0 ], [ 34, 0 ], [ 27, 1 ], [ 24, 0 ], [ 22, 1 ] ], 500 ], [ 2248, "\u6709\u540d.s7", "37wan_s0273", "37wan", [ [ 31, 1 ], [ 13, 1 ], [ 4, 1 ], [ 35, 1 ], [ 21, 0 ], [ 22, 0 ], [ 30, 0 ], [ 33, 1 ] ], 500 ], [ 5851, "\u67ef\u82e5\u5f64.s15", "95k_s081", "95k", [ [ 28, 1 ], [ 33, 1 ], [ 21, 0 ], [ 22, 1 ], [ 3, 1 ], [ 24, 0 ], [ 17, 0 ], [ 15, 1 ] ], 520 ], [ 203957, "\u5e84\u606d\u6625.s1", "s04", "\u5fc3\u52a8", [ [ 1, 1 ], [ 27, 0 ], [ 8, 1 ], [ 21, 0 ], [ 32, 1 ], [ 31, 1 ], [ 19, 0 ], [ 13, 1 ] ], 80 ], [ 205760, "\u5f02\u754c \u6b8b\u5f71.s6", "s03", "\u5fc3\u52a8", [ [ 35, 0 ], [ 9, 0 ], [ 18, 1 ], [ 29, 1 ], [ 17, 1 ], [ 16, 1 ], [ 19, 0 ], [ 2, 1 ] ], 500 ], [ 207060, "\u53ef\u80fd\u8fd8\u662f\u8c37\u66e6.s1", "s04", "\u5fc3\u52a8", [ [ 16, 1 ], [ 35, 1 ], [ 25, 1 ], [ 13, 0 ], [ 14, 0 ], [ 32, 1 ], [ 29, 1 ], [ 27, 0 ] ], 510 ], [ 7861, "\u5929\u5929\u8bf4\u518d\u89c1.s5", "yx567_s0125", "7k7k", [ [ 9, 0 ], [ 18, 1 ], [ 14, 1 ], [ 2, 0 ], [ 6, 1 ], [ 30, 0 ], [ 16, 1 ], [ 35, 1 ] ], 520 ], [ 862, "\u989c\u82d1.s7", "s04", "\u5fc3\u52a8", [ [ 32, 1 ], [ 5, 1 ], [ 2, 1 ], [ 11, 0 ], [ 26, 1 ], [ 28, 0 ], [ 34, 0 ], [ 10, 1 ] ], 520 ], [ 7765, "\u6d77\u71d5.s2", "yx567_s0125", "7k7k", [ [ 23, 1 ], [ 20, 0 ], [ 32, 1 ], [ 2, 1 ], [ 16, 0 ], [ 17, 1 ], [ 25, 0 ], [ 3, 1 ] ], 510 ], [ 200966, "\u59d0\u73a9\u7684\u5bc2\u5bde.s1", "s04", "\u5fc3\u52a8", [ [ 6, 1 ], [ 18, 1 ], [ 35, 0 ], [ 32, 0 ], [ 25, 1 ], [ 31, 1 ], [ 13, 0 ], [ 4, 1 ] ], 500 ], [ 8366, "\u54c8\u6839\u8fbe\u65af\u4e44.s8", "yx567_s0125", "7k7k", [ [ 31, 1 ], [ 19, 1 ], [ 13, 0 ], [ 21, 0 ], [ 1, 1 ], [ 22, 0 ], [ 25, 1 ], [ 16, 1 ] ], 170 ], [ 2968, "\u82cf\u5a9a\u5a77.s7", "s04", "\u5fc3\u52a8", [ [ 19, 1 ], [ 27, 0 ], [ 15, 1 ], [ 30, 0 ], [ 29, 1 ], [ 26, 1 ], [ 17, 0 ], [ 9, 1 ] ], 500 ], [ 5468, "\u2225:\u8def\u4ebd\u66f1.s1", "s04", "\u5fc3\u52a8", [ [ 18, 1 ], [ 2, 0 ], [ 7, 1 ], [ 14, 0 ], [ 9, 0 ], [ 33, 1 ], [ 35, 1 ], [ 26, 1 ] ], 500 ], [ 7870, "\u51ef\u4e50\u5927\u738b.s1", "yx567_s0125", "7k7k", [ [ 29, 0 ], [ 3, 1 ], [ 27, 0 ], [ 34, 1 ], [ 8, 1 ], [ 25, 1 ], [ 15, 1 ], [ 6, 0 ] ], 520 ], [ 8671, "\u5929\u5e1d.s4", "yx567_s0125", "7k7k", [ [ 19, 1 ], [ 29, 0 ], [ 1, 1 ], [ 18, 1 ], [ 2, 0 ], [ 16, 0 ], [ 33, 1 ], [ 34, 1 ] ], 500 ], [ 5273, "\u65e0\u4eba\u53ca\u4f60.s02", "s02", "\u5fc3\u52a8", [ [ 3, 0 ], [ 5, 1 ], [ 8, 1 ], [ 21, 1 ], [ 26, 1 ], [ 9, 0 ], [ 6, 1 ], [ 27, 0 ] ], 520 ], [ 5376, "\u4e01\u5c27\u83f2.s5", "95k_s081", "95k", [ [ 34, 1 ], [ 22, 1 ], [ 16, 0 ], [ 17, 0 ], [ 20, 1 ], [ 29, 0 ], [ 31, 1 ], [ 25, 1 ] ], 500 ], [ 8186, "1234.s1", "yx567_s0125", "7k7k", [ [ 21, 1 ], [ 7, 1 ], [ 29, 0 ], [ 24, 0 ], [ 34, 1 ], [ 27, 1 ], [ 2, 0 ], [ 4, 1 ] ], 510 ], [ 4088, "\u5a49\u7b90\u537f.s1", "s04", "\u5fc3\u52a8", [ [ 9, 1 ], [ 7, 1 ], [ 8, 1 ], [ 24, 0 ], [ 27, 0 ], [ 35, 0 ], [ 17, 1 ], [ 1, 1 ] ], 500 ], [ 16889, "\u4e1c\u65b9\u5de8\u8fdf.s1", "s04", "\u5fc3\u52a8", [ [ 18, 1 ], [ 13, 1 ], [ 33, 1 ], [ 35, 0 ], [ 9, 0 ], [ 14, 1 ], [ 16, 0 ], [ 34, 1 ] ], 520 ], [ 2889, "\u5eb7\u6f6d\u6c14.s4", "95k_s081", "95k", [ [ 2, 0 ], [ 6, 1 ], [ 29, 1 ], [ 1, 1 ], [ 4, 1 ], [ 27, 0 ], [ 22, 0 ], [ 17, 1 ] ], 520 ], [ 194498, "\u8346\u65e0\u971c.s1", "yx567_s0125", "7k7k", [ [ 24, 1 ], [ 31, 0 ], [ 20, 0 ], [ 29, 1 ], [ 34, 0 ], [ 18, 1 ], [ 8, 1 ], [ 21, 1 ] ], 500 ] ] ]
//         [ 203957, "\u5e84\u606d\u6625.s1", "s04", "\u5fc3\u52a8", [ [ 1, 1 ], [ 27, 0 ], [ 8, 1 ], [ 21, 0 ], [ 32, 1 ], [ 31, 1 ], [ 19, 0 ], [ 13, 1 ] ], 80 ]
//============================================================================
Json::Value sxd_client::Mod_WishPool_Base_get_wish_list() {
    Json::Value data;
    return this->send_and_receive(data, 359, 1);
}

//============================================================================
// R171 ◊£∏£À˚»À
// {module:359, action:5,
// request:[Utils.IntUtil],
// Example
//     [ 203957 ]
// response:[Utils.UByteUtil, Utils.IntUtil, Utils.IntUtil]}
// WishPoolData.as 123:
//     this.result = param1[0];
//     _loc_2.st_player_id == param1[1]
//     this.wishOtherAwardCoins = param1[2];
// Example
//     [ 2, 203957, 200000 ]
//============================================================================
Json::Value sxd_client::Mod_WishPool_Base_wish_other(int id) {
    Json::Value data;
    data.append(id);
    return this->send_and_receive(data, 359, 5);
}

//============================================================================
// R171 ¡Ï»°
// {module:359, action:6,
// request:[Utils.UByteUtil],
// WishPoolView.as 243:
//     modValue = value == 2 ? (Mod_WishPool_Base.YES) : (Mod_WishPool_Base.NO);
//     _data.call(Mod_WishPool_Base.get_award, get_award_callback, [modValue], true, DataBase.SOCKETSERVERTOWN);
// Example
//     [ 0 ], Mod_WishPool_Base.YES, Ω±¿¯∑≠±∂
//     [ 1 ], Mod_WishPool_Base.NO
// response:[Utils.UByteUtil]}
// Example
//     [ 2 ]
//============================================================================
Json::Value sxd_client::Mod_WishPool_Base_get_award(int flag) {
    Json::Value data;
    data.append(flag);
    return this->send_and_receive(data, 359, 6);
}

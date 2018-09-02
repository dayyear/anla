#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_Player_Base {
public:
    static const int SUCCEED = 0;
    static const int ACTION_SUCCESS = 88;
};

class Mod_Farm_Base {
public:
    static const int SUCCESS = 8;
};

void sxd_client::gift3() {
    Json::Value data = this->Mod_Player_Base_get_game_assistant_info();
    Json::Value game_assistant_info = data;
    // 灵石 is_can_get_stone(30)
    if (game_assistant_info[30].asInt() == Mod_Player_Base::SUCCEED) {
        data = this->Mod_SealSoul_Base_get_day_stone();
        if (data[0].asInt() == 1)
            common::log(boost::str(boost::format("【礼包】领取 [灵石×%1%]") % data[1]), iEdit);
        else
            common::log(boost::str(boost::format("【礼包】领取灵石失败，result[%1%]") % data[0]), iEdit);
    }
    // 俸禄 is_get_camp_salary[13]
    if (game_assistant_info[13].asInt() == Mod_Player_Base::SUCCEED) {
        data = this->Mod_Player_Base_get_player_camp_salary();
        if (data[0].asInt() == Mod_Player_Base::ACTION_SUCCESS)
            common::log(boost::str(boost::format("【礼包】领取俸禄 [铜钱×%1%]") % data[1]), iEdit);
        else
            common::log(boost::str(boost::format("【礼包】领取俸禄失败，result[%1%]") % data[0]), iEdit);
    }
    // 仙令
    data = this->Mod_Farm_Base_player_is_player_get_xian_ling_gift();
    if (data[0].asInt()) {
        data = this->Mod_Farm_Base_player_get_xian_ling_gift();
        if (data[0].asInt() == Mod_Farm_Base::SUCCESS)
            common::log(boost::str(boost::format("【礼包】领取 [仙令×%1%]") % data[1]), iEdit);
        else
            common::log(boost::str(boost::format("【礼包】领取仙令失败，[%1%]") % data[0]), iEdit);
    }
}

//============================================================================
// R172 灵石
// {module:34, action:18,
// request:[Utils.ByteUtil],
// SealStoneView.as 60:
//     this.getDaySealStone(0);
// Example
//     [ 0 ]
// response:[Utils.ByteUtil, Utils.IntUtil]};
// SealSoulData.as 154:
//     oObject.list(param1, this.getDayStone, ["state", "stone_coun"]);
// SealStoneView.as 163:
//     if (_loc_1["state"] == 1)
// Example
//     [ 1, 70 ]
//============================================================================
Json::Value sxd_client::Mod_SealSoul_Base_get_day_stone() {
    Json::Value data;
    data.append(0);
    return this->send_and_receive(data, 34, 18);
}

//============================================================================
// R172 俸禄
// {module:0, action:20, request:[], response:[Utils.UByteUtil, Utils.IntUtil]};
// PlayerData.as 352:
//     this.campSalaryStatus = param1[0];
//     this.getPlayerCampSalary = param1[1];
// Example
//     [ 87, 326000 ]
//============================================================================
Json::Value sxd_client::Mod_Player_Base_get_player_camp_salary() {
    Json::Value data;
    return this->send_and_receive(data, 0, 20);
}

//============================================================================
// R172 仙令数量
// {module:13, action:19, request:[], response:[Utils.ByteUtil]};
// FarmData.as 546:
//     this.getXianLingNum = param1[0];
// Example
//     [ 8 ]
//     [ 0 ]
//============================================================================
Json::Value sxd_client::Mod_Farm_Base_player_is_player_get_xian_ling_gift() {
    Json::Value data;
    return this->send_and_receive(data, 13, 19);
}

//============================================================================
// R172 仙令
// {module:13, action:20, request:[], response:[Utils.UByteUtil, Utils.ByteUtil]};
// FarmData.as 552:
//     this.getXianLingResult = param1[0];
// Example
//     [ 8, 8 ]
//============================================================================
Json::Value sxd_client::Mod_Farm_Base_player_get_xian_ling_gift() {
    Json::Value data;
    return this->send_and_receive(data, 13, 20);
}

class Mod_FunctionEnd_Base {
public:
    static const int SUCCESS = 0;
};

void sxd_client::function_end() {
    Json::Value data = this->Mod_FunctionEnd_Base_game_function_end_gift();
    Json::Value gifts = data[0];
    for (const auto& gift : gifts) {
        int id = gift[0].asInt();
        if (gift[8].asInt() == 0)
            this->Mod_FunctionEnd_Base_random_award(id);
        data = this->Mod_FunctionEnd_Base_get_game_function_end_gift(id);
        if (data[0].asInt() != Mod_FunctionEnd_Base::SUCCESS)
            common::log(boost::str(boost::format("【礼包】领取失败，result[%1%]") % data[0]), iEdit);
        else
            common::log(boost::str(boost::format("【礼包】领取 [%1%]") % db.get_code(version, "EndFunctionGift", id)["text"]), iEdit);
    }
}

//============================================================================
// R171 随机礼包
// {module:127, action:1,
// request:[],
// response:[[Utils.ShortUtil, Utils.ShortUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.ShortUtil, Utils.ShortUtil, Utils.IntUtil, Utils.ByteUtil]]}
// FunctionEndData.as 56:
//     _loc_4 = GiftType.getEndFunctionGift(_loc_1[_loc_3][0]);
//     GiftType.getEndFunctionGift(_loc_1[_loc_3][0]).giftId = _loc_4.id;
//     _loc_4.iconPath = URI.functionEndUrl + _loc_4.id + ".png";
//     _loc_4.flag = 1;
//     _loc_4.message = "";
//     _loc_4.addMsg = "";
//     _loc_4.state_point = _loc_1[_loc_3][1];       // 境界
//     _loc_4.skill = _loc_1[_loc_3][2];             // 阅历
//     _loc_4.xian_ling = _loc_1[_loc_3][3];         // 仙令
//     _loc_4.fame = _loc_1[_loc_3][4];              // 声望
//     _loc_4.nimbus = _loc_1[_loc_3][5];            // 灵气
//     _loc_4.coin_buff = _loc_1[_loc_3][6];         // 铜钱加成
//     _loc_4.exp_buff = _loc_1[_loc_3][7];          // 经验加成
//     _loc_4.ingot = _loc_1[_loc_3][8];             // 元宝
//     _loc_4.sortNum = 2000 + _loc_4.sort * 100;    // 排序
//     _loc_4.bei = _loc_1[_loc_3][9];               // 翻倍
// Example
//     [ [ [ 13, 0, 0, 0, 0, 0, 0, 0, 0, 10 ], [ 7, 0, 0, 0, 0, 0, 0, 0, 0, 2 ], [ 16, 0, 0, 0, 0, 0, 0, 0, 0, 2 ], [ 1, 0, 0, 0, 0, 0, 0, 0, 0, 2 ], [ 4, 0, 0, 0, 0, 0, 0, 0, 0, 4 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_FunctionEnd_Base_game_function_end_gift() {
    Json::Value data;
    return this->send_and_receive(data, 127, 1);
}

//============================================================================
// R171 随机
// {module:127, action:3,
// request:[Utils.ShortUtil],
// response:[Utils.ShortUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil]}
// FunctionEndData.as 202:
//     _loc_3.state_point = param1[0];
//     _loc_3.skill = param1[1];
//     _loc_3.xian_ling = param1[2];
//     _loc_3.fame = param1[3];
//     _loc_3.nimbus = param1[4];
//     _loc_3.exp_buff = param1[5];
//     this.randomIngot = param1[6];
//============================================================================
Json::Value sxd_client::Mod_FunctionEnd_Base_random_award(int id) {
    Json::Value data;
    data.append(id);
    return this->send_and_receive(data, 127, 3);
}

//============================================================================
// R171 领取
// {module:127, action:2,
// request:[Utils.ShortUtil],
// response:[Utils.UByteUtil, Utils.ShortUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.ShortUtil, Utils.ShortUtil, Utils.ByteUtil]}
// FunctionEndData.as 191:
//     this.result = param1[0];
//     this.isHaveNext = param1[8] == 1;
//============================================================================
Json::Value sxd_client::Mod_FunctionEnd_Base_get_game_function_end_gift(int id) {
    Json::Value data;
    data.append(id);
    return this->send_and_receive(data, 127, 2);
}

class Mod_Item_Base {
public:
    static const int ACTION_SUCCESS = 20;
};

void sxd_client::gift() {
    Json::Value data = this->Mod_Item_Base_get_player_gift_all_info();
    Json::Value gifts = data[0];
    for (const auto& gift : gifts) {
        int id = gift[0].asInt();
        std::string message = gift[2].asString();
        data = this->Mod_Item_Base_player_get_super_gift(id);
        if (data[0].asInt() != Mod_Item_Base::ACTION_SUCCESS)
            common::log(boost::str(boost::format("【礼包】领取失败，result[%1%]") % data[0]), iEdit);
        else
            common::log(boost::str(boost::format("【礼包】%1%") % common::utf2gbk(message)), iEdit);
    }
}

//============================================================================
// R172 各种礼包
// {module:2, action:6, request:[],
// response:[[Utils.IntUtil, Utils.IntUtil, Utils.StringUtil, Utils.StringUtil, [Utils.UByteUtil, Utils.IntUtil, Utils.IntUtil]]]};
// ItemData.as 1966:
//     this._allGiftList = param1[0];
// ItemData.as 1792:
//     _loc_2.giftId = param1[0];
//     _loc_2.type = param1[1];
//     _loc_2.name = GiftType.getCommonGiftName(_loc_2.type);
//     _loc_2.message = param1[2];
//     _loc_2.addMsg = param1[3];
// Example
//     [ [ [ 6265143, 136, "\u83b7\u5f97\u5e2e\u6d3e\u5165\u4fb5\u7ec8\u7ed3\u5956\u52b1\u9752\u9f99\u4ee4x12\uff01", "", [ [ 19, 12, 1007 ] ] ], [ 6265168, 70, "", "", [ [ 12, 2450, 0 ] ] ], [ 6265026, 90, "\u60a8\u5728\u4ed9\u754c\u7ade\u6280\u573a\u4e2d[\u79ef\u52061320],\u5956\u52b1[\u8363\u8a8962].", "", [ [ 17, 62, 0 ] ] ] ] ]
//============================================================================
Json::Value sxd_client::Mod_Item_Base_get_player_gift_all_info() {
    Json::Value data;
    return this->send_and_receive(data, 2, 6);
}

//============================================================================
// R172 领取
// {module:2, action:29,
// request:[Utils.IntUtil],
// Example
//     [ 6265143 ]
// response:[Utils.UByteUtil, [Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.ShortUtil, Utils.ShortUtil]]};
// ItemData.as 1105:
//     this.result = param1[0];
// Example
//     [ 20, null ]
//============================================================================
Json::Value sxd_client::Mod_Item_Base_player_get_super_gift(int id) {
    Json::Value data;
    data.append(id);
    return this->send_and_receive(data, 2, 29);
}

//============================================================================
// R172 阵营战礼包
// {module:42, action:18, request:[],
// response:[Utils.UByteUtil, Utils.IntUtil, Utils.IntUtil]};
// HeroesWarData.as 233:
//     oObject.list(this._objGetEndGift, _loc_1, ["has_gift", "fame", "coin"]);
// Example
//    [ 27, 1760, 880000 ]
//    [ 28, 0, 0 ]
//============================================================================
void sxd_client::Mod_HeroesWar_Base_get_end_gift() {
    Json::Value data;
    this->send_frame(data, 42, 18);
}

//============================================================================
// R172 自定义挑战礼包
// {module:136, action:20, request:[], response:[Utils.UByteUtil, Utils.IntUtil, Utils.ShortUtil, Utils.ShortUtil]};
// Example
//     [ 10, 0, 0, 0 ]
//     [ 13, 0, 0, 0 ]
//============================================================================
void sxd_client::Mod_StChallenge_Base_get_end_li_bao() {
    Json::Value data;
    this->send_frame(data, 136, 20);
}

//============================================================================
// R172 极限挑战宝箱
// {module:169, action:5, request:[], response:[Utils.UByteUtil]};
// Example
//     [ 8 ]
//     [ 6 ]
//     no response
//============================================================================
void sxd_client::Mod_UnlimitChallenge_Base_get_end_award() {
    Json::Value data;
    this->send_frame(data, 169, 5);
}


#include <boost/algorithm/string/join.hpp>
#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_Laba_Base {
public:
    static const int SUCCESS = 2;
};

void sxd_client::five_elements_laba() {
    std::string scraps[] = { "", "初级金灵碎片", "初级木灵碎片", "初级水灵碎片", "初级火灵碎片", "初级土灵碎片", "中级金灵碎片", "中级木灵碎片", "中级水灵碎片", "中级火灵碎片", "中级土灵碎片" };
    Json::Value data = this->Mod_Laba_Base_laba_info();
    int free_count = data[0].asInt();
    int coin_count = data[1].asInt();
    while (free_count + coin_count > 0) {
        data = this->Mod_Laba_Base_draw();
        if (data[0].asInt() != Mod_Laba_Base::SUCCESS) {
            common::log(boost::str(boost::format("【五行天仪】转一次失败，result[%1%]") % data[0]));
            return;
        }
        std::vector<std::string> items;
        std::transform(data[1][0][0].begin(), data[1][0][0].end(), std::back_inserter(items), [&scraps](const Json::Value& x) {
            return boost::str(boost::format("[%1%]") % scraps[x[0].asInt()]);
        });
        common::log(boost::str(boost::format("【五行天仪】转一次，获得：%1%") % boost::algorithm::join(items, "，")));
        free_count = data[3].asInt();
        coin_count = data[4].asInt();
    }
}

//============================================================================
// R177 五行天仪面板
// "module":172, "action":0, "request":[], "response":[Utils.ByteUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.IntUtil,[Utils.ByteUtil,Utils.ByteUtil],Utils.IntUtil]
// LabaData.as 36:
//     _loc1_.free_count = this._aryLabaInfo[0];
//     _loc1_.coin_count = this._aryLabaInfo[1];
//     _loc1_.ingot_count = this._aryLabaInfo[2];
//     _loc1_.ten_ingot_count = this._aryLabaInfo[3];
//     _loc1_.score = this._aryLabaInfo[4];
// Example
//     [ 1, 9, 0, 0, 11350, [ [ 8, 1 ], [ 3, 0 ], [ 6, 3 ], [ 4, 0 ], [ 9, 3 ], [ 7, 0 ], [ 5, 1 ], [ 1, 2 ], [ 10, 3 ], [ 2, 0 ] ], 500 ]
//============================================================================
Json::Value sxd_client::Mod_Laba_Base_laba_info() {
    Json::Value data;
    return this->send_and_receive(data, 172, 0);
}

//============================================================================
// R177 转一次
// "module":172, "action":1, "request":[Utils.UByteUtil], "response":[Utils.UByteUtil,[[Utils.ByteUtil]],Utils.IntUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.ByteUtil,[Utils.ByteUtil,Utils.ByteUtil],Utils.IntUtil]
// LabaData.as 66:
//     _loc1_.result = this._aryDraw[0];
//     for each(_loc5_ in this._aryDraw[1]) {
//        _loc3_ = [];
//        for each(_loc6_ in _loc5_[0])
//           _loc3_.push(_loc6_[0]);
//        _loc2_.push(_loc3_);
//     }
//     _loc1_.score = this._aryDraw[2];
//     _loc1_.free_count = this._aryDraw[3];
//     _loc1_.coin_count = this._aryDraw[4];
//     _loc1_.ingot_count = this._aryDraw[5];
//     _loc1_.ten_ingot_count = this._aryDraw[6];
//     for each(_loc7_ in this._aryDraw[7]) {
//        _loc8_ = {};
//        _loc8_.scrap_id = _loc7_[0];
//        _loc8_.number = _loc7_[1];
//        _loc4_.push(_loc8_);
//     }
//     _loc1_.proficiency = this._aryDraw[8]; // 熟练度
// Example
//     [ 2, [ [ [ [ 7 ], [ 1 ], [ 2 ] ] ] ], 11360, 0, 9, 0, 0, [ [ 8, 1 ], [ 3, 0 ], [ 6, 3 ], [ 4, 0 ], [ 9, 3 ], [ 7, 0 ], [ 5, 1 ], [ 1, 2 ], [ 10, 3 ], [ 2, 0 ] ], 500 ]
//============================================================================
Json::Value sxd_client::Mod_Laba_Base_draw() {
    Json::Value data;
    data.append(0);
    return this->send_and_receive(data, 172, 1);
}


#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_NineRegions_Base {
public:
    static const int SUCCESS = 5;
    static const int NEW_START = 22;
    static const int CONTINUE = 23;
    static const int TONG = 24;
    static const int YIN = 25;
    static const int JIN = 26;
    static const int BUFF_0 = 33;
};

//============================================================================
// R179 九空无界
//============================================================================
void sxd_client::nine_regions() {
    auto data = this->Mod_NineRegions_Base_open_panel();
    int cur_jie = data[1].asInt();
    for (;;) {
        data = this->Mod_NineRegions_Base_get_calabash_info(cur_jie);
        int state = data[0].asInt();
        int value = data[3].asInt();
        int avaliable_times = data[4].asInt();

        int calabash;
        switch (state) {
        case Mod_NineRegions_Base::NEW_START:
            // NEW_START
            if (avaliable_times == 0)
                return;
            data = this->Mod_NineRegions_Base_call(cur_jie);
            if (data[0].asInt() != Mod_NineRegions_Base::SUCCESS) {
                common::log(boost::str(boost::format("【九空无界】召唤仙葫失败，result[%1%]") % data[0]), iEdit);
                return;
            }
            calabash = data[1].asInt();
            switch (calabash) {
            case Mod_NineRegions_Base::TONG:
                // 铜仙葫
                common::log(boost::str(boost::format("【九空无界】召唤仙葫，获得 [铜仙葫]，[灵气×%1%]") % data[3]), iEdit);
                break;
            case Mod_NineRegions_Base::YIN:
                // 银仙葫
                common::log(boost::str(boost::format("【九空无界】召唤仙葫，获得 [银仙葫]，[灵气×%1%]") % data[3]), iEdit);
                break;
            case Mod_NineRegions_Base::JIN:
                // 金仙葫
                common::log(boost::str(boost::format("【九空无界】召唤仙葫，获得 [金仙葫]，[灵气×%1%]") % data[3]), iEdit);
                break;
            default:
                // error
                common::log(boost::str(boost::format("【九空无界】未知仙葫品质，[calabash=%1%]") % calabash), iEdit);
                return;
            }
            break;
        case Mod_NineRegions_Base::CONTINUE:
            // CONTINUE
            if (value >= 17) {
                // 收取
                data = this->Mod_NineRegions_Base_collect(cur_jie);
                if (data[0].asInt() != Mod_NineRegions_Base::SUCCESS) {
                    common::log(boost::str(boost::format("【九空无界】收取失败，result[%1%]") % data[0]), iEdit);
                    return;
                }
                std::vector<std::string> items;
                std::transform(data[1].begin(), data[1].end(), std::back_inserter(items), [this](const Json::Value& x) {
                    return boost::str(boost::format("[%1%×%2%]") % db.get_code(version, "Item", x[0].asInt())["text"] % x[1]);
                });
                common::log(boost::str(boost::format("【九空无界】收取，获得 %1%") % boost::algorithm::join(items, "，")), iEdit);
            } else {
                // 聚气
                data = this->Mod_NineRegions_Base_gathering(cur_jie);
                if (data[0].asInt() != Mod_NineRegions_Base::SUCCESS) {
                    common::log(boost::str(boost::format("【九空无界】聚气失败，result[%1%]") % data[0]), iEdit);
                    return;
                }
                common::log(boost::str(boost::format("【九空无界】聚气，当前 [灵气×%1%]") % data[2]), iEdit);
            }
            break;
        default:
            // error
            common::log(boost::str(boost::format("【九空无界】未知状态，[state=%1%]") % state), iEdit);
            return;
        }
    }
}

//============================================================================
// R179 面板
// "module":58,"action":0,"request":[],"response":[Utils.ByteUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil]
// NineRegionsData.as 88:
//     this.nineRegionsInfo.isFrist = param1[0] == 1;
//     this.nineRegionsInfo.curJie = param1[1];
//     this.nineRegionsInfo.curLevel = param1[2];
//     this.hiddenLevel = param1[3];
// Example
//
//============================================================================
Json::Value sxd_client::Mod_NineRegions_Base_open_panel() {
    Json::Value data;
    return this->send_and_receive(data, 58, 0);
}

//============================================================================
// R179 仙葫数量
// "module":58,"action":29,"request":[],"response":[Utils.ByteUtil,Utils.ByteUtil]
// NineRegionsData.as 502:
//     this._calabashInfo.usable_count = param1[0];
//     this._calabashInfo.total_count = param1[1];
// Example
//
//============================================================================
Json::Value sxd_client::Mod_NineRegions_Base_get_calabash_count() {
    Json::Value data;
    return this->send_and_receive(data, 58, 29);
}

//============================================================================
// R179 聚灵面板
// "module":58,"action":25,"request":[Utils.ShortUtil],"response":[Utils.UByteUtil,Utils.UByteUtil,[Utils.IntUtil,Utils.ShortUtil],Utils.IntUtil,Utils.ByteUtil]
// NineRegionsData.as 410:
//     this._calabashInfo.state = param1[0];
//     this._calabashInfo.calabash = param1[1];
//     for each(_loc2_ in param1[2])
//         _loc3_.item_id = _loc2_[0];
//         _loc3_.item_count = _loc2_[1];
//     this._calabashInfo.value = param1[3];
//     this._calabashInfo.avaliable_times = param1[4];
// Example
//
//============================================================================
Json::Value sxd_client::Mod_NineRegions_Base_get_calabash_info(int cur_jie) {
    Json::Value data;
    data.append(cur_jie);
    return this->send_and_receive(data, 58, 25);
}

//============================================================================
// R179 召唤仙葫
// "module":58,"action":26,"request":[Utils.ShortUtil],"response":[Utils.UByteUtil,Utils.UByteUtil,[Utils.IntUtil,Utils.ShortUtil],Utils.ByteUtil]
// NineRegionsData.as 436:
//     this._calabashInfo.callResult = param1[0];
//     this._calabashInfo.calabash = param1[1];
//     for each(_loc2_ in param1[2])
//         _loc3_.item_id = _loc2_[0];
//         _loc3_.item_count = _loc2_[1];
//     this._calabashInfo.value = param1[3];
// Example
//
//============================================================================
Json::Value sxd_client::Mod_NineRegions_Base_call(int cur_jie) {
    Json::Value data;
    data.append(cur_jie);
    return this->send_and_receive(data, 58, 26);
}

//============================================================================
// R179 聚气
// "module":58,"action":27,"request":[Utils.ShortUtil,Utils.UByteUtil],"response":[Utils.UByteUtil,[Utils.IntUtil,Utils.ShortUtil],Utils.ByteUtil]
// NineRegionsData.as 460:
//     this._calabashInfo.gatherResult = param1[0];
//     for each(_loc2_ in param1[1])
//         _loc3_.item_id = _loc2_[0];
//         _loc3_.item_count = _loc2_[1];
//     this._calabashInfo.value = param1[2];
// Example
//
//============================================================================
Json::Value sxd_client::Mod_NineRegions_Base_gathering(int cur_jie) {
    Json::Value data;
    data.append(cur_jie);
    data.append(Mod_NineRegions_Base::BUFF_0);
    return this->send_and_receive(data, 58, 27);
}

//============================================================================
// R179 收取
// "module":58,"action":28,"request":[Utils.ShortUtil],"response":[Utils.UByteUtil,[Utils.IntUtil,Utils.ShortUtil]]
// NineRegionsData.as 483:
//     this._calabashInfo.collectResult = param1[0];
//     for each(_loc2_ in param1[1])
//         _loc3_.item_id = _loc2_[0];
//         _loc3_.item_count = _loc2_[1];
// Example
//
//============================================================================
Json::Value sxd_client::Mod_NineRegions_Base_collect(int cur_jie) {
    Json::Value data;
    data.append(cur_jie);
    return this->send_and_receive(data, 58, 28);
}

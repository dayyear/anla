#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_Fate_Base {
public:
    static const int SUCCESS = 0;
    static const int FULL_TEMP_BAG = 12;
};

static std::string fate_names[] = { "", "厄命", "逢赌必输", "十衰九败", "穷鬼缠身", "衰神附体", "血光灾厄", "死亡连锁", "不死凶命", "千年未竟", "斩铁", "霸者横栏", "吉星", "千惊万喜", "信牢", "一元复始", "无惧", "天地无用", "热血", "叱咤风云", "飞仙", "兽腾", "贪狼", "逢龙遇虎", "岩打", "战狂", "破军", "轩辕剑体", "虎贲", "辉煌", "夜叉", "大破坏神", "散华", "不死鸟", "万寿无疆", "太初", "快刀乱麻", "百步穿杨", "破碎虚空", "斗转星移", "诛仙", "真逢龙遇虎", "真轩辕剑体", "真大破坏神", "命格碎片", "真霸者横栏", "真千惊万喜", "真天地无用", "天命圣", "天命破", "天命斗", "天命诛", "天衣无缝", "", "", "千里火", "天命闪", "千军万马", "天命盾", "绝对伤害", "石破天惊", "神工鬼力", "天命斩", "新命格碎片", "绝世无双", "顽石契约", "金城汤池", "天命佑", "绝对减伤", "天道绝", "命宫碎片", "命宫", "浩气长存", "天道悟", "力敌千钧", "无懈可击", "天道源" };
static std::string npc_names[] = { "", "周一仙", "小仙女", "左慈", "张道陵", "姜子牙" };

//============================================================================
// R180 批量猎命
//============================================================================
void sxd_client::batch_fate(int count) {
    for (int i = 0; i < count; i++) {
        auto data = this->Mod_Fate_Base_get_fate_npc();
        auto npcs = data[1];
        std::vector<Json::Value> npcs_valid;
        std::copy_if(npcs.begin(), npcs.end(), std::back_inserter(npcs_valid), [](const Json::Value& x) {return x[1].asInt()==1;});
        auto npc_opt = *std::max_element(npcs_valid.begin(), npcs_valid.end(), [](const Json::Value& x1, const Json::Value& x2) {return x1[0].asInt()<x2[0].asInt();});

        data = this->Mod_Fate_Base_appoint_fate_npc(npc_opt[0].asInt());
        auto msg = data[0].asInt();
        if (msg == Mod_Fate_Base::FULL_TEMP_BAG) {
            common::log("【猎命】猎命空间已满");

            // 命格空间
            data = this->Mod_Fate_Base_get_temp_fate();
            auto temp_fates = data[0];

            // 一键卖出
            std::vector<Json::Value> temp_fates_bad;
            std::copy_if(temp_fates.begin(), temp_fates.end(), std::back_inserter(temp_fates_bad), [](const Json::Value& x) {return x[1].asInt()<=9;});
            std::vector<int> temp_fate_ids_bad;
            std::transform(temp_fates_bad.begin(), temp_fates_bad.end(), std::back_inserter(temp_fate_ids_bad), [](const Json::Value& x) {return x[0].asInt();});
            if (temp_fate_ids_bad.size()) {
                data = this->Mod_Fate_Base_sale_temp_fate(temp_fate_ids_bad);
                common::log("【猎命】一键卖出");
            }

            // 一键拾取
            this->Mod_Fate_Base_merge_all_in_bag();
            std::vector<Json::Value> temp_fates_good;
            std::copy_if(temp_fates.begin(), temp_fates.end(), std::back_inserter(temp_fates_good), [](const Json::Value& x) {return x[1].asInt()>9;});
            std::vector<int> temp_fate_ids_good;
            std::transform(temp_fates_good.begin(), temp_fates_good.end(), std::back_inserter(temp_fate_ids_good), [](const Json::Value& x) {return x[0].asInt();});
            if (temp_fate_ids_good.size()) {
                data = this->Mod_Fate_Base_pickup_fate(temp_fate_ids_good);
                common::log("【猎命】一键拾取");
            }
            this->Mod_Fate_Base_merge_all_in_bag();
            continue;
        } else if (msg != Mod_Fate_Base::SUCCESS) {
            common::log(boost::str(boost::format("【猎命】猎命失败，msg[%1%]") % data[0]));
            return;
        }
        common::log(boost::str(boost::format("【猎命】猎命 [%1%]，获得 [%2%]，遇见 [%3%]") % npc_names[npc_opt[0].asInt()] % fate_names[data[1].asInt()] % npc_names[data[2].asInt()]));
    }
}

//============================================================================
// R179 猎命
//============================================================================
void sxd_client::fate() {
    this->Mod_Fate_Base_merge_all_in_bag();

    // 命格空间
    auto data = this->Mod_Fate_Base_get_temp_fate();
    auto temp_fates = data[0];

    // 一键卖出
    std::vector<Json::Value> temp_fates_bad;
    std::copy_if(temp_fates.begin(), temp_fates.end(), std::back_inserter(temp_fates_bad), [](const Json::Value& x) {return x[1].asInt()<=9;});
    std::vector<int> temp_fate_ids_bad;
    std::transform(temp_fates_bad.begin(), temp_fates_bad.end(), std::back_inserter(temp_fate_ids_bad), [](const Json::Value& x) {return x[0].asInt();});
    if (temp_fate_ids_bad.size()) {
        data = this->Mod_Fate_Base_sale_temp_fate(temp_fate_ids_bad);
        if (data[0].asInt() != Mod_Fate_Base::SUCCESS) {
            common::log(boost::str(boost::format("【猎命】一键卖出失败，msg[%1%]") % data[0]));
            return;
        }
        common::log("【猎命】一键卖出");
    }

    // 一键拾取
    std::vector<Json::Value> temp_fates_good;
    std::copy_if(temp_fates.begin(), temp_fates.end(), std::back_inserter(temp_fates_good), [](const Json::Value& x) {return x[1].asInt()>9;});
    std::vector<int> temp_fate_ids_good;
    std::transform(temp_fates_good.begin(), temp_fates_good.end(), std::back_inserter(temp_fate_ids_good), [](const Json::Value& x) {return x[0].asInt();});
    if (temp_fate_ids_good.size()) {
        data = this->Mod_Fate_Base_pickup_fate(temp_fate_ids_good);
        if (data[0].asInt() != Mod_Fate_Base::SUCCESS) {
            common::log(boost::str(boost::format("【猎命】一键拾取失败，msg[%1%]") % data[0]));
            return;
        }
        common::log("【猎命】一键拾取");
    }

    this->Mod_Fate_Base_merge_all_in_bag();

    for (;;) {
        data = this->Mod_Fate_Base_get_fate_npc();
        int free_appoint_times = data[0].asInt();
        if (free_appoint_times == 0)
            break;
        auto npcs = data[1];
        std::vector<Json::Value> npcs_valid;
        std::copy_if(npcs.begin(), npcs.end(), std::back_inserter(npcs_valid), [](const Json::Value& x) {return x[1].asInt()==1;});
        auto npc_opt = *std::max_element(npcs_valid.begin(), npcs_valid.end(), [](const Json::Value& x1, const Json::Value& x2) {return x1[0].asInt()<x2[0].asInt();});

        data = this->Mod_Fate_Base_appoint_fate_npc(npc_opt[0].asInt());
        if (data[0].asInt() != Mod_Fate_Base::SUCCESS) {
            common::log(boost::str(boost::format("【猎命】猎命失败，msg[%1%]") % data[0]));
            return;
        }
        common::log(boost::str(boost::format("【猎命】猎命 [%1%]，获得 [%2%]，遇见 [%3%]") % npc_names[npc_opt[0].asInt()] % fate_names[data[1].asInt()] % npc_names[data[2].asInt()]));
    }
}

//============================================================================
// R179 一键合成
// "module":22,"action":17,"request":[],"response":[]
//============================================================================
void sxd_client::Mod_Fate_Base_merge_all_in_bag() {
    Json::Value data;
    this->send_frame(data, 22, 17);
}

//============================================================================
// R179 NPC
// "module":22,"action":0,"request":[],"response":[Utils.ByteUtil,[Utils.ByteUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.ShortUtil],Utils.IntUtil]
// FateController.as 79:
//     _loc2_.freeAppointTimes = _loc1_[0];
//     var _loc3_:Array = this.renderFateNpc(_loc1_[1],_loc1_[2]);
//         _loc8_.fateNpcId = param1[_loc7_][0];
//         _loc8_.status = param1[_loc7_][1];
//         _loc8_.canBuyTimer = param1[_loc7_][2];
//         _loc8_.maxBuyTimes = param1[_loc7_][3];
//         _loc8_.buyPrice = param1[_loc7_][4];
// Example
//
//============================================================================
Json::Value sxd_client::Mod_Fate_Base_get_fate_npc() {
    Json::Value data;
    return this->send_and_receive(data, 22, 0);
}

//============================================================================
// R179 猎命
// "module":22,"action":9,"request":[Utils.ByteUtil],"response":[Utils.UByteUtil,Utils.ShortUtil,Utils.ByteUtil,Utils.LongUtil]
// FateController.as 506:
//     _loc2_.msg = _loc1_[0];
//     _loc2_.fateId = _loc1_[1];
//     _loc2_.fateNpcId = _loc1_[2];
//     _loc2_.tempFateId = _loc1_[3];
// Example
//
//============================================================================
Json::Value sxd_client::Mod_Fate_Base_appoint_fate_npc(int npc_id) {
    Json::Value data;
    data.append(npc_id);
    return this->send_and_receive(data, 22, 9);
}

//============================================================================
// R179 命格空间
// "module":22,"action":1,"request":[],"response":[[Utils.LongUtil,Utils.ShortUtil]]
// FateController.as 136:
//     return this.renderTempFate(_loc1_[0]);
//         _loc5_.tempFateId = param1[_loc4_][0];
//         _loc5_.fateId = param1[_loc4_][1];
// Example
//
//============================================================================
Json::Value sxd_client::Mod_Fate_Base_get_temp_fate() {
    Json::Value data;
    return this->send_and_receive(data, 22, 1);
}

//============================================================================
// R179 一键拾取
// "module":22,"action":2,"request":[[Utils.LongUtil]],"response":[Utils.UByteUtil,[Utils.LongUtil]]
// FateController.as 171:
//     _loc2_.msg = _loc1_[0];
//     _loc2_.gridList = _loc1_[1];
// Example
//
//============================================================================
Json::Value sxd_client::Mod_Fate_Base_pickup_fate(const std::vector<int> &temp_fate_ids) {
    Json::Value data, data1;
    for (int id : temp_fate_ids) {
        Json::Value data2;
        data2.append(id);
        data1.append(data2);
    }
    data.append(data1);
    return this->send_and_receive(data, 22, 2);
}

//============================================================================
// R179 一键卖出
// "module":22,"action":3,"request":[[Utils.LongUtil]],"response":[Utils.UByteUtil]
// FateController.as 180:
//     _loc2_.msg = _loc1_[0];
// Example
//
//============================================================================
Json::Value sxd_client::Mod_Fate_Base_sale_temp_fate(const std::vector<int> &temp_fate_ids) {
    Json::Value data;
    Json::Value data1;
    for (int id : temp_fate_ids) {
        Json::Value data2;
        data2.append(id);
        data1.append(data2);
    }
    data.append(data1);
    return this->send_and_receive(data, 22, 3);
}

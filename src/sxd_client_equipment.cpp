#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_Equipment_Base {
public:
    static const int TURE = 26;
    static const int FALS = 27;
    static const int ACTION_SUCCESS = 28;
    static const int LEVEL_LIMIT = 33;
    static const int INSUFFICIENT_COINS = 34;
    static const int NO_ENOUGH_MATERIALS = 36;
    static const int LEVEL_2_FUNCTION_NO_OPEN = 37;
};

void sxd_client::equipment() {
    // 200:"太初10级",201:"混沌1级",
    auto data = this->Mod_Role_Base_get_role_list(player_id);
    auto roles = data[14];
    data = this->Mod_Item_Base_get_all_player_item_infos();
    // 有主的才可能是装备
    std::vector<Json::Value> equips;
    std::copy_if(data[0].begin(), data[0].end(), std::back_inserter(equips), [](const Json::Value& x) {return x[16].asInt();});
    for (const auto& equip : equips) {
        int player_item_id = equip[0].asInt();
        int player_role_id = equip[16].asInt();
        // 排除掉客栈里伙伴的装备
        if (std::find_if(roles.begin(), roles.end(), [player_role_id](const Json::Value& role) {return role[2].asInt() == player_role_id;}) == roles.end())
            continue;

        for (;;) {
            // 铜钱不够一亿则不强化
            data = this->Mod_Player_Base_get_player_info();
            int64_t coins = data[3].asInt64();
            if (coins < 100000000)
                return;

            data = this->Mod_Equipment_Base_upgrade_equip(player_item_id, -1, Mod_Equipment_Base::FALS);
            if (data[0].asInt() == Mod_Equipment_Base::LEVEL_LIMIT || data[0].asInt() == Mod_Equipment_Base::INSUFFICIENT_COINS || data[0].asInt() == Mod_Equipment_Base::NO_ENOUGH_MATERIALS || data[0].asInt() == Mod_Equipment_Base::LEVEL_2_FUNCTION_NO_OPEN)
                break;
            else if (data[0].asInt() != Mod_Equipment_Base::ACTION_SUCCESS) {
                common::log(boost::str(boost::format("【强化】[%1%] 失败，result[%2%]") % db.get_code(version, "Item", equip[1].asInt())["text"] % data[0]), iEdit);
                break;
            }
            common::log(boost::str(boost::format("【强化】[%1%] 至 [%2%] 级") % db.get_code(version, "Item", equip[1].asInt())["text"] % data[1][0][3]), iEdit);
        }
    }
}

//============================================================================
// R196 强化
// "module":77,"action":7,"request":[Utils.IntUtil,Utils.IntUtil,Utils.UByteUtil],"response":[Utils.UByteUtil,[Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil]]
// SuperUpgradeView.as:
//     this._data.call(Mod_Equipment_Base.upgrade_equip,callBack,[intPlayerItemId,intSelectGemId,!!intUseCoinUpgrade?Mod_Equipment_Base.TRUE:Mod_Equipment_Base.FALSE]);
// UpgradeData.as:
//     oObject.list(this._aryUpgradeEquip,_loc1_,["result"]);
//     oObject.list(this._aryUpgradeEquip[1][0],_loc1_,["player_item_id","item_id","player_role_id","upgrade_level","upgrade_price","gold_oil_id"]);
// Example
//     [ 46783924, -1, 27 ] -> [ **28**, [ [ 46783924, 1038, 808172, **59**, 648000, 0 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_Equipment_Base_upgrade_equip(int player_item_id, int select_gem_id, int use_coin_upgrade) {
    Json::Value data;
    data.append(player_item_id);
    data.append(select_gem_id);
    data.append(use_coin_upgrade);
    return this->send_and_receive(data, 77, 7);
}

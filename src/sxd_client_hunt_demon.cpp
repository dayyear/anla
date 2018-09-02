#include <boost/algorithm/string/join.hpp>
#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_HuntDemon_Base {
public:
    static const int SUCCESS = 5;
};

void sxd_client::hunt_demon() {
    Json::Value data = this->Mod_HuntDemon_Base_open_hunt_demon();
    int free_times = data[0].asInt();
    int coin_times = data[1].asInt();
    while (free_times) {
        data = this->Mod_HuntDemon_Base_hunt_demon(0);
        free_times = data[3].asInt();
        if (data[0].asInt() != Mod_HuntDemon_Base::SUCCESS) {
            common::log(boost::str(boost::format("¡¾ÁÔÑý¡¿Ãâ·ÑÁÔÑýÊ§°Ü£¬result[%1%]") % data[0]), iEdit);
            return;
        }
        std::vector<std::string> items;
        std::transform(data[1].begin(), data[1].end(), std::back_inserter(items), [this](const Json::Value& x) {
            if (x[0].asInt()==1)
                return boost::str(boost::format("[%1%¡Á%2%]") % db.get_code(version, "Item", x[1].asInt())["text"] % x[2]);
            else if (x[0].asInt()==2)
                return boost::str(boost::format("[ÔÄÀú¡Á%1%]") % x[2]);
            else if (x[0].asInt()==0)
                return boost::str(boost::format("[Í­Ç®¡Á%1%]") % x[2]);
            else
                return boost::str(boost::format("[Î´ÖªÎïÆ·¡Á%1%]") % x[2]);
        });
        common::log(boost::str(boost::format("¡¾ÁÔÑý¡¿Ãâ·ÑÁÔÑý£¬»ñµÃ£º%1%") % boost::algorithm::join(items, "£¬")), iEdit);
    }
    while (coin_times) {
        data = this->Mod_HuntDemon_Base_hunt_demon(1);
        coin_times = data[4].asInt();
        if (data[0].asInt() != Mod_HuntDemon_Base::SUCCESS) {
            common::log(boost::str(boost::format("¡¾ÁÔÑý¡¿Í­Ç®ÁÔÑýÊ§°Ü£¬result[%1%]") % data[0]), iEdit);
            return;
        }
        std::vector<std::string> items;
        std::transform(data[1].begin(), data[1].end(), std::back_inserter(items), [this](const Json::Value& x) {
            if (x[0].asInt()==1)
                return boost::str(boost::format("[%1%¡Á%2%]") % db.get_code(version, "Item", x[1].asInt())["text"] % x[2]);
            else if (x[0].asInt()==2)
                return boost::str(boost::format("[ÔÄÀú¡Á%1%]") % x[2]);
            else if (x[0].asInt()==0)
                return boost::str(boost::format("[Í­Ç®¡Á%1%]") % x[2]);
            else
                return boost::str(boost::format("[Î´ÖªÎïÆ·¡Á%1%]") % x[2]);
        });
        common::log(boost::str(boost::format("¡¾ÁÔÑý¡¿Í­Ç®ÁÔÑý£¬»ñµÃ£º%1%") % boost::algorithm::join(items, "£¬")), iEdit);
    }
}

//============================================================================
// R172 ÁÔÑý
// {module:105, action:1, request:[Utils.ByteUtil],
// response:[Utils.ByteUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ShortUtil, [Utils.ByteUtil, Utils.IntUtil]]};
// HuntDemonData.as 47:
//     this._free_times = uint(param1[0]);
//     this._coin_times = uint(param1[1]);
//     this._ingot_times = uint(param1[2]);
//     this._need_ingot = uint(param1[3]);
// Example
//     [ 3, 10, 5, 50, null ]
//============================================================================
Json::Value sxd_client::Mod_HuntDemon_Base_open_hunt_demon() {
    Json::Value data;
    data.append(1);
    return this->send_and_receive(data, 105, 1);
}

//============================================================================
// R172 ÁÔÑý
// {module:105, action:2, request:[Utils.ByteUtil, Utils.ByteUtil],
//     0: Ãâ·Ñ, 1: Í­Ç®, 2: Ôª±¦
// response:[Utils.UByteUtil, [Utils.UByteUtil, Utils.IntUtil, Utils.IntUtil], Utils.ByteUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ShortUtil, [Utils.ByteUtil, Utils.IntUtil]]};
//
//     this._hunt_demon_result = param1[0];
//     this._hunt_demon_is_lucky = param1[2];
//     this._free_times = param1[3];
//     this._coin_times = param1[4];
//     this._ingot_times = param1[5];
//     this._need_ingot = param1[6];
//     for each (_loc_3 in param1[1])
//         oObject.list(_loc_3, _loc_2, ["award_type", "item_id", "number"]);
// Example
//
//============================================================================
Json::Value sxd_client::Mod_HuntDemon_Base_hunt_demon(int type) {
    Json::Value data;
    data.append(type);
    data.append(0);
    return this->send_and_receive(data, 105, 2);
}


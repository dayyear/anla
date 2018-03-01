#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_CourtyardPet_Base {
public:
    static const int NORMAL = 0;
    static const int INGOT = 1;
    static const int SUCCESS = 2;
};

void sxd_client::courtyard_pet() {
    Json::Value data = this->Mod_CourtyardPet_Base_get_player_info();
    int count = data[0].asInt();
    while (count) {
        data = this->Mod_CourtyardPet_Base_call(Mod_CourtyardPet_Base::NORMAL);
        if (data[0].asInt() != Mod_CourtyardPet_Base::SUCCESS) {
            common::log(boost::str(boost::format("¡¾³èÎï¡¿Áé·ûÕÙ»½Ê§°Ü£¬result[%1%]") % data[0]));
            return;
        }
        common::log("¡¾³èÎï¡¿Áé·ûÕÙ»½");
        count--;
    }
    common::log("¡¾³èÎï¡¿Áé·ûÒÑÓÃÍê");
}

//============================================================================
// R171
// ³èÎï·¿
// {module:216, action:0, request:[],
// response:[Utils.ShortUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.ByteUtil]}
// CourtyardPetData.as 48:
//     oObject.list(param1, _loc_2, ["normal_call_count", "normal_total_count", "ingot_call_count", "ingot_total_count", "ingot_cost", "lucky_count", "total_count"]);
// Example
//     [ 10, 10, 20, 20, 50, 2, 20 ]
//     [ 9, 10, 20, 20, 50, 2, 20 ]
//============================================================================
Json::Value sxd_client::Mod_CourtyardPet_Base_get_player_info() {
    Json::Value data;
    return this->send_and_receive(data, 216, 0);
}

//============================================================================
// R171
// Áé·ûÕÙ»½
// {module:216, action:1,
// request:[Utils.UByteUtil],
// Example
//     [ 0 ]
// response:[Utils.UByteUtil, Utils.ShortUtil, Utils.UByteUtil]};
// CourtyardPetData.as 61:
//     oObject.list(param1, this._callResult, ["result", "courtyard_pet_id", "new_pet_flag"]);
// Example
//     [ 2, 10, 7 ]
//============================================================================
Json::Value sxd_client::Mod_CourtyardPet_Base_call(int type) {
    Json::Value data;
    data.append(type);
    return this->send_and_receive(data, 216, 1);
}

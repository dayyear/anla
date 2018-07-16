#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_Fish_Base {
public:
    static const int SUCCESS = 0;
};

//============================================================================
// R179 µöÓã
//============================================================================
void sxd_client::fish() {
    for (;;) {
        auto data = this->Mod_Fish_Base_get_player_fishhook();
        int fishhook_number = data[0].asInt();
        if (fishhook_number == 0)
            break;

        data = this->Mod_Fish_Base_do_fishing();
        if (data[0].asInt() != Mod_Fish_Base::SUCCESS) {
            common::log(boost::str(boost::format("¡¾µöÓã¡¿µöÓãÊ§°Ü£¬result[%1%]") % data[0]));
            return;
        }
        common::log(boost::str(boost::format("¡¾µöÓã¡¿µöÓã»ñµÃ£º[%1%¡Á%2%]") % db.get_code(version, "Item", data[1].asInt())["text"] % data[2]));
    }
}

//============================================================================
// R179 Ãæ°å
// "module":217,"action":0,"request":[],"response":[Utils.IntUtil]
// FishData.as 27:
//     oObject.list(this._aryGetPlayerFishhook,_loc1_,["fishhook_number"]);
// Example
//     [ 4 ]
//============================================================================
Json::Value sxd_client::Mod_Fish_Base_get_player_fishhook() {
    Json::Value data;
    return this->send_and_receive(data, 217, 0);
}

//============================================================================
// R179 µöÓã
// "module":217,"action":1,"request":[],"response":[Utils.UByteUtil,Utils.IntUtil,Utils.IntUtil]
// FishData.as 35:
//     oObject.list(this._aryDoFishing,_loc1_,["result","item_id","number"]);
// Example
//     [ 0, 1747, 400000 ]
//============================================================================
Json::Value sxd_client::Mod_Fish_Base_do_fishing() {
    Json::Value data;
    return this->send_and_receive(data, 217, 1);
}

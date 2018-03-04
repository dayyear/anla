#include <thread>
#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_SaintAreaTown_Base {
public:
    static const int WORLD = 1;
    static const int ST_TOWN = 4;
};

//============================================================================
// R171 聊天
// {module:6, action:0,
// request:[Utils.UByteUtil, Utils.StringUtil, Utils.StringUtil, Utils.StringUtil],
// ChatView.as 126:
//     _data.call(Mod_Chat_Base.chat_with_players, callBack, [data.messageType, data.message, data.eipNum, data.eipIndex]);
// Example
//     世界聊天：[ 1, "\u65b0\u5e74\u5feb\u4e50", "", "" ]
//     仙界聊天：[ 4, "\u5176\u4e50\u878d\u878d", "", "" ]
// response:[Utils.IntUtil, Utils.UByteUtil]};
// Example
//     no response
//============================================================================
void sxd_client::Mod_Chat_Base_chat_with_players(int type, const std::string& message) {
    Json::Value data;
    data.append(type);
    data.append(message);
    data.append("");
    data.append("");
    this->send_frame(data, 6, 0);
}

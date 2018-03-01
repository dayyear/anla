#include <thread>
#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

void sxd_client::chat(const std::string& player_name) {
    // read config
    Json::Value config;
    std::istringstream(db.get_config(player_name.c_str(), "Chat")) >> config;
    // chat
    std::string message = config[rand() % config.size()].asString();
    this->Mod_Chat_Base_chat_with_players(common::gbk2utf(message));
    common::log(boost::str(boost::format("¡¾ÊÀ½ç¡¿%1%") % message));
}

//============================================================================
// R171 ÁÄÌì
// {module:6, action:0,
// request:[Utils.UByteUtil, Utils.StringUtil, Utils.StringUtil, Utils.StringUtil],
// Example
//     [ 1, "\u65b0\u5e74\u5feb\u4e50", "", "" ]
// response:[Utils.IntUtil, Utils.UByteUtil]};
// Example
//     no response
//============================================================================
void sxd_client::Mod_Chat_Base_chat_with_players(const std::string& message) {
    Json::Value data;
    data.append(1);
    data.append(message);
    data.append("");
    data.append("");
    this->send_frame(data, 6, 0);
}

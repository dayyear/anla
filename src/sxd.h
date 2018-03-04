#ifndef SXD_H_
#define SXD_H_

#include "sxd_client.h"

class sxd {
public:
    sxd();
    virtual ~sxd();

    static void run();
    static void login();
    static void analyze();
    static void collect();

private:
    static void play(const std::string& version, const std::string& user_id, const std::string& url, const std::string& cookie);

    static void StUnionActivity(sxd_client& sxd_client_super_town, const std::string& version);
    static void WishPool(sxd_client& sxd_client_super_town, const std::string& version);

    static int SaintAreaLogin(sxd_client& sxd_client_town, sxd_client& sxd_client_saint_area, int player_id, const std::string& nickname);
    static void SaTakeBible(sxd_client& sxd_client_saint_area);

    static int ServerChatRoomLogin(sxd_client& sxd_client_town, sxd_client& sxd_client_chat_room, int player_id, int& player_id_chat_room, std::string& servername, const std::string& player_name);
    static void ServerChatRoomPet(sxd_client& sxd_client_town, sxd_client& sxd_client_chat_room, int player_id_chat_room, const std::string& nickname, const std::string& servername);

    static void collect_protocol(const std::string& version, const std::string& path);
    static void collect_end_function_gift(const std::string& version, const std::string& path);
    static void collect_function(const std::string& version, const std::string& path);
    static void collect_gift(const std::string& version, const std::string& path);
    static void collect_item(const std::string& version, const std::string& path);
    static void collect_lucky_shop_item(const std::string& version, const std::string& path);
    static void collect_role(const std::string& version, const std::string& path);
    static void collect_town(const std::string& version, const std::string& path);
};

#endif /* SXD_H_ */

#ifndef SXD_CLIENT_H_
#define SXD_CLIENT_H_

#include <string>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <json/json.h>

#include "database.h"

class sxd_client {
private:
    boost::asio::io_service ios;
    boost::asio::ip::tcp::resolver resolver;
    boost::asio::ip::tcp::socket sock;

    std::string version;        // 版本，构造函数初始化
    std::string user_id;        // 对应数据库表config的id，构造函数初始化
    int player_id;              // 在login中赋值
    short pre_module;
    short pre_action;

    //============================================================================
    // - sxd_client.cpp
    //============================================================================
public:
    sxd_client(const std::string& version, const std::string& user_id);
    virtual ~sxd_client();
    void connect(const std::string& host, const std::string& port);
private:
    void send_frame(const Json::Value& data, short module, short action);
    void receive_frame(Json::Value& data, short& module, short& action);
    Json::Value send_and_receive(const Json::Value& data_s, short module_s, short action_s);

public:
    //============================================================================
    // - sxd_client_town.cpp
    //============================================================================
    int login_town(const std::string& web_page);
    Json::Value Mod_Player_Base_login(const std::string& player_name, const std::string& hash_code, const std::string& time, const std::string& source, int regdate, const std::string& id_card, int open_time, char is_newst, const std::string& stage, const std::string& client);
    Json::Value Mod_Player_Base_get_player_info();
    Json::Value Mod_Player_Base_player_info_contrast(int player_id);
    Json::Value Mod_Player_Base_get_player_function();
    Json::Value Mod_Player_Base_get_game_assistant_info();
    Json::Value Mod_Player_Base_server_time();
    Json::Value Mod_Role_Base_get_role_list(int player_id);
    Json::Value Mod_Town_Base_enter_town(int town_map_id);

    //============================================================================
    // - sxd_client_chat.cpp
    //============================================================================
    void Mod_Chat_Base_chat_with_players(int type, const std::string& message);

    //============================================================================
    // - sxd_client_gift.cpp
    //============================================================================
    void gift3();
    void Mod_SealSoul_Base_get_day_stone();
    void Mod_Player_Base_get_player_camp_salary();
    Json::Value Mod_Farm_Base_player_is_player_get_xian_ling_gift();
    void Mod_Farm_Base_player_get_xian_ling_gift();
    void function_end();
    Json::Value Mod_FunctionEnd_Base_game_function_end_gift();
    Json::Value Mod_FunctionEnd_Base_random_award(int id);
    Json::Value Mod_FunctionEnd_Base_get_game_function_end_gift(int id);
    void gift();
    Json::Value Mod_Item_Base_get_player_gift_all_info();
    Json::Value Mod_Item_Base_player_get_super_gift(int id);
    void Mod_HeroesWar_Base_get_end_gift();
    void Mod_StChallenge_Base_get_end_li_bao();
    void Mod_UnlimitChallenge_Base_get_end_award();

    //============================================================================
    // - sxd_client_lucky_shop.cpp
    //============================================================================
    void lucky_shop();
    Json::Value Mod_LuckyStore_Base_get_lucky_store_item_list();
    Json::Value Mod_LuckyStore_Base_buy_lucky_store_item(int npc_id, int item_id, int lucky_store_id);
    void black_shop();
    Json::Value Mod_LuckyStore_Base_black_shop_item_list();
    Json::Value Mod_LuckyStore_Base_buy_black_shop_item(int black_shop_id);
    Json::Value Mod_LuckyStore_Base_scrap_item_list();
    Json::Value Mod_LuckyStore_Base_link_soul_item_list();

    //============================================================================
    // - sxd_client_item.cpp
    //============================================================================
    void item_reel();
    void item_use();
    void item_sell();
    Json::Value Mod_Item_Base_get_player_pack_item_list();
    Json::Value Mod_Item_Base_get_player_warehouse_grids();
    Json::Value Mod_Item_Base_get_player_home_warehouse_grids();
    Json::Value Mod_Item_Base_get_item_basical_infos(int ids[], int count);
    Json::Value Mod_Item_Base_get_facture_reel_data(int item_id);
    Json::Value Mod_Item_Base_player_use_grid_reel(int player_item_id);
    Json::Value Mod_Item_Base_large_use_grid_item(int player_item_id);
    Json::Value Mod_Item_Base_player_use_grid_item(int box_id);
    Json::Value Mod_Item_Base_player_sell_item(int box_id);

    //============================================================================
    // - sxd_client_get_peach.cpp
    //============================================================================
    void get_peach();
    Json::Value Mod_GetPeach_Base_peach_info();
    Json::Value Mod_GetPeach_Base_batch_get_peach();

    //============================================================================
    // - sxd_client_farm.cpp
    //============================================================================
    void harvest();
    void plant();
    Json::Value Mod_Farm_Base_get_farmlandinfo_list();
    Json::Value Mod_Farm_Base_harvest(int land_id, bool is_double = false);
    Json::Value Mod_Farm_Base_buy_coin_tree_count_info();
    Json::Value Mod_Farm_Base_get_player_roleinfo_list();
    Json::Value Mod_Farm_Base_plant_herbs(int land_id, int play_role_id, int type, int herbs_star_level);

    //============================================================================
    // - sxd_client_three_anniversary_signIn.cpp
    //============================================================================
    void sign_in();
    Json::Value Mod_ThreeAnniversarySignIn_Base_get_sign_in_status();
    Json::Value Mod_ThreeAnniversarySignIn_Base_get_player_sign_in_info();
    Json::Value Mod_ThreeAnniversarySignIn_Base_player_sign_in();

    //============================================================================
    // - sxd_client_link_fate.cpp
    //============================================================================
    void link_fate();
    Json::Value Mod_LinkFate_Base_get_link_fate_box();
    Json::Value Mod_LinkFate_Base_get_player_link_fate_stone_pack();
    Json::Value Mod_LinkFate_Base_one_key_open_box(int id);
    Json::Value Mod_LinkFate_Base_auto_merge_link_fate_stone();

    //============================================================================
    // - sxd_client_training.cpp
    //============================================================================
    void training();
    Json::Value Mod_Training_Base_training(int player_role_id, int type);
    Json::Value Mod_Training_Base_modify_role_data(int player_role_id);

    //============================================================================
    // - sxd_client_chaos_equipment.cpp
    //============================================================================
    void space_find();
    Json::Value Mod_SpaceFind_Base_open_space_find();
    Json::Value Mod_SpaceFind_Base_do_space_find(int type);
    Json::Value Mod_SpaceFind_Base_get_space_find();
    void chaos_equipment();
    Json::Value Mod_ChaosEquipment_Base_get_pack_chaos_monster_list();
    Json::Value Mod_ChaosEquipment_Base_make_chaos_monster(int id);
    Json::Value Mod_ChaosEquipment_Base_resolve_player_chaos_monster(int id);

    //============================================================================
    // - sxd_client_email.cpp
    //============================================================================
    void email();
    Json::Value Mod_Email_Base_get_email_info();
    Json::Value Mod_Email_Base_get_attachment_award(int id);
    Json::Value Mod_Email_Base_delete_email(int type, int id);

    //============================================================================
    // - sxd_client_super_sport.cpp
    //============================================================================
    void super_sport();
    Json::Value Mod_SuperSport_Base_open_super_sport();
    Json::Value Mod_SuperSport_Base_start_challenge(int id);

    //============================================================================
    // - sxd_client_courtyard_pet.cpp
    //============================================================================
    void courtyard_pet();
    Json::Value Mod_CourtyardPet_Base_get_player_info();
    Json::Value Mod_CourtyardPet_Base_call(int type);

    //============================================================================
    // - sxd_client_super_town.cpp
    //============================================================================
    int login_super_town(sxd_client* sxd_client_town);
    Json::Value Mod_StcLogin_Base_get_status();
    Json::Value Mod_StcLogin_Base_get_login_info();
    Json::Value Mod_StLogin_Base_login(const std::string& server_name, int player_id_town, const std::string& nickname, int time, const std::string& pass_code);
    Json::Value Mod_StTown_Base_enter_town();
    Json::Value Mod_StTown_Base_get_players();

    //============================================================================
    // - sxd_client_st_union.cpp
    //============================================================================
    std::string get_st_union_name();
    Json::Value Mod_StUnion_Base_get_player_st_union_info();
    void st_union_god_incense();
    Json::Value Mod_StUnionActivity_Base_st_union_god_incense(int id);
    void st_union_activity();
    Json::Value Mod_StUnionActivity_Base_get_st_union_tree_info();
    Json::Value Mod_StUnionActivity_Base_need_bless_player();
    Json::Value Mod_StUnionActivity_Base_choose_wish_item(int id);
    Json::Value Mod_StUnionActivity_Base_start_bless();
    Json::Value Mod_StUnionActivity_Base_bless_st_union_player(int id);
    Json::Value Mod_StUnionActivity_Base_player_get_tree_gift();
    void st_union_task();
    Json::Value Mod_StUnionTask_Base_get_challenge_info();
    Json::Value Mod_StUnionTask_Base_fight();

    //============================================================================
    // - sxd_client_wish_pool.cpp
    //============================================================================
    void wish_pool();
    Json::Value Mod_WishPool_Base_get_wish_pool_info();
    Json::Value Mod_WishPool_Base_choose_awards(int ids[], int count);
    Json::Value Mod_WishPool_Base_wish_self();
    Json::Value Mod_WishPool_Base_get_wish_list();
    Json::Value Mod_WishPool_Base_wish_other(int id);
    Json::Value Mod_WishPool_Base_get_award(int flag);

    //============================================================================
    // - sxd_client_st_take_bible.cpp
    //============================================================================
    void st_take_bible();
    Json::Value Mod_StTakeBible_Base_get_take_bible_info();
    Json::Value Mod_StTakeBible_Base_refresh();
    Json::Value Mod_StTakeBible_Base_start_take_bible();

    //============================================================================
    // - sxd_client_marry_home.cpp
    //============================================================================
    void furniture_effect();
    Json::Value Mod_MarryHome_Base_batch_get_furniture_effect();

    //============================================================================
    // - sxd_client_st_arena.cpp
    //============================================================================
    void st_arena();
    Json::Value Mod_StArena_Base_get_race_step();
    Json::Value Mod_StArena_Base_open_st_arena();
    Json::Value Mod_StArena_Base_challenge(int player_id);
    Json::Value Mod_StArena_Base_refresh_player_list();
    void exploit_shop();
    Json::Value Mod_StArena_Base_exploit_shop_item_list();
    Json::Value Mod_StArena_Base_buy_exploit_shop_item(int id, int count);

    //============================================================================
    // - sxd_client_st_super_sport.cpp
    //============================================================================
    void get_rank_award(sxd_client* sxd_client_town);
    Json::Value Mod_StSuperSport_Base_get_player_st_super_sport();
    Json::Value Mod_StSuperSport_Base_get_rank_award(int level);
    void get_score_award();
    Json::Value Mod_StSuperSport_Base_can_get_score_award();
    Json::Value Mod_StSuperSport_Base_player_score_award_info();
    Json::Value Mod_StSuperSport_Base_player_get_score_award(int index);
    void point_race(sxd_client* sxd_client_town);
    Json::Value Mod_StSuperSport_Base_get_st_super_sport_status();
    Json::Value Mod_StSuperSport_Base_challenge_list();
    Json::Value Mod_StSuperSport_Base_get_rank_award();
    Json::Value Mod_StSuperSport_Base_challenge(int index);
    void war_race(sxd_client* sxd_client_town);
    Json::Value Mod_StSuperSport_Base_get_race_step();
    Json::Value Mod_StSuperSport_Base_get_race_list(int group);
    Json::Value Mod_StSuperSport_Base_bet(int id);

    // - sxd_client_st_daoyuan_shop.cpp
    void st_daoyuan_shop();
    Json::Value Mod_StDaoyuanShop_Base_daoyuan_shop_item_list();
    Json::Value Mod_StDaoyuanShop_Base_buy_daoyuan_shop_item(int id, int count);

    //============================================================================
    // - sxd_client_saint_area.cpp
    //============================================================================
    int login_saint_area(sxd_client* sxd_client_town);
    Json::Value Mod_SaintAreaLogin_Base_get_status();
    Json::Value Mod_SaintAreaLogin_Base_get_login_info();
    Json::Value Mod_SaintAreaLogin_Base_login(const std::string& server_name, int player_id, const std::string& nickname, int time, const std::string& pass_code);
    Json::Value Mod_SaintAreaTown_Base_enter_town();

    //============================================================================
    // - sxd_client_sa_take_bible.cpp
    //============================================================================
    void sa_take_bible();
    Json::Value Mod_SaTakeBible_Base_get_take_bible_info();
    Json::Value Mod_SaTakeBible_Base_refresh();
    Json::Value Mod_SaTakeBible_Base_start_take_bible();

    //============================================================================
    // - sxd_client_server_chat_room.cpp
    //============================================================================
    int login_server_chat(sxd_client* sxd_client_town);
    Json::Value Mod_ServerChatRoom_Base_get_recent_room_list();
    Json::Value Mod_ServerChatRoom_Base_get_chat_room_status();
    Json::Value Mod_ServerChatRoom_Base_get_chat_room_logincode(int id);
    Json::Value Mod_ServerChatRoom_Base_login_chat_room(const std::string& node, int player_id, const std::string& servername, const std::string& stagename, int timestamp, const std::string& login_code);
    void pet_escort(sxd_client* sxd_client_town);
    Json::Value Mod_ServerChatRoom_Base_get_player_pet_escort_info();
    Json::Value Mod_ServerChatRoom_Base_feed_pet(int type);
    Json::Value Mod_ServerChatRoom_Base_escort_pet(int type);
    Json::Value Mod_ServerChatRoom_Base_chat_with_players(const std::string& message, const std::string& eip_num = "", const std::string& eip_index = "");
    Json::Value Mod_ServerChatRoom_Base_get_pet_escort_award();

public:
    //============================================================================
    // - sxd_client_quest.cpp
    //============================================================================
    Json::Value Mod_Quest_Base_list_player_quest();
    Json::Value Mod_Quest_Base_can_receive_quest(int town_map_id);
    Json::Value Mod_Mission_Base_get_sections(int town_map_id);
};

#endif /* SXD_CLIENT_H_ */

#ifndef SXD_CLIENT_H_
#define SXD_CLIENT_H_

#include <string>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <json/json.h>
#include <thread>

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

    int iEdit;
    bool bLogin;
    int x, y;

    //============================================================================
    // - sxd_client.cpp
    //============================================================================
public:
    sxd_client(const std::string& version, const int hwnd = -1);
    virtual ~sxd_client();
    void connect(const std::string& host, const std::string& port);
    bool is_login() {
        return bLogin;
    }

private:
    void send_frame(const Json::Value& data, short module, short action);
    void receive_frame(Json::Value& data, short& module, short& action);
    Json::Value send_and_receive(const Json::Value& data_s, short module_s, short action_s, std::function<bool(const Json::Value&)> f = NULL);
    Json::Value send_and_receive(const Json::Value& data_s, short module_s, short action_s, short module_r0, short action_r0, std::function<bool(const Json::Value&)> f = NULL);

public:
    //============================================================================
    // - sxd_client_town.cpp
    //============================================================================
    int login_town(const std::string& web_page);
    Json::Value Mod_Player_Base_login(const std::string& player_name, const std::string& hash_code, const std::string& time, const std::string& source, int regdate, const std::string& id_card, int open_time, char is_newst, const std::string& stage, const std::string& client);
    Json::Value Mod_Player_Base_get_player_info();
    Json::Value Mod_Player_Base_player_info_contrast(int player_id);
    Json::Value Mod_Player_Base_get_player_info_by_username(std::string username);
    Json::Value Mod_Player_Base_get_player_function();
    Json::Value Mod_Player_Base_get_game_assistant_info();
    Json::Value Mod_Player_Base_server_time();
    Json::Value Mod_Role_Base_get_role_list(int player_id);
    Json::Value Mod_Town_Base_enter_town(int town_map_id);
    void town_move_to(int x, int y);
    Json::Value Mod_Town_Base_move_to(int x1, int y1, int x2, int y2);
    Json::Value Mod_Player_Base_get_player_war_cd_time(int type);

    //============================================================================
    // - sxd_client_bug.cpp
    //============================================================================
    void bug();
    void mission_practice();
    Json::Value Mod_MissionPractice_Base_start_practice(int mission_id);
    Json::Value Mod_MissionPractice_Base_quickly();

    //============================================================================
    // - sxd_client_chat.cpp
    //============================================================================
    void Mod_Chat_Base_chat_with_players(int type, const std::string& message);

    //============================================================================
    // - sxd_client_gift.cpp
    //============================================================================
    void gift3();
    Json::Value Mod_SealSoul_Base_get_day_stone();
    Json::Value Mod_Player_Base_get_player_camp_salary();
    Json::Value Mod_Farm_Base_player_is_player_get_xian_ling_gift();
    Json::Value Mod_Farm_Base_player_get_xian_ling_gift();
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
    int get_empty_pack_num();
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
    Json::Value Mod_Item_Base_get_all_player_item_infos();
    Json::Value Mod_GiftItemPack_Base_put_item_to_package(int box_id);
    Json::Value Mod_Item_Base_move_pack_grid_item_to_supergift_warehouse(int box_id);

    //============================================================================
    // - sxd_client_release_welfare.cpp
    //============================================================================
    void release_welfare();
    Json::Value Mod_ReleaseWelfare_Base_get_info();
    Json::Value Mod_ReleaseWelfare_Base_get_welfare(int id);

    //============================================================================
    // - sxd_client_equipment.cpp
    //============================================================================
    void equipment();
    Json::Value Mod_Equipment_Base_upgrade_equip(int player_item_id, int select_gem_id, int use_coin_upgrade);

    //============================================================================
    // - sxd_client_library.cpp
    //============================================================================
    void library();
    Json::Value Mod_FindImmortal_Base_exchange_xian_ling(int num);
    Json::Value Mod_Library_Base_get_library_info();
    Json::Value Mod_Library_Base_activate_library_upgrade();
    Json::Value Mod_Library_Base_up_library_level(int job_id);

    //============================================================================
    // - sxd_client_research.cpp
    //============================================================================
    void research();
    Json::Value Mod_Research_Base_research_list();
    Json::Value Mod_Research_Base_research_upgrade(int id);

    //============================================================================
    // - sxd_client_rune.cpp
    //============================================================================
    void rune();
    Json::Value Mod_Rune_Base_rune_list();
    Json::Value Mod_Rune_Base_rune_use();

    //============================================================================
    // - sxd_client_get_peach.cpp
    //============================================================================
    void get_peach();
    Json::Value Mod_GetPeach_Base_peach_info();
    Json::Value Mod_GetPeach_Base_batch_get_peach();
    Json::Value Mod_GetPeach_Base_get_peach();

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
    // - sxd_client_wager_shop.cpp
    //============================================================================
    void wager_shop();
    Json::Value Mod_WagerShop_Base_get_turntable_info();
    Json::Value Mod_WagerShop_Base_turntable();

    //============================================================================
    // - sxd_client_hide_treasure_map.cpp
    //============================================================================
    void hide_treasure_map();
    void hide_treasure_map_search();
    Json::Value Mod_HideTreasureMap_Base_buy_item_info();
    Json::Value Mod_HideTreasureMap_Base_buy_item();
    Json::Value Mod_HideTreasureMap_Base_player_use_grid_item(int box_id);
    Json::Value Mod_HideTreasureMap_Base_start_grub(int box_id, int town_map_id, int x, int y);

    //============================================================================
    // - sxd_client_dice_messenger.cpp
    //============================================================================
    void dice_messenger();
    Json::Value Mod_DiceMessenger_Base_get_info();
    Json::Value Mod_DiceMessenger_Base_play_dice();
    Json::Value Mod_DiceMessenger_Base_get_award();
    Json::Value Mod_DiceMessenger_Base_tol_get();


    //============================================================================
    // - sxd_client_dunhuang_treasure.cpp
    //============================================================================
    void dunhuang_treasure();
    Json::Value Mod_DunhuangTreasure_Base_get_dunhuang_treasure_info();
    Json::Value Mod_DunhuangTreasure_Base_open_treasure(int type);

    //============================================================================
    // - sxd_client_memoirs.cpp
    //============================================================================
    Json::Value Mod_Memoirs_Base_is_activity_open();

    //============================================================================
    // - sxd_client_spring_carnival.cpp
    //============================================================================
    void spring_carnival();
    void spring_big_run();
    Json::Value Mod_SpringBigRun_Base_panel_info();
    Json::Value Mod_SpringBigRun_Base_gain_award();
    Json::Value Mod_SpringBigRun_Base_gain_total_award(int id);
    void spring_open_year();
    Json::Value Mod_NewYearSmallActivity_Base_get_open_year_award(int id);
    void assistant_turntable_two();
    Json::Value Mod_Memoirs_Base_assistant_turntable_lottery_two();
    void charge_back();
    Json::Value Mod_ChargeBack_Base_activate_fuction_of_mascot_befall(int function_id);
    void regression_dice();
    Json::Value Mod_Regression_Base_dice();
    void fan_fan_le();
    Json::Value Mod_FanFanLe_Base_get_info();
    Json::Value Mod_FanFanLe_Base_flop(int id);
    Json::Value Mod_FanFanLe_Base_get_extra_award();
    void online_award();
    Json::Value Mod_SpringActivity_Base_get_online_award_info();
    Json::Value Mod_SpringActivity_Base_get_online_award(int id);
    void lucky_poker();
    Json::Value Mod_LuckyPoker_Base_get_info();
    Json::Value Mod_LuckyPoker_Base_lottery_draw();
    Json::Value Mod_LuckyPoker_Base_exchange_award();
    Json::Value Mod_Memoirs_Base_get_exchange_item_info(int reflection_id);
    Json::Value Mod_Memoirs_Base_exchange_item_award(int reflection_id, int award_id);
    void golden_worker();
    Json::Value Mod_ActivityGame_Base_start_play_kuang_gong();
    Json::Value Mod_ActivityGame_Base_kuang_gong_game_over(int golden);
    Json::Value Mod_ActivityGame_Base_player_score_exchange_info();
    Json::Value Mod_ActivityGame_Base_exchange_award(int award_id);
    void mid_back_lottery();
    Json::Value Mod_MidBackLottery_Base_get_back_lottery_info();
    Json::Value Mod_MidBackLottery_Base_back_lottery(int id);
    Json::Value Mod_MidBackLottery_Base_memento_coin_exchange(int item_id);
    Json::Value Mod_MidBackLottery_Base_get_memento_coin_exchange_info();
    void thanks_feedback();
    Json::Value Mod_ThanksFeedback_Base_open_panel();
    Json::Value Mod_ThanksFeedback_Base_get_award(int id);

    //============================================================================
    // - sxd_client_regression.cpp
    //============================================================================
    void regression();
    void back_assistant();
    Json::Value Mod_Regression_Base_get_back_assistant_award(int id);
    void go_back_shop();
    Json::Value Mod_GoBackShop_Base_open_panel();
    Json::Value Mod_GoBackShop_Base_buy_shop_item(int type, int shop_item_id);
    void go_back_login();
    Json::Value Mod_GoBackLogin_Base_get_award(int id);
    void back_send_gift();
    Json::Value Mod_Regression_Base_get_back_send_gift_award();
    void go_back_task();
    Json::Value Mod_GoBackTask_Base_get_award(int id);

    //============================================================================
    // - sxd_client_find_treasure.cpp
    //============================================================================
    void find_treasure();
    Json::Value Mod_FindTreasure_Base_open_find_treasure();
    Json::Value Mod_FindTreasure_Base_find_treasure(int is_lock);
    Json::Value Mod_FindTreasure_Base_exchange_score_award(int id);

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
    Json::Value Mod_Training_Base_panel_show(int player_role_id);
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

    void courtyard_pet_quest();
    Json::Value Mod_CourtyardPet_Base_get_courtyard_pet_list();
    Json::Value Mod_CourtyardPet_Base_get_quest_list();
    Json::Value Mod_CourtyardPet_Base_get_courtyard_normal_pet_list();
    Json::Value Mod_CourtyardPet_Base_reload();
    Json::Value Mod_CourtyardPet_Base_accept_quest(int quest_id, int pet_list[], int count);
    Json::Value Mod_CourtyardPet_Base_get_award(int quest_id);

    //============================================================================
    // - sxd_client_pot_world.cpp
    //============================================================================
    void pot_world();
    Json::Value Mod_PotWorld_Base_get_info();
    Json::Value Mod_PotWorld_Base_get_store_info(int building_id);
    Json::Value Mod_PotWorld_Base_buy_item(int item_id);
    Json::Value Mod_PotWorld_Base_get_forever_zhu_fu_list();
    Json::Value Mod_PotWorld_Base_get_fu_info();
    Json::Value Mod_PotWorld_Base_merge_item(int item_id);
    Json::Value Mod_PotWorld_Base_get_pack_info();

    //============================================================================
    // - sxd_client_beelzebub_trials.cpp
    //============================================================================
    void beelzebub_trials();
    Json::Value Mod_BeelzebubTrials_Base_get_moral_award();

    //============================================================================
    // - sxd_client_pet_animal.cpp
    //============================================================================
    void pet_animal();
    Json::Value Mod_PetAnimal_Base_pet_animal_info();
    Json::Value Mod_PetAnimal_Base_feed_pet_animal();
    Json::Value Mod_PetAnimal_Base_up_pet_animal();

    //============================================================================
    // - sxd_client_travel_event.cpp
    //============================================================================
    void travel_event();
    Json::Value Mod_TravelEvent_Base_get_event_and_answer();
    Json::Value Mod_TravelEvent_Base_lottery_draw();
    Json::Value Mod_TravelEvent_Base_answer_travel_event(int event_id, int answer_id);

    //============================================================================
    // - sxd_client_hunt_demon.cpp
    //============================================================================
    void hunt_demon();
    Json::Value Mod_HuntDemon_Base_open_hunt_demon();
    Json::Value Mod_HuntDemon_Base_hunt_demon(int type);

    //============================================================================
    // - sxd_client_awake.cpp
    //============================================================================
    void awake();
    Json::Value Mod_Awake_Base_player_forbidden_book_info();
    Json::Value Mod_Awake_Base_explore_forbidden_book(int forbidden_type, int times_type);

    //============================================================================
    // - sxd_client_bai_lian_qian_kun.cpp
    //============================================================================
    void bai_lian_qian_kun();
    Json::Value Mod_BaiLianQianKun_Base_one_key_challenge();
    Json::Value Mod_BaiLianQianKun_Base_get_info();
    Json::Value Mod_BaiLianQianKun_Base_challenge();

    //============================================================================
    // - sxd_client_five_elements_laba.cpp
    //============================================================================
    void five_elements_laba();
    Json::Value Mod_Laba_Base_laba_info();
    Json::Value Mod_Laba_Base_draw();

    //============================================================================
    // - sxd_client_roll_cake.cpp
    //============================================================================
    void roll_cake();
    Json::Value Mod_RollCake_Base_get_state();
    Json::Value Mod_RollCake_Base_get_count();
    Json::Value Mod_RollCake_Base_roll();
    Json::Value Mod_RollCake_Base_reroll();
    Json::Value Mod_RollCake_Base_get_award();

    //============================================================================
    // - sxd_client_send_flower.cpp
    //============================================================================
    void send_flower();
    Json::Value Mod_Friend_Base_get_friend_list();
    Json::Value Mod_SendFlower_Base_player_send_flower_info(int player_id);
    Json::Value Mod_SendFlower_Base_send_player_flower(int player_id);

    //============================================================================
    // - sxd_client_fate.cpp
    //============================================================================
    void batch_fate(int count);
    void fate();
    void Mod_Fate_Base_merge_all_in_bag();
    Json::Value Mod_Fate_Base_get_fate_npc();
    Json::Value Mod_Fate_Base_appoint_fate_npc(int npc_id);
    Json::Value Mod_Fate_Base_get_temp_fate();
    Json::Value Mod_Fate_Base_pickup_fate(const std::vector<long long> &temp_fate_ids);
    Json::Value Mod_Fate_Base_sale_temp_fate(const std::vector<long long> &temp_fate_ids);

    //============================================================================
    // - sxd_client_find_immortal.cpp
    //============================================================================
    void find_immortal();
    Json::Value Mod_FindImmortal_Base_open_find_immortal();
    Json::Value Mod_FindImmortal_Base_start_find_immortal();
    Json::Value Mod_FindImmortal_Base_pickup_award();

    void find_immortal2();
    Json::Value Mod_FindImmortal_Base_is_five_blessings_open();
    Json::Value Mod_FindImmortal_Base_open_five_blessings();
    Json::Value Mod_FindImmortal_Base_start_bless();
    Json::Value Mod_FindImmortal_Base_end_bless();

    //============================================================================
    // - sxd_client_rob_money.cpp
    //============================================================================
    void rob_money();
    Json::Value Mod_RobMoney_Base_get_rob_money_info();
    Json::Value Mod_RobMoney_Base_search();
    Json::Value Mod_RobMoney_Base_rob();

    //============================================================================
    // - sxd_client_nine_regions.cpp
    //============================================================================
    void nine_regions();
    Json::Value Mod_NineRegions_Base_open_panel();
    Json::Value Mod_NineRegions_Base_get_calabash_count();
    Json::Value Mod_NineRegions_Base_get_calabash_info(int cur_jie);
    Json::Value Mod_NineRegions_Base_call(int cur_jie);
    Json::Value Mod_NineRegions_Base_gathering(int cur_jie);
    Json::Value Mod_NineRegions_Base_collect(int cur_jie);

    //============================================================================
    // - sxd_client_dance.cpp
    //============================================================================
    void dance();
    Json::Value Mod_Dance_Base_get_dance_info();
    Json::Value Mod_Dance_Base_start_dance();
    Json::Value Mod_Dance_Base_receive_award();

    //============================================================================
    // - sxd_client_marry.cpp
    //============================================================================
    void marry();
    Json::Value Mod_Marry_Base_get_marry_box();

    //============================================================================
    // - sxd_client_coin_mission.cpp
    //============================================================================
    void coin_mission();
    Json::Value Mod_CoinMission_Base_get_coin_mission_info();
    Json::Value Mod_CoinMission_Base_get_deploys(int mission_id);
    Json::Value Mod_CoinMission_Base_fight(int mission_id);

    //============================================================================
    // - sxd_client_hero_mission.cpp
    //============================================================================
    void hero_mission();
    Json::Value Mod_HeroMission_Base_get_hero_mission_list(int town_id);
    Json::Value Mod_HeroMission_Base_start_practice(int town_id, int auto_num, int hero_type);
    Json::Value Mod_HeroMission_Base_quickly();

    //============================================================================
    // - sxd_client_lucky_super_number.cpp
    //============================================================================
    void lucky_super_number();
    Json::Value Mod_StLuckySuperNumber_Base_get_lucky_super_number_info();
    Json::Value Mod_StLuckySuperNumber_Base_draw(int position);

    //============================================================================
    // - sxd_client_sunday_fruit.cpp
    //============================================================================
    void sunday_fruit();
    Json::Value Mod_SundayFruit_Base_active_status();
    Json::Value Mod_SundayFruit_Base_sunday_fruit_info();
    Json::Value Mod_SundayFruit_Base_draw();

    //============================================================================
    // - sxd_client_partner_link.cpp
    //============================================================================
    void partner_link();
    Json::Value Mod_PartnerLink_Base_get_shopping_info();
    Json::Value Mod_PartnerLink_Base_buy_item(int type);

    //============================================================================
    // - sxd_client_dragonball.cpp
    //============================================================================
    void dragonball();
    Json::Value Mod_Dragonball_Base_get_dragonball_skill_data();
    Json::Value Mod_Dragonball_Base_get_tmp_dragonball_basic_info();
    Json::Value Mod_Dragonball_Base_sacrifice(int mode, int auuto);
    Json::Value Mod_Dragonball_Base_collect_dragonball(int ids[], int count);
    Json::Value Mod_Dragonball_Base_sell_dragonball(int ids[], int count);

    //============================================================================
    // - npc_friendship.cpp
    //============================================================================
    void npc_friendship();
    Json::Value Mod_NpcFriendship_Base_get_info();
    Json::Value Mod_NpcFriendship_Base_get_give_present_info();
    Json::Value Mod_NpcFriendship_Base_give_present(int id, int npc_id);

    //============================================================================
    // - sxd_client_tower.cpp
    //============================================================================
    void tower();
    Json::Value Mod_Tower_Base_get_tower_info();
    Json::Value Mod_Tower_Base_start_challenge(int war_idx, int auto_sell_l, int auto_sell_z);
    Json::Value Mod_Tower_Base_reset_layer();
    Json::Value Mod_Tower_Base_start_auto_challenge(int auto_sell_l,int auto_sell_z);
    Json::Value Mod_Tower_Base_quickly_auto_challenge(int auto_sell_l,int auto_sell_z);
    Json::Value Mod_Tower_Base_enter_next();

    //============================================================================
    // - sxd_client_faction.cpp
    //============================================================================
    std::string get_faction_name();

    void faction_god();
    Json::Value Mod_Faction_Base_faction_god_info();
    Json::Value Mod_Faction_Base_incense();

    void faction_roll_cake();
    Json::Value Mod_Faction_Base_faction_roll_cake_info();
    Json::Value Mod_Faction_Base_roll_cake();

    void seal_satan();
    Json::Value Mod_Faction_Base_seal_satan_member_list();
    void Mod_Faction_Base_join_seal_satan();
    //Json::Value Mod_Faction_Base_join_seal_satan();

    void faction_lucky_wheel();
    Json::Value Mod_LuckyWheel_Base_open_lucky_wheel();
    Json::Value Mod_LuckyWheel_Base_start_lucky_wheel();

    void faction_join_feast();
    Json::Value Mod_Faction_Base_join_feast();

    void faction_approve();
    Json::Value Mod_Faction_Base_my_faction_info();
    Json::Value Mod_Faction_Base_request_list(int faction_id);
    Json::Value Mod_Faction_Base_accept_request(int id);

    void faction_war();
    Json::Value Mod_FactionWar_Base_get_faction_war_list();
    Json::Value Mod_FactionWar_Base_sign_up();

    //============================================================================
    // - sxd_client_assistant.cpp
    //============================================================================
    void assistant();
    Json::Value Mod_Assistant_Base_info();
    Json::Value Mod_Assistant_Base_get_award(int sn);

    //============================================================================
    // - sxd_client_fish.cpp
    //============================================================================
    void fish();
    Json::Value Mod_Fish_Base_get_player_fishhook();
    Json::Value Mod_Fish_Base_do_fishing();

    //============================================================================
    // - sxd_client_super_town.cpp
    //============================================================================
    int login_super_town(sxd_client* sxd_client_town);
    Json::Value Mod_StcLogin_Base_get_status();
    Json::Value Mod_StcLogin_Base_get_login_info();
    Json::Value Mod_StLogin_Base_login(const std::string& server_name, int player_id_town, const std::string& nickname, int time, const std::string& pass_code);
    Json::Value Mod_StTown_Base_enter_town();
    Json::Value Mod_StTown_Base_get_players();
    void st_town_move_to(int x, int y);
    Json::Value Mod_StTown_Base_move_to(int x1, int y1, int x2, int y2);

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
    std::vector<Json::Value> get_all_st_union_members();
    Json::Value Mod_StUnionTask_Base_get_st_union_list(int page);
    Json::Value Mod_StUnionTask_Base_get_st_union_info_simp(int st_union_i);
    void st_union_approve();
    Json::Value Mod_StUnion_Base_get_request_join_info();
    Json::Value Mod_StUnion_Base_deal_join_request(int id);
    void st_union_nimal();
    Json::Value Mod_StUnionAnimal_Base_get_animal_info();
    Json::Value Mod_StUnionAnimal_Base_fight(int type);

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
    // - sxd_client_marry_home.cpp
    //============================================================================
    void furniture_effect();
    Json::Value Mod_MarryHome_Base_batch_get_furniture_effect();

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

    // - sxd_client_st_big_turntable.cpp
    void st_big_turntable();
    Json::Value Mod_StBigTurntable_Base_get_big_turntable_info();
    Json::Value Mod_StBigTurntable_Base_turntable();

    //============================================================================
    // - sxd_client_st_altar.cpp
    //============================================================================
    void st_altar();
    Json::Value Mod_StAltar_Base_get_altar_info();
    Json::Value Mod_StAltar_Base_get_end_award();

    //============================================================================
    // - sxd_client_st_mine.cpp
    //============================================================================
    void st_mine();
    Json::Value Mod_StMine_Base_get_mine_mountain_info();
    Json::Value Mod_StMine_Base_enter_mine_mountain(int id);
    Json::Value Mod_StMine_Base_leave_mine_hole();
    Json::Value Mod_StMine_Base_receive_award();
    Json::Value Mod_StMine_Base_rob_mine_hole(int hole_id, int robbed_player_id);

    //============================================================================
    // - sxd_client_st_practice_room.cpp
    //============================================================================
    void st_practice_room();

    Json::Value Mod_StPracticeRoom_Base_get_rooms();
    Json::Value Mod_StPracticeRoom_Base_enter_room();
    Json::Value Mod_StPracticeRoom_Base_get_free_room_info();
    Json::Value Mod_StPracticeRoom_Base_get_free_room_info_by_page(int page);
    Json::Value Mod_StPracticeRoom_Base_get_room_info(int id);

    Json::Value Mod_StPracticeRoom_Base_get_seat_info(int room_id, int seat_id);
    Json::Value Mod_StPracticeRoom_Base_get_exp_flag();
    Json::Value Mod_StPracticeRoom_Base_player_leave_seat();
    Json::Value Mod_StPracticeRoom_Base_get_room_practice_exp();
    Json::Value Mod_StPracticeRoom_Base_sit_down(int room_id, int seat_id);

    //============================================================================
    // - sxd_client_st_take_bible.cpp
    //============================================================================
    void st_take_bible();
    Json::Value Mod_StTakeBible_Base_get_take_bible_info();
    Json::Value Mod_StTakeBible_Base_refresh();
    Json::Value Mod_StTakeBible_Base_start_take_bible();

    void st_rob_bible(sxd_client* sxd_client_town);
    Json::Value Mod_StTakeBible_Base_open_take_bible();
    Json::Value Mod_StTakeBible_Base_get_rob_take_bible_cd_time();
    Json::Value Mod_StTakeBible_Base_get_player_take_bible(int player_id, int my_level, int loc3, int sequence_id);
    Json::Value Mod_StTakeBible_Base_rob(int rob_id, int sequence_id);

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
    // - sxd_client_sa_super_sport.cpp
    //============================================================================
    void sa_super_sport();
    Json::Value Mod_SaSuperSport_Base_race_info();
    Json::Value Mod_SaSuperSport_Base_challage(int seq, int rank);

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

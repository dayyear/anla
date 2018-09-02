#include <boost/regex.hpp>
#include <boost/format.hpp>
#define BOOST_THREAD_PROVIDES_GENERIC_SHARED_MUTEX_ON_WIN
#include <boost/thread.hpp>

#include <winsock2.h>
#include <windows.h>

#include "resource.h"
#include "database.h"
#include "common.h"
#include "sxd_client.h"
#include "sxd_web.h"

#define SB_SETTEXTA (WM_USER+1)
#define SB_SETPARTS (WM_USER+4)
#define STATUSCLASSNAME "msctls_statusbar32"
#define SBARS_SIZEGRIP 0x100

#define g_szClassName "myWindowClass"
#define version "R184"

int iEdit = 0;

void auto_play(sxd_client& sxd_client_town, sxd_client& sxd_client_super_town, sxd_client& sxd_client_saint_area, sxd_client& sxd_client_chat_room, const std::vector<std::string>& function_signs) {

    /*data = sxd_client_town.Mod_Player_Base_get_player_info();
     SendMessage(GetDlgItem(hwnd, IDC_MAIN_STATUS), SB_SETTEXTA, 0, (LPARAM) "运行中...");
     SendMessage(GetDlgItem(hwnd, IDC_MAIN_STATUS), SB_SETTEXTA, 1, (LPARAM) "请登录");*/

    // gift
    sxd_client_town.item_use();
    sxd_client_town.gift3(); // 灵石, 俸禄, 仙令
    sxd_client_town.function_end(); // 随机礼包
    sxd_client_town.gift(); // 各种礼包
    sxd_client_town.Mod_HeroesWar_Base_get_end_gift(); // 阵营战礼包
    sxd_client_town.Mod_StChallenge_Base_get_end_li_bao(); // 自定义挑战礼包
    sxd_client_town.Mod_UnlimitChallenge_Base_get_end_award(); // 极限挑战宝箱
    sxd_client_town.item_use();

    // lucky shop
    if (!common::common::contain(function_signs, "LuckyShop"))
        common::log("【神秘商人】未开启", 0);
    else {
        sxd_client_town.item_sell();                            // 物品出售
        sxd_client_town.lucky_shop();                            // 神秘商人
        sxd_client_town.black_shop();                            // 珍奇异宝
        sxd_client_town.item_reel();                            // 卷轴合成
        sxd_client_town.item_use();                            // 物品使用
    }

    // release welfare，更新福利
    sxd_client_town.release_welfare();
    sxd_client_town.item_use();

    // rune
    if (!common::common::contain(function_signs, "Rune"))
        common::log("【招财神符】未开启", 0);
    else {
        sxd_client_town.rune();
    }

    // get peach
    if (!common::common::contain(function_signs, "GetPeach"))
        common::log("【摘仙桃】未开启", 0);
    else {
        sxd_client_town.get_peach();
    }

    // farm
    if (!common::common::contain(function_signs, "Farm"))
        common::log("【药园】未开启", 0);
    else {
        sxd_client_town.harvest();
        if (!common::contain(function_signs, "CoinTree"))
            common::log("【药园】未开启 [发财树]", 0);
        else {
            sxd_client_town.plant();
            sxd_client_town.harvest();
        }
    }

    // 周年活动
    if (!common::contain(function_signs, "HaloRole"))
        common::log("【周年活动】未开启", 0);
    else {
        // sign in，签到
        sxd_client_town.sign_in();
        // wager shop，游乐城赌坊
        sxd_client_town.wager_shop();
        // hide treasure map，藏宝图
        sxd_client_town.hide_treasure_map();
        // dice messenger，骰子使者
        sxd_client_town.dice_messenger();
        // dunhuang treasure，敦煌秘宝
        sxd_client_town.dunhuang_treasure();
    }

    // link fate
    if (!common::contain(function_signs, "LinkFate"))
        common::log("【结缘】未开启", 0);
    else {
        sxd_client_town.link_fate();
    }

    // training
    if (!common::contain(function_signs, "Training"))
        common::log("【培养】未开启", 0);
    else {
        sxd_client_town.training();
    }

    // chaos equipment
    if (!common::contain(function_signs, "ChaosVoid"))
        common::log("【混沌虚空】未开启", 0);
    else {
        sxd_client_town.space_find();           // 混沌虚空
        sxd_client_town.chaos_equipment();           // 混沌异兽
    }

    // email
    if (!common::contain(function_signs, "Email"))
        common::log("【邮箱】未开启", 0);
    else {
        sxd_client_town.email();
    }

    // super sport
    if (!common::contain(function_signs, "SuperSport"))
        common::log("【竞技场】未开启", 0);
    else {
        sxd_client_town.super_sport();
    }

    // courtyard pet
    if (!common::contain(function_signs, "CourtyardPet"))
        common::log("【宠物】未开启", 0);
    else {
        sxd_client_town.courtyard_pet();        // 宠物房
        sxd_client_town.courtyard_pet_quest();        // 宠物任务
    }

    // pot world
    if (!common::contain(function_signs, "PotWorld"))
        common::log("【壶中界】未开启", 0);
    else {
        sxd_client_town.pot_world();
    }

    // beelzebub trials
    if (!common::contain(function_signs, "BeelzebubTrials"))
        common::log("【魔王试炼】未开启", 0);
    else {
        sxd_client_town.beelzebub_trials();
    }

    // pet animal
    if (!common::contain(function_signs, "PetAnimal"))
        common::log("【叶公好龙】未开启", 0);
    else {
        sxd_client_town.pet_animal();
    }

    // travel event
    if (!common::contain(function_signs, "TravelEvent"))
        common::log("【仙旅奇缘】未开启", 0);
    else {
        sxd_client_town.travel_event();
    }

    // hunt demon
    if (!common::contain(function_signs, "HuntDemon"))
        common::log("【猎妖】未开启", 0);
    else {
        sxd_client_town.hunt_demon();
    }

    // awake
    if (!common::contain(function_signs, "Awake"))
        common::log("【觉醒】未开启", 0);
    else {
        sxd_client_town.awake();
    }

    // bai lian qian kun
    if (!common::contain(function_signs, "BaiLianQianKun"))
        common::log("【百炼乾坤】未开启", 0);
    else {
        sxd_client_town.bai_lian_qian_kun();
    }

    // five elements laba
    if (!common::contain(function_signs, "FiveElementsLaBa"))
        common::log("【五行天仪】未开启", 0);
    else {
        sxd_client_town.five_elements_laba();
    }

    // roll cake
    if (!common::contain(function_signs, "RollCake"))
        common::log("【吉星高照】未开启", 0);
    else {
        sxd_client_town.roll_cake();
    }

    // send flower
    if (!common::contain(function_signs, "SendFlower"))
        common::log("【送花】未开启", 0);
    else {
        sxd_client_town.send_flower();
    }

    // fate
    if (!common::contain(function_signs, "Fate"))
        common::log("【猎命】未开启", 0);
    else {
        sxd_client_town.fate();
    }

    // find immortal
    if (!common::contain(function_signs, "FindImmortal"))
        common::log("【喜从天降】未开启", 0);
    else {
        sxd_client_town.find_immortal();      // 喜从天降
        sxd_client_town.find_immortal2();      // 五福临门
    }

    // rob money
    if (!common::contain(function_signs, "RobMoney"))
        common::log("【劫镖】未开启", 0);
    else {
        sxd_client_town.rob_money();
    }

    // nine regions，九空无界
    sxd_client_town.nine_regions();

    // nimbus
    if (!common::contain(function_signs, "Nimbus"))
        common::log("【灵脉】未开启", 0);
    else {
        //sxd_client_town.nimbus();
    }

    // dance
    if (!common::contain(function_signs, "DanceAction"))
        common::log("【群仙乱舞】未开启", 0);
    else {
        sxd_client_town.dance();
    }

    // marry
    if (!common::contain(function_signs, "Marry"))
        common::log("【结婚】未开启", 0);
    else {
        sxd_client_town.marry();
    }

    // coin mission
    if (!common::contain(function_signs, "CoinMission"))
        common::log("【铜钱副本】未开启", 0);
    else {
        sxd_client_town.coin_mission();
    }

    // hero mission
    if (!common::contain(function_signs, "HeroMissionPractice"))
        common::log("【英雄扫荡】未开启", 0);
    else {
        sxd_client_town.hero_mission();
    }

    // lucky super number，幸运大比拼
    sxd_client_town.lucky_super_number();

    // sunday fruit，周末水果机
    sxd_client_town.sunday_fruit();

    // partner link
    if (!common::contain(function_signs, "PartnerLink"))
        common::log("【天缘系统】未开启", 0);
    else {
        sxd_client_town.partner_link();     // 缘魂宝箱
    }

    // dragonball
    if (!common::contain(function_signs, "Dragonball"))
        common::log("【神龙上供】未开启", 0);
    else {
        sxd_client_town.dragonball();
    }

    // faction
    if (!common::contain(function_signs, "Faction"))
        common::log("【帮派】未开启", 0);
    else {
        std::string faction_name = sxd_client_town.get_faction_name();
        if (faction_name.size() == 0)
            common::log("【帮派】未加入帮派", 0);
        else {
            sxd_client_town.faction_god();              // 帮派祭神
            sxd_client_town.seal_satan();              // 七星封魔
            sxd_client_town.faction_roll_cake();              // 帮派吉星高照
            sxd_client_town.faction_lucky_wheel();              // 帮派转盘
            sxd_client_town.faction_join_feast();              // 吃仙宴
            sxd_client_town.faction_approve();              // 审核
            sxd_client_town.faction_war();              // 帮派战
        }
    }

    // assistant
    if (!common::contain(function_signs, "DailyBox"))
        common::log("【活跃度】未开启", 0);
    else {
        sxd_client_town.assistant();
    }

    // fish
    if (!common::contain(function_signs, "Fishing"))
        common::log("【钓鱼】未开启", 0);
    else {
        sxd_client_town.fish();
    }

    // super town
    try {
        if (sxd_client_super_town.is_login()) {
            // st_union
            if (!common::contain(function_signs, "st_union"))
                common::log("【仙盟】未开启", 0);
            else {
                std::string st_union_name = sxd_client_super_town.get_st_union_name();
                if (st_union_name.size() == 0)
                    common::log("【仙盟】未加入仙盟", 0);
                else {
                    common::log(boost::str(boost::format("【仙盟】进入仙盟 [%1%]") % st_union_name), 0);
                    sxd_client_super_town.st_union_god_incense();       // 仙盟上香
                    sxd_client_super_town.st_union_activity();       // 仙盟之树
                    sxd_client_super_town.st_union_task();       // 魔神挑战
                    sxd_client_super_town.st_union_approve();       // 仙盟审核
                    sxd_client_super_town.st_union_nimal();       // 仙盟神兽
                }
            }

            // wish pool
            if (!common::contain(function_signs, "WishPool"))
                common::log("【许愿池】未开启", 0);
            else {
                sxd_client_super_town.wish_pool();
            }

            // furniture effect
            if (!common::contain(function_signs, "MarryHome"))
                common::log("【家园】未开启", 0);
            else {
                sxd_client_super_town.furniture_effect();
            }

            // st super sport
            if (!common::contain(function_signs, "st_super_sport"))
                common::log("【神魔竞技】未开启", 0);
            else {
                if (common::contain(function_signs, "Levitate"))
                    common::log("【神魔竞技】主角已成圣，升级为 [圣域竞技场]", 0);
                else {
                    sxd_client_super_town.get_rank_award(&sxd_client_town);     // 排名奖励
                    sxd_client_super_town.get_score_award();     // 神魔大礼
                    sxd_client_super_town.point_race(&sxd_client_town);     // 积分赛
                    sxd_client_super_town.war_race(&sxd_client_town);     // 神魔大战
                }
            }

            // st arena
            if (!common::contain(function_signs, "StDaoYuanShop"))
                common::log("【仙界商店】未开启", 0);
            else {
                sxd_client_super_town.st_daoyuan_shop();
            }

            // st big turntable
            if (!common::contain(function_signs, "HaloRole"))
                common::log("【周年活动】未开启", 0);
            else {
                sxd_client_super_town.st_big_turntable();           // 游乐城大转盘
            }

            // st altar
            if (!common::contain(function_signs, "StAltar"))
                common::log("【仙界神坛】未开启", 0);
            else {
                sxd_client_super_town.st_altar();
            }

            // st mine
            if (!common::contain(function_signs, "StMine"))
                common::log("【矿山系统】未开启", 0);
            else {
                sxd_client_super_town.st_mine();
            }

            // st practice room
            if (!common::contain(function_signs, "StPracticeRoom"))
                common::log("【仙界练功房】未开启", 0);
            else {
                if (common::contain(function_signs, "Levitate"))
                    common::log("【仙界练功房】主角已成圣，升级为 [圣域练功房]", 0);
                else {
                    sxd_client_super_town.st_practice_room();
                }
            }

            // st take bible
            if (!common::contain(function_signs, "ServerTakeBible"))
                common::log("【仙界取经】未开启", 0);
            else {
                if (common::contain(function_signs, "Levitate"))
                    common::log("【仙界取经】主角已成圣，升级为 [圣域取经]", 0);
                else {
                    sxd_client_super_town.st_take_bible();                      // 护送取经
                    sxd_client_super_town.st_rob_bible(&sxd_client_town);                      // 拦截取经
                }
            }

            // st arena
            if (!common::contain(function_signs, "StArena"))
                common::log("【仙界竞技场】未开启", 0);
            else {
                sxd_client_super_town.st_arena();           // 挑战
                sxd_client_town.exploit_shop();           // 荣誉商店买内丹
            }
        }
    } catch (const std::exception& ex) {
        common::log(boost::str(boost::format("发现错误(super town)：%1%") % ex.what()), iEdit);
    }

    // saint area
    try {
        if (sxd_client_saint_area.is_login()) {
            sxd_client_saint_area.sa_take_bible();                  // 圣域取经
            sxd_client_saint_area.sa_super_sport();                  // 圣域竞技场
        }
    } catch (const std::exception& ex) {
        common::log(boost::str(boost::format("发现错误(saint area)：%1%") % ex.what()), iEdit);
    }

    // server chat room
    try {
        if (sxd_client_chat_room.is_login()) {
            sxd_client_chat_room.pet_escort(&sxd_client_town);
        }
    } catch (const std::exception& ex) {
        common::log(boost::str(boost::format("发现错误(server chat room)：%1%") % ex.what()), iEdit);
    }

    // sleep and walk
    common::log("【休息】10分钟...\r\n", iEdit);
    int x = 200, y = 450;
    for (int i = 0; i < 59; i++) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        if (sxd_client_town.is_login())
            sxd_client_town.town_move_to(x + rand() % 1000, y);
        if (sxd_client_super_town.is_login())
            sxd_client_super_town.st_town_move_to(x + rand() % 1000, y);
        if (sxd_client_chat_room.is_login())
            sxd_client_chat_room.Mod_ServerChatRoom_Base_get_player_pet_escort_info();
    }
}

// Step 4: the Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {

        // Create Edit Control
        HWND hEdit = CreateWindow ("Edit", "", WS_CHILDWINDOW | WS_VISIBLE | WS_EX_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL, 0, 0, 0, 0, hwnd, (HMENU) IDC_MAIN_EDIT, ((LPCREATESTRUCT) lParam)->hInstance, NULL);
        SendMessage(hEdit, WM_SETFONT, (WPARAM) GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));
        iEdit = (int) hEdit;

        // Create Status bar
        int statwidths[] = { 100, -1 };
        HWND hStatus = CreateWindowEx(0, STATUSCLASSNAME, NULL, WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0, hwnd, (HMENU) IDC_MAIN_STATUS, GetModuleHandle(NULL), NULL);
        SendMessage(hStatus, SB_SETPARTS, sizeof(statwidths) / sizeof(int), (LPARAM) statwidths);
        SendMessage(hStatus, SB_SETTEXTA, 0, (LPARAM) "请登录");

        break;
    }
    case WM_SIZE: {
        RECT rect;
        HWND hStatus = GetDlgItem(hwnd, IDC_MAIN_STATUS);
        SendMessage(hStatus, WM_SIZE, 0, 0);
        GetWindowRect(hStatus, &rect);
        int iStatusHeight = rect.bottom - rect.top;
        MoveWindow(GetDlgItem(hwnd, IDC_MAIN_EDIT), 0, 0, LOWORD(lParam), HIWORD(lParam) - iStatusHeight, TRUE);
        break;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_MAIN_EDIT: {
            switch (HIWORD(wParam)) {
            case EN_MAXTEXT:
                SendMessage(GetDlgItem(hwnd, IDC_MAIN_EDIT), EM_SETSEL, 0, -1);
                SendMessage(GetDlgItem(hwnd, IDC_MAIN_EDIT), WM_CLEAR, 0, 0);
                break;
            }
            break;
        }
        case ID_USER_EXIT:
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            break;
        case ID_USER_LOGIN: {
            EnableMenuItem(GetMenu(hwnd), ID_USER_LOGIN, MF_DISABLED | MF_GRAYED);
            SendMessage(GetDlgItem(hwnd, IDC_MAIN_STATUS), SB_SETTEXTA, 0, (LPARAM) "运行中...");
            boost::thread t([hwnd]() {
                try {
                    const char* filename = ".\\sxdweb\\sxdweb.tmp";
                    std::remove(filename);
                    if (common::read_file(filename).size()) throw std::runtime_error("未能删除文件 [sxdweb.tmp]");

                    // initiation
                    STARTUPINFO si;
                    PROCESS_INFORMATION pi;
                    ZeroMemory( &si, sizeof(si) );
                    si.cb = sizeof(si);
                    ZeroMemory( &pi, sizeof(pi) );

                    // Start the child process.
                    char sxdweb[] = ".\\sxdweb\\sxdweb.exe 度日如年 V587";
                    if (!CreateProcessA( NULL, sxdweb, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) throw std::runtime_error("未能找到神仙道登录器 [sxdweb.exe]");

                    // Wait until child process exits.
                    WaitForSingleObject(pi.hProcess, INFINITE);

                    // Close process and thread handles.
                    CloseHandle(pi.hProcess);
                    CloseHandle(pi.hThread);

                    // Open login file
                    std::string user_ini = common::read_file(filename);
                    if (!user_ini.size()) throw std::runtime_error("未能找到用户登录信息");

                    boost::smatch match;
                    if (!regex_search(user_ini, match, boost::regex("url=(?<url>.*?)\r\ncode=(?<code>.*?)\r\ntime=(?<time>.*?)\r\nhash=(?<hash>.*?)\r\ntime1=(?<time1>.*?)\r\nhash1=(?<hash1>.*?)\r\n")))
                    throw std::runtime_error("无效的用户登录信息");

                    std::string url(match["url"]);
                    std::string code(match["code"]);
                    std::string time(match["time"]);
                    std::string hash(match["hash"]);
                    std::string time1(match["time1"]);
                    std::string hash1(match["hash1"]);

                    std::ostringstream oss;
                    oss << "Cookie: user=" << code << ";";
                    oss << "_time=" << time << ";_hash=" << hash << ";";
                    oss << "login_time_sxd_xxxxxxxx=" << time1 << ";login_hash_sxd_xxxxxxxx=" << hash1 << "\r\n";
                    std::string cookie = oss.str();

                    // get web page from url and cookie
                    std::string web_page;
                    {
                        if (!boost::regex_search(url, match, boost::regex("http://(.*?)(:\\d*)?/(.*)")))
                        throw std::runtime_error("[url] 匹配出错");
                        std::string host(match[1]);
                        std::string port(match[2]);
                        port = port.size() ? port.substr(1) : "80";
                        sxd_web web;
                        web.connect(host, port);
                        web_page = web.get(url, cookie);
                    }

                    // login town
                    sxd_client sxd_client_town(version, iEdit);
                    sxd_client sxd_client_super_town(version, iEdit);
                    sxd_client sxd_client_saint_area(version, iEdit);
                    sxd_client sxd_client_chat_room(version, iEdit);
                    if (sxd_client_town.login_town(web_page)) throw std::runtime_error("登录失败");

                    // get player functions
                    std::vector<std::string> function_signs;
                    {
                        Json::Value data = sxd_client_town.Mod_Player_Base_get_player_function();
                        common::log(boost::str(boost::format("【登录】玩家已开启 [%1%] 项功能") % data[0].size()), 0);
                        for (const auto& item : data[0]) {
                            try {
                                std::string function_name = db.get_code(version, "Function", item[0].asInt())["sign"];
                                common::log(boost::str(boost::format("【Function】[%1%(%2%)]") % function_name % item[0]), 0);
                                function_signs.push_back(function_name);
                            } catch (const std::exception& ex) {
                                common::log(boost::str(boost::format("发现错误(function)：%1%") % ex.what()), iEdit);
                            }
                        }
                    }

                    // super_town
                    if (!common::contain(function_signs, "SuperTown")) common::log("【仙界】未开启", 0);
                    else sxd_client_super_town.login_super_town(&sxd_client_town);

                    // saint area
                    if (!common::contain(function_signs, "SaintArea")) common::log("【圣域】未开启", 0);
                    else sxd_client_saint_area.login_saint_area(&sxd_client_town);

                    // chat room
                    if (!common::contain(function_signs, "ServerChatRoom")) common::log("【全网聊天】未开启", 0);
                    else sxd_client_chat_room.login_server_chat(&sxd_client_town);

                    for(;;) {
                        auto_play(sxd_client_town, sxd_client_super_town, sxd_client_saint_area, sxd_client_chat_room, function_signs);
                        ////////////////////////////////////////////
                        //common::log("【登录】退出\r\n", iEdit);
                        //break;
                        ////////////////////////////////////////////
                    }
                } catch (const std::exception& ex) {
                    common::log(boost::str(boost::format("发现错误(ID_USER_LOGIN)：%1%") % ex.what()), iEdit);
                }
                EnableMenuItem(GetMenu(hwnd), ID_USER_LOGIN, MF_ENABLED);
                SendMessage(GetDlgItem(hwnd, IDC_MAIN_STATUS), SB_SETTEXTA, 0, (LPARAM) "请登录");
            });
            break;
        }
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    //Step 1: Registering the Window Class
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    //wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICON));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    //wc.lpszMenuName = NULL;
    wc.lpszMenuName = MAKEINTRESOURCE(IDR_MYMENU);
    wc.lpszClassName = g_szClassName;
    //wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm = (HICON) LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICON), IMAGE_ICON, 16, 16, 0);

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
        MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Step 2: Creating the Window
    hwnd = CreateWindowEx( WS_EX_CLIENTEDGE, g_szClassName, "安啦", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Step 3: The Message Loop
    while (GetMessage(&Msg, NULL, 0, 0) > 0) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}

#include <iostream>
#include <iomanip>
#include <string>
#include <thread>

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

#include "database.h"
#include "sxd_web.h"
#include "sxd_client.h"
#include "sxd_const.h"
#include "common.h"
#include "protocol.h"

#include "sxd.h"

sxd::sxd() {
}

sxd::~sxd() {
}

void sxd::run() {
    srand((unsigned) time( NULL));
    std::string user_ini = common::read_file("user.ini");
    boost::regex user_regex("\\[(?<id>.*?)\\]\r\nurl=(?<url>.*?)\r\ncode=(?<user>.*?)\r\ntime=(?<_time>.*?)\r\nhash=(?<_hash>.*?)\r\ntime1=(?<login_time_sxd_xxxxxxxx>.*?)\r\nhash1=(?<login_hash_sxd_xxxxxxxx>.*?)\r\nname=(?<name>.*?)\r\n");
    for (auto it = boost::sregex_iterator(user_ini.begin(), user_ini.end(), user_regex); it != boost::sregex_iterator(); it++) {
        try {
            common::log("", 1, 1, 0);
            common::log(boost::str(boost::format("【%1%】开始...") % (*it)["name"]));
            std::ostringstream oss;
            oss << "Cookie: user=" << (*it)["user"] << ";";
            oss << "_time=" << (*it)["_time"] << ";_hash=" << (*it)["_hash"] << ";";
            oss << "login_time_sxd_xxxxxxxx=" << (*it)["login_time_sxd_xxxxxxxx"] << ";login_hash_sxd_xxxxxxxx=" << (*it)["login_hash_sxd_xxxxxxxx"] << "\r\n";
            std::string url = (*it)["url"];
            std::string cookie = oss.str();
            sxd::play("R171", url, cookie);
            //std::thread thread([url, cookie]() {sxd::play("R171", url, cookie);});
        } catch (const std::exception& ex) {
            common::log(boost::str(boost::format("发现错误：%1%") % ex.what()));
        }
    }
}

void sxd::login() {
    std::string response_http_version;
    unsigned int status_code;
    std::string status_message;
    std::string response_header;
    std::string response_body;

    auto users = db.get_all_users();
    for (auto& user : users) {
        try {
            common::log(boost::str(boost::format("%1% [%2%]") % user["name"] % user["id"]));
            std::string id = user["id"];
            std::string host1 = user["host"];
            std::string port1 = user["port"];
            std::string url1 = user["url"];
            std::string cookie1 = user["cookie"];

            // GET1
            sxd_web web1;
            web1.connect(host1, port1);
            web1.service("GET", url1, "HTTP/1.1", "Cookie: " + cookie1 + "\r\n", "", response_http_version, status_code, status_message, response_header, response_body);
            boost::regex location_regex("Location: https?://(.*?)/(.*?)\r\n");
            boost::smatch match;
            if (!regex_search(response_header, match, location_regex))
                throw std::runtime_error("[Location]匹配出错");
            std::string host2(match[1]);
            std::string port2("80");
            std::string url2("/" + match[2]);

            // GET2
            sxd_web web2;
            web2.connect(host2, port2);
            web2.service("GET", url2, "HTTP/1.1", "", "", response_http_version, status_code, status_message, response_header, response_body);
            std::list<std::string> cookies;
            boost::regex set_cookie_regex("Set-Cookie: (.*?);");

            if (!boost::regex_search(response_header, match, boost::regex("Set-Cookie: user=(.*?);")))
                throw std::runtime_error("[Set-Cookie]匹配出错");
            std::string code = match[1];
            if (!boost::regex_search(response_header, match, boost::regex("Set-Cookie: _time=(.*?);")))
                throw std::runtime_error("[Set-Cookie]匹配出错");
            std::string time = match[1];
            if (!boost::regex_search(response_header, match, boost::regex("Set-Cookie: _hash=(.*?);")))
                throw std::runtime_error("[Set-Cookie]匹配出错");
            std::string hash = match[1];
            if (!boost::regex_search(response_header, match, boost::regex("Set-Cookie: login_time_sxd_xxxxxxxx=(.*?);")))
                throw std::runtime_error("[Set-Cookie]匹配出错");
            std::string time1 = match[1];
            if (!boost::regex_search(response_header, match, boost::regex("Set-Cookie: login_hash_sxd_xxxxxxxx=(.*?);")))
                throw std::runtime_error("[Set-Cookie]匹配出错");
            std::string hash1 = match[1];

            // update user.ini
            std::string user_ini = common::read_file("user.ini");
            boost::regex user_regex("\\[" + id + "\\]\r\nurl=(?<url>.*?)\r\ncode=(?<user>.*?)\r\ntime=(?<_time>.*?)\r\nhash=(?<_hash>.*?)\r\ntime1=(?<login_time_sxd_xxxxxxxx>.*?)\r\nhash1=(?<login_hash_sxd_xxxxxxxx>.*?)\r\nname=(?<name>.*?)\r\n");
            std::string user_replace("[" + id + "]\r\nurl=$1\r\ncode=" + code + "\r\ntime=" + time + "\r\nhash=" + hash + "\r\ntime1=" + time1 + "\r\nhash1=" + hash1 + "\r\nname=$7\r\n");
            user_ini = boost::regex_replace(user_ini, user_regex, user_replace);
            common::write_file("user.ini", user_ini);
        } catch (const std::exception& ex) {
            common::log(boost::str(boost::format("发现错误：%1%") % ex.what()));
        }
        //break;
    } //for user
} //update_user_ini

void sxd::analyze() {
    // read file to be analyzed
    std::cout << "请输入数据文件名：" << std::endl;
    std::string file_name;
    std::cin >> file_name;
    //file_name = "Debug/9400.txt";
    common::log(boost::str(boost::format("分析数据文件：%1%") % file_name));
    std::string str = common::read_file(file_name);

    // split items and loop
    std::vector<std::string> times;
    for (auto it = boost::sregex_iterator(str.begin(), str.end(), boost::regex("\r\n(?<time>\\[\\d{4}.*?\\])\r\n"), boost::match_not_dot_newline); it != boost::sregex_iterator(); it++)
        times.push_back((*it)[1]);
    std::vector<std::string> blocks;
    boost::algorithm::split_regex(blocks, str, boost::regex("\r\n(?<time>\\[\\d{4}.*?\\])\r\n"));

    for (unsigned i = 1; i < blocks.size(); i++) {
        std::stringstream ss;
        int block_size = 0;
        auto block = blocks[i];
        common::log(times[i - 1]);

        // supply ss line by line
        std::vector<std::string> lines;
        boost::algorithm::split_regex(lines, block, boost::regex("\r\n"));
        for (auto& line : lines) {
            if (line.size() == 0)
                continue;
            line = line.substr(10, 49);
            for (auto it = boost::sregex_iterator(line.begin(), line.end(), boost::regex("([0-9A-F]{2}) "), boost::match_not_dot_newline); it != boost::sregex_iterator(); it++) {
                std::istringstream is((*it)[1]);
                int temp;
                is >> std::hex >> temp;
                common::write_int8(ss, temp);
                block_size++;
            }
        }

        // analyze a block with frame by frame
        int size = 0;
        while (size < block_size) {
            // read frame size
            int frame_size = common::read_int32(ss);
            size += frame_size + 4;
            // read module and action
            short module = common::read_int16(ss);
            short action = common::read_int16(ss);
            // protocol and pattern
            mss protocol;
            Json::Value pattern;
            Json::Value data;
            if (module == 0x789C) {
                // stream to memory
                std::ostringstream oss;
                common::write_int16(oss, module);
                common::write_int16(oss, action);
                oss << common::read_string(ss, frame_size - 4);
                std::string str = oss.str();
                // decompress
                boost::iostreams::array_source as(str.c_str(), str.size());
                boost::iostreams::filtering_istream fis;
                fis.push(boost::iostreams::zlib_decompressor());
                fis.push(as);
                // re-read module and action
                module = common::read_int16(fis);
                action = common::read_int16(fis);

                // get response pattern from database corresponding to module and action
                protocol = db.get_protocol("R171", module, action);
                std::istringstream(protocol[i % 2 ? "request" : "response"]) >> pattern;
                // decode frame
                protocol::decode_frame(fis, data, pattern);
            } else {
                // get response pattern from database corresponding to module and action
                protocol = db.get_protocol("R171", module, action);
                std::istringstream(protocol[i % 2 ? "request" : "response"]) >> pattern;
                // decode frame
                protocol::decode_frame(ss, data, pattern);
                if ((i % 2) && (module != 0 || action != 0) && (module != 94 || action != 0) && (module != 293 || action != 0) && (module != 336 || action != 12))
                    common::read_int32(ss);
            }
            // log
            common::log(boost::str(boost::format("  %1% method:  %2%.%3%(%4%,%5%)") % (i % 2 ? "   Send" : "Receive") % protocol["class"] % protocol["method"] % protocol["module"] % protocol["action"]));
            common::log(boost::str(boost::format("  %1% pattern: %2%") % (i % 2 ? "   Send" : "Receive") % protocol[i % 2 ? "request" : "response"]));
            common::log(boost::str(boost::format("  %1% data:    %2%") % (i % 2 ? "   Send" : "Receive") % boost::regex_replace(data.toStyledString(), boost::regex("[\n\t]+"), " ")));
        }
    } // for items
}

void sxd::play(const std::string& version, const std::string& url, const std::string& cookie) {
    //============================================================================
    // 1. host and port for web
    //============================================================================
    boost::smatch match;
    if (!regex_search(url, match, boost::regex("http://(.*?)(:\\d*)?/(.*)")))
        throw std::runtime_error("[url] 匹配出错");
    std::string host_web(match[1]);
    std::string port_web(match[2]);
    port_web = port_web.size() ? port_web.substr(1) : "80";

    //============================================================================
    // 2. play page and play parameters
    //============================================================================
    sxd_web web;
    web.connect(host_web, port_web);
    std::string response_body = web.get(url, cookie);
    if (!regex_search(response_body, match, boost::regex("\"&player_name=(.*?)\"[\\s\\S]*\"&hash_code=(.*?)\"[\\s\\S]*\"&time=(.*?)\"[\\s\\S]*\"&ip=(.*?)\"[\\s\\S]*\"&port=(.*?)\"[\\s\\S]*\"&server_id=(.*?)\"[\\s\\S]*\"&source=(.*?)\"[\\s\\S]*\"&regdate=(.*?)\"[\\s\\S]*\"&id_card=(.*?)\"[\\s\\S]*\"&open_time=(.*?)\"[\\s\\S]*\"&is_newst=(.*?)\"[\\s\\S]*\"&stage=(.*?)\"[\\s\\S]*\"&client=(.*?)\"")))
        throw std::runtime_error("请使用登录器重新登录");

    std::string player_name(match[1]);                           // 用于login(0,0)
    std::string hash_code(match[2]);                             // 用于login(0,0)
    std::string time2(match[3]);                                 // 用于login(0,0)
    std::string host_town(match[4]);                             // 用于socket.Connect()
    std::string port_town(match[5]);                             // 用于socket.Connect()
    std::string source(match[7]);                                // 用于login(0,0)
    int regdate = boost::lexical_cast<int>(match[8]);            // 用于login(0,0)
    std::string id_card(match[9]);                               // 用于login(0,0)
    int open_time = boost::lexical_cast<int>(match[10]);         // 用于login(0,0)
    char is_newst = boost::lexical_cast<int>(match[11]);         // 用于login(0,0)
    std::string stage = common::uri_decode(match[12]);           // 用于login(0,0)
    std::string client = common::uri_decode(match[13]);          // 用于login(0,0)

    //============================================================================
    // - town connect
    //============================================================================
    sxd_client sxd_client_town(version), sxd_client_super_town(version), sxd_client_saint_area(version), sxd_client_chat_room(version);
    sxd_client_town.connect(host_town, port_town);
    common::log(boost::str(boost::format("【登录】连接服务器 [%1%:%2%] 成功") % host_town % port_town));

    //============================================================================
    // - 登录
    //============================================================================
    Json::Value data = sxd_client_town.Mod_Player_Base_login(player_name, hash_code, time2, source, regdate, id_card, open_time, is_newst, stage, client);
    if (data[0].asInt())
        throw std::runtime_error(boost::str(boost::format("【登录】失败，logined[%1%]") % data[0].asInt()));
    int player_id = data[1].asInt();
    common::log(boost::str(boost::format("【登录】成功，player_id[%1%]") % player_id));

    //============================================================================
    // - 玩家基本信息
    //============================================================================
    data = sxd_client_town.Mod_Player_Base_get_player_info();
    std::string nickname = data[0].asString();
    int town_map_id = data[9].asInt();
    common::log(boost::str(boost::format("【登录】玩家基本信息，昵称[%1%]，[%2%]级，[VIP%3%]，元宝[%4%]，铜钱[%5%]") % common::utf2gbk(nickname) % data[1] % data[14] % data[2] % data[3]));

    //============================================================================
    // - 玩家排名信息
    //============================================================================
    data = sxd_client_town.Mod_Player_Base_player_info_contrast(player_id);
    std::string faction_name = common::utf2gbk(data[0][0][2].asString());
    common::log(boost::str(boost::format("【登录】玩家排名信息，竞技排名[%1%]，帮派[%2%]，战力[%3%]，声望[%4%]，阅历[%5%]，成就[%6%]，先攻[%7%]，境界[%8%]，鲜花[%9%]，仙令[%10%]") % data[0][0][1] % common::utf2gbk(data[0][0][2].asString()) % data[0][0][3] % data[0][0][4] % data[0][0][5] % data[0][0][6] % data[0][0][7] % data[0][0][8] % data[0][0][9] % data[0][0][10]));

    //============================================================================
    // - 进入城镇
    //============================================================================
    data = sxd_client_town.Mod_Town_Base_enter_town(town_map_id, player_id);
    if (data[0].asInt() != Mod_Town_Base::SUCCESS) {
        common::log(boost::str(boost::format("【登录】玩家进入 [%1%] 失败，result[%2%]") % db.get_code(version, "Town", town_map_id)["text"] % data[0]));
    } else {
        common::log(boost::str(boost::format("【登录】玩家进入 [%1%]") % db.get_code(version, "Town", town_map_id)["text"]));
    }

    //============================================================================
    // - 玩家已开通功能
    //============================================================================
    std::vector<std::string> function_names;
    data = sxd_client_town.Mod_Player_Base_get_player_function();
    std::ostringstream oss;
    oss << "【登录】玩家已开通 [" << data[0].size() << "] 项功能 ";
    for (const auto& item : data[0]) {
        std::string function_name = db.get_function_type(version.c_str(), item[0].asInt())["name"];
        //oss << function_name << ", ";
        function_names.push_back(function_name);
    }
    common::log(oss.str());

    if (std::find(function_names.begin(), function_names.end(), "神秘商人") == function_names.end())
        common::log("【神秘商人】未开通");
    else {
        sxd_client_town.lucky_shop(player_name);
        sxd_client_town.black_shop(player_name);
        sxd_client_town.item_reel(player_name);
        sxd_client_town.item_use(player_name);
        sxd_client_town.item_sell(player_name);
    }
    if (std::find(function_names.begin(), function_names.end(), "世界 ") == function_names.end())
        common::log("【世界】未开通");
    else {
        sxd_client_town.chat(player_name);
    }

    //============================================================================
    // - 随机礼包
    //============================================================================
    data = sxd_client_town.Mod_FunctionEnd_Base_game_function_end_gift();
    Json::Value gifts = data[0];
    for (const auto& gift : gifts) {
        int id = gift[0].asInt();
        if (gift[8].asInt() == 0)
            sxd_client_town.Mod_FunctionEnd_Base_random_award(id);
        data = sxd_client_town.Mod_FunctionEnd_Base_get_game_function_end_gift(id);
        if (data[0].asInt() == Mod_FunctionEnd_Base::SUCCESS)
            common::log(boost::str(boost::format("【随机礼包】领取 [%1%]") % db.get_end_function_gift(version.c_str(), id)["name"]));
        else
            common::log(boost::str(boost::format("【随机礼包】领取失败，result[%1%]") % data[0]));
    }

    //============================================================================
    // - 摘仙桃
    //============================================================================
    if (std::find(function_names.begin(), function_names.end(), "摘仙桃") != function_names.end()) {
        data = sxd_client_town.Mod_GetPeach_Base_peach_info();
        int fruit_lv = 70 + data[0].asInt() * 5;
        int peach_num = data[1].asInt();
        common::log(boost::str(boost::format("【摘仙桃】当前 [%1%] 个 [%2%] 级仙桃") % peach_num % fruit_lv));
        if (peach_num > 0) {
            data = sxd_client_town.Mod_GetPeach_Base_batch_get_peach();
            if (data[0].asInt() == 0)
                common::log(boost::str(boost::format("【摘仙桃】一键摘桃成功，获得经验值[%1%]") % data[1]));
            else
                common::log(boost::str(boost::format("【摘仙桃】一键摘桃失败，result[%1%]") % data[0]));
        }
    } else
        common::log("【摘仙桃】未开通");

    //============================================================================
    // - 药园
    //============================================================================
    if (std::find(function_names.begin(), function_names.end(), "药园") != function_names.end()) {
        // 药园土地列表
        data = sxd_client_town.Mod_Farm_Base_get_farmlandinfo_list();
        Json::Value lands = data[0];
        // 收获
        for (const auto& land : lands) {
            if (land[1].asInt()) {
                data = sxd_client_town.Mod_Farm_Base_harvest(land[0].asInt());
                if (data[0].asInt() == Mod_Farm_Base::SUCCESS)
                    common::log(boost::str(boost::format("【药园】[%1%] 收获 [%2%]，获得铜钱[%3%]，经验值[%4%]，仙令[%5%]") % common::utf2gbk(data[1].asString()) % common::utf2gbk(data[2].asString()) % data[5] % data[3] % data[6]));
                else
                    common::log(boost::str(boost::format("【药园】收获失败，result[%1%]") % data[0]));
            }
        }
        // 种植发财树
        if (std::find(function_names.begin(), function_names.end(), "发财树") != function_names.end()) {
            // 获取种植伙伴信息
            data = sxd_client_town.Mod_Farm_Base_get_player_roleinfo_list();
            Json::Value player_roles = data[0];
            // 最优伙伴
            Json::Value player_role_max = *std::max_element(player_roles.begin(), player_roles.end(), [](const Json::Value& x1, const Json::Value& x2) {return x1[3].asInt()<x2[3].asInt();});
            int player_role_max_id = player_role_max[0].asInt();
            // 最优土地
            Json::Value land_max = *std::max_element(lands.begin(), lands.end(), [](const Json::Value& x1, const Json::Value& x2) {return x1[9].asInt()<x2[9].asInt();});
            int land_max_id = land_max[0].asInt();

            for (;;) {
                data = sxd_client_town.Mod_Farm_Base_buy_coin_tree_count_info();
                if (data[0].asInt() == 0) {
                    common::log("【药园】仙露已用完");
                    break;
                }
                data = sxd_client_town.Mod_Farm_Base_plant_herbs(land_max_id, player_role_max_id, Mod_Farm_Base::COIN_HERBS, 1);
                if (data[0].asInt() == Mod_Farm_Base::SUCCESS) {
                    common::log(boost::str(boost::format("【药园】[%1%] 种植 [普通发财树]") % common::utf2gbk(data[4].asString())));
                    data = sxd_client_town.Mod_Farm_Base_harvest(land_max_id);
                    if (data[0].asInt() == Mod_Farm_Base::SUCCESS)
                        common::log(boost::str(boost::format("【药园】[%1%] 收获 [%2%]，获得铜钱[%3%]") % common::utf2gbk(data[1].asString()) % common::utf2gbk(data[2].asString()) % data[5]));
                    else {
                        common::log(boost::str(boost::format("【药园】收获失败，result[%1%]") % data[0]));
                        break;
                    }
                } else {
                    common::log(boost::str(boost::format("【药园】种植失败，result[%1%]") % data[0]));
                    break;
                }
            }
        } else
            common::log("【药园】未开通");
    } else
        common::log("【药园】未开通");

    //============================================================================
    // - 新年签到
    //============================================================================
    if (std::find(function_names.begin(), function_names.end(), "周年活动") == function_names.end())
        common::log("【新年签到】未开通");
    else {
        data = sxd_client_town.Mod_ThreeAnniversarySignIn_Base_get_sign_in_status();
        if (data[0].asInt() != Mod_ThreeAnniversarySignIn_Base::YES)
            common::log(boost::str(boost::format("【新年签到】活动未开启，isActivity[%1%]") % data[0]));
        else {
            if (data[1].asInt() != Mod_ThreeAnniversarySignIn_Base::YES)
                common::log(boost::str(boost::format("【新年签到】活动不可参与，isCanJoin[%1%]") % data[1]));
            else {
                data = sxd_client_town.Mod_ThreeAnniversarySignIn_Base_get_player_sign_in_info();
                if (data[3].asInt() == Mod_ThreeAnniversarySignIn_Base::YES)
                    common::log("【新年签到】今日已签到");
                else {
                    data = sxd_client_town.Mod_ThreeAnniversarySignIn_Base_player_sign_in();
                    if (data[0].asInt() == Mod_ThreeAnniversarySignIn_Base::SUCCESS)
                        common::log("【新年签到】今日签到成功");
                    else
                        common::log(boost::str(boost::format("【新年签到】今日签到失败，result[%1%]") % data[0]));
                }
            }
        }
    }

    //============================================================================
    // - 结缘
    //============================================================================
    if (std::find(function_names.begin(), function_names.end(), "结缘") == function_names.end())
        common::log("【结缘】未开通");
    else {
        data = sxd_client_town.Mod_LinkFate_Base_get_link_fate_box();
        Json::Value items = data[0];

        std::ostringstream oss;
        for (const auto& item : items)
            oss << " [" << item[1] << "] 个 [" << db.get_item_type(version.c_str(), item[0].asInt())["name"] << "]，";
        common::log(boost::str(boost::format("【结缘】当前%1%") % oss.str().substr(0, oss.str().size() - 2)));

        for (unsigned i = 0; i < items.size(); i++) {
            if (items[i][1].asInt()) {
                int id = items[i][0].asInt();
                data = sxd_client_town.Mod_LinkFate_Base_one_key_open_box(id);
                if (data[0].asInt() != Mod_LinkFate_Base::SUCCESS) {
                    common::log(boost::str(boost::format("【结缘】十连开失败，result[%1%]") % data[0]));
                    break;
                }
                common::log(boost::str(boost::format("【结缘】十连开 [%1%]") % db.get_item_type(version.c_str(), id)["name"]));
                data = sxd_client_town.Mod_LinkFate_Base_auto_merge_link_fate_stone();
                if (data[0].asInt() != Mod_LinkFate_Base::SUCCESS) {
                    common::log(boost::str(boost::format("【结缘】一键吞噬失败，result[%1%]") % data[0]));
                    break;
                }
                common::log("【结缘】一键吞噬");
                // update items
                data = sxd_client_town.Mod_LinkFate_Base_get_link_fate_box();
                items = data[0];
                if (items[i][1].asInt())
                    i--;
            }
        }
    }

    if (std::find(function_names.begin(), function_names.end(), "培养") == function_names.end())
        common::log("【培养】未开通");
    else {
        sxd_client_town.training(player_name, player_id);
    }

    if (std::find(function_names.begin(), function_names.end(), "混沌虚空") == function_names.end())
        common::log("【混沌虚空】未开通");
    else {
        // 混沌虚空
        data = sxd_client_town.Mod_SpaceFind_Base_open_space_find();
        int count = data[0].asInt();
        common::log(boost::str(boost::format("【混沌虚空】今日还可抓捕 [%1%] 次") % count));
        for (int i = 0; i < count; i++) {
            data = sxd_client_town.Mod_SpaceFind_Base_do_space_find(Mod_SpaceFind_Base::NORMAL);
            if (data[0] != Mod_SpaceFind_Base::SUCCESS) {
                common::log(boost::str(boost::format("【混沌虚空】寻找异兽失败，result[%1%]") % data[0]));
                break;
            }
            common::log(boost::str(boost::format("【混沌虚空】寻找异兽，发现 [%1%]") % db.get_item_type(version.c_str(), data[1].asInt())["name"]));
            data = sxd_client_town.Mod_SpaceFind_Base_get_space_find();
            if (data[0] != Mod_SpaceFind_Base::SUCCESS) {
                common::log(boost::str(boost::format("【混沌虚空】抓捕异兽失败，result[%1%]") % data[0]));
                break;
            }
            std::ostringstream oss;
            for (const auto& item : data[1])
                oss << "[" << item[1] << "] 个 [" << db.get_item_type(version.c_str(), item[0].asInt())["name"] << "]，";
            common::log(boost::str(boost::format("【混沌虚空】抓捕异兽，获得 %1%") % oss.str().substr(0, oss.str().size() - 2)));
        }

        // 混沌异兽
        data = sxd_client_town.Mod_ChaosEquipment_Base_get_pack_chaos_monster_list();
        common::log(boost::str(boost::format("【混沌虚空】灵宝制作：[%1%] 个 [灵液]") % data[0]));
        std::ostringstream oss;
        for (const auto& item : data[1])
            oss << "[" << db.get_item_type(version.c_str(), item[1].asInt())["name"] << "]，";
        common::log(boost::str(boost::format("【混沌虚空】异兽背包：%1%") % oss.str().substr(0, oss.str().size() - 2)));
        oss.str("");
        for (const auto& item : data[2])
            oss << "[" << db.get_item_type(version.c_str(), item[1].asInt())["name"] << "]，";
        common::log(boost::str(boost::format("【混沌虚空】灵宝背包：%1%") % oss.str().substr(0, oss.str().size() - 2)));
        oss.str("");
        for (const auto& item : data[3])
            oss << "[" << item[1] << "] 个 [" << db.get_item_type(version.c_str(), item[0].asInt())["name"] << "]，";
        common::log(boost::str(boost::format("【混沌虚空】异兽图鉴：%1%") % oss.str().substr(0, oss.str().size() - 2)));

        Json::Value scraps = data[3];
        for (unsigned i = 0; i < scraps.size(); i++) {
            mss scrap_item = db.get_item_type(version.c_str(), scraps[i][0].asInt());
            std::string scrap_comment = scrap_item["comment"];
            std::string scrap_name = scrap_item["name"];
            mss monster_item = db.get_item_type(version.c_str(), common::gbk2utf(scrap_name.substr(0, scrap_name.size() - 4)).c_str());
            int monster_id = boost::lexical_cast<int>(monster_item["id"]);
            std::string monster_name = monster_item["name"];
            // green and blue
            if ((scrap_comment.find("00ff00") != std::string::npos && scraps[i][1].asInt() >= 10) || (scrap_comment.find("00b7ee") != std::string::npos && scraps[i][1].asInt() >= 20)) {
                // 合成
                data = sxd_client_town.Mod_ChaosEquipment_Base_make_chaos_monster(monster_id);
                if (data[0].asInt() != Mod_ChaosEquipment_Base::SUCCESS) {
                    common::log(boost::str(boost::format("【混沌虚空】合成 [%1%] 失败") % monster_name));
                    break;
                }
                common::log(boost::str(boost::format("【混沌虚空】合成 [%1%]") % monster_name));
                // 分解
                data = sxd_client_town.Mod_ChaosEquipment_Base_resolve_player_chaos_monster(data[1][0][0].asInt());
                if (data[0].asInt() != Mod_ChaosEquipment_Base::SUCCESS) {
                    common::log(boost::str(boost::format("【混沌虚空】分解 [%1%] 失败") % monster_name));
                    break;
                }
                common::log(boost::str(boost::format("【混沌虚空】分解 [%1%]") % monster_name));
                // update scraps
                data = sxd_client_town.Mod_ChaosEquipment_Base_get_pack_chaos_monster_list();
                scraps = data[3];
                scrap_item = db.get_item_type(version.c_str(), scraps[i][0].asInt());
                scrap_comment = scrap_item["comment"];
                if ((scrap_comment.find("00ff00") != std::string::npos && scraps[i][1].asInt() >= 10) || (scrap_comment.find("00b7ee") != std::string::npos && scraps[i][1].asInt() >= 20))
                    i--;
            }
        }
    }

    if (std::find(function_names.begin(), function_names.end(), "邮箱") == function_names.end())
        common::log("【邮箱】未开通");
    else {
        sxd_client_town.email();
    }

    if (std::find(function_names.begin(), function_names.end(), "竞技场") == function_names.end())
        common::log("【竞技场】未开通");
    else {
        sxd_client_town.super_sport();
    }

    if (std::find(function_names.begin(), function_names.end(), "宠物") == function_names.end())
        common::log("【宠物】未开通");
    else {
        sxd_client_town.courtyard_pet();
    }

    //============================================================================
    // - 仙界
    //============================================================================
    if (std::find(function_names.begin(), function_names.end(), "仙界") == function_names.end())
        common::log("【仙界】未开通");
    else {
        data = sxd_client_town.Mod_StcLogin_Base_get_status();
        if (data[0].asInt() != Mod_StcLogin_Base::OPEN)
            common::log(boost::str(boost::format("【仙界】入口未开启，status[%1%]") % data[0]));
        else {
            common::log("【仙界】入口已开启");

            //============================================================================
            // - 仙界登录信息
            //============================================================================
            data = sxd_client_town.Mod_StcLogin_Base_get_login_info();
            std::string host_super_town = data[0].asString();
            std::string port_super_town = data[1].asString();
            std::string server_name_super_town = data[2].asString();
            int time_super_town = data[3].asInt();
            std::string pass_code = data[4].asString();

            //============================================================================
            // - super town connect
            //============================================================================
            sxd_client_super_town.connect(host_super_town, port_super_town);
            common::log(boost::str(boost::format("【仙界】连接服务器 [%1%:%2%] 成功") % host_super_town % port_super_town));

            //============================================================================
            // - 仙界登录
            //============================================================================
            data = sxd_client_super_town.Mod_StLogin_Base_login(server_name_super_town, player_id, nickname, time_super_town, pass_code);
            if (data[0].asInt())
                throw std::runtime_error(boost::str(boost::format("【仙界】登录失败，result[%1%]") % data[0]));
            int player_id_super_town = data[1].asInt();
            common::log(boost::str(boost::format("【仙界】登录成功，player_id[%1%]") % player_id_super_town));

            //============================================================================
            // - 进入仙界
            //============================================================================
            data = sxd_client_super_town.Mod_StTown_Base_enter_town(player_id_super_town);
            if (data[0].asInt() != Mod_StTown_Base::SUCCESS) {
                common::log(boost::str(boost::format("【仙界】玩家进入 [仙界] 失败，result[%1%]") % data[0]));
            } else {
                common::log("【仙界】玩家进入 [仙界]");
            }

            // 仙盟
            if (std::find(function_names.begin(), function_names.end(), "仙盟") == function_names.end())
                common::log("【仙盟】未开通");
            else {
                // 仙盟之树
                StUnionActivity(sxd_client_super_town, version);
                // 魔神挑战
                sxd_client_super_town.st_union_task();
            }

            // 许愿池
            if (std::find(function_names.begin(), function_names.end(), "许愿池") == function_names.end())
                common::log("【许愿池】未开通");
            else {
                WishPool(sxd_client_super_town, version);
            }
        }
    }

    //============================================================================
    // - 圣域
    //============================================================================
    if (std::find(function_names.begin(), function_names.end(), "圣域") == function_names.end())
        common::log("【圣域】未开通");
    else {
        if (!SaintAreaLogin(sxd_client_town, sxd_client_saint_area, player_id, nickname)) {
            SaTakeBible(sxd_client_saint_area);
        }
    }

    //============================================================================
    // - 聊天室
    //============================================================================
    if (std::find(function_names.begin(), function_names.end(), "聊天室") == function_names.end())
        common::log("【聊天室】未开通");
    else {
        int player_id_chat_room;
        std::string servername;
        if (!ServerChatRoomLogin(sxd_client_town, sxd_client_chat_room, player_id, player_id_chat_room, servername, player_name)) {
            ServerChatRoomPet(sxd_client_town, sxd_client_chat_room, player_id_chat_room, nickname, servername);
        }
    }
}

void sxd::StUnionActivity(sxd_client& sxd_client_super_town, const std::string& version) {
    // tree
    Json::Value data = sxd_client_super_town.Mod_StUnionActivity_Base_get_st_union_tree_info();
    Json::Value tree_info = data;
    if (tree_info[0].asInt() == 0) {
        common::log("【仙盟之树】未加仙盟");
        return;
    }
    if (tree_info[6].asInt()) {
        data = sxd_client_super_town.Mod_StUnionActivity_Base_player_get_tree_gift();
        if (data[0].asInt() != Mod_StUnionActivity_Base::SUCCESS) {
            common::log(boost::str(boost::format("【仙盟之树】领取失败，result[%1%]") % data[0]));
            return;
        }
        common::log(boost::str(boost::format("【仙盟之树】领取 [%1%×%2%]") % db.get_gift_type(version.c_str(), tree_info[5][0][1].asInt())["name"] % tree_info[5][0][3]));
    } else if (tree_info[2].asInt() == tree_info[3].asInt()) {
        common::log("【仙盟之树】今日许愿成功");
    } else if (tree_info[5].size() == 3) {
        int index = rand() % 3;
        data = sxd_client_super_town.Mod_StUnionActivity_Base_choose_wish_item(tree_info[5][index][0].asInt());
        if (data[0].asInt() != Mod_StUnionActivity_Base::SUCCESS) {
            common::log(boost::str(boost::format("【仙盟之树】许愿选择失败，result[%1%]") % data[0]));
            return;
        }
        common::log(boost::str(boost::format("【仙盟之树】许愿选择第 [%1%] 个物品 [%2%]") % (index + 1) % db.get_gift_type(version.c_str(), tree_info[5][index][1].asInt())["name"]));
    }
    // players
    data = sxd_client_super_town.Mod_StUnionActivity_Base_need_bless_player();
    Json::Value players = data[0];
    //int count = data[1].asInt();
    for (const auto& player : players) {
        //if (count == 0)
        //    break;
        if (player[4].asInt())
            continue;
        data = sxd_client_super_town.Mod_StUnionActivity_Base_bless_st_union_player(player[0].asInt());
        if (data[0].asInt() != Mod_StUnionActivity_Base::SUCCESS) {
            common::log(boost::str(boost::format("【仙盟之树】祝福 [%1%] 失败，result[%2%]") % common::utf2gbk(player[1].asString()) % data[0]));
            return;
        }
        common::log(boost::str(boost::format("【仙盟之树】祝福 [%1%]") % common::utf2gbk(player[1].asString())));
        //count--;
    }

}

void sxd::WishPool(sxd_client& sxd_client_super_town, const std::string& version) {
    Json::Value data = sxd_client_super_town.Mod_WishPool_Base_get_wish_pool_info();
    Json::Value wish_pool_info = data;
    common::log(boost::str(boost::format("【许愿池】[祝福值×%1%]") % wish_pool_info[1]));
    int wish_count = 0;
    for (const auto& item : wish_pool_info[0])
        if (item[1].asInt() == Mod_WishPool_Base::YES)
            wish_count++;
    if (wish_count == 0) {
        int choose_index[] = { 0, 1, 2 };
        int choose_id[3];
        for (int i = 0; i < 3; i++)
            choose_id[i] = wish_pool_info[0][choose_index[i]][0].asInt();
        data = sxd_client_super_town.Mod_WishPool_Base_choose_awards(choose_id, 3);
        if (data[0].asInt() != Mod_WishPool_Base::SUCCESS) {
            common::log(boost::str(boost::format("【许愿池】许愿失败，result[%1%]") % data[0]));
            return;
        }
        common::log("【许愿池】许愿");
    }

    if (wish_pool_info[1] < 500) {
        if (wish_pool_info[5] == Mod_WishPool_Base::NO) {
            data = sxd_client_super_town.Mod_WishPool_Base_wish_self();
            if (data[0].asInt() != Mod_WishPool_Base::SUCCESS) {
                common::log(boost::str(boost::format("【许愿池】祝福自己失败，result[%1%]") % data[0]));
                return;
            }
            common::log("【许愿池】祝福自己");
        }
    } else {
        if (wish_pool_info[4] == Mod_WishPool_Base::NO) {
            data = sxd_client_super_town.Mod_WishPool_Base_get_award(Mod_WishPool_Base::NO);
            if (data[0].asInt() != Mod_WishPool_Base::SUCCESS) {
                common::log(boost::str(boost::format("【许愿池】领取奖励失败，result[%1%]") % data[0]));
                return;
            }
            common::log("【许愿池】领取奖励");
        } else
            common::log("【许愿池】奖励已领取");
    }

    int count = wish_pool_info[6].asInt();
    data = sxd_client_super_town.Mod_WishPool_Base_get_wish_list();
    Json::Value players = data[0];
    // 优先给自己人祝福
    for (const auto& player : players) {
        if (player[5].asInt() >= 500)
            continue;
        std::string name = player[1].asString();
        // 检测自己人
        if (db.get_records("user", ("name='" + name + "'").c_str()).size() == 0)
            continue;
        data = sxd_client_super_town.Mod_WishPool_Base_wish_other(player[0].asInt());
        if (data[0].asInt() != Mod_WishPool_Base::SUCCESS) {
            common::log(boost::str(boost::format("【许愿池】祝福 [%1%] 失败，result[%2%]") % common::utf2gbk(name) % data[0]));
            return;
        }
        common::log(boost::str(boost::format("【许愿池】祝福 [%1%]") % common::utf2gbk(name)));
        count--;
    }
    // 给陌生人祝福
    for (const auto& player : players) {
        if (count <= 0)
            break;
        if (player[5].asInt() >= 500)
            continue;
        std::string name = player[1].asString();
        data = sxd_client_super_town.Mod_WishPool_Base_wish_other(player[0].asInt());
        if (data[0].asInt() != Mod_WishPool_Base::SUCCESS) {
            common::log(boost::str(boost::format("【许愿池】祝福 [%1%] 失败，result[%2%]") % common::utf2gbk(name) % data[0]));
            return;
        }
        common::log(boost::str(boost::format("【许愿池】祝福 [%1%]") % common::utf2gbk(name)));
        count--;
    }
}

int sxd::SaintAreaLogin(sxd_client& sxd_client_town, sxd_client& sxd_client_saint_area, int player_id, const std::string& nickname) {
    // 圣域状态
    Json::Value data = sxd_client_town.Mod_SaintAreaLogin_Base_get_status();
    if (data[0].asInt() != Mod_SaintAreaLogin_Base::OPEN) {
        common::log(boost::str(boost::format("【圣域】入口未开启，status[%1%]") % data[0]));
        return 1;
    }
    common::log("【圣域】入口已开启");

    // 圣域登录信息
    data = sxd_client_town.Mod_SaintAreaLogin_Base_get_login_info();
    if (data[1].asInt() == 0) {
        common::log("【圣域】玩家未成圣");
        return 2;
    }
    std::string host_saint_area = data[0].asString();
    std::string port_saint_area = data[1].asString();
    std::string server_name_saint_area = data[2].asString();
    int time_saint_area = data[3].asInt();
    std::string pass_code = data[4].asString();

    // 连接
    sxd_client_saint_area.connect(host_saint_area, port_saint_area);
    common::log(boost::str(boost::format("【圣域】连接服务器 [%1%:%2%] 成功") % host_saint_area % port_saint_area));

    // 圣域登录
    data = sxd_client_saint_area.Mod_SaintAreaLogin_Base_login(server_name_saint_area, player_id, nickname, time_saint_area, pass_code);
    if (data[0].asInt() != Mod_SaintAreaLogin_Base::SUCCESS) {
        common::log(boost::str(boost::format("【圣域】登录失败，result[%1%]") % data[0]));
        return 3;
    }
    int player_id_saint_area = data[1].asInt();
    common::log(boost::str(boost::format("【圣域】登录成功，player_id[%1%]") % player_id_saint_area));

    // 进入圣域
    data = sxd_client_saint_area.Mod_SaintAreaTown_Base_enter_town();
    if (data[0].asInt() != Mod_SaintAreaTown_Base::SUCCESS) {
        common::log(boost::str(boost::format("【圣域】玩家进入 [圣域] 失败，result[%1%]") % data[0]));
        return 4;
    }
    common::log("【圣域】玩家进入 [圣域]");

    return 0;
}

void sxd::SaTakeBible(sxd_client& sxd_client_saint_area) {
    std::string protectors[] = { "", "白龙马", "沙悟净", "猪八戒", "孙悟空", "唐僧" };
    // first get
    Json::Value data = sxd_client_saint_area.Mod_SaTakeBible_Base_get_take_bible_info();
    Json::Value bible_info = data;
    if (bible_info[2].asInt() == bible_info[3].asInt()) {
        common::log("【圣域护送取经】次数已用完");
        return;
    }
    if (bible_info[6].asInt() == 0) {
        data = sxd_client_saint_area.Mod_SaTakeBible_Base_refresh();
        if (data[0].asInt() != Mod_SaTakeBible_Base::SUCCESS) {
            common::log(boost::str(boost::format("【圣域护送取经】刷新取经使者失败，msg[%1%]") % data[0]));
            return;
        }
        common::log(boost::str(boost::format("【圣域护送取经】刷新取经使者，获得 [%1%]") % protectors[data[1].asInt()]));
    }
    // second get
    bible_info = sxd_client_saint_area.Mod_SaTakeBible_Base_get_take_bible_info();
    if (bible_info[6].asInt() == 0) {
        common::log(boost::str(boost::format("【圣域护送取经】数据异常，can_protection[%1%]") % bible_info[6]));
        return;
    }
    if (bible_info[5].asInt() == 0) {
        data = sxd_client_saint_area.Mod_SaTakeBible_Base_start_take_bible();
        if (data[0].asInt() != Mod_SaTakeBible_Base::SUCCESS) {
            common::log(boost::str(boost::format("【圣域护送取经】护送失败，msg[%1%]") % data[0]));
            return;
        }
        common::log(boost::str(boost::format("【圣域护送取经】开始护送，取经使者 [%1%]") % protectors[bible_info[6].asInt()]));
    }
}

int sxd::ServerChatRoomLogin(sxd_client& sxd_client_town, sxd_client& sxd_client_chat_room, int player_id, int& player_id_chat_room, std::string& servername, const std::string& player_name) {
    // 聊天室状态
    Json::Value data = sxd_client_town.Mod_ServerChatRoom_Base_get_chat_room_status();
    if (data[0].asInt() != Mod_ServerChatRoom_Base::SUCCESS) {
        common::log(boost::str(boost::format("【聊天室】状态获取失败，result[%1%]") % data[0]));
        return 1;
    }

    // 聊天室登录信息
    data = sxd_client_town.Mod_ServerChatRoom_Base_get_chat_room_logincode(data[1][0][0].asInt());
    std::string node = data[1].asString();
    servername = data[2].asString();
    std::string stagename = data[3].asString();
    int timestamp = data[4].asInt();
    std::string login_code = data[5].asString();
    std::string ip = data[6].asString();
    std::string port = data[7].asString();

    // 连接
    sxd_client_chat_room.connect(ip, port);
    common::log(boost::str(boost::format("【聊天室】连接服务器 [%1%:%2%] 成功") % ip % port));

    // 聊天室登录
    data = sxd_client_chat_room.Mod_ServerChatRoom_Base_login_chat_room(node, player_id, servername, stagename, timestamp, login_code);
    if (data[0].asInt() != Mod_ServerChatRoom_Base::SUCCESS) {
        common::log(boost::str(boost::format("【聊天室】登录失败，result[%1%]") % data[0]));
        return 2;
    }
    player_id_chat_room = data[1].asInt();
    common::log(boost::str(boost::format("【聊天室】登录成功，player_id[%1%]") % player_id_chat_room));

    // read config
    Json::Value config;
    std::istringstream(db.get_config(player_name.c_str(), "ServerChatRoom")) >> config;
    // chat
    std::string message = config[rand() % config.size()].asString();
    sxd_client_chat_room.Mod_ServerChatRoom_Base_chat_with_players(common::gbk2utf(message));
    common::log(boost::str(boost::format("【全网】%1%") % message));

    return 0;
}

void sxd::ServerChatRoomPet(sxd_client& sxd_client_town, sxd_client& sxd_client_chat_room, int player_id_chat_room, const std::string& nickname, const std::string& servername) {
    for (int i = 0; i < 10; i++) {
        Json::Value data = sxd_client_chat_room.Mod_ServerChatRoom_Base_get_player_pet_escort_info();
        Json::Value pet_escort_info = data;
        switch (pet_escort_info[2].asInt()) {

        case Mod_ServerChatRoom_Base::CAN_FEED:
            if (pet_escort_info[6].asInt() == Mod_ServerChatRoom_Base::UNDO) {
                data = sxd_client_town.Mod_ServerChatRoom_Base_feed_pet(Mod_ServerChatRoom_Base::NORMAL);
                if (data[0].asInt() != Mod_ServerChatRoom_Base::SUCCESS) {
                    common::log(boost::str(boost::format("【宠物派遣】喂养失败，result[%1%]") % data[0]));
                    return;
                }
                common::log("【宠物派遣】喂养一次");
            }
            std::this_thread::sleep_for(std::chrono::seconds(10));
            data = sxd_client_chat_room.Mod_ServerChatRoom_Base_chat_with_players(boost::str(boost::format("MSG7_%1%_%2%_%3%") % player_id_chat_room % nickname % servername));
            if (data[1].asInt() != Mod_ServerChatRoom_Base::SUCCESS) {
                common::log(boost::str(boost::format("【宠物派遣】邀请失败，result[%1%]") % data[1]));
                return;
            }
            common::log(boost::str(boost::format("【宠物派遣】邀请 [%1%/10]") % (i + 1)));
            std::this_thread::sleep_for(std::chrono::seconds(10));
            break;

        case Mod_ServerChatRoom_Base::CAN_ESCORT:
            data = sxd_client_town.Mod_ServerChatRoom_Base_escort_pet(Mod_ServerChatRoom_Base::NORMAL);
            if (data[0].asInt() != Mod_ServerChatRoom_Base::SUCCESS) {
                common::log(boost::str(boost::format("【宠物派遣】派遣失败，result[%1%]") % data[0]));
                return;
            }
            common::log("【宠物派遣】派遣");
            return;

        case Mod_ServerChatRoom_Base::ESCORTING:
        case Mod_ServerChatRoom_Base::INGOT_ESCORTING:
            common::log("【宠物派遣】派遣中...");
            return;

        case Mod_ServerChatRoom_Base::ESCORT_DONE:
            if (pet_escort_info[8].asInt() == Mod_ServerChatRoom_Base::UNDO) {
                data = sxd_client_town.Mod_ServerChatRoom_Base_get_pet_escort_award();
                if (data[0].asInt() != Mod_ServerChatRoom_Base::SUCCESS) {
                    common::log(boost::str(boost::format("【宠物派遣】领取失败，result[%1%]") % data[0]));
                    return;
                }
                common::log("【宠物派遣】领取");
            } else {
                common::log("【宠物派遣】今日派遣任务已完成");
            }
            return;

        default:
            common::log(boost::str(boost::format("【宠物派遣】未知状态，status[%1%]") % pet_escort_info[2]));
            return;
        }
    }
}

void sxd::collect() {
    try {
        sxd::collect_protocol("R171", "H:\\神仙道\\基础数据准备\\R171\\Main\\Action\\com\\protocols");
        sxd::collect_end_function_gift("R171", "H:\\神仙道\\基础数据准备\\R171\\Main\\Action\\com\\assist\\server\\source\\GiftTypeData.as");
        sxd::collect_function("R171", "H:\\神仙道\\基础数据准备\\R171\\templet\\com\\assist\\server\\source\\FunctionTypeData.as");
        sxd::collect_gift("R171", "H:\\神仙道\\基础数据准备\\R171\\Main\\Action\\com\\assist\\server\\source\\GiftTypeData.as");
        sxd::collect_item("R171", "H:\\神仙道\\基础数据准备\\R171\\templet\\com\\assist\\server\\source\\ItemTypeData.as");
        sxd::collect_lucky_shop_item("R171", "H:\\神仙道\\基础数据准备\\R171\\templet\\com\\assist\\server\\source\\ItemTypeData.as");
        sxd::collect_role("R171", "H:\\神仙道\\基础数据准备\\R171\\scripts\\com\\assist\\server\\RoleType.as");
        sxd::collect_town("R171", "H:\\神仙道\\基础数据准备\\R171\\templet\\com\\assist\\server\\source\\TownTypeData.as");
    } catch (const std::exception& ex) {
        std::cerr << boost::str(boost::format("发现错误：%1%") % ex.what()) << std::endl;
    }
}

void sxd::collect_end_function_gift(const std::string& version, const std::string& path) {
    std::string type = "EndFunctionGift";
    db.execute("BEGIN");
    db.execute(("DELETE FROM code where version='" + version + "' and type='" + type + "'").c_str());
    std::string content = common::read_file(path);
    // regex 1
    boost::smatch match;
    boost::regex regex1("endFunctionGift.*?\r\n");
    boost::regex_search(content, match, regex1);
    content = match.str();
    // regex2
    boost::regex regex2("\\[(\\d+?), \"(.*?)\", \"(.*?)\".*?\\]");
    while (boost::regex_search(content, match, regex2)) {
        auto sql = boost::format("INSERT INTO code(version, type, value, text, sign, comment) VALUES('%1%', '%2%', '%3%', '%4%', '%5%', '%6%')");
        sql % version % type % match[1] % match[3] % match[2] % "";
        common::log(common::utf2gbk(sql.str()));
        db.execute(sql.str().c_str());
        content = match.suffix();
    }
    db.execute("COMMIT");
}

void sxd::collect_function(const std::string& version, const std::string& path) {
    std::string type = "Function";
    db.execute("BEGIN");
    db.execute(("DELETE FROM code where version='" + version + "' and type='" + type + "'").c_str());
    std::string content = common::read_file(path);
    // regex 1
    boost::smatch match;
    boost::regex regex1("List:Object = .*?;");
    boost::regex_search(content, match, regex1);
    content = match.str();
    // regex2
    boost::regex regex2("(\\d+?):\\[\"(.*?)\",\".*?\",\"(.*?)\"\\]");
    while (boost::regex_search(content, match, regex2)) {
        auto sql = boost::format("INSERT INTO code(version, type, value, text, sign, comment) VALUES('%1%', '%2%', '%3%', '%4%', '%5%', '%6%')");
        sql % version % type % match[1] % match[3] % match[2] % "";
        common::log(common::utf2gbk(sql.str()));
        db.execute(sql.str().c_str());
        content = match.suffix();
    }
    db.execute("COMMIT");
}

void sxd::collect_gift(const std::string& version, const std::string& path) {
    std::string type = "Gift";
    db.execute("BEGIN");
    db.execute(("DELETE FROM code where version='" + version + "' and type='" + type + "'").c_str());
    std::string content = common::read_file(path);
    // regex 1
    boost::smatch match;
    boost::regex regex1("GiftTypes.*?\r\n");
    boost::regex_search(content, match, regex1);
    content = match.str();
    // regex2
    boost::regex regex2("\\[(\\d+?), \"(.*?)\", \"(.*?)\".*?\\]");
    while (boost::regex_search(content, match, regex2)) {
        auto sql = boost::format("INSERT INTO code(version, type, value, text, sign, comment) VALUES('%1%', '%2%', '%3%', '%4%', '%5%', '%6%')");
        sql % version % type % match[1] % match[3] % match[2] % "";
        common::log(common::utf2gbk(sql.str()));
        db.execute(sql.str().c_str());
        content = match.suffix();
    }
    db.execute("COMMIT");
}

void sxd::collect_item(const std::string& version, const std::string& path) {
    std::string type = "Item";
    db.execute("BEGIN");
    db.execute(("DELETE FROM code where version='" + version + "' and type='" + type + "'").c_str());
    std::string content = common::read_file(path);
    // regex 1
    boost::smatch match;
    boost::regex regex1("Items:Object = .*?;\r\n");
    boost::regex_search(content, match, regex1);
    content = match.str();
    // regex2
    boost::regex regex2("(\\d+?):\\[\"(.*?)\",\\d*?,\".*?\",\"(.*?)\".*?]");
    while (boost::regex_search(content, match, regex2)) {
        auto sql = boost::format("INSERT INTO code(version, type, value, text, sign, comment) VALUES('%1%', '%2%', '%3%', '%4%', '%5%', '%6%')");
        sql % version % type % match[1] % match[2] % "" % match[3];
        common::log(common::utf2gbk(sql.str()));
        db.execute(sql.str().c_str());
        content = match.suffix();
    }
    db.execute("COMMIT");
}

void sxd::collect_lucky_shop_item(const std::string& version, const std::string& path) {
    db.execute("BEGIN");
    db.execute("DELETE FROM lucky_shop_item where version='" + version + "'");
    std::string content = common::read_file(path);
    // regex 1
    boost::smatch match;
    boost::regex regex1("AryShopItem:Object = .*?;\r\n");
    boost::regex_search(content, match, regex1);
    content = match.str();
    // regex2
    boost::regex regex2("(?<id>\\d+?):\\[(?<item_id>\\d*?),(?<count>\\d*?),(?<ingot>\\d*?),(?<coin>\\d*?)\\]");
    while (boost::regex_search(content, match, regex2)) {
        auto sql = boost::format("INSERT INTO lucky_shop_item(version, id, item_id, count, ingot, coin) VALUES('%1%', %2%, %3%, %4%, %5%, %6%)");
        sql % version % match["id"] % match["item_id"] % match["count"] % match["ingot"] % match["coin"];
        common::log(common::utf2gbk(sql.str()));
        db.execute(sql.str());
        content = match.suffix();
    }
    db.execute("COMMIT");

}

void sxd::collect_role(const std::string& version, const std::string& path) {
    std::string type = "Role";
    db.execute("BEGIN");
    db.execute(("DELETE FROM code where version='" + version + "' and type='" + type + "'").c_str());
    std::string content = common::read_file(path);
    // regex 1
    boost::smatch match;
    boost::regex regex1("Roles:Object = .*?;\r\n");
    boost::regex_search(content, match, regex1);
    content = match.str();
    // regex2
    boost::regex regex2("(\\d+?):\\[(.*?),\"(.*?)\",.*?]");
    while (boost::regex_search(content, match, regex2)) {
        auto sql = boost::format("INSERT INTO code(version, type, value, text, sign, comment) VALUES('%1%', '%2%', '%3%', '%4%', '%5%', '%6%')");
        sql % version % type % match[1] % match[3] % match[2] % "";
        common::log(common::utf2gbk(sql.str()));
        db.execute(sql.str().c_str());
        content = match.suffix();
    }
    db.execute("COMMIT");
}

void sxd::collect_town(const std::string& version, const std::string& path) {
    std::string type = "Town";
    db.execute("BEGIN");
    db.execute(("DELETE FROM code where version='" + version + "' and type='" + type + "'").c_str());
    std::string content = common::read_file(path);
    // regex 1
    boost::smatch match;
    boost::regex regex1("Towns:Object = .*?;\r\n");
    boost::regex_search(content, match, regex1);
    content = match.str();
    // regex2
    boost::regex regex2("(\\d+?):\\[\"(.*?)\",\"(.*?)\",.*?\r\n");
    while (boost::regex_search(content, match, regex2)) {
        auto sql = boost::format("INSERT INTO code(version, type, value, text, sign, comment) VALUES('%1%', '%2%', '%3%', '%4%', '%5%', '%6%')");
        sql % version % type % match[1] % match[3] % match[2] % "";
        common::log(common::utf2gbk(sql.str()));
        db.execute(sql.str().c_str());
        content = match.suffix();
    }
    db.execute("COMMIT");
}

void sxd::collect_protocol(const std::string& version, const std::string& path) {
    db.execute("BEGIN");
    db.execute(("DELETE FROM protocol where version='" + version + "'").c_str());

    boost::filesystem::directory_iterator ite;
    for (boost::filesystem::directory_iterator it(path); it != ite; ++it) {
        std::string file = it->path().string();
        std::string filename = it->path().filename().string();
        if (!boost::regex_match(filename, boost::regex("Mod_.*_Base\\.as")))
            continue;

        std::string content = common::read_file(file);
        boost::smatch match;
        boost::regex_search(content, match, boost::regex("public class (?<class>.*?) extends Object"));
        common::log(boost::str(boost::format("class: %1%") % match["class"]));

        for (auto it = boost::sregex_iterator(content.begin(), content.end(), boost::regex("public static const (?<method>.*?):Object = {module:(?<module>\\d*?), action:(?<action>\\d*?), request:(?<request>.*?), response:(?<response>.*?)};"), boost::match_not_dot_newline); it != boost::sregex_iterator(); it++) {
            std::ostringstream sql;
            sql << "INSERT INTO protocol(version, module, class, action, method, request, response) VALUES(";
            sql << "'" << version << "', ";
            sql << (*it)["module"] << ", ";
            sql << "'" << match["class"] << "', ";
            sql << (*it)["action"] << ", ";
            sql << "'" << (*it)["method"] << "', ";
            sql << "'" << boost::regex_replace(std::string((*it)["request"]), boost::regex("Utils(.*?)Util"), "\"$&\"") << "', ";
            sql << "'" << boost::regex_replace(std::string((*it)["response"]), boost::regex("Utils(.*?)Util"), "\"$&\"") << "'";
            sql << ")";
            db.execute(sql.str().c_str());
        }
    }
    db.execute("COMMIT");
} //sxd::collect_protocols


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
#include "common.h"
#include "protocol.h"

#include "sxd.h"

sxd::sxd() {
}

sxd::~sxd() {
}

void sxd::run(int index) {
    int i = 0;
    std::string user_ini = common::read_file("user.ini");
    boost::regex user_regex("\\[(?<user_id>.*?)\\]\r\nurl=(?<url>.*?)\r\ncode=(?<user>.*?)\r\ntime=(?<_time>.*?)\r\nhash=(?<_hash>.*?)\r\ntime1=(?<login_time_sxd_xxxxxxxx>.*?)\r\nhash1=(?<login_hash_sxd_xxxxxxxx>.*?)\r\nname=(?<name>.*?)\r\n");
    for (auto it = boost::sregex_iterator(user_ini.begin(), user_ini.end(), user_regex); it != boost::sregex_iterator(); it++) {
        try {
            if (index && index != ++i)
                continue;
            common::log("", 1, 1, 0);
            common::log(boost::str(boost::format("【%1%】开始...") % (*it)["name"]));
            std::ostringstream oss;
            oss << "Cookie: user=" << (*it)["user"] << ";";
            oss << "_time=" << (*it)["_time"] << ";_hash=" << (*it)["_hash"] << ";";
            oss << "login_time_sxd_xxxxxxxx=" << (*it)["login_time_sxd_xxxxxxxx"] << ";login_hash_sxd_xxxxxxxx=" << (*it)["login_hash_sxd_xxxxxxxx"] << "\r\n";
            std::string user_id = (*it)["user_id"];
            std::string url = (*it)["url"];
            std::string cookie = oss.str();
            sxd::play("R172", user_id, url, cookie);
            //std::thread thread([url, cookie]() {sxd::play("R172", url, cookie);});
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
                protocol = db.get_protocol("R172", module, action);
                std::istringstream(protocol[i % 2 ? "request" : "response"]) >> pattern;
                // decode frame
                protocol::decode_frame(fis, data, pattern);
            } else {
                // get response pattern from database corresponding to module and action
                protocol = db.get_protocol("R172", module, action);
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

bool contain(const std::vector<std::string>& v, const std::string& s) {
    return std::find(v.begin(), v.end(), s) != v.end();
}

void sxd::play(const std::string& version, const std::string& user_id, const std::string& url, const std::string& cookie) {

    // initialize four clients
    sxd_client sxd_client_town(version, user_id);
    sxd_client sxd_client_super_town(version, user_id);
    sxd_client sxd_client_saint_area(version, user_id);
    sxd_client sxd_client_chat_room(version, user_id);
    Json::Value data;

    // get web page from url and cookie
    std::string web_page;
    {
        boost::smatch match;
        if (!regex_search(url, match, boost::regex("http://(.*?)(:\\d*)?/(.*)")))
            throw std::runtime_error("[url] 匹配出错");
        std::string host(match[1]);
        std::string port(match[2]);
        port = port.size() ? port.substr(1) : "80";
        sxd_web web;
        web.connect(host, port);
        web_page = web.get(url, cookie);
    }

    // login town
    if (sxd_client_town.login_town(web_page))
        return;

    // get player functions
    std::vector<std::string> function_names;
    std::vector<std::string> function_ids;
    {
        data = sxd_client_town.Mod_Player_Base_get_player_function();
        common::log(boost::str(boost::format("【登录】玩家已开启 [%1%] 项功能") % data[0].size()));
        for (const auto& item : data[0]) {
            function_ids.push_back(item[0].asString());
            std::string function_name = db.get_code(version, "Function", item[0].asInt())["text"];
            common::log(boost::str(boost::format("【Function】[%1%(%2%)]") % function_name % item[0]), 0);
            function_names.push_back(function_name);
        }
    }

    // gift
    sxd_client_town.gift3();                                    // 灵石, 俸禄, 仙令
    sxd_client_town.function_end();                             // 随机礼包
    sxd_client_town.gift();                                     // 各种礼包
    sxd_client_town.Mod_HeroesWar_Base_get_end_gift();          // 阵营战礼包
    sxd_client_town.Mod_StChallenge_Base_get_end_li_bao();      // 自定义挑战礼包
    sxd_client_town.Mod_UnlimitChallenge_Base_get_end_award();  // 极限挑战宝箱

    // lucky shop
    if (!contain(function_names, "神秘商人"))
        common::log("【神秘商人】未开启");
    else {
        sxd_client_town.item_sell();                            // 物品出售
        sxd_client_town.lucky_shop();                           // 神秘商人
        sxd_client_town.black_shop();                           // 珍奇异宝
        sxd_client_town.item_reel();                            // 卷轴合成
        sxd_client_town.item_use();                             // 物品使用
    }

    // get peach
    if (!contain(function_names, "摘仙桃"))
        common::log("【摘仙桃】未开启");
    else {
        sxd_client_town.get_peach();
    }

    // farm
    if (!contain(function_names, "药园"))
        common::log("【药园】未开启");
    else {
        sxd_client_town.harvest();
        if (!contain(function_names, "发财树"))
            common::log("【药园】未开启 [发财树]");
        else
            sxd_client_town.plant();
    }

    // sign in
    if (!contain(function_names, "周年活动"))
        common::log("【新年签到】未开启");
    else {
        sxd_client_town.sign_in();
    }

    // link fate
    if (!contain(function_names, "结缘"))
        common::log("【结缘】未开启");
    else {
        sxd_client_town.link_fate();
    }

    // training
    if (!contain(function_names, "培养"))
        common::log("【培养】未开启");
    else {
        sxd_client_town.training();
    }

    // chaos equipment
    if (!contain(function_names, "混沌虚空"))
        common::log("【混沌虚空】未开启");
    else {
        sxd_client_town.space_find();           // 混沌虚空
        sxd_client_town.chaos_equipment();      // 混沌异兽
    }

    // email
    if (!contain(function_names, "邮箱"))
        common::log("【邮箱】未开启");
    else {
        sxd_client_town.email();
    }

    // super sport
    if (!contain(function_names, "竞技场"))
        common::log("【竞技场】未开启");
    else {
        sxd_client_town.super_sport();
    }

    // courtyard pet
    if (!contain(function_names, "宠物"))
        common::log("【宠物】未开启");
    else {
        sxd_client_town.courtyard_pet();
    }

    // super town
    if (!contain(function_names, "仙界"))
        common::log("【仙界】未开启");
    else if (!sxd_client_super_town.login_super_town(&sxd_client_town)) {

        // st_union
        if (!contain(function_names, "仙盟"))
            common::log("【仙盟】未开启");
        else {
            std::string st_union_name = sxd_client_super_town.get_st_union_name();
            if (st_union_name.size() == 0)
                common::log("【仙盟】未加入仙盟");
            else {
                common::log(boost::str(boost::format("【仙盟】进入仙盟 [%1%]") % st_union_name));
                sxd_client_super_town.st_union_god_incense();       // 仙盟上香
                sxd_client_super_town.st_union_activity();          // 仙盟之树
                sxd_client_super_town.st_union_task();              // 魔神挑战
            }
        }

        // wish pool
        if (!contain(function_names, "许愿池"))
            common::log("【许愿池】未开启");
        else {
            sxd_client_super_town.wish_pool();
        }

        // st take bible
        if (!contain(function_names, "跨服取经"))
            common::log("【仙界取经】未开启");
        else {
            if (contain(function_names, "主角飞升"))
                common::log("【仙界取经】主角已成圣，升级为 [圣域取经]");
            else
                sxd_client_super_town.st_take_bible();
        }

        // furniture effect
        if (!contain(function_names, "家园"))
            common::log("【家园】未开启");
        else {
            sxd_client_super_town.furniture_effect();
        }

        // st arena
        if (!contain(function_ids, "132"))
            common::log("【仙界竞技场】未开启");
        else {
            sxd_client_super_town.st_arena();           // 挑战
            sxd_client_town.exploit_shop();             // 荣誉商店买内丹
        }

        // st super sport
        if (!contain(function_ids, "93"))
            common::log("【神魔竞技】未开启");
        else {
            if (contain(function_names, "主角飞升"))
                common::log("【神魔竞技】主角已成圣，升级为 [圣域竞技场]");
            else {
                sxd_client_super_town.get_rank_award(&sxd_client_town);     // 排名奖励
                sxd_client_super_town.get_score_award();                    // 神魔大礼
                sxd_client_super_town.point_race(&sxd_client_town);         // 积分赛
                sxd_client_super_town.war_race(&sxd_client_town);           // 神魔大战(未测试)
            }
        }

        // st arena
        if (!contain(function_names, "仙界商店"))
            common::log("【仙界商店】未开启");
        else {
            sxd_client_super_town.st_daoyuan_shop();
        }

    }

    // saint area
    if (!contain(function_names, "圣域"))
        common::log("【圣域】未开启");
    else if (!sxd_client_saint_area.login_saint_area(&sxd_client_town)) {
        sxd_client_saint_area.sa_take_bible();                  // 圣域取经
    }

    // server chat room
    if (!contain(function_names, "聊天室"))
        common::log("【全网聊天】未开启");
    else if (!sxd_client_chat_room.login_server_chat(&sxd_client_town)) {
        sxd_client_chat_room.pet_escort(&sxd_client_town);
    }
}

void sxd::collect() {
    try {
        sxd::collect_protocol("R172", "H:\\神仙道\\基础数据准备\\R172\\Main\\Action\\com\\protocols");
        sxd::collect_end_function_gift("R172", "H:\\神仙道\\基础数据准备\\R172\\Main\\Action\\com\\assist\\server\\source\\GiftTypeData.as");
        sxd::collect_function("R172", "H:\\神仙道\\基础数据准备\\R172\\templet\\com\\assist\\server\\source\\FunctionTypeData.as");
        sxd::collect_gift("R172", "H:\\神仙道\\基础数据准备\\R172\\Main\\Action\\com\\assist\\server\\source\\GiftTypeData.as");
        sxd::collect_item("R172", "H:\\神仙道\\基础数据准备\\R172\\templet\\com\\assist\\server\\source\\ItemTypeData.as");
        sxd::collect_lucky_shop_item("R172", "H:\\神仙道\\基础数据准备\\R172\\templet\\com\\assist\\server\\source\\ItemTypeData.as");
        sxd::collect_role("R172", "H:\\神仙道\\基础数据准备\\R172\\scripts\\com\\assist\\server\\RoleType.as");
        sxd::collect_town("R172", "H:\\神仙道\\基础数据准备\\R172\\templet\\com\\assist\\server\\source\\TownTypeData.as");
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


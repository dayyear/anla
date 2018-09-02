#include <regex>
#include <ctime>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/format.hpp>
#include <boost/asio.hpp>
#define BOOST_THREAD_PROVIDES_GENERIC_SHARED_MUTEX_ON_WIN
#include <boost/thread.hpp>

#include "protocol.h"
#include "common.h"
#include "sxd_client.h"

sxd_client::sxd_client(const std::string& version, const int hwnd) :
        resolver(ios), sock(ios), player_id(0), pre_module(0), pre_action(0), bLogin(0), x(0), y(0) {
    this->version = version;
    this->iEdit = hwnd;
}

sxd_client::~sxd_client() {
    //common::log("before sock.close", 0);
    try {
        sock.close();
    } catch (const std::exception& ex) {
        common::log(boost::str(boost::format("发现错误(~sxd_client)：%1%") % ex.what()), iEdit);
    }
    //common::log("after sock.close", 0);
}

void sxd_client::connect(const std::string& host, const std::string& port) {
    boost::asio::ip::tcp::resolver::query query(host, port);
    boost::asio::connect(sock, resolver.resolve(query));
}

void sxd_client::send_frame(const Json::Value& data, short module, short action) {
    // stream
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    // get request pattern from database with module and action
    mss protocol = db.get_protocol(version.c_str(), module, action);
    Json::Value pattern;
    std::istringstream(protocol["request"]) >> pattern;
    // encode frame
    std::ostringstream oss;
    protocol::encode_frame(oss, data, pattern);
    std::string frame = oss.str();
    // write frame size
    int frame_size = frame.size() + 4;
    if ((module != 0 || action != 0) && (module != 94 || action != 0) && (module != 293 || action != 0) && (module != 336 || action != 12)) {
        frame_size += 4;
    }
    common::write_int32(request_stream, frame_size);
    // write module and action
    common::write_int16(request_stream, module);
    common::write_int16(request_stream, action);
    // write frame
    request_stream.write(frame.c_str(), frame.size());
    // write previous module and action
    if ((module != 0 || action != 0) && (module != 94 || action != 0) && (module != 293 || action != 0) && (module != 336 || action != 12)) {
        common::write_int16(request_stream, pre_module);
        common::write_int16(request_stream, pre_action);
    }
    // write
    boost::asio::write(sock, request);
    // save module and action
    pre_module = module;
    pre_action = action;
    // log
    /*if (module == 1 && action == 2)
     return;
     if (module == 95 && action == 4)
     return;*/
    common::log(boost::str(boost::format("     Send method:  %1%.%2%(%3%,%4%)") % protocol["class"] % protocol["method"] % protocol["module"] % protocol["action"]), 0);
    //common::log(boost::str(boost::format("     Send pattern: %1%") % protocol["request"]), 0);
    common::log(boost::str(boost::format("     Send data:    %1%") % std::regex_replace(data.toStyledString(), std::regex("[\n\t]+"), " ")), 0);
} //send_frame

void sxd_client::receive_frame(Json::Value& data, short& module, short& action) {
    // stream
    boost::asio::streambuf response;
    std::istream response_stream(&response);
    // read frame size

    // 1. timed out
    boost::thread t([this, &response]() {
        try {
            boost::asio::read(sock, response, boost::asio::transfer_exactly(4));
        }
        //catch (boost::thread_interrupted& ex) {
        //    common::log("发现错误(receive_frame)", iEdit);
        //}
        catch (const std::exception& ex) {
            common::log(boost::str(boost::format("发现错误(receive_frame)：%1%") % ex.what()), iEdit);
        }
    });
    if (!t.try_join_for(boost::chrono::seconds(60))) {
        //t.interrupt();
        //t.join();
        throw std::runtime_error("Request timed out in receive_frame");
    }

    // 2. no timed out
    //boost::asio::read(sock, response, boost::asio::transfer_exactly(4));

    int frame_size = common::read_int32(response_stream);
    // read module and action
    boost::asio::read(sock, response, boost::asio::transfer_exactly(frame_size));
    module = common::read_int16(response_stream);
    action = common::read_int16(response_stream);
    // protocol and pattern
    mss protocol;
    Json::Value pattern;
    if (module == 0x789C) {
        // stream to memory
        std::ostringstream oss;
        common::write_int16(oss, module);
        common::write_int16(oss, action);
        oss << response_stream.rdbuf();
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
        protocol = db.get_protocol(version.c_str(), module, action);
        std::istringstream(protocol["response"]) >> pattern;
        // decode frame
        protocol::decode_frame(fis, data, pattern);
    } else {
        // get response pattern from database corresponding to module and action
        protocol = db.get_protocol(version.c_str(), module, action);
        std::istringstream(protocol["response"]) >> pattern;
        // decode frame
        protocol::decode_frame(response_stream, data, pattern);
    }
    // log
    /*if (module == 1 && action == 2)
     return;
     if (module == 95 && action == 5)
     return;*/
    common::log(boost::str(boost::format("  Receive method:  %1%.%2%(%3%,%4%)") % protocol["class"] % protocol["method"] % protocol["module"] % protocol["action"]), 0);
    //common::log(boost::str(boost::format("  Receive pattern: %1%") % protocol["response"]), 0);
    common::log(boost::str(boost::format("  Receive data:    %1%") % std::regex_replace(data.toStyledString(), std::regex("[\n\t]+"), " ")), 0);
    // chat
    std::string chatrooms[] = { "活动", "世界", "帮派", "帮派战", "仙界", "仙盟", "圣域", "圣盟", "聊天室" };
    if (module == 6 && action == 1)
        for (const auto& chat : data[0]) {
            auto message = common::utf2gbk(chat[5].asString());
            auto emotion = chat[6].asString();
            if (message.find("MSG") == 0)
                continue;
            if (message.find("BeelzebubTrials") == 0)
                continue;
            if (message.find("当日快速审批") != std::string::npos)
                continue;
            if (emotion.size())
                message.append(boost::str(boost::format("[表情%1%]") % emotion));
            common::log(boost::str(boost::format("【%1%聊天】%2%[%3%%4%]:%5%") % chatrooms[chat[4].asInt()] % common::utf2gbk(chat[1].asString()) % common::utf2gbk(chat[11].asString()) % common::utf2gbk(chat[10].asString()) % message), iEdit);
        }
    if (module == 336 && action == 21) {
        auto message = common::utf2gbk(data[6].asString());
        auto emotion = data[4].asString();
        if (message.find("MSG") == 0)
            return;
        if (message.find("当日快速审批") != std::string::npos)
            return;
        if (emotion.size())
            message.append(boost::str(boost::format("[表情%1%]") % emotion));
        common::log(boost::str(boost::format("【全网聊天】%1%[%2%%3%]:%4%") % common::utf2gbk(data[1].asString()) % common::utf2gbk(data[3].asString()) % common::utf2gbk(data[2].asString()) % message), iEdit);
    }

} //receive_frame

Json::Value sxd_client::send_and_receive(const Json::Value& data_s, short module_s, short action_s, std::function<bool(const Json::Value&)> f) {
    short module_r, action_r;
    Json::Value data_r;
    this->send_frame(data_s, module_s, action_s);
    auto start = std::time(NULL);
    for (;;) {
        this->receive_frame(data_r, module_r, action_r);
        if (module_s == module_r && action_s == action_r && (!f || f(data_r)))
            return data_r;
        auto end = std::time(NULL);
        if (end - start > 60) {
            throw std::runtime_error("Request timed out");
        }
    }
    throw std::runtime_error("Impossible");
}

Json::Value sxd_client::send_and_receive(const Json::Value& data_s, short module_s, short action_s, short module_r0, short action_r0, std::function<bool(const Json::Value&)> f) {
    short module_r, action_r;
    Json::Value data_r;
    this->send_frame(data_s, module_s, action_s);
    auto start = std::time(NULL);
    for (;;) {
        this->receive_frame(data_r, module_r, action_r);
        if (module_r0 == module_r && action_r0 == action_r && (!f || f(data_r)))
            return data_r;
        auto end = std::time(NULL);
        if (end - start > 60) {
            throw std::runtime_error("Request timed out");
        }
    }
    throw std::runtime_error("Impossible");
}


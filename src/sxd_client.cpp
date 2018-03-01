#include <regex>
#include <ctime>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/format.hpp>
#include <boost/asio.hpp>

#include "protocol.h"
#include "common.h"
#include "sxd_client.h"

sxd_client::sxd_client(const std::string& version) :
        resolver(ios), sock(ios), version(version) {
}

sxd_client::~sxd_client() {
    sock.close();
}

void sxd_client::connect(const std::string& host, const std::string& port) {
    this->host = host;
    this->port = port;
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
    common::log(boost::str(boost::format("     Send method:  %1%.%2%(%3%,%4%)") % protocol["class"] % protocol["method"] % protocol["module"] % protocol["action"]), false);
    common::log(boost::str(boost::format("     Send pattern: %1%") % protocol["request"]), false);
    common::log(boost::str(boost::format("     Send data:    %1%") % std::regex_replace(data.toStyledString(), std::regex("[\n\t]+"), " ")), false);
} //send_frame

void sxd_client::receive_frame(Json::Value& data, short& module, short& action) {
    // stream
    boost::asio::streambuf response;
    std::istream response_stream(&response);
    // read frame size
    boost::asio::read(sock, response, boost::asio::transfer_exactly(4));
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
    common::log(boost::str(boost::format("  Receive method:  %1%.%2%(%3%,%4%)") % protocol["class"] % protocol["method"] % protocol["module"] % protocol["action"]), false);
    common::log(boost::str(boost::format("  Receive pattern: %1%") % protocol["response"]), false);
    common::log(boost::str(boost::format("  Receive data:    %1%") % std::regex_replace(data.toStyledString(), std::regex("[\n\t]+"), " ")), false);
} //receive_frame

Json::Value sxd_client::send_and_receive(const Json::Value& data_s, short module_s, short action_s) {
    short module_r, action_r;
    Json::Value data_r;
    this->send_frame(data_s, module_s, action_s);
    /*#define MAX_LOOP 100
     for (int i = 0; i < MAX_LOOP; i++) {
     this->receive_frame(data_r, module_r, action_r);
     if (module_s == module_r && action_s == action_r)
     return data_r;
     }
     throw std::runtime_error("MAX_LOOP");*/
    auto start = std::time(NULL);
    for (;;) {
        this->receive_frame(data_r, module_r, action_r);
        if (module_s == module_r && action_s == action_r)
            return data_r;
        auto end = std::time(NULL);
        if (end - start > 10) {
            throw std::runtime_error("Request timed out");
        }
    }
    throw std::runtime_error("Impossible");
}


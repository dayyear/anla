#include <iostream>
#include <fstream>
#include <regex>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>

#include "protocol.h"
#include "common.h"
#include "test.h"
#include "database.h"
#include "sxd_web_ssl.h"

test::test() {
}

test::~test() {
}

void test::protocol_encode_frame() {
    try {
        Json::Value pattern, data;
        std::istringstream("[\"Utils.UByteUtil\", \"Utils.ByteUtil\", \"Utils.ShortUtil\", \"Utils.IntUtil\", \"Utils.LongUtil\", \"Utils.StringUtil\"]") >> pattern;
        std::istringstream("[1, 2, 258, 16909060, 72623859790382856, \"Utils.StringUtil\"]") >> data;

        std::stringstream ss;
        protocol::encode_frame(ss, data, pattern);
        common::write_file("protocol_test.txt", ss.str());

        protocol::decode_frame(ss, data, pattern);
        Json::StreamWriterBuilder builder;
        builder["commentStyle"] = "None";
        std::cout << Json::writeString(builder, data) << '\n';
        std::cout << std::regex_replace(data.toStyledString(), std::regex("[\n\t]+"), " ") << '\n';

    } catch (const std::exception& ex) {
        std::cerr << "发现错误：" << ex.what() << '\n';
    }
}


void test::zlib() {
    // compress
    std::vector<char> v;
    {
        boost::iostreams::back_insert_device<std::vector<char>> snk { v };
        boost::iostreams::filtering_ostream os;
        os.push(boost::iostreams::zlib_compressor());
        os.push(snk);
        os << "Boost" << std::flush;
        os.pop();
    }

    // decompress
    {
        boost::iostreams::array_source src { v.data(), v.size() };
        boost::iostreams::filtering_istream is;
        is.push(boost::iostreams::zlib_decompressor());
        is.push(src);
        std::string s;
        is >> s;
        std::cout << s << '\n';
    }

    // decompress a file
    {
        std::string frame = common::read_file("frame.txt");
        boost::iostreams::array_source src { frame.c_str(), frame.size() };
        boost::iostreams::filtering_istream is;
        is.push(boost::iostreams::zlib_decompressor());
        is.push(src);

        short module = common::read_int16(is);
        short action = common::read_int16(is);
        Json::Value pattern;
        std::istringstream(db.get_protocol("R170", module, action)["response"]) >> pattern;

        std::cout << pattern << std::endl;
        Json::Value data = protocol::decode_frame(is, pattern);
        std::cout << data << std::endl;
    }
}

void test::https() {
    try {
        sxd_web_ssl web;
        //web.connect("theboostcpplibraries.com", "443");
        //std::cout << web.get("https://theboostcpplibraries.com/boost.asio-network-programming");
        web.connect("www.baidu.com", "443");
        std::cout << web.get("https://www.baidu.com/");
    } catch (const std::exception& ex) {
        std::cerr << "发现错误：" << ex.what() << '\n';
    }
}


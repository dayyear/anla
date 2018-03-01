#include <iostream>
#include <fstream>
#include <regex>

#include <boost/date_time.hpp>

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

void test::chrono_time_point() {
    try {
        // http://www.boost.org/doc/libs/1_65_1/doc/html/date_time/date_time_io.html#date_time.format_flags
        std::cout << "--boost::posix_time test--" << '\n';
        boost::posix_time::ptime now(boost::posix_time::microsec_clock::local_time());
        std::cout << std::setw(40) << "now: " << now << '\n';
        std::cout << std::setw(40) << "to_simple_string: " << boost::posix_time::to_simple_string(now) << '\n';
        std::cout << std::setw(40) << "to_iso_string: " << boost::posix_time::to_iso_string(now) << '\n';
        std::cout << std::setw(40) << "to_iso_string.substr(9, 6): " << boost::posix_time::to_iso_string(now).substr(9, 6) << '\n';
        std::cout << std::setw(40) << "to_iso_extended_string: " << boost::posix_time::to_iso_extended_string(now) << '\n';
        std::cout << std::setw(40) << "to_iso_extended_string.substr(11, 8): " << boost::posix_time::to_iso_extended_string(now).substr(11, 8) << '\n';

        std::cout << std::setw(40) << "fractional_seconds: " << now.time_of_day().fractional_seconds() << '\n';

        boost::posix_time::time_facet* tf = new boost::posix_time::time_facet();
        std::cout.imbue(std::locale(std::locale::classic(), tf));
        for (char c = 'a'; c <= 'z'; c++) {
            std::ostringstream oss;
            oss << "%" << c << "-%" << (char) (c - 32);
            tf->format(oss.str().c_str());
            std::cout << std::setw(40) << oss.str() + ": " << now << '\n';
        }

        now = boost::posix_time::from_iso_string("20181112T010203.456");
        std::cout << std::setw(40) << "%Y-%m-%d %H:%M:%S: " << common::to_string(now, "%Y-%m-%d %H:%M:%S") << '\n';
        std::cout << std::setw(40) << "%y-%m-%d %H:%M:%s: " << common::to_string(now, "%y-%m-%d %H:%M:%s") << '\n';
    } catch (const std::exception& ex) {
        std::cerr << "发现错误：" << ex.what() << '\n';
    }
    //boost::date_time::

    //boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
    //boost::posix_time::time_duration t = now.time_of_day();
    //std::cout << t << '\n';
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


#include <iostream>
#include <memory>
#include <string>
#include <boost/asio.hpp>
#include "protocol.h"
#include "common.h"

protocol::protocol() {
}

protocol::~protocol() {
}

void protocol::encode_frame(std::ostream& os, const Json::Value& data, const Json::Value& pattern) {
    if (!data.isArray() && !data.isNull())
        throw std::runtime_error("data is not an array json");
    if (!pattern.isArray() && !pattern.isNull())
        throw std::runtime_error("pattern is not an array json");
    if (data.size() != pattern.size())
        throw std::runtime_error("data and pattern are not same size");

    for (unsigned int i = 0; i < pattern.size(); i++) {
        if (pattern[i].isString()) {
            std::string item = pattern[i].asString();
            if (item.compare("Utils.UByteUtil") == 0) {
                common::write_int8(os, (int8_t) data[i].asInt());
            } else if (item.compare("Utils.ByteUtil") == 0) {
                common::write_int8(os, (int8_t) data[i].asInt());
            } else if (item.compare("Utils.ShortUtil") == 0) {
                common::write_int16(os, (int16_t) data[i].asInt());
            } else if (item.compare("Utils.IntUtil") == 0) {
                common::write_int32(os, (int32_t) data[i].asInt());
            } else if (item.compare("Utils.LongUtil") == 0) {
                common::write_int64(os, (int64_t) data[i].asInt64());
            } else if (item.compare("Utils.StringUtil") == 0) {
                common::write_string(os, data[i].asString());
            } else {
                throw std::runtime_error("Unknown item string: " + item);
            }
        } else if (pattern[i].isArray()) {
            int length = data[i].size();
            common::write_int16(os, (int16_t) length);
            for (int j = 0; j < length; j++)
                encode_frame(os, data[i][j], pattern[i]);
        } else {
            throw std::runtime_error("unknown item string: " + pattern[i].asString());
        }
    }
} //encode_frame

void protocol::decode_frame(std::istream& is, Json::Value& data, const Json::Value& pattern) {
    data.clear();
    for (unsigned int i = 0; i < pattern.size(); i++) {
        if (pattern[i].isString()) {
            std::string item = pattern[i].asString();
            if (item.compare("Utils.UByteUtil") == 0) {
                data.append(common::read_int8(is));
            } else if (item.compare("Utils.ByteUtil") == 0) {
                data.append(common::read_int8(is));
            } else if (item.compare("Utils.ShortUtil") == 0) {
                data.append(common::read_int16(is));
            } else if (item.compare("Utils.IntUtil") == 0) {
                data.append(common::read_int32(is));
            } else if (item.compare("Utils.LongUtil") == 0) {
                data.append(common::read_int64(is));
            } else if (item.compare("Utils.StringUtil") == 0) {
                data.append(common::read_string(is));
            } else {
                throw std::runtime_error("Unknown item string: " + item);
            }
        } else if (pattern[i].isArray()) {
            Json::Value pattern_ = pattern[i];
            Json::Value data_;
            int16_t length = common::read_int16(is);
            for (int i = 0; i < length; i++)
                data_.append(decode_frame(is, pattern_));
            data.append(data_);
        } else {
            throw std::runtime_error("unknown item string: " + pattern[i].asString());
        }
    }
} //decode_frame

Json::Value protocol::decode_frame(std::istream& is, const Json::Value& pattern) {
    Json::Value data;
    decode_frame(is, data, pattern);
    return data;
}

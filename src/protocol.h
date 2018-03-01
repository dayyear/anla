#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <vector>
#include <json/json.h>

class protocol {
public:
    protocol();
    virtual ~protocol();
    static void encode_frame(std::ostream& os, const Json::Value& data, const Json::Value& pattern);
    static void decode_frame(std::istream& is, Json::Value& data, const Json::Value& pattern);
    static Json::Value decode_frame(std::istream& is, const Json::Value& pattern);
};

#endif /* PROTOCOL_H_ */

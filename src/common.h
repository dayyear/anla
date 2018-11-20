#ifndef COMMON_H_
#define COMMON_H_

#include <iostream>
#include <string>

class common {
public:
    common(void);
    ~common(void);

    // 字符编码转换
    static std::string utf2gbk(const char* s);
    static std::string utf2gbk(const std::string& s);
    static std::string gbk2utf(const char* s);
    static std::string gbk2utf(const std::string& s);

    // uri编码解码
    static std::string uri_decode(const std::string& s);
    static std::string uri_encode(const std::string& s);

    // 文本文件读写
    static std::string read_file(const std::string& file_name);
    static void write_file(const std::string& file_name, const std::string& s);

    // 字节顺序反转
    static int16_t reverse_byte_order(int16_t x);
    static int32_t reverse_byte_order(int32_t x);
    static int64_t reverse_byte_order(int64_t x);

    // 读写流
    static int8_t read_int8(std::istream& is);
    static int16_t read_int16(std::istream& is);
    static int32_t read_int32(std::istream& is);
    static int64_t read_int64(std::istream& is);
    static std::string read_string(std::istream& is);
    static std::string read_string(std::istream& is, int16_t length);
    static void write_int8(std::ostream& os, int8_t i);
    static void write_int16(std::ostream& os, int16_t i);
    static void write_int32(std::ostream& os, int32_t i);
    static void write_int64(std::ostream& os, int64_t i);
    static void write_string(std::ostream& os, const std::string& str);

    // 时间日期格式化
    static std::string to_string(const std::time_t t, const char* format);

    // 日志
    static void log(const std::string& message, int hwnd = -1, bool file = true, bool time = true);

    // 其他
    static bool contain(const std::vector<std::string>& v, const std::string& s);
};

#endif /* COMMON_H_ */

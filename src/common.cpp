#include <fstream>
#include <memory>
#include <sstream>
#include <regex>
#include <windows.h>

#include <boost/locale/encoding.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/filesystem.hpp>

#include "common.h"

common::common(void) {
}

common::~common(void) {
}

std::string common::utf2gbk(const char* s) {
    return boost::locale::conv::between(s, "gbk", "utf-8");
}

std::string common::utf2gbk(const std::string& s) {
    return utf2gbk(s.c_str());
}

std::string common::gbk2utf(const char* s) {
    return boost::locale::conv::between(s, "utf-8", "gbk");
}

std::string common::gbk2utf(const std::string& s) {
    return gbk2utf(s.c_str());
}

const char HEX2DEC[256] = {
/*       0  1  2  3   4  5  6  7   8  9  A  B   C  D  E  F */
/* 0 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 1 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 2 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 3 */0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1,

/* 4 */-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 5 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 6 */-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 7 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

/* 8 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 9 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* A */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* B */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

/* C */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* D */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* E */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* F */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

std::string common::uri_decode(const std::string& s) {
    // Note from RFC1630:  "Sequences which start with a percent sign
    // but are not followed by two hexadecimal characters (0-9, A-F) are reserved
    // for future extension"

    const unsigned char * pSrc = (const unsigned char *) s.c_str();
    const int SRC_LEN = s.length();
    const unsigned char * const SRC_END = pSrc + SRC_LEN;
    const unsigned char * const SRC_LAST_DEC = SRC_END - 2;   // last decodable '%'

    char * const pStart = new char[SRC_LEN];
    char * pEnd = pStart;

    while (pSrc < SRC_LAST_DEC) {
        if (*pSrc == '%') {
            char dec1, dec2;
            if (-1 != (dec1 = HEX2DEC[*(pSrc + 1)]) && -1 != (dec2 = HEX2DEC[*(pSrc + 2)])) {
                *pEnd++ = (dec1 << 4) + dec2;
                pSrc += 3;
                continue;
            }
        }

        *pEnd++ = *pSrc++;
    }

    // the last 2- chars
    while (pSrc < SRC_END)
        *pEnd++ = *pSrc++;

    std::string sResult(pStart, pEnd);
    delete[] pStart;
    return sResult;
}

// Only alphanum is safe.
const char SAFE[256] = {
/*      0 1 2 3  4 5 6 7  8 9 A B  C D E F */
/* 0 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 1 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 2 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 3 */1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,

/* 4 */0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 5 */1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
/* 6 */0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 7 */1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,

/* 8 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 9 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* A */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* B */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

/* C */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* D */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* E */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* F */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

std::string common::uri_encode(const std::string& s) {
    const char DEC2HEX[16 + 1] = "0123456789ABCDEF";
    const unsigned char * pSrc = (const unsigned char *) s.c_str();
    const int SRC_LEN = s.length();
    unsigned char * const pStart = new unsigned char[SRC_LEN * 3];
    unsigned char * pEnd = pStart;
    const unsigned char * const SRC_END = pSrc + SRC_LEN;

    for (; pSrc < SRC_END; ++pSrc) {
        if (SAFE[*pSrc])
            *pEnd++ = *pSrc;
        else {
            // escape this char
            *pEnd++ = '%';
            *pEnd++ = DEC2HEX[*pSrc >> 4];
            *pEnd++ = DEC2HEX[*pSrc & 0x0F];
        }
    }

    std::string sResult((char *) pStart, (char *) pEnd);
    delete[] pStart;
    return sResult;
}

std::string common::read_file(const std::string& file_name) {
    std::ifstream file(file_name, std::ios::binary | std::ios::in);
    std::string s((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    return s;
}

void common::write_file(const std::string& file_name, const std::string& s) {
    std::ofstream file(file_name, std::ios::binary | std::ios::out);
    file << s;
    file.close();
}

int16_t common::reverse_byte_order(int16_t x) {
    return ((x >> 8) & 0x00FF) | ((x << 8) & 0xFF00);
}

int32_t common::reverse_byte_order(int32_t x) {
    return ((x >> 24) & 0x000000FF) | ((x >> 8) & 0x0000FF00) | ((x << 8) & 0x00FF0000) | ((x << 24) & 0xFF000000);
}

int64_t common::reverse_byte_order(int64_t x) {
    return ((x >> 56) & 0x00000000000000FF) | ((x >> 40) & 0x000000000000FF00) | ((x >> 24) & 0x0000000000FF0000) | ((x >> 8) & 0x00000000FF000000) | ((x << 8) & 0x000000FF00000000) | ((x << 24) & 0x0000FF0000000000) | ((x << 40) & 0x00FF000000000000) | ((x << 56) & 0xFF00000000000000);
}

int8_t common::read_int8(std::istream& is) {
    int8_t i;
    is.read((char*) &i, 1);
    return i;
}

int16_t common::read_int16(std::istream& is) {
    int16_t i;
    is.read((char*) &i, 2);
    return reverse_byte_order(i);
}

int32_t common::read_int32(std::istream& is) {
    int32_t i;
    is.read((char*) &i, 4);
    return reverse_byte_order(i);
}

int64_t common::read_int64(std::istream& is) {
    int64_t i;
    is.read((char*) &i, 8);
    return reverse_byte_order(i);
}

std::string common::read_string(std::istream& is) {
    return read_string(is, read_int16(is));
}

std::string common::read_string(std::istream& is, int16_t length) {
    /*std::unique_ptr<char[]> str(new char[length + 1]);
     is.read(str.get(), length);
     str[length] = '\0';
     return std::string(str.get());*/
    std::unique_ptr<char[]> str(new char[length]);
    is.read(str.get(), length);
    return std::string(str.get(), length);
}

void common::write_int8(std::ostream& os, int8_t i) {
    os.write((char*) &i, 1);
}

void common::write_int16(std::ostream& os, int16_t i) {
    i = reverse_byte_order(i);
    os.write((char*) &i, 2);
}

void common::write_int32(std::ostream& os, int32_t i) {
    i = reverse_byte_order(i);
    os.write((char*) &i, 4);
}

void common::write_int64(std::ostream& os, int64_t i) {
    i = reverse_byte_order(i);
    os.write((char*) &i, 8);
}

void common::write_string(std::ostream& os, const std::string& str) {
    write_int16(os, str.size());
    os.write(str.c_str(), str.size());
}

std::string common::to_string(const boost::posix_time::ptime& pt, const char* format) {
    // http://www.boost.org/doc/libs/1_65_1/doc/html/date_time/date_time_io.html#date_time.format_flags
    //                 012345678901234
    // microsec_clock: 20180205T200317.430210
    // second_clock:   20180205T205831
    std::string str(format);
    std::string iso_string = boost::posix_time::to_iso_string(pt);
    str = std::regex_replace(str, std::regex("%Y"), iso_string.substr(0, 4));
    str = std::regex_replace(str, std::regex("%y"), iso_string.substr(2, 2));
    str = std::regex_replace(str, std::regex("%m"), iso_string.substr(4, 2));
    str = std::regex_replace(str, std::regex("%d"), iso_string.substr(6, 2));
    str = std::regex_replace(str, std::regex("%H"), iso_string.substr(9, 2));
    str = std::regex_replace(str, std::regex("%M"), iso_string.substr(11, 2));
    str = std::regex_replace(str, std::regex("%S"), iso_string.substr(13, 2));
    str = std::regex_replace(str, std::regex("%s"), iso_string.substr(13));

    return str;
}

void common::log(const std::string& message, int hwnd, bool file, bool time) {
    boost::posix_time::ptime now(boost::posix_time::second_clock::local_time());
    std::string path = "log";
    boost::filesystem::create_directory(path);
    if (hwnd) {
        std::ofstream ofile(path + "\\" + to_string(now, "%Y-%m-%d.txt"), std::ios::binary | std::ios::out | std::ios::app);
        if (time) {
            if (hwnd < 0)
                std::cout << to_string(now, "%H:%M:%S") << " ";
            else {
                SendMessage((HWND) hwnd, EM_SETSEL, SendMessage((HWND) hwnd, WM_GETTEXTLENGTH, 0, 0), SendMessage((HWND) hwnd, WM_GETTEXTLENGTH, 0, 0));
                SendMessage((HWND) hwnd, EM_REPLACESEL, 0, (LPARAM) (to_string(now, "%H:%M:%S") + " ").c_str());
            }
            ofile << to_string(now, "%H:%M:%S") << " ";
        }
        if (hwnd < 0)
            std::cout << message << std::endl;
        else {
            SendMessage((HWND) hwnd, EM_SETSEL, SendMessage((HWND) hwnd, WM_GETTEXTLENGTH, 0, 0), SendMessage((HWND) hwnd, WM_GETTEXTLENGTH, 0, 0));
            SendMessage((HWND) hwnd, EM_REPLACESEL, 0, (LPARAM) (message + "\r\n").c_str());
        }
        ofile << message << std::endl;
    }
    if (file) {
        std::ofstream ofile(path + "\\" + to_string(now, "%Y-%m-%d.log"), std::ios::binary | std::ios::out | std::ios::app);
        if (time)
            ofile << to_string(now, "%H:%M:%S") << " ";
        ofile << message << std::endl;
        ofile.close();
    }
}

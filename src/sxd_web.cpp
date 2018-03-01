#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include "sxd_web.h"

sxd_web::sxd_web() :
        sock(ios) {
}

sxd_web::~sxd_web() {
    sock.close();
}

void sxd_web::connect(const std::string& host, const std::string& port) {
    this->host = host;
    this->port = port;
    boost::asio::ip::tcp::resolver resolver(ios);
    boost::asio::ip::tcp::resolver::query query(host, port);
    boost::asio::connect(sock, resolver.resolve(query));
} //sxd_web::connect

void sxd_web::service(const std::string& method, const std::string& request_uri, const std::string& request_http_version, const std::string& request_header, const std::string& content, std::string& response_http_version, unsigned& status_code, std::string& status_message, std::string& response_header, std::string& response_body) {
    // -------------------------------------------------------------------------------------------
    // 0. prepare
    // -------------------------------------------------------------------------------------------
    // method
    if (method != "GET" && method != "POST")
        throw std::runtime_error("未知的 [method]: " + method);
    // stream
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    boost::asio::streambuf response;
    std::istream response_stream(&response);
    // Transfer-Encoding
    std::string transfer_encoding("");
    // Content-Length
    int content_length(-1);

    // -------------------------------------------------------------------------------------------
    // 1. write
    // -------------------------------------------------------------------------------------------
    request_stream << method << " " << request_uri << " " << request_http_version << "\r\n";
    request_stream << "Host: " << host << "\r\n";
    request_stream << "User-Agent: " << "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/55.0.2883.87 Safari/537.36" << "\r\n";
    if (method == "GET") {
        request_stream << request_header << "\r\n";
    }
    else if (method == "POST") {
        request_stream << "Content-Type: " << "application/x-www-form-urlencoded" << "\r\n";
        request_stream << "Content-Length: " << content.size() << "\r\n";
        request_stream << request_header << "\r\n";
        request_stream << content;
    }
    boost::asio::write(sock, request);

    // -------------------------------------------------------------------------------------------
    // 2. read status line
    // -------------------------------------------------------------------------------------------
    boost::asio::read_until(sock, response, "\r\n");
    response_stream >> response_http_version;
    response_stream >> status_code;
    std::getline(response_stream, status_message);
    boost::algorithm::trim(status_message);

    // -------------------------------------------------------------------------------------------
    // 3. read response header
    // -------------------------------------------------------------------------------------------
    std::ostringstream osstream;
    boost::asio::read_until(sock, response, "\r\n\r\n");
    boost::regex transfer_encoding_regex("Transfer-Encoding: (.*)");
    boost::regex content_length_regex("Content-Length: (.*)");
    std::string header;
    while (std::getline(response_stream, header) && header != "\r") {
        boost::algorithm::trim(header);
        boost::smatch match;
        osstream << header << "\r\n";
        if (regex_search(header, match, transfer_encoding_regex))
            transfer_encoding = boost::lexical_cast<std::string>(match[1]);
        if (regex_search(header, match, content_length_regex))
            content_length = boost::lexical_cast<int>(match[1]);
    }
    response_header = osstream.str();

    // -------------------------------------------------------------------------------------------
    // 4. read response body
    // -------------------------------------------------------------------------------------------
    osstream.str("");
    if (transfer_encoding == "chunked") {
        content_length = 0;
        size_t chunk_size;
        do {
            boost::asio::read_until(sock, response, "\r\n");
            std::string chunk_size_string;
            std::getline(response_stream, chunk_size_string);
            chunk_size = std::stoi(chunk_size_string, nullptr, 16);
            content_length += chunk_size;

            if (chunk_size + 2 > response.size())
                boost::asio::read(sock, response, boost::asio::transfer_exactly(chunk_size + 2 - response.size()));
            std::unique_ptr<char[]> buff(new char[chunk_size + 2]);
            response_stream.read(buff.get(), chunk_size + 2);
            buff[chunk_size] = '\0';
            osstream << buff.get();
        } while (chunk_size > 0);
        response_body = osstream.str();
    } else if (content_length >= 0) {
        if ((size_t) content_length > response.size())
            boost::asio::read(sock, response, boost::asio::transfer_exactly(content_length - response.size()));
        std::unique_ptr<char[]> buff(new char[content_length + 1]);
        response_stream.read(buff.get(), content_length);
        buff[content_length] = '\0';
        osstream << buff.get();
        response_body = osstream.str();
    } else
        throw std::runtime_error("未知的[Content-Length]");

    // -------------------------------------------------------------------------------------------
    // E. response body verification
    // -------------------------------------------------------------------------------------------
    if (response_body.length() != (size_t) content_length)
        throw std::runtime_error("[Content-Length]与实际[Response-Body]长度不一致");

} //sxd_web::service

std::string sxd_web::get(const std::string& request_uri, const std::string& request_header) {
    std::string response_http_version;
    unsigned int status_code;
    std::string status_message;
    std::string response_header;
    std::string response_body;

    this->service("GET", request_uri, "HTTP/1.1", request_header, "", response_http_version, status_code, status_message, response_header, response_body);
    return response_body;
}

std::string sxd_web::post(const std::string& request_uri, const std::string& request_header, const std::string& content){
    std::string response_http_version;
    unsigned int status_code;
    std::string status_message;
    std::string response_header;
    std::string response_body;

    this->service("POST", request_uri, "HTTP/1.1", request_header, content, response_http_version, status_code, status_message, response_header, response_body);
    return response_body;
}

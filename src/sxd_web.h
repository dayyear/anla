#ifndef SXD_WEB_H_
#define SXD_WEB_H_

#include <string>
#include <boost/asio.hpp>

class sxd_web {
public:
    sxd_web();
    virtual ~sxd_web();

    void connect(const std::string& host, const std::string& port);
    void service(const std::string& method, const std::string& request_uri, const std::string& request_http_version, const std::string& request_header, const std::string& content, std::string& response_http_version, unsigned int& status_code, std::string& status_message, std::string& response_header, std::string& response_body);
    std::string get(const std::string& request_uri, const std::string& request_header = "");
    std::string post(const std::string& request_uri, const std::string& request_header = "", const std::string& content="");

private:
    boost::asio::io_service ios;
    boost::asio::ip::tcp::socket sock;
    std::string host;
    std::string port;
};

#endif /* SXD_WEB_H_ */

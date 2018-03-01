#ifndef SXD_WEB_SSL_H_
#define SXD_WEB_SSL_H_

#include <string>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

class sxd_web_ssl {
public:
    sxd_web_ssl();
    virtual ~sxd_web_ssl();

    void connect(const std::string& host, const std::string& port);
    void service(const std::string& method, const std::string& request_uri, const std::string& request_http_version, const std::string& request_header, const std::string& content, std::string& response_http_version, unsigned int& status_code, std::string& status_message, std::string& response_header, std::string& response_body);
    std::string get(const std::string& request_uri, const std::string& request_header = "");
    std::string post(const std::string& request_uri, const std::string& request_header = "", const std::string& content="");

private:
    bool on_peer_verify(bool preverified, boost::asio::ssl::verify_context& context) {
        return true;
    };

    boost::asio::io_service ios;
    boost::asio::ssl::context ssl_context;
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_stream;
    std::string host;
    std::string port;
};

#endif /* SXD_WEB_SSL_H_ */

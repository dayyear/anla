#ifndef SXD_H_
#define SXD_H_

#include "sxd_client.h"

class sxd {
public:
    sxd();
    virtual ~sxd();

    static void run(std::string arg, bool auto_exit);
    static void login();
    static void analyze();
    static void collect();

private:
    static void auto_play(const std::string& version, const std::string& user_id, const std::string& url, const std::string& cookie);
    static void batch_fate(const std::string& version, const std::string& user_id, const std::string& url, const std::string& cookie);

    static void collect_protocol(const std::string& version, const std::string& path);
    static void collect_end_function_gift(const std::string& version, const std::string& path);
    static void collect_function(const std::string& version, const std::string& path);
    static void collect_gift(const std::string& version, const std::string& path);
    static void collect_item(const std::string& version, const std::string& path);
    static void collect_lucky_shop_item(const std::string& version, const std::string& path);
    static void collect_role(const std::string& version, const std::string& path);
    static void collect_town(const std::string& version, const std::string& path);
    static void collect_facture_reel(const std::string& version, const std::string& path);
};

#endif /* SXD_H_ */

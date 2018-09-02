#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_Email_Base {
public:
    static const int GET = 4;
    static const int UNGET = 5;
    static const int SUCCESS = 8;
    static const int ONE = 14;
};

void sxd_client::email() {
    Json::Value data = this->Mod_Email_Base_get_email_info();
    Json::Value email_info = data;
    for (const auto& email : email_info[0]) {
        if (email[8].asInt() == Mod_Email_Base::UNGET && email[7].size()) {
            data = this->Mod_Email_Base_get_attachment_award(email[0].asInt());
            if (data[0].asInt() != Mod_Email_Base::SUCCESS) {
                common::log(boost::str(boost::format("【邮箱】收取附件失败，result[%1%]") % data[0]), iEdit);
                continue;
            }
            std::ostringstream oss;
            for (const auto& item : email[7])
                oss << "[" << db.get_code(version, "Item", item[0].asInt())["text"] << "×" << item[1] << "]，";
            common::log(boost::str(boost::format("【邮箱】收取附件 %1%") % oss.str().substr(0, oss.str().size() - 2)), iEdit);
        }
        data = this->Mod_Email_Base_delete_email(Mod_Email_Base::ONE, email[0].asInt());
        if (data[0].asInt() != Mod_Email_Base::SUCCESS) {
            common::log(boost::str(boost::format("【邮箱】删除邮件 [%1%] 失败，result[%2%]") % common::utf2gbk(email[1].asString()) % data[0]), iEdit);
            return;
        }
        common::log(boost::str(boost::format("【邮箱】删除邮件 [%1%]") % common::utf2gbk(email[1].asString())), iEdit);
    }

}

//============================================================================
// R171
// 邮件
// {module:345, action:0,
// request:[], response:[[Utils.IntUtil, Utils.StringUtil, Utils.StringUtil, Utils.StringUtil, Utils.IntUtil, Utils.UByteUtil, Utils.UByteUtil, [Utils.IntUtil, Utils.IntUtil], Utils.UByteUtil, Utils.LongUtil, Utils.UByteUtil, Utils.ByteUtil]]}
// EmailData.as 36:
//     oObject.list(_loc_2, _loc_3, ["id", "name", "subject", "content", "receipt_time",
//         "status", "type", "attachment_award_list", "attachment_status", "valid_time", "delete_flag", "send_type"]);
//     for each (_loc_5 in _loc_3.attachment_award_list)
//         oObject.list(_loc_5, _loc_6, ["item_id", "item_num"]);
// Example
// 未收取(attachment_status[8]=5)
//     [ [ [ 55340, "\u8bb8\u613f\u6c60\u5956\u52b1", "\u8bb8\u613f\u6c60\u6bcf\u65e5\u793c\u5305", "\u60a8\u5728\u8bb8\u613f\u6c60\u6d3b\u52a8\u83b7\u5f97\u4e86\u6bcf\u65e5\u793c\u5305\u5956\u52b1\uff0c\u8bf7\u53ca\u65f6\u67e5\u6536\u3002", 1519488016,
//         0, 3, [ [ 5598, 1 ] ], 5, 1520352016, 6, 1 ] ] ]
// 收取(attachment_status[8]=4)
//     [ [ [ 55340, "\u8bb8\u613f\u6c60\u5956\u52b1", "\u8bb8\u613f\u6c60\u6bcf\u65e5\u793c\u5305", "\u60a8\u5728\u8bb8\u613f\u6c60\u6d3b\u52a8\u83b7\u5f97\u4e86\u6bcf\u65e5\u793c\u5305\u5956\u52b1\uff0c\u8bf7\u53ca\u65f6\u67e5\u6536\u3002", 1519488016,
//         0, 3, [ [ 5598, 1 ] ], 4, 1520352016, 6, 1 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_Email_Base_get_email_info() {
    Json::Value data;
    return this->send_and_receive(data, 345, 0);
}

//============================================================================
// R171
// 收取
// {module:345, action:2,
// request:[Utils.IntUtil],
// Example
//     [ 55340 ]
// response:[Utils.UByteUtil]}
// Example
//     [ 8 ]
//============================================================================
Json::Value sxd_client::Mod_Email_Base_get_attachment_award(int id) {
    Json::Value data;
    data.append(id);
    return this->send_and_receive(data, 345, 2);
}

//============================================================================
// R171
// 删除
// {module:345, action:3,
// request:[Utils.UByteUtil, Utils.IntUtil],
// Example
//     [ 14, 55340 ] Mod_Email_Base.ONE=14
// response:[Utils.UByteUtil]}
// Example
//     [ 8 ]
//============================================================================
Json::Value sxd_client::Mod_Email_Base_delete_email(int type, int id) {
    Json::Value data;
    data.append(type);
    data.append(id);
    return this->send_and_receive(data, 345, 3);
}

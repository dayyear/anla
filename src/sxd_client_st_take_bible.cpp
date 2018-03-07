#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_StTakeBible_Base {
public:
    static const int SUCCESS = 9;
};

// 参考圣域取经
void sxd_client::st_take_bible() {
    std::string protectors[] = { "", "白龙马", "沙悟净", "猪八戒", "孙悟空", "唐僧" };
    // first get
    Json::Value data = this->Mod_StTakeBible_Base_get_take_bible_info();
    Json::Value bible_info = data;
    if (bible_info[2].asInt() == bible_info[3].asInt()) {
        common::log("【仙界取经】次数已用完");
        return;
    }
    if (bible_info[6].asInt() == 0) {
        data = this->Mod_StTakeBible_Base_refresh();
        if (data[0].asInt() != Mod_StTakeBible_Base::SUCCESS) {
            common::log(boost::str(boost::format("【仙界取经】刷新取经使者失败，msg[%1%]") % data[0]));
            return;
        }
        common::log(boost::str(boost::format("【仙界取经】刷新取经使者，获得 [%1%]") % protectors[data[1].asInt()]));
    }
    // second get
    bible_info = this->Mod_StTakeBible_Base_get_take_bible_info();
    if (bible_info[6].asInt() == 0) {
        common::log(boost::str(boost::format("【仙界取经】数据异常，can_protection[%1%]") % bible_info[6]));
        return;
    }
    if (bible_info[5].asInt() == 0) {
        data = this->Mod_StTakeBible_Base_start_take_bible();
        if (data[0].asInt() != Mod_StTakeBible_Base::SUCCESS) {
            common::log(boost::str(boost::format("【仙界取经】护送失败，msg[%1%]") % data[0]));
            return;
        }
        common::log(boost::str(boost::format("【仙界取经】开始护送，取经使者 [%1%]") % protectors[bible_info[6].asInt()]));
    }
}

//============================================================================
// R172 护送取经操作面板
// {module:114, action:2, request:[],
// response:[[Utils.UByteUtil, Utils.ByteUtil, Utils.IntUtil, Utils.ShortUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.IntUtil], [Utils.IntUtil, Utils.StringUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ByteUtil], Utils.ByteUtil, Utils.ByteUtil, Utils.ShortUtil, Utils.ByteUtil, Utils.UByteUtil, Utils.StringUtil, Utils.ByteUtil, Utils.ByteUtil]};
// 参考圣域取经
//============================================================================
Json::Value sxd_client::Mod_StTakeBible_Base_get_take_bible_info() {
    Json::Value data;
    return this->send_and_receive(data, 114, 2);
}

//============================================================================
// R172 刷新取经使者
// {module:114, action:8, request:[],
// response:[Utils.UByteUtil, Utils.UByteUtil, Utils.ShortUtil, Utils.ByteUtil]};
// 参考圣域取经
//============================================================================
Json::Value sxd_client::Mod_StTakeBible_Base_refresh() {
    Json::Value data;
    return this->send_and_receive(data, 114, 8);
}

//============================================================================
// R172 开始护送
// {module:114, action:10, request:[],
// response:[Utils.UByteUtil, Utils.ByteUtil]};
// 参考圣域取经
//============================================================================
Json::Value sxd_client::Mod_StTakeBible_Base_start_take_bible() {
    Json::Value data;
    return this->send_and_receive(data, 114, 10);
}

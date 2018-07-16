#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_Training_Base {
public:
    static const int GENERAL = 0;       // 普通培养，xxx铜钱
    static const int STRENGTHEN = 1;    // 加强培养，2元宝
    static const int FAVORITEM = 5;     // 喜好品培养，女娲石碎片×1
    static const int SUCCEED = 6;
    static const int NOENOUGHITEM = 10;
};

void sxd_client::training() {
    // config
    std::string config = db.get_config(user_id.c_str(), "Training");
    common::log(boost::str(boost::format("【Training】[%1%]") % config), 0);
    // roles
    Json::Value data = this->Mod_Role_Base_get_role_list(player_id);
    auto roles = data[14];
    bool find = false;
    for (const auto& role : roles) {
        std::string role_name = common::utf2gbk(role[0].asString());
        int player_role_id = role[2].asInt();
        if (role_name.find(config) != std::string::npos) {
            find = true;
            data = this->Mod_Training_Base_panel_show(player_role_id);
            int level = data[1].asInt();
            int elixir_soul_upper = data[9].asInt();
            int base_data0 = data[8][0][0].asInt();
            int base_data1 = data[8][0][1].asInt();
            int base_data2 = data[8][0][2].asInt();
            int training_upper = level * 4 + 20 + elixir_soul_upper;
            if (training_upper == base_data0 && training_upper == base_data1 && training_upper == base_data2) {
                common::log(boost::str(boost::format("【培养】[%1%] 已达到培养上限[%2%]") % role_name % training_upper));
                return;
            }

            for (;;) {
                data = this->Mod_Training_Base_training(player_role_id, Mod_Training_Base::FAVORITEM);
                if (data[0].asInt() == Mod_Training_Base::SUCCEED) {
                    this->Mod_Training_Base_modify_role_data(player_role_id);
                    common::log(boost::str(boost::format("【培养】[%1%] 喜好品培养，武力[%2%]，绝技[%3%]，法力[%4%]") % role_name % data[1] % data[2] % data[3]));
                    if (training_upper == data[1].asInt() && training_upper == data[2].asInt() && training_upper == data[3].asInt()) {
                        common::log(boost::str(boost::format("【培养】[%1%] 已达到培养上限[%2%]") % role_name % training_upper));
                        return;
                    }
                } else if (data[0].asInt() == Mod_Training_Base::NOENOUGHITEM) {
                    common::log(boost::str(boost::format("【培养】[%1%] 喜好品培养失败，女娲石碎片数量不足") % role_name), 0);
                    return;
                } else {
                    common::log(boost::str(boost::format("【培养】[%1%] 喜好品培养失败，result[%2%]") % role_name % data[0]));
                    return;
                }
            }
            break;
        } //if find training_name
    } //for roles
    if (!find)
        common::log(boost::str(boost::format("【培养】未发现伙伴[%1%]") % config), 0);
}

//============================================================================
// R180 面板
// "module":12,"action":0,"request":[Utils.IntUtil],"response":[Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.StringUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.UByteUtil,[Utils.IntUtil,Utils.IntUtil,Utils.IntUtil],Utils.IntUtil]
// RoleMsgData：
//     oObject.list(param1,_loc2_,["id","lv","coin","role_name","strength","agile","intellect","tranining_button","base_data","elixir_soul_upper"]);
// Example
//     [ 58431 ] --> [ 58431, 257, 150000, "\u795e\u98de\u536b", 2962, 2499, 1218, 1, [ [ 1227, 1228, 1228 ] ], 180 ]
//     [ 58431 ] --> [ 58431, 257, 150000, "\u795e\u98de\u536b", 2962, 2499, 1218, 1, [ [ 1228, 1228, 1228 ] ], 180 ]
//                   257*4 + 20 + 180 = 1228
//============================================================================
Json::Value sxd_client::Mod_Training_Base_panel_show(int player_role_id) {
    Json::Value data;
    data.append(player_role_id);
    return this->send_and_receive(data, 12, 0);
}

//============================================================================
// R171
// 培养
// {module:12, action:1,
// request:[Utils.IntUtil, Utils.UByteUtil],
// Example
//     [ 808183, 5 ] 808183表示培养对象，5表示喜好品培养
// response:[Utils.UByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil]}
// Example
//     [ 6, 322, 360, 298 ] 6表示培养成功，后三项武力、绝技、法术
//============================================================================
Json::Value sxd_client::Mod_Training_Base_training(int player_role_id, int type) {
    Json::Value data;
    data.append(player_role_id);
    data.append(type);
    return this->send_and_receive(data, 12, 1);
}

//============================================================================
// R171
// 保存
// {module:12, action:2,
// request:[Utils.IntUtil],
// Example
//     [ 808183 ]
// response:[Utils.UByteUtil, [Utils.IntUtil, Utils.IntUtil, Utils.IntUtil], Utils.IntUtil]}
// Example
//     [ 6, [ [ 322, 360, 298 ] ], 25 ]
//============================================================================
Json::Value sxd_client::Mod_Training_Base_modify_role_data(int player_role_id) {
    Json::Value data;
    data.append(player_role_id);
    return this->send_and_receive(data, 12, 2);
}

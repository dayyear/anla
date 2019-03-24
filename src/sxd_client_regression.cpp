#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_Memoirs_Base {
public:
    static const int OPEN = 145;  // -111
};

class Mod_Regression_Base {
public:
    static const int SUCCESS = 0;
};

//============================================================================
// 回归活动
// 1. 回归活跃; 2. 回归商店; 3. 回归登录; 4. 回归送礼; 5. 回归任务
//============================================================================
void sxd_client::regression() {
    auto data = this->Mod_Memoirs_Base_is_activity_open();
    auto boards = data[0];
    for (auto& board : boards) {
        auto& board_info = board[0];
        auto board_type = board[1].asInt();
        /* 153: 回归, REGRESSION */
        if (board_type == 153) {
            for (auto& activity : board_info) {
                auto activity_type = activity[0].asInt();
                auto is_open = activity[1].asInt();
                /* 20: 回归活跃, back_assistant */
                if (activity_type == 20 && is_open == Mod_Memoirs_Base::OPEN)
                    back_assistant();
                /* 151: 回归商店, go_back_shop */
                if (activity_type == 151 && is_open == Mod_Memoirs_Base::OPEN)
                    go_back_shop();
                /* 152: 回归登录, go_back_login */
                if (activity_type == 152 && is_open == Mod_Memoirs_Base::OPEN)
                    go_back_login();
                /* 19: 回归送礼, back_send_gift */
                if (activity_type == 19 && is_open == Mod_Memoirs_Base::OPEN)
                    back_send_gift();
                /* 124: 回归任务, go_back_task */
                /*if (activity_type == 124 && is_open == Mod_Memoirs_Base::OPEN)
                 go_back_task();*/
            }
            break;
        }
    }
}

//============================================================================
// 回归活跃
//============================================================================
void sxd_client::back_assistant() {
    for (int id = 1; id <= 6; ++id) {
        auto data = this->Mod_Regression_Base_get_back_assistant_award(id);
        if (data[0].asInt() == Mod_Regression_Base::SUCCESS)
            common::log(boost::str(boost::format("【回归活跃】领取第 [%1%] 个活跃礼包") % id), iEdit);
    }
}

//============================================================================
// 2019030101 回归活跃，领取奖励
// "module":254, "action":6, "request":[Utils.IntUtil], "response":[Utils.UByteUtil,Utils.IntUtil]
// Example
//     [ 1(1-6) ] -> [ 0(Mod_Regression_Base::SUCCESS), 4344 ]
//============================================================================
Json::Value sxd_client::Mod_Regression_Base_get_back_assistant_award(int id) {
    Json::Value data;
    data.append(id);
    return this->send_and_receive(data, 254, 6);
}

class Mod_GoBackShop_Base {
public:
    static const int LIMIT_ITEM = 1;
    static const int SUCCESS = 2;
};

//============================================================================
// 回归商店
//============================================================================
void sxd_client::go_back_shop() {
    auto data = this->Mod_GoBackShop_Base_open_panel();
    auto limit_item = data[1][0];
    int shop_item_id = limit_item[0].asInt();
    int item_id = limit_item[1].asInt();
    std::string item_name = db.get_code(version, "Item", item_id)["text"];
    int item_num = limit_item[2].asInt();
    int price = limit_item[3].asInt();
    int left_times = limit_item[4].asInt();
    while (left_times) {
        data = this->Mod_GoBackShop_Base_buy_shop_item(Mod_GoBackShop_Base::LIMIT_ITEM, shop_item_id);
        if (data[0].asInt() != Mod_GoBackShop_Base::SUCCESS) {
            common::log(boost::str(boost::format("【回归商店】购买限定商品 [%1%×%2%] 失败，result[%3%]") % item_name % item_num % data[0]), iEdit);
            return;
        }
        common::log(boost::str(boost::format("【回归商店】购买限定商品 [%1%×%2%]，花费 [元宝×%3%]") % item_name % item_num % price), iEdit);
        // refresh and continue
        data = this->Mod_GoBackShop_Base_open_panel();
        limit_item = data[1][0];
        left_times = limit_item[4].asInt();
    }
}

//============================================================================
// 2019030101 回归商店，面板
// "module":459,"action":1,"request":[],"response":[[Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil],[Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil],Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,[Utils.IntUtil,Utils.IntUtil]]
// GoBackShopData.as:
//     oObject.list(param1,this.gobackshopObject,["normal_item","limit_item","total_times","total_cost","rebate_cost","rebate_info"]);
//     for each(_loc3_ in this.gobackshopObject.normal_item)
//         _loc2_ = this.parseItem(_loc3_,["shop_item_id","item_id","item_num","price","left_times"]);
//     for each(_loc3_ in this.gobackshopObject.limit_item)
//         _loc2_ = this.parseItem(_loc3_,["shop_item_id","item_id","item_num","price","left_times"]);
// Example
//     [ [ [ 2, 347, 100, 80, 3 ], [ 1, 5822, 2, 50, 3 ], [ 3, 1740, 20, 60, 3 ] ], [ [ 32, 4954(血魂石), 1, 150, 2 ] ], 1, 150, 0, [ [ 10, 10 ], [ 20, 15 ], [ 30, 20 ], [ 40, 25 ], [ 50, 30 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_GoBackShop_Base_open_panel() {
    Json::Value data;
    return this->send_and_receive(data, 459, 1);
}

//============================================================================
// 2019030101 回归商店，购买
// "module":459,"action":2,"request":[Utils.UByteUtil,Utils.IntUtil],"response":[Utils.UByteUtil]
// RegressionActivityView.as:
//     _data.call(Mod_GoBackShop_Base.buy_shop_item,buy_shop_itemBack,[type,buyId]);
// Example
//     [ 1(Mod_GoBackShop_Base::LIMIT_ITEM), 32(血魂石) ] -> [ 2(Mod_GoBackShop_Base::SUCCESS) ]
//============================================================================
Json::Value sxd_client::Mod_GoBackShop_Base_buy_shop_item(int type, int shop_item_id) {
    Json::Value data;
    data.append(type);
    data.append(shop_item_id);
    return this->send_and_receive(data, 459, 2);
}

class Mod_GoBackLogin_Base {
public:
    static const int SUCCESS = 0;
};

//============================================================================
// 回归登录
//============================================================================
void sxd_client::go_back_login() {
    for (int id = 1; id <= 7; ++id) {
        auto data = this->Mod_GoBackLogin_Base_get_award(id);
        if (data[0].asInt() == Mod_GoBackLogin_Base::SUCCESS)
            common::log(boost::str(boost::format("【回归登录】领取第 [%1%] 天登录礼包") % id), iEdit);
    }
}

//============================================================================
// 2019030101 回归登录，领取
// "module":461,"action":2,"request":[Utils.IntUtil,Utils.IntUtil],"response":[Utils.UByteUtil]
// RegressionActivityView.as:
//     _data.call(Mod_GoBackLogin_Base.get_award,this.get_awardBack,[this.loginDay,!!param2?param2.award_id:0]);
// Example
//     [ 1(1-7), 0 ] -> [ 0(Mod_GoBackLogin_Base::SUCCESS) ]
//============================================================================
Json::Value sxd_client::Mod_GoBackLogin_Base_get_award(int id) {
    Json::Value data;
    data.append(id);
    data.append(0);
    return this->send_and_receive(data, 461, 2);
}

//============================================================================
// 回归送礼
//============================================================================
void sxd_client::back_send_gift() {
    auto data = this->Mod_Regression_Base_get_back_send_gift_award();
    if (data[0].asInt() == Mod_Regression_Base::SUCCESS)
        common::log(boost::str(boost::format("【回归送礼】领取奖励")), iEdit);
}

//============================================================================
// 2019030101 回归送礼，领取
// "module":254, "action":4, "request":[], "response":[Utils.UByteUtil,Utils.IntUtil]
// Example
//     [ 0(Mod_Regression_Base::SUCCESS), 4368 ]
//============================================================================
Json::Value sxd_client::Mod_Regression_Base_get_back_send_gift_award() {
    Json::Value data;
    return this->send_and_receive(data, 254, 4);
}

class Mod_GoBackTask_Base {
public:
    static const int SUCCESS = 0;
    static const int PACK_FULL = 4;
};

//============================================================================
// 回归任务
//============================================================================
void sxd_client::go_back_task() {
    for (int id = 1; id <= 7; ++id) {
        try {
            this->item_use();
            auto data = this->Mod_GoBackTask_Base_get_award(id);
            if (data[0].asInt() == Mod_GoBackTask_Base::SUCCESS) {
                common::log(boost::str(boost::format("【回归任务】领取第 [%1%] 个任务奖励") % id), iEdit);
                this->item_use();
            } else if (data[0].asInt() == Mod_GoBackTask_Base::PACK_FULL)
                common::log(boost::str(boost::format("【回归任务】领取第 [%1%] 个任务奖励失败，背包已满") % id), iEdit);
        } catch (const std::exception& ex) {
            common::log(boost::str(boost::format("发现错误(go back task)：%1%") % ex.what()));
        }
    }
}

//============================================================================
// 2019030101 回归任务，领取
// "module":425, "action":1, "request":[Utils.IntUtil], "response":[Utils.UByteUtil]
// Example
//     [ 2(1-7) ] -> [ 0(Mod_GoBackTask_Base::SUCCESS) ]
//============================================================================
Json::Value sxd_client::Mod_GoBackTask_Base_get_award(int id) {
    Json::Value data;
    data.append(id);
    return this->send_and_receive(data, 425, 1);
}


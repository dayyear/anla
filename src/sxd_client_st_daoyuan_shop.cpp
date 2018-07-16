#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_StDaoyuanShop_Base {
public:
    static const int SUCCESS = 0;
    static const int NOT_ENOUGH_DAOYUAN = 2;
};

//============================================================================
// R172 仙界商店
//============================================================================
void sxd_client::st_daoyuan_shop() {

    // item list
    Json::Value data = this->Mod_StDaoyuanShop_Base_daoyuan_shop_item_list();
    Json::Value items = data[0];

    // buy
    int ids[] = { 21, 15 };
    std::string names[] = { "普通进阶丹", "真元" };
    for (int i = 0; i < 2; i++) {
        int id = ids[i];
        std::string name = names[i];
        auto item_select = std::find_if(items.begin(), items.end(), [id](const Json::Value& x) {return x[0].asInt()==id;});
        if (item_select == items.end())
            continue;
        int count = (*item_select)[1].asInt();
        if (count == 0)
            continue;

        data = this->Mod_StDaoyuanShop_Base_buy_daoyuan_shop_item(id, (*item_select)[1].asInt());
        if (data[0].asInt() == Mod_StDaoyuanShop_Base::NOT_ENOUGH_DAOYUAN) {
            common::log(boost::str(boost::format("【仙界商店】购买 [%1%] 失败，道缘不足") % name), 0);
            continue;
        }
        if (data[0].asInt() != Mod_StDaoyuanShop_Base::SUCCESS) {
            common::log(boost::str(boost::format("【仙界商店】购买 [%1%] 失败，result[%2%]") % name % data[0]), 0);
            continue;
        }
        common::log(boost::str(boost::format("【仙界商店】购买 [%1%×%2%]") % name % count));
    }
}

//============================================================================
// R172 仙界商店
// {module:132, action:0, request:[], response:[[Utils.ByteUtil, Utils.ShortUtil, Utils.ShortUtil], Utils.ByteUtil]};
// StDaoYuanShopData.as 37:
//     var _loc_3:* = param1[0];
//     for each (_loc_4 in _loc_3)
//         oObject.list(_loc_4, _loc_7, ["item_id", "item_num", "item_discount"]);
//============================================================================
Json::Value sxd_client::Mod_StDaoyuanShop_Base_daoyuan_shop_item_list() {
    Json::Value data;
    return this->send_and_receive(data, 132, 0);
}

//============================================================================
// R172 买
// {module:132, action:1,
// request:[Utils.ByteUtil, Utils.ShortUtil],
// StDaoYuanShopView.as 128:
//     _data.call(Mod_StDaoyuanShop_Base.buy_daoyuan_shop_item, this.buy_item_callback, [item_id, num], true, 1);
// response:[Utils.UByteUtil, Utils.ShortUtil]};
// StDaoYuanShopData.as 22:
//     this.result = param1[0];
//     this.reduce_num = param1[1];
//============================================================================
Json::Value sxd_client::Mod_StDaoyuanShop_Base_buy_daoyuan_shop_item(int id, int count) {
    Json::Value data;
    data.append(id);
    data.append(count);
    return this->send_and_receive(data, 132, 1);
}


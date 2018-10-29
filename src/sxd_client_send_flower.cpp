#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_Friend_Base {
public:
    static const int FRIEND = 14;
};

class Mod_SendFlower_Base {
public:
    static const int SUCCESS = 0;
    static const int YES = 3;
};

//============================================================================
// R177 送花
//============================================================================
void sxd_client::send_flower() {
    Json::Value data = this->Mod_Friend_Base_get_friend_list();
    Json::Value friends = data[0];
    if (!friends.size()) {
        common::log("【送花】失败，没有好友", iEdit);
        return;
    }
    auto freind = friends[rand() % friends.size()];
    int id = freind[0].asInt();
    std::string name = common::utf2gbk(freind[1].asString());
    data = this->Mod_SendFlower_Base_player_send_flower_info(id);
    if (data[8].asInt() == Mod_SendFlower_Base::YES) {
        data = this->Mod_SendFlower_Base_send_player_flower(id);
        if (data[0].asInt() == Mod_SendFlower_Base::SUCCESS)
            common::log(boost::str(boost::format("【送花】给好友 [%1%] 赠送一朵鲜花") % name), iEdit);
        else
            common::log(boost::str(boost::format("【送花】给好友 [%1%] 送花失败，result[%2%]") % name % data[0]), iEdit);
    }
}

//============================================================================
// R177 好友列表
// "module":9, "action":5, "request":[Utils.UByteUtil], "response":[[Utils.IntUtil,Utils.StringUtil,Utils.IntUtil,Utils.UByteUtil,Utils.IntUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.IntUtil,Utils.IntUtil,Utils.ByteUtil,Utils.ByteUtil]]
// FriendController.as 78:
//     var _loc1_:Array = this.ownData.getFriendList;
//     return this.renderFriendList(_loc1_[0]);
//     _loc4_.id = param1[_loc3_][0];
//     _loc4_.msgNum = param1[_loc3_][2];
//     _loc4_.name = param1[_loc3_][1];
// Example
//     [ [ [ 355547, "\u9ad8\u5343\u5b57.s1", 0, 18, 935, 0, 0, 0, 104, 0, 0 ], [ 288299, "\u5ea6\u65e5\u5982\u5e74.s1", 0, 18, 1591, 0, 0, 0, 5, 0, 0 ], [ 311260, "\u75af\u795e..s1", 0, 18, 656, 0, 0, 0, 3, 0, 0 ], [ 350598, "\u6697\u591c\u661f\u7a7as1.s1", 0, 18, 1011, 0, 0, 0, 100, 0, 0 ], [ 355549, "\u8463\u4e58\u516d.s1", 0, 18, 923, 0, 0, 0, 107, 0, 0 ], [ 355553, "\u7504\u6590\u6590.s1", 0, 18, 923, 0, 0, 0, 104, 0, 0 ], [ 355546, "\u90ce\u68ee\u4e09.s1", 0, 18, 953, 0, 0, 0, 104, 0, 0 ], [ 355531, "\u4f0d\u4e8c\u4e00.s1", 0, 18, 953, 0, 0, 0, 6, 1, 0 ], [ 355548, "\u548c\u8b07\u66b4.s1", 0, 18, 923, 0, 0, 0, 103, 0, 0 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_Friend_Base_get_friend_list() {
    Json::Value data;
    data.append(Mod_Friend_Base::FRIEND);
    return this->send_and_receive(data, 9, 5);
}

//============================================================================
// R177 送花面板
// "module":31, "action":0,"request":[Utils.IntUtil], "response":[Utils.UByteUtil,Utils.IntUtil,Utils.StringUtil,Utils.IntUtil,Utils.IntUtil,Utils.StringUtil,Utils.IntUtil,Utils.IntUtil,Utils.UByteUtil,[Utils.IntUtil,Utils.StringUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil]]
// SendFlowerData.as 93:
//     oObject.list(param1,this._objSendFlowerInfo,["result","player_id","nickname","role_id","max_player_id","max_nickname","max_flower_count","total_flower_count","is_can_send"]);
// Example
//     [ 0, 355547, "\u9ad8\u5343\u5b57.s1", 104, 355527, "\u5e84\u606d\u6625.s1", 192, 195, 3, [ [ 355527, "\u5e84\u606d\u6625.s1", 5, 1, 1526564233 ], [ 355527, "\u5e84\u606d\u6625.s1", 5, 1, 1526477875 ], [ 355527, "\u5e84\u606d\u6625.s1", 5, 1, 1527082596 ], [ 355527, "\u5e84\u606d\u6625.s1", 5, 1, 1526909858 ], [ 355527, "\u5e84\u606d\u6625.s1", 5, 1, 1527946553 ], [ 355527, "\u5e84\u606d\u6625.s1", 5, 1, 1526391524 ], [ 355527, "\u5e84\u606d\u6625.s1", 5, 1, 1526996283 ], [ 355527, "\u5e84\u606d\u6625.s1", 5, 1, 1527865547 ], [ 355527, "\u5e84\u606d\u6625.s1", 5, 1, 1526828876 ], [ 355527, "\u5e84\u606d\u6625.s1", 5, 1, 1528092698 ], [ 355527, "\u5e84\u606d\u6625.s1", 5, 1, 1526305089 ], [ 355527, "\u5e84\u606d\u6625.s1", 5, 1, 1528299379 ], [ 355527, "\u5e84\u606d\u6625.s1", 5, 1, 1527428179 ], [ 355527, "\u5e84\u606d\u6625.s1", 5, 1, 1527168990 ], [ 355527, "\u5e84\u606d\u6625.s1", 5, 1, 1527687352 ], [ 355527, "\u5e84\u606d\u6625.s1", 5, 1, 1527773748 ], [ 355527, "\u5e84\u606d\u6625.s1", 5, 1, 1528208419 ], [ 355527, "\u5e84\u606d\u6625.s1", 5, 1, 1526737143 ], [ 355527, "\u5e84\u606d\u6625.s1", 5, 1, 1527341722 ], [ 355527, "\u5e84\u606d\u6625.s1", 5, 1, 1526650700 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_SendFlower_Base_player_send_flower_info(int player_id) {
    Json::Value data;
    data.append(player_id);
    return this->send_and_receive(data, 31, 0);
}

//============================================================================
// R177 送花
// "module":31, "action":1, "request":[Utils.IntUtil,Utils.IntUtil], "response":[Utils.UByteUtil]
// SendFlowerData.as 111:
//     this._objSendPlayerFlower = {"result":param1[0]};
// Example
//     [ 0 ]
//============================================================================
Json::Value sxd_client::Mod_SendFlower_Base_send_player_flower(int player_id) {
    Json::Value data;
    data.append(player_id);
    data.append(1);
    return this->send_and_receive(data, 31, 1);
}

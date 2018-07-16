#include <boost/algorithm/string/trim.hpp>
#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_TravelEvent_Base {
public:
    static const int SUCCESS = 2;
};

void sxd_client::travel_event() {
    for (;;) {

        // get event and answer
        Json::Value data = this->Mod_TravelEvent_Base_get_event_and_answer();
        int event_id = data[0].asInt();
        std::string event = data[1].asString();
        Json::Value answers = data[2];
        int msg = data[3].asInt();
        int cur_answer_times = data[4].asInt();
        int tol_answer_times = data[5].asInt();
        if (msg != Mod_TravelEvent_Base::SUCCESS)
            return;

        // lottery draw
        if (cur_answer_times == tol_answer_times) {
            data = this->Mod_TravelEvent_Base_lottery_draw();
            if (data[0].asInt() != Mod_TravelEvent_Base::SUCCESS) {
                common::log(boost::str(boost::format("¡¾ÏÉÂÃÆæÔµ¡¿ÁìÈ¡ [ÏÉÂÃÃØ±¦]Ê§°Ü£¬result[%1%]") % data[0]));
                return;
            }
            common::log("¡¾ÏÉÂÃÆæÔµ¡¿ÁìÈ¡ [ÏÉÂÃÃØ±¦]");
        }

        // answer travel event
        common::log(boost::str(boost::format("¡¾ÏÉÂÃÆæÔµ¡¿ÎÊ£º%1%") % boost::algorithm::trim_copy(common::utf2gbk(event))));
        int answer_index = rand() % answers.size();
        data = this->Mod_TravelEvent_Base_answer_travel_event(event_id, answers[answer_index][0].asInt());
        common::log(boost::str(boost::format("¡¾ÏÉÂÃÆæÔµ¡¿´ð£º%1%. %2%%3%") % boost::algorithm::trim_copy(common::utf2gbk(answers[answer_index][1].asString())) % boost::algorithm::trim_copy(common::utf2gbk(answers[answer_index][2].asString())) % boost::algorithm::trim_copy(common::utf2gbk(data[0].asString()))));
    }
}

//============================================================================
// R172 ÏÉÂÃÆæÔµ
// {module:21, action:1, request:[], response:[Utils.IntUtil, Utils.StringUtil, [Utils.IntUtil, Utils.StringUtil, Utils.StringUtil], Utils.UByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil]};
// TravelEventController.as 31:
//     _loc_2.eventId = _loc_1[0];
//     _loc_2.event = _loc_1[1];
//     _loc_2.answerList = this.renderAnswer(_loc_1[2]);
//     _loc_2.msg = _loc_1[3];
//     _loc_2.cur_answer_times = _loc_1[4];
//     _loc_2.tol_answer_times = _loc_1[5];
//     _loc_2.joinCount = _loc_1[6];
// Example
//     [ 9, "\u6b64\u5730\u51fa\u73b0\u4e86\u4e00\u4e2a\u5de8\u5927\u7684\u86f9\uff0c\u5b83\u6b63\u51c6\u5907\u7fbd\u5316\u6210\u8776\uff0c\u53ef\u662f\u5728\u7834\u5f00\u86f9\u7684\u77ac\u95f4\u597d\u4f3c\u5361\u4f4f\u4e86\uff0c\u4e0d\u65ad\u6323\u624e\u7740\u8981\u51fa\u6765\u3002\n", [ [ 37, "B", "\u653e\u4efb\u5176\u81ea\u751f\u81ea\u706d\u3002\r\n" ], [ 36, "A", "\u4ee5\u5251\u5288\u5f00\uff0c\u52a9\u5b83\u4e00\u81c2\u4e4b\u529b\u3002\r\n" ] ], 2, 1, 5, 0 ]
//============================================================================
Json::Value sxd_client::Mod_TravelEvent_Base_get_event_and_answer() {
    Json::Value data;
    return this->send_and_receive(data, 21, 1);
}

//============================================================================
// R172 ÏÉÂÃÃØ±¦
// {module:21, action:4, request:[], response:[Utils.UByteUtil, [Utils.IntUtil, Utils.IntUtil]]};
// TravelEventData.as 64:
//     this.drawResult = param1[0];
//     this.awardArray = param1[1];
// Example
//     [ 2, [ [ 2452, 30 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_TravelEvent_Base_lottery_draw() {
    Json::Value data;
    return this->send_and_receive(data, 21, 4);
}

//============================================================================
// R172 ´ðÌâ
// {module:21, action:2, request:[Utils.IntUtil, Utils.IntUtil],
// Example
//     [ 9, 37 ]
// response:[Utils.StringUtil, [Utils.UByteUtil, Utils.IntUtil], Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil, Utils.IntUtil]};
//
//     var _loc_1:* = _data.travelEvent.travelEvent;
//     var _loc_2:* = _loc_1[0];
//     _loc_3.answerValue = this.renderAward(_loc_1[1], _loc_3);
// Example
//     [ "\u4f60\u83b7\u5f97300\u94dc\u94b1,30\u9605\u5386\uff01", [ [ 3, 50200 ], [ 4, 0 ], [ 5, 0 ], [ 6, 0 ], [ 7, 30 ] ], 1, 10, 0, 0, 1, 5 ]
//============================================================================
Json::Value sxd_client::Mod_TravelEvent_Base_answer_travel_event(int event_id, int answer_id) {
    Json::Value data;
    data.append(event_id);
    data.append(answer_id);
    return this->send_and_receive(data, 21, 2);
}


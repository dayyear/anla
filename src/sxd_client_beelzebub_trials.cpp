#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_BeelzebubTrials_Base {
public:
    static const int SUCCESS = 5;
};

void sxd_client::beelzebub_trials() {
    Json::Value data = this->Mod_BeelzebubTrials_Base_get_moral_award();
    if (data[0] != Mod_BeelzebubTrials_Base::SUCCESS)
        return;

    std::vector<std::string> items;
    std::transform(data[1].begin(), data[1].end(), std::back_inserter(items), [this](const Json::Value& x) {
        return boost::str(boost::format("[%1%×%2%]") % db.get_code(version, "Item", x[0].asInt())["text"] % x[1]);
    });
    common::log(boost::str(boost::format("【魔王试炼】领取道行奖励：%1%") % boost::algorithm::join(items, "，")), iEdit);
}

//============================================================================
// R172 道行奖励
// {module:57, action:24,
// request:[Utils.IntUtil],
// Example
//     [ 0 ]
// response:[Utils.UByteUtil, [Utils.IntUtil, Utils.IntUtil]]};
// BeelzebubTrialsData.as 787:
//     this.moralObj.result = param1[0];
//     for each (_loc_2 in param1[1])
//         oObject.list(_loc_2, _loc_3, ["item_id", "count"]);
// Example
//     [5,[[1739,4500],[1748,16000],[1747,2000000]]]
//============================================================================
Json::Value sxd_client::Mod_BeelzebubTrials_Base_get_moral_award() {
    Json::Value data;
    data.append(0);
    return this->send_and_receive(data, 57, 24);
}


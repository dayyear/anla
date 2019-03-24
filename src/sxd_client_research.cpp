#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_Research_Base {
public:
    static const int SUCCEED = 0;
};

//============================================================================
// 奇术
//============================================================================
void sxd_client::research() {
    auto data = this->Mod_Research_Base_research_list();
    auto research_list = data[0];
    for (int i = 0; i < 10; ++i) {
        int research_id = research_list[rand() % research_list.size()][1].asInt();
        data = this->Mod_Research_Base_research_upgrade(research_id);
        if (data[0].asInt() == Mod_Research_Base::SUCCEED)
            common::log(boost::str(boost::format("【奇术】升级奇术 [%1%]") % research_id), iEdit);
    }
}

//============================================================================
// R196 面板
// "module":7,"action":0,"request":[Utils.IntUtil],"response":[[Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil],Utils.LongUtil,Utils.IntUtil,Utils.IntUtil]
// ResearchView.as:
//     this._data.call(Mod_Research_Base.research_list,param1,[ResearchType.All]);
// ResearchData.as:
//     for each(_loc3_ in this._aryResearchList[0])
//         oObject.list(_loc3_,_loc2_,["order_key","research_id","level","last_level","player_level","type","skill","cd_time"]);
// Example
//     [ 0 ] ->
//     [ [ [ 16, 24, 100, 101, 101, 1, 222693, 0 ],
//         [ 7, 8, 249, 250, 250, 1, 6005, 0 ],
//         [ 1, 1, 250, 251, 251, 1, 2415, 0 ],
//         [ 17, 25, 167, 168, 168, 1, 715233, 0 ],
//         [ 9, 9, 249, 250, 250, 1, 4325, 0 ],
//         [ 2, 16, 163, 164, 164, 1, 674898, 0 ],
//         [ 11, 6, 179, 180, 180, 1, 2079, 0 ],
//         [ 5, 5, 249, 250, 250, 1, 1685, 0 ],
//         [ 6, 18, 152, 153, 153, 1, 571778, 0 ],
//         [ 13, 7, 179, 180, 180, 1, 1388, 0 ],
//         [ 29, 30, 20, -1, -1, 2, 0, 0 ],
//         [ 14, 22, 36, 37, 37, 1, 17116, 0 ],
//         [ 12, 21, 33, 34, 34, 1, 30089, 0 ],
//         [ 4, 17, 247, 248, 248, 1, 1889510, 0 ],
//         [ 3, 2, 249, 250, 250, 1, 2405, 0 ],
//         [ 18, 26, 130, 131, 131, 1, 397880, 0 ],
//         [ 10, 20, 194, 195, 195, 1, 1029888, 0 ],
//         [ 8, 19, 194, 195, 195, 1, 1029888, 0 ],
//         [ 19, 27, 188, 189, 189, 1, 953353, 0 ],
//         [ 20, 28, 134, 135, 135, 1, 426452, 0 ],
//         [ 15, 23, 109, 110, 110, 1, 268368, 0 ] ], 172355366, 0, 0 ]
//============================================================================
Json::Value sxd_client::Mod_Research_Base_research_list() {
    Json::Value data;
    data.append(0);
    return this->send_and_receive(data, 7, 0);
}

//============================================================================
// 升级
// "module":7,"action":1,"request":[Utils.IntUtil],"response":[Utils.UByteUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.IntUtil,Utils.LongUtil,Utils.IntUtil,Utils.IntUtil]
// ResearchView.as:
//     this.research_upgrade(this._research.researchID,this.researchUpgradeCallBack);
// ResearchData.as:
//     oObject.list(this._aryResearchUpgrade,_loc1_,["result","type_id","research_id","level","last_level","player_level","cd_time","skill","myskill","ingot","curr_time"]);
// Example
//     [ 16 ] -> [ 0, 1, 16, 163, 164, 164, 0, 674898, 172355366, 0, 0 ]
//============================================================================
Json::Value sxd_client::Mod_Research_Base_research_upgrade(int id) {
    Json::Value data;
    data.append(id);
    return this->send_and_receive(data, 7, 1);
}

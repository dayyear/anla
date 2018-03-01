#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"


class QuestType {
public:
    static const int LevelLimit = -1;
    static const int Acceptable = 0;
    static const int Accepted = 1;
    static const int Completed = 2;
};

//============================================================================
// R171 当前任务
// {module:3, action:6, request:[],
// response:[[Utils.IntUtil, Utils.ByteUtil]]};
// Example
// quest_id, state
//     [ [ [ 897, 1 ], [ 894, 1 ], [ 874, 1 ], [ 900, 1 ], [ 876, 1 ], [ 893, 1 ], [ 875, 1 ], [ 872, 2 ], [ 889, 1 ], [ 899, 1 ], [ 890, 1 ], [ 895, 1 ], [ 898, 1 ], [ 896, 1 ], [ 892, 1 ], [ 887, 1 ] ] ]
//     887:[2,24200,0,"巨斧青龙兵","...","...","去[九重仙池7)]击败",193,193,5100000,1700000,0,0,"...","...","...",872,1],
//     872:[64,53070,5,25200,190,126000,0,"九重仙池(7)",0,0,0],
//============================================================================
Json::Value sxd_client::Mod_Quest_Base_list_player_quest() {
    Json::Value data;
    return this->send_and_receive(data, 3, 6);
}

//============================================================================
// R171 可接任务
// {module:3, action:8,
// request:[Utils.IntUtil],
// Example
//     [ 33 ]
// response:[[Utils.IntUtil]]};
// Example
//     [ [ [ 877 ], [ 869 ], [ 873 ], [ 891 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_Quest_Base_can_receive_quest(int town_map_id) {
    Json::Value data;
    data.append(town_map_id);
    return this->send_and_receive(data, 3, 8);
}

//============================================================================
// R171 副本
// {module:4, action:0,
// request:[Utils.IntUtil],
// Example
//     [ 33 ]
// response:[[Utils.IntUtil, Utils.ByteUtil, Utils.UByteUtil, Utils.ByteUtil, Utils.ShortUtil, Utils.ByteUtil]]};
// ChooseMission2View.as 59:
//     oObject.list(tempArr, tempObj, ["mission_id", "status", "rank", "is_boss", "complete_count", "difficulty"]);
// Example
// status=1, 已通关, 可扫荡; status=0, 未通关
//     [ [ [ 866, 1, 74, 0, 0, 0 ], [ 867, 1, 75, 0, 0, 0 ], [ 868, 1, 74, 1, 0, 0 ], [ 869, 1, 74, 0, 0, 0 ], [ 870, 1, 74, 0, 0, 0 ], [ 871, 1, 74, 1, 0, 0 ], [ 872, 1, 74, 0, 0, 0 ], [ 873, 1, 74, 0, 0, 0 ], [ 874, 1, 74, 1, 0, 0 ], [ 875, 1, 74, 0, 0, 0 ], [ 876, 1, 76, 0, 0, 0 ], [ 877, 1, 76, 1, 0, 0 ], [ 878, 1, 76, 0, 0, 0 ], [ 879, 1, 76, 0, 0, 0 ], [ 880, 1, 76, 1, 0, 0 ], [ 881, 1, 76, 0, 0, 0 ], [ 882, 1, 76, 0, 0, 0 ], [ 883, 1, 76, 1, 0, 0 ], [ 884, 1, 76, 0, 0, 0 ], [ 885, 1, 76, 0, 0, 0 ], [ 886, 1, 76, 1, 0, 0 ], [ 887, 1, 76, 0, 0, 0 ], [ 888, 1, 76, 0, 0, 0 ], [ 889, 1, 76, 1, 0, 0 ], [ 890, 0, 76, 0, 0, 0 ] ] ]
//============================================================================
Json::Value sxd_client::Mod_Mission_Base_get_sections(int town_map_id){
    Json::Value data;
    data.append(town_map_id);
    return this->send_and_receive(data, 4, 0);
}

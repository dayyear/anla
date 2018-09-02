#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_StTakeBible_Base {
public:
    static const int SUCCESS = 9;
};

static std::string protectors[] = { "", "白龙马", "沙悟净", "猪八戒", "孙悟空", "唐僧" };

// 参考圣域取经
void sxd_client::st_take_bible() {
    try {
        // first get
        Json::Value data = this->Mod_StTakeBible_Base_get_take_bible_info();
        Json::Value bible_info = data;
        if (bible_info[2].asInt() == bible_info[3].asInt()) {
            common::log("【仙界取经】次数已用完", 0);
            return;
        }
        if (bible_info[6].asInt() == 0) {
            data = this->Mod_StTakeBible_Base_refresh();
            if (data[0].asInt() != Mod_StTakeBible_Base::SUCCESS) {
                common::log(boost::str(boost::format("【仙界取经】刷新取经使者失败，msg[%1%]") % data[0]), iEdit);
                return;
            }
            common::log(boost::str(boost::format("【仙界取经】刷新取经使者，获得 [%1%]") % protectors[data[1].asInt()]), iEdit);
        }
        // second get
        bible_info = this->Mod_StTakeBible_Base_get_take_bible_info();
        if (bible_info[6].asInt() == 0) {
            common::log(boost::str(boost::format("【仙界取经】数据异常，can_protection[%1%]") % bible_info[6]), iEdit);
            return;
        }
        if (bible_info[5].asInt() == 0) {
            data = this->Mod_StTakeBible_Base_start_take_bible();
            if (data[0].asInt() != Mod_StTakeBible_Base::SUCCESS) {
                common::log(boost::str(boost::format("【仙界取经】护送失败，msg[%1%]") % data[0]), iEdit);
                return;
            }
            common::log(boost::str(boost::format("【仙界取经】开始护送，取经使者 [%1%]") % protectors[bible_info[6].asInt()]), iEdit);
        }
    } catch (const std::exception& ex) {
        common::log(boost::str(boost::format("发现错误(st take bible)：%1%") % ex.what()));
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

//============================================================================
// R179 拦截使者
//============================================================================
void sxd_client::st_rob_bible(sxd_client* sxd_client_town) {
    try {
        // read config
        int config;
        std::istringstream(db.get_config(user_id.c_str(), "StRobMinFame")) >> config;

        // get my_level, my_fame and function_names
        auto data = sxd_client_town->Mod_Player_Base_get_player_info();
        int my_level = data[1].asInt();
        int my_fame = data[21].asInt();
        data = sxd_client_town->Mod_Player_Base_get_player_function();
        std::vector<std::string> function_names;
        for (const auto& item : data[0])
            try {
                function_names.push_back(db.get_code(version, "Function", item[0].asInt())["text"]);
            } catch (const std::exception& ex) {
                common::log(boost::str(boost::format("发现错误(function)：%1%") % ex.what()), iEdit);
            }

        // get cd_time
        data = this->Mod_StTakeBible_Base_get_rob_take_bible_cd_time();
        int cd_time = data[0].asInt();
        if (cd_time)
            return;

        // get can_rob_times and bibles
        data = this->Mod_StTakeBible_Base_open_take_bible();
        int can_rob_times = data[2].asInt();
        if (!can_rob_times)
            return;
        auto bibles = data[0];

        // get all members with st union
        auto all_members = this->get_all_st_union_members();

        // get server_time
        data = sxd_client_town->Mod_Player_Base_server_time();
        int server_time = data[0].asInt();

        // filter arrive soon and myself
        std::vector<Json::Value> bibles_valid;
        std::copy_if(bibles.begin(), bibles.end(), std::back_inserter(bibles_valid), [server_time, this](const Json::Value& x) {return x[3].asInt() > server_time + 30 && x[0].asInt() != this->player_id;});

        for (auto& bible : bibles_valid) {
            int player_id = bible[0].asInt();
            int protect_player_id = bible[5].asInt();
            int sequence_id = bible[6].asInt();
            int loc3 = (my_fame >= 1200000 && std::find(function_names.begin(), function_names.end(), "仙界等级") != function_names.end()) ? 1 : 0;
            data = this->Mod_StTakeBible_Base_get_player_take_bible(player_id, my_level, loc3, sequence_id);
            // 8 + 20 = 28
            for (const auto& item : data)
                bible.append(item);
            // 28 + 1 = 29
            int id = protect_player_id ? protect_player_id : player_id;
            auto temp = std::find_if(all_members.begin(), all_members.end(), [id](const Json::Value& x) {return x[0].asInt()==id;});
            if (temp == all_members.end())
                bible.append(INT_MAX);
            else
                bible.append((*temp)[7].asInt());
        }

        // rob_fame >= {config} && !is_robbed && be_rob_times < total_can_rob_times && !protect_time
        std::vector<Json::Value> bibles_opt;
        std::copy_if(bibles_valid.begin(), bibles_valid.end(), std::back_inserter(bibles_opt), [&config](const Json::Value& x) {return x[8+9].asInt() >= config && !x[8+14].asInt() && x[8+5].asInt() < x[8+6].asInt() && !x[8+15].asInt();});
        if (!bibles_opt.size())
            return;

        // min power
        auto bible_opt = *std::min_element(bibles_opt.begin(), bibles_opt.end(), [](const Json::Value& x,const Json::Value& y) {return x[8+20+0].asInt() < y[8+20+0].asInt();});

        int player_id = bible_opt[0].asInt();
        int protection = bible_opt[1].asInt();
        int sequence_id = bible_opt[6].asInt();
        std::string nick_name = common::utf2gbk(bible_opt[8 + 1].asString());
        //int level = bible_opt[8 + 4].asInt();
        //int power = bible_opt[8 + 20 + 0].asInt();
        //int be_rob_times = bible_opt[8 + 5].asInt();
        //int total_can_rob_times = bible_opt[8 + 6].asInt();
        //int is_robbed = bible_opt[8 + 14].asInt();
        //common::log(boost::str(boost::format("【仙界取经】取经使者 [%1%]，取经玩家 [%2%(%3%)]，[%4%]级，战力[%5%]，[%6%/%7%]，is_robbed[%8%]") % protectors[protection] % nick_name % player_id % level % power % be_rob_times % total_can_rob_times % is_robbed), 0);

        data = this->Mod_StTakeBible_Base_rob(player_id, sequence_id);
        if (data[0].asInt() != Mod_StTakeBible_Base::SUCCESS) {
            common::log(boost::str(boost::format("【仙界取经】拦截失败，result[%1%]") % data[0]), iEdit);
            return;
        }
        int rob_coin = data[2].asInt();
        int rob_fame = data[3].asInt();
        int rob_st_exp = data[4].asInt();
        if (rob_fame)
            common::log(boost::str(boost::format("【仙界取经】拦截 [%1%(%2%)] 的 [%3%]，战胜，获得 [铜钱×%4%]，[声望×%5%]，[道缘×%6%]") % nick_name % player_id % protectors[protection] % rob_coin % rob_fame % rob_st_exp), iEdit);
        else
            common::log(boost::str(boost::format("【仙界取经】拦截 [%1%(%2%)] 的 [%3%]，战败") % nick_name % player_id % protectors[protection]), iEdit);
    } catch (const std::exception& ex) {
        common::log(boost::str(boost::format("发现错误(st rob bible)：%1%") % ex.what()));
    }
}

//============================================================================
// R179 大面板
// "module":114,"action":0,"request":[],"response":[[Utils.IntUtil,Utils.UByteUtil,Utils.IntUtil,Utils.IntUtil,Utils.ShortUtil,Utils.IntUtil,Utils.ByteUtil,Utils.ByteUtil],Utils.IntUtil,Utils.ShortUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.UByteUtil,Utils.IntUtil,[Utils.ByteUtil,Utils.ShortUtil,Utils.StringUtil,Utils.StringUtil,Utils.StringUtil,Utils.ShortUtil],Utils.ByteUtil]
// StTakeBibleController.as:
//     _loc2_.classTakePlayer = this.renderClssTakePlayer(_loc1_[0]);
//         private function renderClssTakePlayer(param1:Array) : Array
//             _loc7_.playerId = param1[_loc6_][0];
//             _loc7_.protection = param1[_loc6_][1];
//             _loc7_.startTime = DateTime.formatServerTime(param1[_loc6_][2]);
//             _loc7_.arrivalTime = DateTime.formatServerTime(param1[_loc6_][3]);
//             _loc7_.distance = param1[_loc6_][4];
//             _loc7_.protectPlayerId = param1[_loc6_][5];
//             _loc7_.sequence_id = param1[_loc6_][6];
//             _loc7_.is_deeds_so = param1[_loc6_][7];
//     _loc2_.protectPlayerId = _loc1_[1];
//     _loc2_.canRobTimes = _loc1_[2];
//     _loc2_.canProtectTimes = _loc1_[3];
//     _loc2_.canTakeBibleTimes = _loc1_[4];
//     _loc2_.quicklyTimes = _loc1_[5];
//     _loc2_.bless = _loc1_[6];
//     _loc2_.type = _loc1_[7];
//     _loc2_.time = _loc1_[8];
//     for each(_loc3_ in _loc1_[9])
//         oObject.list(_loc3_,_loc4_,["sequence_id","remain_recapture_time","last_rob_nickname","last_rob_stage_name","last_rob_server_name","rob_role_id"]);
//     _loc2_.buff = _loc1_[10];
// Example
//     [ [ [ 13617, 1, 1528977053, 1528978253, 1200, 0, 1, 0 ], [ 7990, 1, 1528976550, 1528977750, 1200, 0, 1, 0 ], [ 207657, 1, 1528976882, 1528978082, 1200, 0, 1, 0 ], [ 6152, 3, 1528975757, 1528977557, 1800, 0, 1, 0 ], [ 207840, 1, 1528976832, 1528978032, 1200, 0, 1, 0 ], [ 199303, 2, 1528976459, 1528977959, 1500, 0, 1, 0 ], [ 3750, 2, 1528977064, 1528978564, 1500, 0, 1, 0 ], [ 6150, 2, 1528975935, 1528977435, 1500, 0, 1, 0 ], [ 3392, 1, 1528976759, 1528977959, 1200, 0, 1, 0 ], [ 203215, 1, 1528976290, 1528977490, 1200, 0, 1, 0 ], [ 3042, 3, 1528977064, 1528978864, 1800, 0, 1, 0 ], [ 7680, 2, 1528976602, 1528978102, 1500, 0, 1, 0 ], [ 7781, 2, 1528976101, 1528977601, 1500, 0, 1, 0 ], [ 3710, 1, 1528976923, 1528978123, 1200, 0, 1, 0 ], [ 12115, 1, 1528976231, 1528977431, 1200, 0, 1, 0 ], [ 14414, 1, 1528976678, 1528977878, 1200, 0, 1, 0 ], [ 6151, 1, 1528976099, 1528977299, 1200, 0, 1, 0 ], [ 7789, 1, 1528976822, 1528978022, 1200, 0, 1, 0 ], [ 206968, 3, 1528976551, 1528978351, 1800, 0, 1, 0 ] ],
//       0, 5, 2, 0, 0, 0, 6, 0, null, 0 ]
//============================================================================
Json::Value sxd_client::Mod_StTakeBible_Base_open_take_bible() {
    Json::Value data;
    return this->send_and_receive(data, 114, 0);
}

//============================================================================
// R179 拦截CD
// "module":114,"action":16,"request":[],"response":[Utils.ShortUtil,Utils.ShortUtil]
// StTakeBibleController.as:
//     _loc2_.cdTime = _loc1_[0];
//     _loc2_.cleanIngot = _loc1_[1];
//============================================================================
Json::Value sxd_client::Mod_StTakeBible_Base_get_rob_take_bible_cd_time() {
    Json::Value data;
    return this->send_and_receive(data, 114, 16);
}

//============================================================================
// R179 护送玩家
// "module":114,"action":9,"request":[Utils.IntUtil,Utils.ShortUtil,Utils.ByteUtil,Utils.ByteUtil],"response":[Utils.IntUtil,Utils.StringUtil,Utils.StringUtil,Utils.StringUtil,Utils.ShortUtil,Utils.ByteUtil,Utils.ByteUtil,Utils.StringUtil,Utils.IntUtil,Utils.ShortUtil,Utils.ShortUtil,Utils.IntUtil,Utils.ShortUtil,Utils.ShortUtil,Utils.ByteUtil,Utils.ShortUtil,Utils.ByteUtil,Utils.ShortUtil,Utils.ShortUtil,Utils.ByteUtil]
// StTakeBibleRoadView.as:
//     this.get_player_take_bible(this._robId,this._sequenceId);
//         private function get_player_take_bible(param1:int, param2:int) : void
//             this._playerId = param1;
//             var _loc3_:int = _ctrl.player.fame >= 1200000 && FunctionType.isOpened(FunctionType.SuperTownLevel)?1:0;
//             _data.call(Mod_StTakeBible_Base.get_player_take_bible,this.getPlayerTakeBibleCallBack,[param1,_ctrl.player.level,_loc3_,param2],true,1);
// StTakeBibleController.as:
//     _loc5_.showName = this.get_palyer_take_bible_show_name(_loc1_[_loc4_],0,_loc5_,["playerId","nickName"]);
//     _loc5_.level = _loc1_[_loc4_][4];
//     _loc5_.beRobTimes = _loc1_[_loc4_][5];
//     _loc5_.totalCanRobTimes = _loc1_[_loc4_][6];
//     _loc5_.protectNickname = _ctrl.player.removeNickNameSuffix(_loc1_[_loc4_][7]);
//     _loc5_.robCoin = _loc1_[_loc4_][8];
//     _loc5_.robFame = _loc1_[_loc4_][9];
//     _loc5_.robStExp = _loc1_[_loc4_][10];
//     _loc5_.awardCoin = _loc1_[_loc4_][11];
//     _loc5_.awardFame = _loc1_[_loc4_][12];
//     _loc5_.awardStExp = _loc1_[_loc4_][13];
//     _loc5_.isRobbed = _loc1_[_loc4_][14];
//     _loc5_.protectTime = _loc1_[_loc4_][15];
//     _loc5_.npcLevel = _loc1_[_loc4_][16];
//     _loc5_.robExpe = _loc1_[_loc4_][17];
//     _loc5_.awardExpe = _loc1_[_loc4_][18];
//     _loc5_.sequence_id = _loc1_[_loc4_][19];
// Example
//     [ 3271, 174, 1, 1 ] --> [ 3271, "\u5251\u7f57\u5239.s5", "\u5fc3\u52a8", "s03", 256, 0, 4, "", 434000, 139, 67, 2680000, 793, 380, 0, 0, 4, 0, 50, 1 ]
// 拦截前：[乜冶.s1(208275)]，[41]级，[0/4]，is_robbed[0]
// 拦截后：[乜冶.s1(208275)]，[41]级，[1/4]，is_robbed[1]
//============================================================================
Json::Value sxd_client::Mod_StTakeBible_Base_get_player_take_bible(int player_id, int my_level, int loc3, int sequence_id) {
    Json::Value data;
    data.append(player_id);
    data.append(my_level);
    data.append(loc3);
    data.append(sequence_id);
    return this->send_and_receive(data, 114, 9);
}

//============================================================================
// R179 拦截
// "module":114,"action":11,"request":[Utils.IntUtil,Utils.ByteUtil],"response":[Utils.UByteUtil,...
// StTakeBibleController.as:
//     _loc2_.msg = _loc1_[0];
//     _loc2_.stageServer = _loc1_[1];
//     _loc2_.takeBibleShowname = this.get_palyer_take_bible_show_name(_loc1_[1][1],0,_loc2_,["takeBiblePlayerId","takeBibleNickname"]);
//     _loc2_.robCoin = _loc1_[2];
//     _loc2_.robFame = _loc1_[3];
//     _loc2_.robStExp = _loc1_[4];
//     _loc2_.warReportId = _loc1_[5];
//     _loc2_.warData = [0,0,0,[_loc1_[6][0]]];
//     _loc2_.bless = _loc1_[7];
//     _loc2_.isFriend = _loc1_[8] == Mod_StTakeBible_Base.YES;
// Example
//     [9,[[203966,"甄斐斐.s1","心动","s04"],[8105,"穆丹婉.s8","7k7k","yx567_s0125"]],0,0,0,560380,[[3,203966,[[203966,"甄斐斐.s1",[[803496,104,"WuShengNanFeiYu","武圣男飞羽",3,498528,498528,173,1,100,130,183,0,10,0,0,1,0,null,0,0,[[0],[0]],null,0,0,0,null,null,0,0,0,0,0],[808206,70,"ShenYangJian","神杨戬",5,413460,413460,173,2,100,125,166,0,10,2,1,0,0,null,0,0,[[0],[0]],null,0,0,0,null,null,0,0,0,0,0],[803539,29,"AKuan","阿宽",1,50256,50256,173,3,100,50,74,0,10,0,0,0,0,null,0,0,[[0],[0]],null,0,0,0,null,null,0,0,0,0,0],[818558,82,"FeiWei","飞卫",3,70334,70334,165,4,100,50,160,0,10,0,0,0,0,null,0,0,[[0],[0]],null,0,0,0,null,null,0,0,0,0,0],[818559,79,"HeXianGu","何仙姑",6,41670,41670,165,8,100,50,157,0,10,0,0,0,0,null,0,0,[[0],[0]],null,0,0,0,null,null,0,0,0,0,0]],[[803498,9,"ChuChu","楚楚",2,38458,38458,173,5,100,75,7,0,10,0,0,0,0,null,0,0,[[0],[0]],null,0,0,0,null,null,0,0,0,0,0]],null,null,null,null,803496,1335,104,173,16159,"",10,7,null,0,null,0,0,null,0,null],[8105,"穆丹婉.s8",[[-2793301,8,"JiangChen","将臣",5,24625,24625,117,1,100,85,71,0,10,0,0,0,0,null,0,0,[[0],[0]],null,0,0,0,null,null,0,0,0,0,0],[-2793290,2,"JianLingNv","剑灵女",2,61143,61143,117,3,100,105,1,0,10,0,0,0,0,null,0,0,[[0],[0]],null,0,0,0,null,null,0,0,0,0,0],[-2793679,23,"XiaoXianTong","小仙童",6,16035,16035,117,4,100,75,16,0,10,0,0,0,0,null,0,0,[[0],[0]],null,0,0,0,null,null,0,0,0,0,0],[-2929488,35,"YangJian","杨戬",5,28105,28105,110,6,100,100,73,0,10,0,0,0,0,null,0,0,[[0],[0]],null,0,0,0,null,null,0,0,0,0,0],[-2866746,12,"JuMang","句芒",3,14359,14359,115,8,100,75,10,0,10,0,0,0,0,null,0,0,[[0],[0]],null,0,0,0,null,null,0,0,0,0,0],[-57880000,5788,"ShaWuJing82","金身罗汉",9,85975,85975,117,9,100,100,271,0,10,20,0,0,0,null,0,0,[[0],[0]],null,0,0,0,null,null,0,0,0,0,0]],null,null,null,null,null,-2793290,841,2,117,86045,"",0,0,null,0,null,0,0,null,0,null]],[[null,null,[[8105,-2793301,0,0,null,"",null,null,null,null,null,"",null,null,"",null,null,"",null,null,"",null,null,110,0,24625,0,0,0,203966,0,0,"",0,66,[[803496,0,null,"",null,null,null,"",null,"",null,"",null,"",null,155,904,497624,1,0,0,803496,9]],null,[[803496,497624,155],[808206,413460,125],[803539,50256,50],[818558,70334,50],[818559,41670,50],[-2793301,24625,110],[-2793290,61143,105],[-2793679,16035,75],[-2929488,28105,100],[-2866746,14359,75],[-57880000,85975,100]],null],[203966,803496,183,0,null,"",null,null,null,null,null,"",null,null,"",null,null,"",null,null,"",null,null,75,0,497624,0,0,0,8105,0,0,"",0,70,[[-2866746,0,null,"",null,null,null,"",null,"",null,"",null,"",null,75,241285,0,1,0,1,-2866746,9],[-2793679,0,null,"",null,null,null,"",null,"",null,"",null,"",null,75,361926,0,1,0,1,-2793679,9]],null,[[803496,497624,75],[808206,413460,125],[803539,50256,50],[818558,70334,50],[818559,41670,50],[-2793301,24625,110],[-2793290,61143,105],[-2793679,0,75],[-2929488,28105,100],[-2866746,0,75],[-57880000,85975,100]],null],[8105,-2793290,1,0,null,"",null,null,null,null,null,"",null,null,"",null,null,"",null,null,"",null,null,0,0,61143,0,0,0,203966,0,0,"",0,70,[[803539,0,null,"",null,null,null,"",null,"",null,"",null,"",null,50,906,49350,1,0,0,803539,9]],null,[[803496,497624,75],[808206,413460,125],[803539,49350,50],[818558,70334,50],[818559,41670,50],[-2793301,24625,110],[-2793290,61143,0],[-2929488,28105,100],[-57880000,85975,100]],null],[203966,808206,166,4,null,"",null,null,null,null,null,"",null,null,"",null,null,"",null,null,"",null,null,0,0,413460,0,0,0,8105,1,0,"",103365,70,[[-2793301,0,null,"",null,null,null,"",null,"",null,"",null,"",null,110,252296,0,1,0,1,-2793301,9]],null,[[803496,497624,75],[808206,413460,0],[803539,49350,50],[818558,70334,50],[818559,41670,50],[-2793301,0,110],[-2793290,61143,0],[-2929488,28105,100],[-57880000,85975,100]],null],[8105,-2929488,73,2,null,"",null,null,null,null,null,"",null,null,"",null,null,"",null,null,"",null,null,0,0,28105,0,0,0,203966,0,0,"",0,70,[[803539,0,null,"",null,null,null,"",null,"",null,"",null,"",null,60,809,48541,1,1,0,803539,8]],null,[[803496,497624,75],[808206,413460,0],[803539,48541,60],[818558,70334,50],[818559,41670,50],[-2793290,61143,0],[-2929488,28105,0],[-57880000,85975,100]],null],[203966,803539,0,0,null,"",null,null,null,null,null,"",null,null,"",null,null,"",null,null,"",null,null,60,0,48541,0,0,0,8105,0,0,"",0,68,[[-2929488,73,[[64,73]],"",null,null,null,"",null,"",null,"",null,"",null,0,3968,24137,1,0,0,0,10]],null,[[803496,497624,75],[808206,413460,0],[803539,48541,60],[818558,70334,50],[818559,41670,50],[-2793290,61143,0],[-2929488,24137,0],[-57880000,85975,100]],null],[203966,803539,0,0,null,"",null,null,null,null,null,"",null,null,"",null,null,"",null,null,"",null,null,85,0,48541,0,0,0,8105,0,0,"",0,66,[[-2793290,0,null,"",null,null,null,"",null,"",null,"",null,"",null,25,7935,53208,1,0,0,-2793290,9]],null,[[803496,497624,75],[808206,413460,0],[803539,48541,85],[818558,70334,50],[818559,41670,50],[-2793290,53208,25],[-2929488,24137,0],[-57880000,85975,100]],null],[8105,-57880000,271,0,null,"",null,null,null,null,null,"",null,null,"",null,null,"",null,null,"",null,null,0,0,85975,0,0,0,203966,0,0,"",0,70,null,[[10048,"803496"]],[[803496,497624,75],[808206,413460,0],[803539,48540,85],[818558,70334,50],[818559,41670,50],[-2793290,53208,25],[-2929488,24137,0],[-57880000,85975,0]],null],[203966,818558,0,0,null,"",null,null,null,null,null,"",null,null,"",null,null,"",null,null,"",null,null,75,0,70334,0,0,0,8105,0,0,"",0,66,[[-2793290,0,null,"",null,null,null,"",null,"",null,"",null,"",null,50,12083,41125,1,0,0,-2793290,8]],null,[[803496,497624,75],[808206,413460,0],[803539,48540,85],[818558,70334,75],[818559,41670,50],[-2793290,41125,50],[-2929488,24137,0],[-57880000,85975,0]],null],[203966,818559,0,0,null,"",null,null,null,null,null,"",null,null,"",null,null,"",null,null,"",null,null,75,0,41670,0,0,0,8105,0,0,"",0,69,[[-2793290,0,null,"",null,null,null,"",null,"",null,"",null,"",null,75,7238,33887,1,0,0,-2793290,7]],null,[[803496,497624,75],[808206,413460,0],[803539,48540,85],[818558,70334,75],[818559,41670,75],[-2793290,33887,75],[-2929488,24137,0],[-57880000,85975,0]],null]],[[8105,2620],[203966,886731]],null,null,[[803498,38458,75],[803496,497624,75],[808206,413460,0],[803539,48540,85],[818558,70334,75],[818559,41670,75],[-2793301,0,110],[-2793290,33887,75],[-2793679,0,75],[-2929488,24137,0],[-2866746,0,75],[-57880000,85975,0]]],[null,null,[[8105,-2793290,0,0,null,"",null,null,null,null,null,"",null,null,"",null,null,"",null,null,"",null,null,100,0,33887,0,0,0,203966,0,0,"",0,66,[[803539,0,[[64,271]],"",null,null,null,"",null,"",null,"",null,"",null,110,984,47556,1,0,0,803539,7]],null,[[803496,497624,75],[808206,413460,0],[803539,47556,110],[818558,70334,75],[818559,41670,75],[-2793290,33887,100],[-2929488,24137,0],[-57880000,85975,0]],null],[203966,803496,0,0,[[64,271,1]],"",null,null,null,null,null,"",null,null,"",null,null,"",null,null,"",null,null,75,0,497624,0,0,0,8105,0,0,"",0,66,[[-2793290,0,null,"",null,null,null,"",null,"",null,"",null,"",null,115,0,33887,0,0,0,-2793290,7]],null,[[803496,497624,75],[808206,413460,0],[803539,47556,110],[818558,70334,75],[818559,41670,75],[-2793290,33887,115],[-2929488,24137,0],[-57880000,85975,0]],null],[8105,-2929488,73,1,null,"",null,null,null,null,null,"",null,null,"",null,null,"",null,null,"",null,null,25,0,24137,0,0,0,203966,0,0,"",0,66,[[803539,0,[[64,271]],"",null,null,null,"",null,"",null,"",null,"",null,135,873,46683,1,0,0,803539,6]],null,[[803496,497624,75],[808206,413460,0],[803539,46683,135],[818558,70334,75],[818559,41670,75],[-2793290,33887,115],[-2929488,24137,25],[-57880000,85975,0]],null],[203966,808206,166,3,null,"",null,null,null,null,null,"",null,null,"",null,null,"",null,null,"",null,null,25,0,413460,0,0,0,8105,1,0,"",0,66,[[-2793290,0,null,"",null,null,null,"",null,"",null,"",null,"",null,140,149860,0,1,0,0,-2793290,6]],null,[[803496,497624,75],[808206,413460,25],[803539,46683,135],[818558,70334,75],[818559,41670,75],[-2793290,0,140],[-2929488,24137,25],[-57880000,85975,0]],null],[8105,-57880000,0,0,null,"",null,null,null,null,null,"",null,null,"",null,null,"",null,null,"",null,null,25,0,85975,0,0,0,203966,0,0,"",0,66,[[803539,0,[[64,271]],"",null,null,null,"",null,"",null,"",null,"",null,160,5387,41296,1,0,0,803539,5]],null,[[803496,497624,75],[808206,413460,25],[803539,41296,160],[818558,70334,75],[818559,41670,75],[-2929488,24137,25],[-57880000,85975,25]],null],[203966,803539,74,0,[[64,271,1]],"",null,null,null,null,null,"",null,null,"",null,null,"",null,null,"",null,null,0,0,41296,0,0,0,8105,0,0,"",0,70,[[-2929488,73,[[64,73]],"",null,null,null,"",null,"",null,"",null,"",null,25,0,24137,0,0,0,-2929488,10]],null,[[803496,497624,75],[808206,413460,25],[803539,41296,0],[818558,70334,75],[818559,41670,75],[-2929488,24137,25],[-57880000,85975,25]],null],[203966,818558,0,0,null,"",null,null,null,null,null,"",null,null,"",null,null,"",null,null,"",null,null,125,0,70334,0,0,0,8105,0,0,"",0,66,[[-2929488,73,[[64,73]],"",null,null,null,"",null,"",null,"",null,"",null,50,19619,4518,1,0,1,-2929488,9]],null,[[803496,497624,75],[808206,413460,25],[803539,41296,0],[818558,70334,125],[818559,41670,75],[-2929488,4518,50],[-57880000,85975,25]],null],[203966,818559,0,0,null,"",null,null,null,null,null,"",null,null,"",null,null,"",null,null,"",null,null,100,0,41670,0,0,0,8105,0,0,"",0,69,[[-2929488,73,[[64,73]],"",null,null,null,"",null,"",null,"",null,"",null,75,7644,0,1,0,0,-2929488,8]],null,[[803496,497624,75],[808206,413460,25],[803539,41296,0],[818558,70334,125],[818559,41670,100],[-2929488,0,75],[-57880000,85975,25]],null]],[[8105,9864],[203966,1063854]],null,null,[[803498,38458,75],[803496,497624,75],[808206,413460,25],[803539,41296,0],[818558,70334,125],[818559,41670,100],[-2793301,0,110],[-2793290,0,140],[-2793679,0,75],[-2929488,0,75],[-2866746,0,75],[-57880000,85975,25]]],[null,null,[[8105,-57880000,0,0,null,"",null,null,null,null,null,"",null,null,"",null,null,"",null,null,"",null,null,50,0,85975,0,0,0,203966,0,0,"",0,66,[[803539,74,[[64,271],[64,74]],"",null,null,null,"",null,"",null,"",null,"",null,25,5584,35712,1,0,0,803539,4]],null,[[803496,497624,75],[808206,413460,25],[803539,35712,25],[818558,70334,125],[818559,41670,100],[-57880000,85975,50]],null],[203966,803496,0,0,null,"",null,null,null,null,null,"",null,null,"",null,null,"",null,null,"",null,null,75,0,497624,0,0,0,8105,0,0,"",0,66,[[-57880000,0,null,"",null,null,null,"",null,"",null,"",null,"",null,50,0,85975,0,0,0,-57880000,10]],null,[[803496,497624,75],[808206,413460,25],[803539,35712,25],[818558,70334,125],[818559,41670,100],[-57880000,85975,50]],null],[203966,808206,166,2,null,"",null,null,null,null,null,"",null,null,"",null,null,"",null,null,"",null,null,50,0,413460,0,0,0,8105,1,0,"",0,66,[[-57880000,0,null,"",null,null,null,"",null,"",null,"",null,"",null,75,119836,0,1,0,0,-57880000,9]],null,[[803496,497624,75],[808206,413460,50],[803539,35712,25],[818558,70334,125],[818559,41670,100],[-57880000,0,75]],null]],[[8105,15448],[203966,1183690]],null,null,null]],0]],0,26]
//============================================================================
Json::Value sxd_client::Mod_StTakeBible_Base_rob(int rob_id, int sequence_id) {
    Json::Value data;
    data.append(rob_id);
    data.append(sequence_id);
    return this->send_and_receive(data, 114, 11);
}

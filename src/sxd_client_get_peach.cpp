#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

void sxd_client::get_peach() {
    Json::Value data = this->Mod_GetPeach_Base_peach_info();
    int fruit_lv = 70 + data[0].asInt() * 5;
    int peach_num = data[1].asInt();
    common::log(boost::str(boost::format("��ժ���ҡ���ǰ [%1%] �� [%2%] ������") % peach_num % fruit_lv));
    if (peach_num > 0) {
        data = this->Mod_GetPeach_Base_batch_get_peach();
        if (data[0].asInt() == 0)
            common::log(boost::str(boost::format("��ժ���ҡ�һ��ժ�ҳɹ�����þ���ֵ[%1%]") % data[1]));
        else
            common::log(boost::str(boost::format("��ժ���ҡ�һ��ժ��ʧ�ܣ�result[%1%]") % data[0]));
    }
}

//============================================================================
// R171 ��ȡ������Ϣ
// {module:40, action:3,
// request:[],
// response:[Utils.ByteUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.ByteUtil]}
// GetPeachController.as 34:
//     _loc_2.peachLv = _loc_1[0];
//     _loc_2.fruitLv = 70 + _loc_2.peachLv * 5;
//     _loc_2.peachNum = _loc_1[1];
//     _loc_2.bAllGet = _loc_1[2] == 1;
//     _loc_2.bCallPeach = _loc_1[3] == 1;
//============================================================================
// Example
//     [ 21, 5, 1, 0 ]
Json::Value sxd_client::Mod_GetPeach_Base_peach_info() {
    Json::Value data;
    return this->send_and_receive(data, 40, 3);
}

//============================================================================
// R171 һ��ժ��
// {module:40, action:5,
// request:[],
// response:[Utils.UByteUtil, Utils.LongUtil]}
// GetPeachData.as 71:
//     this.batchGetPeachResult = param1[0];
//     this.warExp = param1[1];
// Mod_GetPeach_Base.as 7:
//     public static const SUCCESS:int = 0;
// Example
//     [ 0, 350000000 ]
//============================================================================
Json::Value sxd_client::Mod_GetPeach_Base_batch_get_peach() {
    Json::Value data;
    return this->send_and_receive(data, 40, 5);
}


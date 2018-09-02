#include <boost/format.hpp>
#include "common.h"
#include "sxd_client.h"

class Mod_PetAnimal_Base {
public:
    static const int SUCCESS = 0;
    static const int TOO_MUCH_EXP = 2;
};

void sxd_client::pet_animal() {

    // pet animal info
    Json::Value data = this->Mod_PetAnimal_Base_pet_animal_info();
    int lv = data[1].asInt();
    int star = data[2].asInt();
    int feed_num = data[4].asInt();
    if (lv >= 10 && star >= 10)     // 龙王已升级至最高等级
        return;

    while (feed_num) {
        // feed
        data = this->Mod_PetAnimal_Base_feed_pet_animal();
        int result = data[0].asInt();

        if (result == Mod_PetAnimal_Base::SUCCESS) {
            feed_num = data[6].asInt();
            int hit = data[7].asInt();
            if (hit)
                common::log(boost::str(boost::format("【叶公好龙】普通培养，暴击！获得 [经验值×%1%]") % data[4]), iEdit);
            else
                common::log(boost::str(boost::format("【叶公好龙】普通培养，获得 [经验值×%1%]") % data[4]), iEdit);

        } else if (result == Mod_PetAnimal_Base::TOO_MUCH_EXP) {
            // up
            this->Mod_PetAnimal_Base_up_pet_animal();
            common::log("【叶公好龙】进化", iEdit);

        } else {
            common::log(boost::str(boost::format("【叶公好龙】培养失败，result[%1%]") % data[0]), iEdit);
            return;
        }
    }
}

//============================================================================
// R172 叶公好龙
// {module:48, action:0, request:[Utils.IntUtil],
// PetView.as 151:
//     _data.call(Mod_PetAnimal_Base.pet_animal_info, this.getPetAnimalInfoBack, [_ctrl.player.playerId]);
// response:[Utils.StringUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil]};
// PetAnimalData.as 19:
//     this.info.reset(param1[1], param1[2], param1[3], param1[4]);
// PetLongInfo.as 46:
//     this.lv = param1;
//     this.star = param2;
//     this.exp = param3;
//     this.feedNum = param4;
//============================================================================
Json::Value sxd_client::Mod_PetAnimal_Base_pet_animal_info() {
    Json::Value data;
    data.append(player_id);
    return this->send_and_receive(data, 48, 0);
}

//============================================================================
// R172 培养
// {module:48, action:2, request:[Utils.ByteUtil],
// Example
//     [ 0 ] onCoin
//     [ 1 ] onIngot
// response:[Utils.UByteUtil, Utils.ByteUtil, Utils.ByteUtil, Utils.IntUtil, Utils.IntUtil, Utils.ByteUtil, Utils.IntUtil, Utils.ByteUtil]};
// PetAnimalData.as 25:
//     this.result = param1[0];
//     this.info.feed(param1[1], param1[2], param1[3], param1[4], param1[5], param1[6], param1[7]);
// PetLongInfo.as 59:
//     this.isUpStar = this.star != param2;
//     this.isUpLv = false;
//     this.lv = param1;
//     this.star = param2;
//     this.feedType = param5;
//     this.hit = param7;
//     this.exp = param3;
//     this.expAdd = param4;
//     this.expMax = PetType.getExp(param1, param2);
//     this.feedNum = param6;
// Example
//     [0,9,10,9039707,52500,0,13,**1**]    暴击
//     [**2**,0,0,0,0,0,0,0]                待进化
//============================================================================
Json::Value sxd_client::Mod_PetAnimal_Base_feed_pet_animal() {
    Json::Value data;
    data.append(0);
    return this->send_and_receive(data, 48, 2);
}

//============================================================================
// R172 进化
// {module:48, action:3, request:[], response:[Utils.ByteUtil, Utils.ByteUtil, Utils.IntUtil]};
// PetAnimalData.as 41:
//     this.info.upLv(param1[0], param1[1], param1[2]);
// PetLongInfo.as 74:
//     this.lv = param1;
//     this.star = param2;
//     this.isUpStar = false;
//     this.isUpLv = true;
//     this.exp = param3;
//     this.expAdd = 0;
//     this.expMax = PetType.getExp(param1, param2);
// Example
//     [ 10, 1, 39707 ]
//     [ 10, 1, 6517049 ]
//============================================================================
Json::Value sxd_client::Mod_PetAnimal_Base_up_pet_animal() {
    Json::Value data;
    return this->send_and_receive(data, 48, 3);
}


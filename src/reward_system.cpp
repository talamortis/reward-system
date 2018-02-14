//Reward system made by Talamortis

#include "Configuration/Config.h"
#include "Player.h"
#include "AccountMgr.h"
#include "ScriptMgr.h"
#include "Define.h"
#include "GossipDef.h"
#include "DataMap.h"

bool RewardSystem_Enable;
uint32 Max_roll;

class RewardTimer : public DataMap::Base
{
public:
    RewardTimer() { Reset(); }
    void Reset()
    {
        timer = urand(2 * HOUR*IN_MILLISECONDS, 4 * HOUR*IN_MILLISECONDS);
    }
    uint32 timer;
};

class reward_system : public PlayerScript
{
public:
    reward_system() : PlayerScript("rewardsystem") {}

    void OnBeforeUpdate(Player* player, uint32 p_time) override
    {
        if (!RewardSystem_Enable)
            return;
        if (player->isAFK())
            return;

        RewardTimer* rewardtimer = p->CustomData.GetDefault<RewardTimer>("RewardTimer");
        if (rewardtimer->timer <= p_time)
        {
            rewardtimer->Reset();

            uint32 roll = urand(1, Max_roll); //Lets make a random number from 1 to Max_roll
            QueryResult result = CharacterDatabase.PQuery("SELECT item, quantity FROM reward_system WHERE roll = %u", roll);
            if (!result)
                return;

            do
            {
                //Lets now get the item
                Field* fields = result->Fetch();
                uint32 item = fields[0].GetUInt32();
                uint32 quantity = fields[1].GetUInt32();

                // now lets add the item
                // TODO: What to do if player bags are full?
                player->AddItem(item, quantity);
            } while (result->NextRow());

            ChatHandler(player->GetSession()).PSendSysMessage("Your playtime has earned you a reward!");
        }
        else
            rewardtimer->timer -= p_time;
    }
};

class reward_system_conf : public WorldScript
{
public:
    reward_system_conf() : WorldScript("reward_system_conf") { }

    void OnBeforeConfigLoad(bool reload) override
    {
        if (!reload) {
            std::string conf_path = _CONF_DIR;
            std::string cfg_file = conf_path + "/reward_system.conf";

#ifdef WIN32
            cfg_file = "reward_system.conf";
#endif // WIN32

            std::string cfg_def_file = cfg_file + ".dist";
            sConfigMgr->LoadMore(cfg_def_file.c_str());
            sConfigMgr->LoadMore(cfg_file.c_str());
            RewardSystem_Enable = sConfigMgr->GetBoolDefault("RewardSystemEnable", true);
            Max_roll = sConfigMgr->GetIntDefault("MaxRoll", 1000);
        }
    }
};

void AddRewardSystemScripts()
{
    new reward_system();
    new reward_system_conf();
}

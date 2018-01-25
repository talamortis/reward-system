//Reward system made by Talamortis

#include "Configuration/Config.h"
#include "Player.h"
#include "AccountMgr.h"
#include "ScriptMgr.h"
#include "Define.h"
#include "GossipDef.h"

int32 roll;
bool RewardSystem_Enable;
uint32 Max_roll;

class reward_system : public PlayerScript
{
public:
    reward_system() : PlayerScript("rewardsystem") {}

    uint32 rewardtimer = urand(2 * HOUR*IN_MILLISECONDS, 4 * HOUR*IN_MILLISECONDS);

    void OnBeforePlayerUpdate(Player* player, uint32 p_time)
    {

        if (!RewardSystem_Enable)
            return;
        {
            if (rewardtimer <= p_time)
            {
                roll = urand(1, Max_roll); //Lets make a random number from 1 - 
                QueryResult result = CharacterDatabase.PQuery("SELECT item, quantity FROM reward_system WHERE roll = '%u'", roll);
                rewardtimer = urand(2 * HOUR*IN_MILLISECONDS, 4 * HOUR*IN_MILLISECONDS);
				
                if (!result || player->isAFK())
                {
                    return;
                } 
                else
                {
                    do
                    {
                        //Lets now get the item
                        Field* fields = result->Fetch();
                        uint32 pItem = fields[0].GetInt32();
                        uint32 quantity = fields[1].GetInt32();

                        // now lets add the item
                        player->AddItem(pItem, quantity);

                        ChatHandler(player->GetSession()).PSendSysMessage("You have rolled %u which gave you item %u", roll, pItem);

                    } while (result->NextRow());
                }
            }
            else
                rewardtimer -= p_time;
        }
    }
};

class reward_system_conf : public WorldScript
{
public:
    reward_system_conf() : WorldScript("reward_system_conf") { }

    void OnBeforeConfigLoad(bool reload) override
    {
        if (!reload) {
            std::string cfg_file = "reward_system.conf";
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
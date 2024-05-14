#include "player_util.hpp"

namespace player {
    bool GetPlayerPed(Player player, Ped* player_ped)
    {
        if (!ENTITY::DOES_ENTITY_EXIST(PLAYER::PLAYER_PED_ID()))
            return false;

        const auto ped = PLAYER::GET_PLAYER_PED(player);

        if (!ENTITY::DOES_ENTITY_EXIST(ped))
            return false;

        if (player_ped)
            *player_ped = ped;

        return true;
    }

    void SendSubtitle(const char* text, int duration)
    {
        HUD::BEGIN_TEXT_COMMAND_PRINT("STRING");
        HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text);
        HUD::END_TEXT_COMMAND_PRINT(duration, true);
    }
}
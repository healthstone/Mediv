/*
 * Copyright (C) 2008-2018 AtieshCore <https://at-wow.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* ScriptData
Name: vip_commandscript
%Complete: 100
Comment: All vip related commands
Category: commandscripts
EndScriptData */

#include "AccountMgr.h"
#include "AditionalData.h"
#include "Chat.h"
#include "GameTime.h"
#include "Group.h"
#include "ScriptMgr.h"
#include "Language.h"
#include "MapManager.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "RBAC.h"
#include "SpellHistory.h"
#include "WorldSession.h"
#include "World.h"

#if TRINITY_COMPILER == TRINITY_COMPILER_GNU
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

class vip_commandscript : public CommandScript
{
public:
    vip_commandscript() : CommandScript("vip_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> vipCommandTable =
                {
                        { "debuff",           rbac::RBAC_PERM_COMMAND_VIP_DEBUFF,       false, &HandleVipDebuffCommand,        "" },
                        { "bank",             rbac::RBAC_PERM_COMMAND_VIP_BANK,         false, &HandleVipBankCommand,          "" },
                        { "mail",             rbac::RBAC_PERM_COMMAND_VIP_MAIL,         false, &HandlePremiumMailCommand,      "" },
                        { "repair",           rbac::RBAC_PERM_COMMAND_VIP_REPAIR,       false, &HandleVipRepairCommand,        "" },
                        { "resettalents",     rbac::RBAC_PERM_COMMAND_VIP_RESETTALENTS, false, &HandleVipResetTalentsCommand,  "" },
                        { "taxi",             rbac::RBAC_PERM_COMMAND_VIP_TAXI,         false, &HandleVipTaxiCommand,          "" },
                        { "home",             rbac::RBAC_PERM_COMMAND_VIP_HOME,         false, &HandleVipHomeCommand,          "" },
                        { "capital",          rbac::RBAC_PERM_COMMAND_VIP_CAPITAL,      false, &HandleVipCapitalCommand,       "" },
                        { "changerace",       rbac::RBAC_PERM_COMMAND_VIP_CHANGERACE,   false, &HandleChangeRaceCommand,       "" },
                        { "customize",        rbac::RBAC_PERM_COMMAND_VIP_CUSTOMIZE,    false, &HandleCustomizeCommand,        "" },
                        { "app",              rbac::RBAC_PERM_COMMAND_VIP_ARPPEAR,      false, &HandleVipAppearCommand,        "" },
                        { "set",              rbac::RBAC_PERM_COMMAND_VIP_SET,          true, &HandleSetVipCommand,            "" },
                        { "del",              rbac::RBAC_PERM_COMMAND_VIP_REMOVE,       true, &HandleDelVipCommand,            "" },
                        { "",                 rbac::RBAC_PERM_COMMAND_VIP,              false, &HandleVipCommand,              "" },
                };
        static std::vector<ChatCommand> coinCommandTable =
                {
                        { "add",              rbac::RBAC_PERM_COMMAND_ADDCOIN,          true, &HandleCoinAddCommand,           "" },
                        { "del",              rbac::RBAC_PERM_COMMAND_ADDCOIN,          true, &HandleCoinDelCommand,           "" },
                        { "",                 rbac::RBAC_PERM_COMMAND_ACCOUNT,          false, &HandleCoinCommand,             "" },
                };
        static std::vector<ChatCommand> commandTable =
                {
                        { "vip",              rbac::RBAC_PERM_COMMAND_VIP,              true,  nullptr,       "", vipCommandTable },
                        { "coin",             rbac::RBAC_PERM_COMMAND_ACCOUNT,           true,  nullptr,      "", coinCommandTable },
                };
        return commandTable;
    }

    static bool HandleSetVipCommand(ChatHandler* handler, uint32 days_bonus, uint32 accountID)
    {
        Player* target = handler->getSelectedPlayerOrSelf();

        if (!accountID)
            accountID = target->GetSession()->GetAccountId();

        time_t current_time = GameTime::GetGameTime();
        time_t unsetdate = current_time + 24 * 60 * 60 * days_bonus;

        bool vip = AccountMgr::GetVipStatus(accountID);

        if (vip)
            AccountMgr::UpdateVipStatus(accountID, unsetdate);
        else
            AccountMgr::SetVipStatus(accountID, unsetdate);

        // check on online
        ObjectGuid::LowType guid = AccountMgr::GetGuidOfOnlineCharacter(accountID);
        if (guid)
        {
            if (Player* p = ObjectAccessor::FindPlayerByLowGUID(guid))
            {
                p->GetAditionalData()->setPremiumStatus(true);
                p->GetAditionalData()->setPremiumUnsetdate(unsetdate);
                handler->PSendSysMessage("VIP privileges has been set for Account: %u Character:[%s] (online) (GUID: %u), for %u days", accountID, p->GetName(), p->GetGUID().GetCounter(), days_bonus);
            }
        }
        else
            handler->PSendSysMessage("VIP privileges has been set for Account: %u, for %u days, no characters online", accountID, days_bonus);
        return true;
    }

    static bool HandleDelVipCommand(ChatHandler* handler, uint32 accountID)
    {
        Player* target = handler->getSelectedPlayerOrSelf();

        if (!accountID)
            accountID = target->GetSession()->GetAccountId();

        bool vip = AccountMgr::GetVipStatus(accountID);
        if (!vip)
        {
            handler->SendSysMessage(LANG_PLAYER_TARGET_NOT_VIP);
            handler->SetSentErrorMessage(true);
            return false;
        }

        AccountMgr::RemoveVipStatus(accountID);
        ObjectGuid::LowType guid = AccountMgr::GetGuidOfOnlineCharacter(accountID);
        if (guid)
        {
            if (Player* p = ObjectAccessor::FindPlayerByLowGUID(guid))
            {
                p->GetAditionalData()->setPremiumStatus(false);
                p->GetAditionalData()->setPremiumUnsetdate(0);
                handler->PSendSysMessage("VIP privileges were removed for Account: %u Character:[%s] (online) (GUID: %u)", accountID, p->GetName(), p->GetGUID().GetCounter());
            }
        }
        else
            handler->PSendSysMessage("VIP privileges were removed for Account: %u, no characters online", accountID);
        return true;
    }

    static bool HandleVipCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();

        if (!player->GetAditionalData()->isPremium() && !player->IsGameMaster())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_VIP);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->GetAditionalData()->isPremium())
            handler->PSendSysMessage(LANG_PLAYER_VIP_TIME_EXIST, (secsToTimeString(player->GetAditionalData()->getPremiumUnsetdate() - GameTime::GetGameTime(), TimeFormat::ShortText)));

        handler->PSendSysMessage("The next VIP commands are available for you:");
        if (sWorld->customGetBoolConfig(CONFIG_VIP_DEBUFF))
            handler->PSendSysMessage(" .vip debuff");
        if (sWorld->customGetBoolConfig(CONFIG_VIP_BANK))
            handler->PSendSysMessage(" .vip bank");
        if (sWorld->customGetBoolConfig(CONFIG_VIP_MAIL))
            handler->PSendSysMessage(" .vip mail");
        if (sWorld->customGetBoolConfig(CONFIG_VIP_REPAIR))
            handler->PSendSysMessage(" .vip repair");
        if (sWorld->customGetBoolConfig(CONFIG_VIP_RESET_TALENTS))
            handler->PSendSysMessage(" .vip resettalents");
        if (sWorld->customGetBoolConfig(CONFIG_VIP_TAXI))
            handler->PSendSysMessage(" .vip taxi");
        if (sWorld->customGetBoolConfig(CONFIG_VIP_HOME))
            handler->PSendSysMessage(" .vip home");
        if (sWorld->customGetBoolConfig(CONFIG_VIP_CAPITAL))
            handler->PSendSysMessage(" .vip capital");
        if (sWorld->customGetBoolConfig(CONFIG_VIP_CHANGE_RACE))
            handler->PSendSysMessage(" .vip changerace");
        if (sWorld->customGetBoolConfig(CONFIG_VIP_CUSTOMIZE))
            handler->PSendSysMessage(" .vip customize");
        if (sWorld->customGetBoolConfig(CONFIG_VIP_APPEAR))
            handler->PSendSysMessage(" .vip app");

        if (handler->HasPermission(rbac::RBAC_PERM_COMMAND_VIP_SET))
            handler->PSendSysMessage(" .vip set");
        if (handler->HasPermission(rbac::RBAC_PERM_COMMAND_VIP_REMOVE))
            handler->PSendSysMessage(" .vip del");

        return true;
    }

    static bool HandleVipDebuffCommand(ChatHandler* handler, const char* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();

        if (!player->GetAditionalData()->isPremium())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_VIP);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!sWorld->customGetBoolConfig(CONFIG_VIP_DEBUFF))
        {
            handler->SendSysMessage(LANG_VIP_COMMAND_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->IsInCombat())
        {
            handler->SendSysMessage(LANG_YOU_IN_COMBAT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->IsInFlight())
        {
            handler->SendSysMessage(LANG_YOU_IN_FLIGHT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->GetMap()->IsBattlegroundOrArena())
        {
            handler->SendSysMessage(LANG_VIP_BG);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->HasStealthAura())
        {
            handler->SendSysMessage(LANG_VIP_STEALTH);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->isDead() || player->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH))
        {
            handler->SendSysMessage(LANG_VIP_DEAD);
            handler->SetSentErrorMessage(true);
            return false;
        }

        player->RemoveAurasDueToSpell(15007); // Resurrection Sickness
        player->RemoveAurasDueToSpell(26013); // Deserter
        return true;
    }

    static bool HandleVipBankCommand(ChatHandler* handler, const char* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();

        if (!player->GetAditionalData()->isPremium())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_VIP);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!sWorld->customGetBoolConfig(CONFIG_VIP_BANK))
        {
            handler->SendSysMessage(LANG_VIP_COMMAND_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->IsInCombat())
        {
            handler->SendSysMessage(LANG_YOU_IN_COMBAT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->IsInFlight())
        {
            handler->SendSysMessage(LANG_YOU_IN_FLIGHT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->GetMap()->IsBattlegroundOrArena())
        {
            handler->SendSysMessage(LANG_VIP_BG);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->HasStealthAura())
        {
            handler->SendSysMessage(LANG_VIP_STEALTH);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->isDead() || player->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH))
        {
            handler->SendSysMessage(LANG_VIP_DEAD);
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->GetSession()->SendShowBank(player->GetGUID());
        return true;
    }

    static bool HandlePremiumMailCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();

        if (!player->GetAditionalData()->isPremium())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_VIP);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!sWorld->customGetBoolConfig(CONFIG_VIP_MAIL))
        {
            handler->SendSysMessage(LANG_VIP_COMMAND_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->IsInCombat())
        {
            handler->SendSysMessage(LANG_YOU_IN_COMBAT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->IsInFlight())
        {
            handler->SendSysMessage(LANG_YOU_IN_FLIGHT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->GetMap()->IsBattlegroundOrArena())
        {
            handler->SendSysMessage(LANG_VIP_BG);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->HasStealthAura())
        {
            handler->SendSysMessage(LANG_VIP_STEALTH);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->isDead() || player->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH))
        {
            handler->SendSysMessage(LANG_VIP_DEAD);
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->GetSession()->SendShowMailBox(player->GetGUID());
        return true;
    }

    static bool HandleVipRepairCommand(ChatHandler* handler, const char* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();

        if (!player->GetAditionalData()->isPremium())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_VIP);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!sWorld->customGetBoolConfig(CONFIG_VIP_REPAIR))
        {
            handler->SendSysMessage(LANG_VIP_COMMAND_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->IsInCombat())
        {
            handler->SendSysMessage(LANG_YOU_IN_COMBAT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->IsInFlight())
        {
            handler->SendSysMessage(LANG_YOU_IN_FLIGHT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->GetMap()->IsBattlegroundOrArena())
        {
            handler->SendSysMessage(LANG_VIP_BG);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->HasStealthAura())
        {
            handler->SendSysMessage(LANG_VIP_STEALTH);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->isDead() || player->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH))
        {
            handler->SendSysMessage(LANG_VIP_DEAD);
            handler->SetSentErrorMessage(true);
            return false;
        }

        player->DurabilityRepairAll(false, 0, false);
        handler->PSendSysMessage(LANG_YOUR_ITEMS_REPAIRED, handler->GetNameLink(player));
        return true;
    }

    static bool HandleVipResetTalentsCommand(ChatHandler* handler, const char* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();

        if (!player->GetAditionalData()->isPremium())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_VIP);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!sWorld->customGetBoolConfig(CONFIG_VIP_RESET_TALENTS))
        {
            handler->SendSysMessage(LANG_VIP_COMMAND_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->IsInCombat())
        {
            handler->SendSysMessage(LANG_YOU_IN_COMBAT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->IsInFlight())
        {
            handler->SendSysMessage(LANG_YOU_IN_FLIGHT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->GetMap()->IsBattlegroundOrArena())
        {
            handler->SendSysMessage(LANG_VIP_BG);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->HasStealthAura())
        {
            handler->SendSysMessage(LANG_VIP_STEALTH);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->isDead() || player->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH))
        {
            handler->SendSysMessage(LANG_VIP_DEAD);
            handler->SetSentErrorMessage(true);
            return false;
        }

        player->ResetTalents(true);
        player->SendTalentsInfoData(false);
        handler->PSendSysMessage(LANG_RESET_TALENTS_ONLINE, handler->GetNameLink(player));
        return true;
    }

    static bool HandleVipTaxiCommand(ChatHandler* handler, const char* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();

        if (!player->GetAditionalData()->isPremium())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_VIP);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!sWorld->customGetBoolConfig(CONFIG_VIP_TAXI))
        {
            handler->SendSysMessage(LANG_VIP_COMMAND_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->IsInCombat())
        {
            handler->SendSysMessage(LANG_YOU_IN_COMBAT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->IsInFlight())
        {
            handler->SendSysMessage(LANG_YOU_IN_FLIGHT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->GetMap()->IsBattlegroundOrArena())
        {
            handler->SendSysMessage(LANG_VIP_BG);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->HasStealthAura())
        {
            handler->SendSysMessage(LANG_VIP_STEALTH);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->isDead() || player->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH))
        {
            handler->SendSysMessage(LANG_VIP_DEAD);
            handler->SetSentErrorMessage(true);
            return false;
        }

        player->SetTaxiCheater(true);
        handler->PSendSysMessage(LANG_YOU_GIVE_TAXIS, handler->GetNameLink(player));
        if (handler->needReportToTarget(player))
            ChatHandler(player->GetSession()).PSendSysMessage(LANG_YOURS_TAXIS_ADDED, handler->GetNameLink());
        return true;
    }

    static bool HandleVipHomeCommand(ChatHandler* handler, const char* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();

        if (!player->GetAditionalData()->isPremium())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_VIP);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!sWorld->customGetBoolConfig(CONFIG_VIP_HOME))
        {
            handler->SendSysMessage(LANG_VIP_COMMAND_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->IsInCombat())
        {
            handler->SendSysMessage(LANG_YOU_IN_COMBAT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->IsInFlight())
        {
            handler->SendSysMessage(LANG_YOU_IN_FLIGHT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->GetMap()->IsBattlegroundOrArena())
        {
            handler->SendSysMessage(LANG_VIP_BG);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->HasStealthAura())
        {
            handler->SendSysMessage(LANG_VIP_STEALTH);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->isDead() || player->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH))
        {
            handler->SendSysMessage(LANG_VIP_DEAD);
            handler->SetSentErrorMessage(true);
            return false;
        }

        player->GetSpellHistory()->ResetCooldown(8690, true);
        player->CastSpell(player, 8690, false);
        return true;
    }

    static bool HandleChangeRaceCommand(ChatHandler* handler, const char* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();

        if (!player->GetAditionalData()->isPremium())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_VIP);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!sWorld->customGetBoolConfig(CONFIG_VIP_CHANGE_RACE))
        {
            handler->SendSysMessage(LANG_VIP_COMMAND_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->IsInCombat())
        {
            handler->SendSysMessage(LANG_YOU_IN_COMBAT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->IsInFlight())
        {
            handler->SendSysMessage(LANG_YOU_IN_FLIGHT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->GetMap()->IsBattlegroundOrArena())
        {
            handler->SendSysMessage(LANG_VIP_BG);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->HasStealthAura())
        {
            handler->SendSysMessage(LANG_VIP_STEALTH);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->isDead() || player->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH))
        {
            handler->SendSysMessage(LANG_VIP_DEAD);
            handler->SetSentErrorMessage(true);
            return false;
        }

        player->SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);
        handler->SendSysMessage(LANG_VIP_CHANGE_RACE);
        return true;
    }

    static bool HandleCustomizeCommand(ChatHandler* handler, const char* /*args*/)
    {

        Player* player = handler->GetSession()->GetPlayer();

        if (!player->GetAditionalData()->isPremium())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_VIP);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!sWorld->customGetBoolConfig(CONFIG_VIP_CUSTOMIZE))
        {
            handler->SendSysMessage(LANG_VIP_COMMAND_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->IsInCombat())
        {
            handler->SendSysMessage(LANG_YOU_IN_COMBAT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->IsInFlight())
        {
            handler->SendSysMessage(LANG_YOU_IN_FLIGHT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->GetMap()->IsBattlegroundOrArena())
        {
            handler->SendSysMessage(LANG_VIP_BG);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->HasStealthAura())
        {
            handler->SendSysMessage(LANG_VIP_STEALTH);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->isDead() || player->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH))
        {
            handler->SendSysMessage(LANG_VIP_DEAD);
            handler->SetSentErrorMessage(true);
            return false;
        }

        player->SetAtLoginFlag(AT_LOGIN_CUSTOMIZE);
        handler->SendSysMessage(LANG_VIP_CHANGE_CUSTOMIZE);
        return true;
    }

    static bool HandleVipCapitalCommand(ChatHandler* handler, const char* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();

        if (!player->GetAditionalData()->isPremium())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_VIP);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!sWorld->customGetBoolConfig(CONFIG_VIP_CAPITAL))
        {
            handler->SendSysMessage(LANG_VIP_COMMAND_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->IsInCombat())
        {
            handler->SendSysMessage(LANG_YOU_IN_COMBAT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->IsInFlight())
        {
            handler->SendSysMessage(LANG_YOU_IN_FLIGHT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->GetMap()->IsBattlegroundOrArena())
        {
            handler->SendSysMessage(LANG_VIP_BG);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->HasStealthAura())
        {
            handler->SendSysMessage(LANG_VIP_STEALTH);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->isDead() || player->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH))
        {
            handler->SendSysMessage(LANG_VIP_DEAD);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->GetTeam() == HORDE)
            player->CastSpell(player, 3567, true);
        else
            player->CastSpell(player, 3561, true);
        return true;
    }

    static bool HandleVipAppearCommand(ChatHandler* handler, char const* args)
    {
        Player* player = handler->GetSession()->GetPlayer();

        if (!player->GetAditionalData()->isPremium())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_VIP);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!sWorld->customGetBoolConfig(CONFIG_VIP_APPEAR))
        {
            handler->SendSysMessage(LANG_VIP_COMMAND_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->IsInCombat())
        {
            handler->SendSysMessage(LANG_YOU_IN_COMBAT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->IsMounted())
        {
            handler->SendSysMessage(LANG_CHAR_NON_MOUNTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->IsInFlight())
        {
            handler->SendSysMessage(LANG_YOU_IN_FLIGHT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->GetMap()->IsBattlegroundOrArena())
        {
            handler->SendSysMessage(LANG_VIP_BG);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->HasStealthAura())
        {
            handler->SendSysMessage(LANG_VIP_STEALTH);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->isDead() || player->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH))
        {
            handler->SendSysMessage(LANG_VIP_DEAD);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* target;
        ObjectGuid targetGuid;
        std::string targetName;
        if (!handler->extractPlayerTarget((char*)args, &target, &targetGuid, &targetName))
            return false;


        if (target == player || targetGuid == player->GetGUID())
        {
            handler->SendSysMessage(LANG_CANT_TELEPORT_SELF);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->GetGroup())
        {
            // check online security
            if (handler->HasLowerSecurity(target, ObjectGuid::Empty))
                return false;

            std::string chrNameLink = handler->playerLink(targetName);

            Map* map = target->GetMap();
            if (target->IsInCombat())
            {
                handler->SendSysMessage(LANG_YOU_IN_COMBAT);
                handler->SetSentErrorMessage(true);
                return false;
            }

            if (target->IsInFlight())
            {
                handler->SendSysMessage(LANG_YOU_IN_FLIGHT);
                handler->SetSentErrorMessage(true);
                return false;
            }

            if (target->GetMap()->IsBattlegroundOrArena() || target->GetAreaId() == 616)
            {
                handler->SendSysMessage(LANG_VIP_BG);
                handler->SetSentErrorMessage(true);
                return false;
            }

            if (target->HasStealthAura())
            {
                handler->SendSysMessage(LANG_VIP_STEALTH);
                handler->SetSentErrorMessage(true);
                return false;
            }

            if (target->isDead() || target->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH))
            {
                handler->SendSysMessage(LANG_VIP_DEAD);
                handler->SetSentErrorMessage(true);
                return false;
            }

            if (player->GetGroup())
            {
                // we are in group, we can go only if we are in the player group
                if (player->GetGroup() != target->GetGroup())
                {
                    handler->SendSysMessage(LANG_VIP_GROUP);
                    handler->SetSentErrorMessage(true);
                    return false;
                }
            }

            // if the player or the player's group is bound to another instance
            // the player will not be bound to another one
            InstancePlayerBind* bind = player->GetBoundInstance(target->GetMapId(), target->GetDifficulty(map->IsRaid()));
            if (!bind)
            {
                Group* group = player->GetGroup();
                // if no bind exists, create a solo bind
                InstanceGroupBind* gBind = group ? group->GetBoundInstance(target) : NULL;                // if no bind exists, create a solo bind
                if (!gBind)
                    if (InstanceSave* save = sInstanceSaveMgr->GetInstanceSave(target->GetInstanceId()))
                        player->BindToInstance(save, !save->CanReset());
            }

            if (map->IsRaid())
                player->SetRaidDifficulty(target->GetRaidDifficulty());
            else
                player->SetDungeonDifficulty(target->GetDungeonDifficulty());

            handler->PSendSysMessage(LANG_APPEARING_AT, chrNameLink);

            // stop flight if need
            if (player->IsInFlight())
                player->FinishTaxiFlight();
            else
                player->SaveRecallPosition();
            // to point to see at target with same orientation
            float x, y, z;
            target->GetContactPoint(player, x, y, z);
            player->TeleportTo(target->GetMapId(), x, y, z, player->GetAbsoluteAngle(target), TELE_TO_GM_MODE);
            player->SetPhaseMask(target->GetPhaseMask(), true);
        }
        else
        {
            // check offline security
            if (handler->HasLowerSecurity(NULL, targetGuid))
                return false;

            std::string nameLink = handler->playerLink(targetName);

            handler->SendSysMessage(LANG_PLAYER_NOT_EXIST_OR_OFFLINE);
            handler->SetSentErrorMessage(true);
            return false;
        }
        return true;
    }

    static bool HandleCoinCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* _player = handler->GetSession()->GetPlayer();
        if (!_player)
            return false;

        Player* target = handler->getSelectedPlayerOrSelf();
        uint32 coins;

        if (handler->HasPermission(rbac::RBAC_PERM_COMMAND_ADDCOIN) && _player != target)
        {
            // GM should can to see the coins of target-player by .coin too
            coins = target->GetAditionalData()->getCoins();
            handler->PSendSysMessage("The AccountID %u [player : %s ] have %u coins", target->GetSession()->GetAccountId(), target->GetName(), coins);
        }
        else
        {
            coins = _player->GetAditionalData()->getCoins();
            handler->PSendSysMessage("You have the %u coins", coins);
        }

        return true;
    }

    static bool HandleCoinAddCommand(ChatHandler* handler, int32 coinAdded, uint32 accountID)
    {
        Player* target = handler->getSelectedPlayerOrSelf();

        if (!accountID)
            accountID = target->GetSession()->GetAccountId();

        if (coinAdded < 0)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 coinCount = coinAdded;
        coinCount += AccountMgr::GetCoins(accountID);
        ObjectGuid::LowType guid = AccountMgr::GetGuidOfOnlineCharacter(accountID);
        if (guid)
        {
            if (Player* p = ObjectAccessor::FindPlayerByLowGUID(guid))
                p->GetAditionalData()->setCoins(coinCount);
        }

        AccountMgr::SetCoins(accountID, coinCount);
        handler->PSendSysMessage("The AccountID %u has received %u coins, and now have a %u coins", accountID, coinAdded, coinCount);
        return true;
    }

    static bool HandleCoinDelCommand(ChatHandler* handler, int32 coinRemoved, uint32 accountID)
    {
        Player* target = handler->getSelectedPlayerOrSelf();
        if (!accountID)
            accountID = target->GetSession()->GetAccountId();

        if (coinRemoved < 0)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }
        uint32 coinCount = AccountMgr::GetCoins(accountID);

        if (coinCount < uint32(coinRemoved))
            coinCount = 0;
        else
            coinCount -= uint32(coinRemoved);

        ObjectGuid::LowType guid = AccountMgr::GetGuidOfOnlineCharacter(accountID);
        if (guid)
        {
            if (Player* p = ObjectAccessor::FindPlayerByLowGUID(guid))
                p->GetAditionalData()->setCoins(coinCount);
        }

        AccountMgr::SetCoins(accountID, coinCount);
        handler->PSendSysMessage("The AccountID %u has removed %u coins, and now have a %u coins", accountID, coinRemoved, coinCount);
        return true;
    }
};

void AddSC_vip_commandscript()
{
    new vip_commandscript();
}

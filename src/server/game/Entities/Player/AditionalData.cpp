#include "AditionalData.h"
#include "AccountMgr.h"
#include "AuctionHouseMgr.h"
#include "Chat.h"
#include "GameTime.h"
#include "MedivhAdditionalMgr.h"
#include "Log.h"
#include "Player.h"
#include "Transport.h"
#include "Unit.h"
#include "WorldSession.h"

AditionalData::AditionalData(Player* player)
{
    pPlayer = player;
    _clean();
}

AditionalData::~AditionalData()
{
    pPlayer = nullptr;
    _clean();
}

void AditionalData::_clean() {
    m_auctionlots = 0;
    m_walking = false;

    /////////////////// Vanish System /////////////////////
    m_vanishTimer = 0;
    m_breakblevanishTimer = 0;
    m_visiblevanish = false;
    m_breakablevanish = false;
    m_vip = false;
    m_premiumTimer = 0;
    m_coins = 0;
    m_unsetdate = 0;
}

void AditionalData::update(uint32 p_time)
{
    /////////////////// Vanish System /////////////////////
    if (m_visiblevanish && m_vanishTimer > 0)
    {
        if (p_time >= m_vanishTimer)
        {
            m_visiblevanish = false;
            m_vanishTimer = 0;
            pPlayer->UpdateObjectVisibility();
        }
        else
            m_vanishTimer -= p_time;
    }

    if (m_breakablevanish && m_breakblevanishTimer > 0)
    {
        if (p_time >= m_breakblevanishTimer)
        {
            m_breakablevanish = false;
            m_breakblevanishTimer = 0;
        }
        else
            m_breakblevanishTimer -= p_time;
    }

    if (m_vip && m_premiumTimer > 0)
    {
        if (p_time >= m_premiumTimer)
        {
            time_t currentGameTime = GameTime::GetGameTime();
            time_t unset = getPremiumUnsetdate();
            if (unset <= currentGameTime)
            {
                setPremiumStatus(false);
                setPremiumUnsetdate(0);
                AccountMgr::RemoveVipStatus(pPlayer->GetSession()->GetAccountId());
                ChatHandler(pPlayer->GetSession()).PSendSysMessage(pPlayer->GetSession()->GetTrinityString(LANG_PLAYER_VIP_TIME_EXPIRED));
            }
            else
            {
                time_t diff = unset - currentGameTime;
                time_t days = diff / DAY;
                if (days < 1)
                {
                    time_t hours = diff % DAY / HOUR;
                    if (hours < 1)
                    {
                        time_t minutes = diff % HOUR / MINUTE;
                        //time_t unsetsec = diff % 60;
                        if (minutes == 5)
                            ChatHandler(pPlayer->GetSession()).PSendSysMessage(pPlayer->GetSession()->GetTrinityString(LANG_PLAYER_VIP_TIME_NEAR_END));
                        if (minutes < 5)
                            ChatHandler(pPlayer->GetSession()).PSendSysMessage(pPlayer->GetSession()->GetTrinityString(LANG_PLAYER_VIP_TIME_EXIST), (secsToTimeString(diff, TimeFormat::ShortText)));
                    }
                }
                m_premiumTimer = 1000 * MINUTE;
            }
        }
        else
            m_premiumTimer -= p_time;
    }
}

void AditionalData::enterOnTransport(Transport* transport)
{
    if (pPlayer->ToUnit()->m_Controlled.empty())
        return;

    for (auto it : pPlayer->ToUnit()->m_Controlled)
    {
        if (it->IsTotem())
            continue;
        // if controlled unit (pet and etc...) is not in combat
        if (!it->GetVictim())
        {
            if (it->GetTransport() && !transport->isPassenger(it)) // remove pet from another transport, if exist
                it->GetTransport()->RemovePassenger(it);

            if (!transport->isPassenger(it))
                transport->AddPassenger(it);
        }
    }
}

void AditionalData::exitFromTransport(Transport* transport)
{
    if (pPlayer->ToUnit()->m_Controlled.empty())
        return;

    for (auto it : pPlayer->ToUnit()->m_Controlled)
    {
        if (it->IsTotem())
            continue;
        // if controlled unit (pet and etc...) is not in combat
        if (!it->GetVictim())
        {
            if (it->GetTransport() && transport->isPassenger(it))
                transport->RemovePassenger(it);
        }
    }
}

void AditionalData::calculateAuctionLotsCounter()
{
    uint32 count = 0;
    if (AuctionHouseObject* AllianceauctionHouse = sAuctionMgr->GetAuctionsMapByHouseId(AUCTIONHOUSE_ALLIANCE))
        AllianceauctionHouse->BuildListAllLots(pPlayer, count);

    if (AuctionHouseObject* AllianceauctionHouse = sAuctionMgr->GetAuctionsMapByHouseId(AUCTIONHOUSE_HORDE))
        AllianceauctionHouse->BuildListAllLots(pPlayer, count);

    if (AuctionHouseObject* AllianceauctionHouse = sAuctionMgr->GetAuctionsMapByHouseId(AUCTIONHOUSE_NEUTRAL))
        AllianceauctionHouse->BuildListAllLots(pPlayer, count);

    TC_LOG_DEBUG("chatmessage", "Player: CalculateAuctionLotsCounter - Player ({}) has {} lots in all auctions", pPlayer->GetName(), count);
    m_auctionlots = count;
}

// Vanish System
void AditionalData::setVanishTimer()
{
    m_vanishTimer = sWorld->customGetIntConfig(CONFIG_VANISH_VISION_TIMER);
    m_breakblevanishTimer = sWorld->customGetIntConfig(CONFIG_VANISH_CC_BREAK_TIMER);
    m_visiblevanish = true;
    m_breakablevanish = true;
}

void AditionalData::stopVanish()
{
    m_vanishTimer = m_breakblevanishTimer;
}

void AditionalData::setPremiumStatus(bool vipstatus)
{
    m_vip = vipstatus;
    if (m_vip)
        m_premiumTimer = 1000 * MINUTE;
    else
        m_premiumTimer = 0;
}

uint32 AditionalData::getVerifiedCoins()
{
    uint32 coinsCount = getCoins();
    uint32 coinsFromDB;
    // it's nessesary, if we will change coins count through web-site or some another way, and player will in-game in this time
    coinsFromDB = AccountMgr::GetCoins(pPlayer->GetSession()->GetAccountId());
    if (coinsCount != coinsFromDB)
    {
        coinsCount = coinsFromDB;
        setCoins(coinsCount);
    }

    return coinsCount;
}

void AditionalData::LearnSpellFromAutoLearnSpells(uint8 level)
{
    if (!sWorld->customGetBoolConfig(CONFIG_PLAYER_AUTO_LEARN_ENABLED))
        return;

    uint8 classid = pPlayer->GetClass();
    for (auto const itr: sMedivhAdditionalMgr->GetPlayerAutoLearnMap())
    {
        if (PlayerAutoLearn const* pAutoLearn = itr.second) {
            if (pAutoLearn->reqlevel > level)
                continue;

            if (pAutoLearn->reqclass != 0)
                if (pAutoLearn->reqclass != classid)
                    continue;

            if (pAutoLearn->reqSpellId != 0)
                if (!pPlayer->HasSpell(pAutoLearn->reqSpellId))
                    continue;

            if (!pPlayer->HasSpell(pAutoLearn->spellId))
                pPlayer->LearnSpell(pAutoLearn->spellId, false);
        }
    }
}
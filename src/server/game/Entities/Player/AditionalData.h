#ifndef AditionalData_h__
#define AditionalData_h__

#include "Define.h"

class Player;
class Transport;

class TC_GAME_API AditionalData
{
public:
    AditionalData(Player* player);
    ~AditionalData();

private:
    void _clean();
public:
    void update(uint32 p_time);

    void enterOnTransport(Transport* transport);
    void exitFromTransport(Transport* transport);

    // Auction info
    void calculateAuctionLotsCounter();
    uint32 getAuctionLotsCount() const { return m_auctionlots; }
    void addLotsCount() { ++m_auctionlots; }
    void removeLotsCount() { m_auctionlots - 1 > 0 ? --m_auctionlots : m_auctionlots = 0; }

    // Walking data from move packets
    void setWalkingFlag(bool walkstatus) { m_walking = walkstatus; }
    bool hasWalkingFlag() const { return m_walking; }

    // Vanish can be visible near 0.3-0.4 sec after using. Also 0.15 sec vanish will evade CC too, but break vanish.
    uint32 getVanishTimer() const { return m_vanishTimer; }
    uint32 getBreakbleVanishTimer() const { return m_breakblevanishTimer; }
    bool underVisibleVanish() const { return m_visiblevanish; }
    bool underBreakbleVanish() const { return m_breakablevanish; }
    void setVanishTimer();
    void stopVanish(); // set Fade Delay time = Breakable time, player will not under vanish, but this 0.15 sec can evade spells

    // VIP
    void setPremiumStatus(bool vipstatus);
    bool isPremium() const { return m_vip; }
    void setPremiumUnsetdate(time_t unsetdate) { m_unsetdate = unsetdate; }
    void setCoins(uint32 coins) { m_coins = coins; }
    uint32 getCoins() const { return m_coins; }
    // should be call ONLY when coin count will changed (add/del)
    uint32 getVerifiedCoins();
    time_t getPremiumUnsetdate() const { return m_unsetdate; }

    void LearnSpellFromAutoLearnSpells(uint8 level);

private:
    Player* pPlayer;
    uint32 m_auctionlots;       // Auction lots count for all auctions
    bool m_walking;             // Player walking

    // Vanish
    uint32 m_vanishTimer;
    uint32 m_breakblevanishTimer;
    bool m_visiblevanish;
    bool m_breakablevanish;

    // VIP
    bool m_vip;                 // Used for VIP func
    uint32 m_premiumTimer;
    uint32 m_coins;             // Coins for ingame store
    time_t m_unsetdate;         // time (unixtime) of unsetdate vip previlegies
};

#endif // AditionalData_h__
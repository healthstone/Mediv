#ifndef __MEDIVHADDITIONALMGR_H
#define __MEDIVHADDITIONALMGR_H

#include "Define.h"

struct PlayerAutoLearn
{
    uint32 spellId;
    uint8 reqlevel;
    uint8 reqclass;
    uint32 reqSpellId;
};

typedef std::unordered_map<uint32, PlayerAutoLearn const*> PlayerAutoLearnContainer;

class TC_GAME_API MedivhAdditionalMgr
{
private:
    MedivhAdditionalMgr();
    ~MedivhAdditionalMgr();

public:
    static MedivhAdditionalMgr* instance();
    void Initialize();

    PlayerAutoLearnContainer const& GetPlayerAutoLearnMap() const { return _playerAutoLearnStore; }

protected:
    void loadPlayerAutoLearnMap();

private:
    PlayerAutoLearnContainer _playerAutoLearnStore;
};

#define sMedivhAdditionalMgr MedivhAdditionalMgr::instance()

#endif

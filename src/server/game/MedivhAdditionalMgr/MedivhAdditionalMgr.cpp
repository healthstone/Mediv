#include "MedivhAdditionalMgr.h"
#include "DatabaseEnv.h"

MedivhAdditionalMgr *MedivhAdditionalMgr::instance() {
    static MedivhAdditionalMgr instance;
    return &instance;
}

MedivhAdditionalMgr::MedivhAdditionalMgr() = default;

MedivhAdditionalMgr::~MedivhAdditionalMgr() {
    for (auto it: _playerAutoLearnStore) {
        delete it.second;
    }
    _playerAutoLearnStore.clear();
}

void MedivhAdditionalMgr::Initialize() {
    loadPlayerAutoLearnMap();
}

void MedivhAdditionalMgr::loadPlayerAutoLearnMap() {
    uint32 oldMSTime = getMSTime();
    _playerAutoLearnStore.clear();                                  // for reload case
    //                  0      1       2           3        4
    QueryResult result = WorldDatabase.Query(
            "SELECT id, SpellId, ReqLevel, ReqClass, ReqSpellId FROM player_spells_for_level");

    if (!result) {
        TC_LOG_INFO("server.loading", ">> Loaded 0 Auto Learn Spells. DB table `player_spells_for_level` is empty!");
        return;
    }

    uint32 count = 0;

    do {
        Field *fields = result->Fetch();

        uint32 id = fields[0].GetUInt32();

        auto *pals = new PlayerAutoLearn();
        pals->spellId = fields[1].GetUInt32();
        pals->reqlevel = fields[2].GetUInt8();
        pals->reqclass = fields[3].GetUInt8();
        pals->reqSpellId = fields[4].GetUInt32();

        _playerAutoLearnStore[id] = pals;

        ++count;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded {} Player Auto Learn Spells in {} ms", count, GetMSTimeDiffToNow(oldMSTime));
}

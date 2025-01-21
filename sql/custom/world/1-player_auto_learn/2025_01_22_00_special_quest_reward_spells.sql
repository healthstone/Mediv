-- WARRIOR
INSERT INTO player_spells_for_level 
(SpellId, ReqLevel, ReqClass, ReqSpellId) VALUES
-- defense stance/sunder armor/ taunt
(71, 10, 1, 0),
(7386, 10, 1, 0),
(355, 10, 1, 0),
-- berserker stance/intercept
(2458, 30, 1, 0),
(20252, 10, 1, 0);

-- PALADIN
INSERT INTO player_spells_for_level 
(SpellId, ReqLevel, ReqClass, ReqSpellId) VALUES
-- ressurection
(7328, 12, 2, 0),
-- undead feeling
(5502, 20, 2, 0);

-- HUNTER
INSERT INTO player_spells_for_level 
(SpellId, ReqLevel, ReqClass, ReqSpellId) VALUES
-- tame beast (common)
(23356, 10, 3, 0),
-- tame beast
(1515, 10, 3, 0),
-- call pet
(883, 10, 3, 0),
-- discall pet
(2641, 10, 3, 0),
-- dressing (common)
(23357, 10, 3, 0),
-- dressing
(5149, 10, 3, 0),
-- feeding
(6991, 10, 3, 0),
-- resurrect pet
(982, 10, 3, 0);

-- ROGUE
INSERT INTO player_spells_for_level 
(SpellId, ReqLevel, ReqClass, ReqSpellId) VALUES
-- poisons
(2842, 20, 4, 0);

-- PRIEST
INSERT INTO player_spells_for_level 
(SpellId, ReqLevel, ReqClass, ReqSpellId) VALUES
-- desperate prayer
(13908, 10, 5, 0);

-- SHAMAN
INSERT INTO player_spells_for_level 
(SpellId, ReqLevel, ReqClass, ReqSpellId) VALUES
-- totem of stone skin
(8071, 4, 7, 0),
-- totem of fire
(3599, 10, 7, 0),
-- totem of water
(5394, 20, 7, 0);

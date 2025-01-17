DELETE FROM `world_custom_config` WHERE `Type` = 'bool' AND `IDInTypeGroup` between 0 and 10;
DELETE FROM `world_custom_config` WHERE `Type` = 'int' AND `IDInTypeGroup` = 0;
INSERT INTO `world_custom_config`(`OptionName`, `Type`, `IDInTypeGroup`, `DefaultValue`, `CustomValue`, `Description`) VALUES 
-- bool configs
('AntiCheats.FakeJumper.Enabled', 'bool', 0, '1', NULL, 'Enable AntiCheat.FakeJumper : when player sent a move opcode (with new Z higher) when has flag falling (client can\'t sent any new packets when falling) It\'s normal for Gagarin hacks and with Air Mode + \"space\" click = \"falling\" mode (player can\'t change orientation of flying, but it move up)'),
('AntiCheats.FakeFlyingmode.Enabled', 'bool', 1, '1', NULL, 'Enable AntiCheat.FakeFlyingmode : when player sent a move opcode with added fly flag but on server this flag doesn\'t restricted for player. It\'s normal for Hitchhiker\'s Hack with Air Mode'),
('AntiCheats.DoubleJump.Enabled', 'bool', 2, '1', NULL, 'Enable AntiCheat.DoubleJump Kick when player sent opcode jump when has flag falling (client can\'t do this)'),
('AntiCheats.FlyHack.Enabled', 'bool', 3, '1', NULL, 'Enable AntiCheat.FlyHack'),
('AntiCheats.SpeedHack.Enabled', 'bool', 4, '1', NULL, 'Enable AntiCheat.SpeedHack'),
('AntiCheats.IgnoreControlMovement.Enabled', 'bool', 5, '1', NULL, 'Enable AntiCheat.IgnoreControlMovement : when player in root state - client can\'t send a new movement packets'),
('AntiCheats.SpeedHack.Kick.Enabled', 'bool', 6, '1', NULL, 'Enable AntiCheat.SpeedHack Kick when detected'),
('AntiCheats.FlyHack.Kick.Enabled', 'bool', 7, '1', NULL, 'Enable AntiCheat.FlyHack Kick when detected'),
('AntiCheats.FakeJumper.Kick.Enabled', 'bool', 8, '1', NULL, 'Enable AntiCheats.FakeJumper Kick when detected'),
('AntiCheats.FakeFlyingmode.Kick.Enabled', 'bool', 9, '1', NULL, 'Enable AntiCheats.FakeFlyingmode Kick when detected'),
('AntiCheats.NoFallingDmg.Kick.Enabled', 'bool', 10, '1', NULL, 'Enable AntiCheats.NoFallingDmg Kick when detected'),
-- int config
('AntiCheats.FlyHackTimer', 'int', 0, '3000', NULL, 'Timer for AntiCheat FlyHack check #Default: 3000 - 1 check in 1 second');


DELETE FROM `trinity_string` WHERE `entry` between 12120 and 12129;
INSERT INTO `trinity_string`(`entry`, `content_default`) VALUES 
-- Anticheat
(12120, 'AntiCheat: SpeedHack Detected for %s, normal distance for this time and speed = %f, distance from packet = %f'),
(12121, 'AntiCheat: FlyHack Detected for %s , player can not fly'),
(12122, 'AntiCheat: FlyHack Detected for %s, player has Swimming flag, but not in water'),
(12123, 'AntiCheat: FlyHack Detected for %s'),
(12124, 'AntiCheat: DoubleJump Detected for %s'),
(12125, 'AntiCheat: Fake Jumper Detected for %s'),
(12126, 'AntiCheat: FakeFlying mode Detected for %s'),
(12127, 'AntiCheat: Wallclimb Detected for Account id : %u, Player %s, diffZ = %f, distance = %f, angle = %f, Map = %u, mapId = %u, X = %f, Y = %f, Z = %f'),
(12128, 'AntiCheat: Ignore control Hack Detected for Account : %u, Player : %s'),
(12129, 'AntiCheat: NoFallingDamage Hack Detected for Account : %u, Player : %s');

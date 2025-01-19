DELETE FROM `world_custom_config` WHERE `Type` = 'int' AND `IDInTypeGroup` between 2 and 3;
INSERT INTO `world_custom_config`(`OptionName`, `Type`, `IDInTypeGroup`, `DefaultValue`, `CustomValue`, `Description`) VALUES 
-- AT configs
('VanishVisionTimer', 'int', 2, '600', NULL, 'Vanish Fade Delay (vanish evade all spells without CC control in this time) (in ms)'),
('VanishCCbreakTimer', 'int', 3, '300', NULL, 'Vanish Should evade CC spells too, when casted, but it\'s a little lime.');
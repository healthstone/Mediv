DELETE FROM `trinity_string` WHERE `entry` IN (12102);
INSERT INTO `trinity_string`(`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`) VALUES 
-- FLAB
(12102, '|cffe6cc80[%s]: We glad to see your first character here, and present you a little bonus :|r %i |TInterface\\MoneyFrame\\UI-GoldIcon:0:0:2:0|t %i |TInterface\\MoneyFrame\\UI-SilverIcon:0:0:2:0|t %i |TInterface\\MoneyFrame\\UI-CopperIcon:0:0:2:0|t', NULL, NULL, NULL, NULL, NULL, NULL, NULL, '|cffe6cc80[%s]: Мы рады видеть вас и вашего первого персонажа, и дарим вам небольшой презент :|r %i |TInterface\\MoneyFrame\\UI-GoldIcon:0:0:2:0|t %i |TInterface\\MoneyFrame\\UI-SilverIcon:0:0:2:0|t %i |TInterface\\MoneyFrame\\UI-CopperIcon:0:0:2:0|t');

DELETE FROM `world_custom_config` WHERE `Type` = 'bool' AND `IDInTypeGroup` = 12;
DELETE FROM `world_custom_config` WHERE `Type` = 'int' AND `IDInTypeGroup` between 4 and 5;
INSERT INTO `world_custom_config`(`OptionName`, `Type`, `IDInTypeGroup`, `DefaultValue`, `CustomValue`, `Description`) VALUES 
-- FLAB configs
('Player.FLABScript.Enabled', 'bool', 12, '0', NULL, 'First character on account will receive a money bonus'),
('FLAB.Script.MaxChars', 'int', 4, '0', NULL, 'Count of characters for give a bonus #Default:     0 - it should be a first character on acc, right?'),
('FLAB.Script.BonusMoney', 'int', 5, '10000000', NULL, 'Count of money ( 1000 gold )');

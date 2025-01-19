DELETE FROM `trinity_string` WHERE `entry` between 12170 and 12183;
INSERT INTO `trinity_string`(`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`) VALUES 
-- Vip Commands
(12170, 'You are not vip.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Доступно только VIP игрокам.'),
(12171, 'Сommand disable', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Команда отключена.'),
(12172, 'error', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Произошла ошибка.'),
(12173, 'You can not use this command on Battleground or the arena!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Вы не можете использовать данную команду на поле боя или на арене!'),
(12174, 'You can not use this command in stealth mode!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Вы не можете использовать данную команду в режиме незаметности!'),
(12175, 'You are dead and you can not use this command!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Вы мертвы и не можете использовать данную команду!'),
(12176, 'Race Change will be requested at next login.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Смена расы будет предложена при следующем входе в игровой мир!'),
(12177, 'Character customize will be requested at next login.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Смена внешности персонажа будет предложена при следующем входе в игровой мир!'),
(12178, 'You must be in the group', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Вы должны находится в группе!'),
(12179, 'Your target player has not vip privileges!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Указанный игрок не имеет ВИП привилегий!'),
(12180, 'The remaining VIP time will expire in: %s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Оставшееся VIP время истечет через: %s'),
(12181, 'The remaining VIP time is less than 5 minutes!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Оставшееся время VIP привилегий составляет менее 5 минут!'),
(12182, 'Your VIP time has been expired!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Время VIP привилегий истекло!'),
(12183, 'The remaining VIP time of this target will expire in: %s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Оставшееся VIP время указанного игрока истечет через: %s');

DELETE FROM `world_custom_config` WHERE `Type` = 'bool' AND `IDInTypeGroup` between 13 and 23;
INSERT INTO `world_custom_config`(`OptionName`, `Type`, `IDInTypeGroup`, `DefaultValue`, `CustomValue`, `Description`) VALUES 
('Config.Vip.Debuff.Command', 'bool', 13, '0', NULL, 'Enable/Disable custom vip command'),
('Config.Vip.Bank.Command', 'bool', 14, '0', NULL, 'Enable/Disable custom vip command'),
('Config.Vip.Mail.Command', 'bool', 15, '1', NULL, 'Enable/Disable custom vip command'),
('Config.Vip.Repair.Command', 'bool', 16, '1', NULL, 'Enable/Disable custom vip command'),
('Config.Vip.Reset.Talents.Command', 'bool', 17, '1', NULL, 'Enable/Disable custom vip command'),
('Config.Vip.Taxi.Command', 'bool', 18, '1', NULL, 'Enable/Disable custom vip command'),
('Config.Vip.Home.Command', 'bool', 19, '1', NULL, 'Enable/Disable custom vip command'),
('Config.Vip.Change.Race.Command', 'bool', 20, '0', NULL, 'Enable/Disable custom vip command'),
('Config.Vip.Customize.Command', 'bool', 21, '0', NULL, 'Enable/Disable custom vip command'),
('Config.Vip.Capital.Command', 'bool', 22, '1', NULL, 'Enable/Disable custom vip command'),
('Config.Vip.Appear.Command', 'bool', 23, '1', NULL, 'Enable/Disable custom vip command');

DELETE FROM `world_custom_config` WHERE `Type` = 'rate' AND `IDInTypeGroup` between 0 and 3;
INSERT INTO `world_custom_config`(`OptionName`, `Type`, `IDInTypeGroup`, `DefaultValue`, `CustomValue`, `Description`) VALUES 
('Rate.XP.Kill.Premium', 'rate', 0, '1', NULL, 'Individual rate for vip player'),
('Rate.XP.Quest.Premium', 'rate', 1, '1', NULL, 'Individual rate for vip player'),
('Rate.Honor.Premium', 'rate', 2, '1', NULL, 'Individual rate for vip player'),
('Rate.Reputation.Gain.Premium', 'rate', 3, '1', NULL, 'Individual rate for vip player');

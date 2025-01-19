-- Pet Scaling
DELETE FROM `spell_script_names` WHERE `spell_id` IN ('34902', '34903', '34904', '61017');
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
('34902', 'spell_hun_pet_scaling'),
('34903', 'spell_hun_pet_scaling'),
('34904', 'spell_hun_pet_scaling'),
('61017', 'spell_hun_pet_scaling');

-- Pet Scaling
DELETE FROM spell_script_names WHERE spell_id IN(35661, 35662, 35663, 35664, -35661, -35662, -35663, -35664);
INSERT INTO spell_script_names VALUES (35661, 'spell_pri_pet_scaling');
INSERT INTO spell_script_names VALUES (35662, 'spell_pri_pet_scaling');
INSERT INTO spell_script_names VALUES (35663, 'spell_pri_pet_scaling');
INSERT INTO spell_script_names VALUES (35664, 'spell_pri_pet_scaling');

-- Grace stuck
DELETE FROM `spell_group_stack_rules` where `group_id` IN (1199);
INSERT INTO `spell_group_stack_rules` (`group_id`, `stack_rule`) VALUES ('1199', '1');
DELETE FROM `spell_group` where `id` IN (1199);
INSERT INTO `spell_group` (`id`, `spell_id`) VALUES ('1199', '47930');

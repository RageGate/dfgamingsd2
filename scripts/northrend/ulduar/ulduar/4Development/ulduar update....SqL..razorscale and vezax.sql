UPDATE `creature_template` SET `mechanic_immune_mask` = 650854271, `ScriptName` = 'boss_vezax' WHERE `entry` = 33271;
UPDATE `creature_template` SET `minlevel` = 83, `maxlevel` = 83, `minhealth` = 1575785, `maxhealth` = 1575785, `faction_A` = 16, `faction_H` = 16, `mindmg` = 412, `maxdmg` = 562, `attackpower` = 536, `mechanic_immune_mask` = 650854271, `ScriptName` = 'boss_saroniteanimus' WHERE `entry` = 33524;
UPDATE `creature_template` SET `minhealth` = 12600, `maxhealth` = 12600, `minlevel` = 80, `maxlevel` = 80, `faction_A` = 32, `faction_H` = 32, `ScriptName` = 'npc_saronitevapors' WHERE `entry` = 33488;

UPDATE gameobject_template SET data10 = 63524 WHERE entry = 194519;
UPDATE gameobject_template SET data10 = 63657 WHERE entry = 194541;
UPDATE gameobject_template SET data10 = 63658 WHERE entry = 194542;
UPDATE gameobject_template SET data10 = 63659 WHERE entry = 194543;
DELETE FROM spell_script_target WHERE entry IN (63524, 63657, 63658,63659, 62505);
INSERT INTO spell_script_target VALUES (63524, 1, 33184);
INSERT INTO spell_script_target VALUES (63657, 1, 33184);
INSERT INTO spell_script_target VALUES (63658, 1, 33184);
INSERT INTO spell_script_target VALUES (63659, 1, 33184);
INSERT INTO spell_script_target VALUES (62505, 1, 33186);
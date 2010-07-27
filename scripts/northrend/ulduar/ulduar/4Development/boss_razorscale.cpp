/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
* This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: boss_razorscale
SD%Complete: 90%
SDComment: missing knockback at beggining of phase2. yells and emotes missing, need to use harpoons to start ground phase instead of timer
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

enum
{
    //yells/emotes

    //razorscale air phase
    SPELL_FIREBALL                 = 62796,
    SPELL_FIREBALL_H               = 63815,
    SPELL_WING_BUFFET              = 62666,
    SPELL_STUN                     = 62794,
    //both
    SPELL_BERSERK                  = 47008,
    DEVOURING_FLAME_VISUAL         = 63236,
    SPELL_FLAME_BREATH             = 63317,
    SPELL_FLAME_BREATH_H           = 64021,
    //ground
    SPELL_FLAME_BUFFET             = 64016,
    SPELL_FLAME_BUFFET_H           = 64023,
    SPELL_FUSE_ARMOR               = 64771,

    //devouring flame target
    AURA_DEVOURING_FLAME           = 64709,
    AURA_DEVOURING_FLAME_H         = 64734,

    //dark rune watcher
    SPELL_LIGHTNING_BOLT           = 63809,
    SPELL_LIGHTNING_BOLT_H         = 64696,
    SPELL_CHAIN_LIGHTNING          = 64758,
    SPELL_CHAIN_LIGHTNING_H        = 64759,

    //dark rune sentinel
    SPELL_BATTLE_SHOUT             = 46763,
    SPELL_BATTLE_SHOUT_H           = 64062,
    SPELL_WHIRLWIND                = 63808,

    //dark rune guardian
    SPELL_STORMSTRIKE              = 64757,

    //NPC ids
    MOB_DARK_RUNE_WATCHER          = 33453,
    MOB_DARK_RUNE_SENTINEL         = 33846,
    MOB_DARK_RUNE_GUARDIAN         = 33388,

    NPC_HARPOON_GUN                = 33184,
    GO_HARPOON_GUN_1               = 194519,
    GO_HARPOON_GUN_2               = 194541,
    GO_HARPOON_GUN_3               = 194542,
    GO_HARPOON_GUN_4               = 194543,
    GO_HARPOON_DESTROYED           = 194565,

    GO_MOLE_MACHINE_S              = 188478, // 1 watcher 1 guard
    GO_MOLE_MACHINE_M              = 186685, // 1 watcher 2 guard
    GO_MOLE_MACHINE_L              = 186880, // 1 sentinel

    AURA_HARPOON                   = 62505,
    
    HARPOON_COUNT                  = 4,
    MAX_POS                        = 3,
    MAX_WAVE                       = 4
};
const struct HarpoonPos { uint32 entry, x, y, z, o; }
Harpoon[HARPOON_COUNT] = 
{
    {GO_HARPOON_GUN_1, 560.47f, -140.27f, 391.52f, 5.226f},
    {GO_HARPOON_GUN_2, 571.95f, -136.01f, 391.51f, 2.286f},
    {GO_HARPOON_GUN_3, 589.92f, -133.62f, 391.90f, 3.299f},
    {GO_HARPOON_GUN_4, 601.82f, -139.88f, 391.52f, 4.035f}
};

const struct AddsPos { uint32 x, y, z; }
ApproxAddsPos[MAX_POS] = 
{
    {651.10f, -216.32f, 391.23f},
    {574.65f, -240.61f, 391.18f},
    {516.87f, -194.87f, 391.18f}
};

//float RazorscaleAddR[3] = {621.63f, -228.67f, 391.18f}; //right
//float RazorscaleAddL[3] = {564.14f, -222.04f, 391.51f}; //left
//float RazorscaleAddM[3] = {591.05f, -209.06f, 392.09f}; //middle

float RazorscaleFlying[3] = {587.62f, -179.02f, 435.41f}; //air
float RazorscaleGround[3] = {587.62f, -179.02f, 391.62f}; //ground

#define GOSSIP_START     "Bring Razorscale down!"

//expedition commander
struct MANGOS_DLL_DECL npc_expedition_commanderAI : public ScriptedAI
{
    npc_expedition_commanderAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
        SetCombatMovement(false);
    }

    ScriptedInstance* m_pInstance;

    void Reset()
    {
    }

    void BeginRazorscaleEvent(Player* pPlayer)
    {
        if (!m_pInstance)
            return;
        if(m_pInstance->GetData(TYPE_RAZORSCALE) == NOT_STARTED || m_pInstance->GetData(TYPE_RAZORSCALE) == FAIL){

        debug_log("SD2: Razorscale - event initiated by player %s", pPlayer->GetName());

        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(DATA_RAZORSCALE))))
        {
            pTemp->SetInCombatWithZone();
            pTemp->AddThreat(pPlayer,0.0f);
            pTemp->AI()->AttackStart(pPlayer);
        }
        }else debug_log("SD2: Razorscale - player %s is a moron. he tried to start the event when its already done, or over", pPlayer->GetName());
    }

    void UpdateAI(const uint32 diff)
    {
        DoMeleeAttackIfReady();

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim()) return;

        if (!m_pInstance)
            return;
    }
};

CreatureAI* GetAI_npc_expedition_commander(Creature* pCreature)
{
    return new npc_expedition_commanderAI(pCreature);
}

bool GossipHello_npc_expedition_commander(Player* pPlayer, Creature* pCreature)
{
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_expedition_commander(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        ((npc_expedition_commanderAI*)pCreature->AI())->BeginRazorscaleEvent(pPlayer);
    }

    return true;
}

// devouring_flame_target
struct MANGOS_DLL_DECL mob_devouring_flame_targetAI : public ScriptedAI
{
    mob_devouring_flame_targetAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        Reset();
        SetCombatMovement(false);
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 Death_Timer;

    void Reset()
    {
        Death_Timer = 25500;
        m_creature->SetDisplayId(11686);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        DoCast(m_creature,  m_bIsRegularMode ? AURA_DEVOURING_FLAME : AURA_DEVOURING_FLAME_H);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (Death_Timer < diff)
        {
            m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }else Death_Timer -= diff;
    }
};

CreatureAI* GetAI_mob_devouring_flame_target(Creature* pCreature)
{
    return new mob_devouring_flame_targetAI(pCreature);
}

// dark rune watcher
struct MANGOS_DLL_DECL mob_dark_rune_watcherAI : public ScriptedAI
{
    mob_dark_rune_watcherAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 Spell_Timer;

    void Reset()
    {
        Spell_Timer = 10000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (Spell_Timer < diff)
        {
            switch(urand(0, 1))
            {
                case 0:
                    DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_LIGHTNING_BOLT : SPELL_LIGHTNING_BOLT_H);
                break;
                case 1:
                    DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_CHAIN_LIGHTNING : SPELL_CHAIN_LIGHTNING_H);
                break;
            }
            Spell_Timer = urand(7000, 11000);
        }else Spell_Timer -= diff;        
        
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_dark_rune_watcher(Creature* pCreature)
{
    return new mob_dark_rune_watcherAI(pCreature);
}

// dark rune sentinel
struct MANGOS_DLL_DECL mob_dark_rune_sentinelAI : public ScriptedAI
{
    mob_dark_rune_sentinelAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        Reset();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 Whirl_Timer;
    uint32 Shout_Timer;

    void Reset()
    {
        Whirl_Timer = 10000;
        Shout_Timer = 2000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (Whirl_Timer < diff)
        {
            DoCast(m_creature, SPELL_WHIRLWIND);
            Whirl_Timer = urand(10000, 15000);
        }else Whirl_Timer -= diff;

        if (Shout_Timer < diff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_BATTLE_SHOUT : SPELL_BATTLE_SHOUT_H);
            Shout_Timer = 30000;
        }else Shout_Timer -= diff;
        
        DoMeleeAttackIfReady();
    }

};

CreatureAI* GetAI_mob_dark_rune_sentinel(Creature* pCreature)
{
    return new mob_dark_rune_sentinelAI(pCreature);
}

// dark rune guardian
struct MANGOS_DLL_DECL mob_dark_rune_guardianAI : public ScriptedAI
{
    mob_dark_rune_guardianAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        Reset();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 Stormstrike_Timer;

    void Reset()
    {
        Stormstrike_Timer = 10000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (Stormstrike_Timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_STORMSTRIKE);
            Stormstrike_Timer = urand(7000, 13000);
        }else Stormstrike_Timer -= diff;
        
        DoMeleeAttackIfReady();
    }

};

CreatureAI* GetAI_mob_dark_rune_guardian(Creature* pCreature)
{
    return new mob_dark_rune_guardianAI(pCreature);
}

std::vector<GameObject*> m_vGOHarpoon(4, NULL);
std::vector<Creature*> m_vNPCHarpoon(4, NULL);

std::vector<Creature*> m_vNPCRepair(3, NULL);
std::vector<GameObject*> m_vGOMoleMachine(4, NULL);

//razorscale
struct MANGOS_DLL_DECL boss_razorscaleAI : public ScriptedAI
{
    boss_razorscaleAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    float MoleX[MAX_WAVE];
    float MoleY[MAX_WAVE];
    float MoleZ[MAX_WAVE];
    uint8 MoleWave[MAX_WAVE];

    uint32 m_uiFireball;
    uint32 m_uiDevouring_Flame;
    uint32 m_uiFlame_Buffet;
    uint32 m_uiFuse_Armor;
    uint32 m_uiFlame_Breath;
    uint32 m_uiBerserk;
    uint32 m_uiMoleDespawn;
    uint32 m_uiDelayedNpcSpawn;
    uint32 m_uiBreakFree;
    uint32 m_uiSpawnMoleTimer;
    uint32 m_uiRepairTimer;
    
    uint8 m_uiPrepareFlyOffPhase;
    uint8 m_uiCurrentRepairngHarpoon;
    
    bool m_bDelayed;
    bool m_bPrepareDespawn;
    
    bool Phase1; // air
    bool Phase2; // harpooned
    bool Phase3; // ground

    void Reset()
    {
        m_uiSpawnMoleTimer = urand(8,10)*IN_MILLISECONDS;
        m_uiBerserk = 900000;

        Phase1 = false;
        Phase2 = false;
        Phase3 = false;
        m_bDelayed = false;
        m_bPrepareDespawn = false;

        if (m_pInstance)
            m_pInstance->SetData(TYPE_RAZORSCALE, NOT_STARTED);
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_RAZORSCALE, DONE);
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_RAZORSCALE, IN_PROGRESS);
        
        EnterAirPhase();
    }

    void FinishTurrent(uint8 entry)
    {
        if(m_vGOHarpoon[entry])
            m_vGOHarpoon[entry]->Delete();
        m_vGOHarpoon[entry] = m_creature->SummonGameobject(Harpoon[entry].entry, Harpoon[entry].x, Harpoon[entry].y, Harpoon[entry].z, Harpoon[entry].o, TEMPSUMMON_MANUAL_DESPAWN);
        m_vNPCHarpoon[entry] = m_creature->SummonCreature(NPC_HARPOON_GUN, Harpoon[entry].x, Harpoon[entry].y, Harpoon[entry].z, Harpoon[entry].o, TEMPSUMMON_MANUAL_DESPAWN, 0);
        // finish yell
    }

    void DestroyTurrents()
    {
        for(uint8 i = 0; i < HARPOON_COUNT; ++i)
        {
            if(m_vGOHarpoon[i])
                m_vGOHarpoon[i]->Delete();
            m_vGOHarpoon[i] = m_creature->SummonGameobject(GO_HARPOON_DESTROYED, Harpoon[i].x, Harpoon[i].y, Harpoon[i].z, Harpoon[i].o, TEMPSUMMON_MANUAL_DESPAWN);
            m_vNPCHarpoon[i] = m_creature->SummonCreature(NPC_HARPOON_GUN, Harpoon[i].x, Harpoon[i].y, Harpoon[i].z, Harpoon[i].o, TEMPSUMMON_MANUAL_DESPAWN, 0);
        }
    }

    void SpawnWaveMole(uint8 count)
    {
        uint32 goid;
        uint8 wave = MoleWave[count] = urand(0,2);
        switch(wave)
        {
            case 0: goid = GO_MOLE_MACHINE_S; break;  // 1 watcher 1 guardian
            case 1: goid = GO_MOLE_MACHINE_M; break;  // 1 watcher 2 guardians
            case 2: goid = GO_MOLE_MACHINE_L; break;  // 1 sentinel
        }

        uint8 random = urand(0,2);
        float x = MoleX[count] = ApproxAddsPos[random].x - 5 + urand(0,10);
        float y = MoleY[count] = ApproxAddsPos[random].y - 5 + urand(0,10);
        float z = MoleZ[count] = ApproxAddsPos[random].z;
        float angle = m_creature->GetAngle(x,y)+M_PI_F;
        if(GameObject* pGO = m_vGOMoleMachine[count] = m_creature->SummonGameobject(goid, x, y, z, angle, TEMPSUMMON_MANUAL_DESPAWN))
            pGO->Use(m_creature);

        m_uiDelayedNpcSpawn = 3*IN_MILLISECONDS;
        m_uiMoleDespawn = 8*IN_MILLISECONDS;
        m_bDelayed = true;
        m_bPrepareDespawn = true;

    }

    void SpawnWaveNPC(uint8 count)
    {
        if(MoleWave[count] == 1 || MoleWave[count] == 2)
        {
            SpawnWaveCreature(MOB_DARK_RUNE_WATCHER, count);
            SpawnWaveCreature(MOB_DARK_RUNE_GUARDIAN, count);
        }
        if(MoleWave[count] == 2)
            SpawnWaveCreature(MOB_DARK_RUNE_GUARDIAN, count);

        if(MoleWave[count] == 3)
            SpawnWaveCreature(MOB_DARK_RUNE_SENTINEL, count);
    }

    void SpawnWaveCreature(uint32 entry, uint8 count)
    {
        if(Creature* pTemp = m_creature->SummonCreature(entry, MoleX[count], MoleY[count], MoleZ[count], 0, TEMPSUMMON_CORPSE_DESPAWN, 0))
            if(Creature* pRepair = m_vNPCRepair[urand(0,2)])
            (pTemp->AI())->AttackStart(pRepair);
    }

    void EnterAirPhase()
    {
        Phase1 = true;
        Phase2 = false;
        SetCombatMovement(false);
        m_creature->SetSpeedRate(MOVE_FLIGHT, 2.0f, true);
        m_creature->SetSpeedRate(MOVE_RUN, 2.0f, true);
        m_creature->SetSpeedRate(MOVE_WALK, 2.0f, true);
        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 50331648);
        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
        WorldPacket heart;
        m_creature->BuildHeartBeatMsg(&heart);
        m_creature->SendMessageToSet(&heart, false);
        m_creature->GetMotionMaster()->MovePoint(0, RazorscaleFlying[0], RazorscaleFlying[1], RazorscaleFlying[2]);
        m_uiCurrentRepairngHarpoon = 0;
        m_uiFireball = 10000;
        m_uiDevouring_Flame = 18000;
        m_uiRepairTimer = 20*IN_MILLISECONDS;

    }

    void EnterHarpoonedPhase()
    {
        Phase1 = false;
        Phase2 = true;
        m_creature->RemoveAurasDueToSpell(AURA_HARPOON);
        SetCombatMovement(false);
        m_creature->SetSpeedRate(MOVE_FLIGHT, 1.0f, true);
        m_creature->SetSpeedRate(MOVE_RUN, 1.0f, true);
        m_creature->SetSpeedRate(MOVE_WALK, 1.0f, true);
        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 50331648);
        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
        WorldPacket heart;
        m_creature->BuildHeartBeatMsg(&heart);
        m_creature->SendMessageToSet(&heart, false);
        m_creature->GetMotionMaster()->MovePoint(0, RazorscaleGround[0], RazorscaleGround[1], RazorscaleGround[2]);
        m_uiBreakFree = 35*IN_MILLISECONDS;
        m_uiPrepareFlyOffPhase = 0;
    }

    void EnterGroundPhase()
    {
        Phase1 = false;
        Phase2 = false;
        Phase3 = true;
        if(m_creature->GetPositionZ() < 398.0f)
            m_creature->RemoveAurasDueToSpell(SPELL_STUN);
        else
        {
            SetCombatMovement(false);
            m_creature->SetSpeedRate(MOVE_FLIGHT, 1.0f, true);
            m_creature->SetSpeedRate(MOVE_RUN, 1.0f, true);
            m_creature->SetSpeedRate(MOVE_WALK, 1.0f, true);
            m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 50331648);
            m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
            WorldPacket heart;
            m_creature->BuildHeartBeatMsg(&heart);
            m_creature->SendMessageToSet(&heart, false);
            m_creature->GetMotionMaster()->MovePoint(0, RazorscaleGround[0], RazorscaleGround[1], RazorscaleGround[2]);
        }

    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_RAZORSCALE, FAIL);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(Phase1)
        {
            // Fireball
            if (m_uiFireball < diff)
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    DoCast(target, m_bIsRegularMode ? SPELL_FIREBALL : SPELL_FIREBALL_H);
                m_uiFireball = 2000;
            }else m_uiFireball -= diff;

            if(m_uiRepairTimer < diff)
            {
                FinishTurrent(m_uiCurrentRepairngHarpoon);
                m_uiRepairTimer = 20*IN_MILLISECONDS;
                m_uiCurrentRepairngHarpoon++;
            }else m_uiRepairTimer -= diff;
        }

        if(!Phase3)
        {
            // First lets spawn mole
            if(m_uiSpawnMoleTimer < diff && !Phase3)
            {
                for(uint8 i = 0; i < MAX_WAVE; ++i)
                    SpawnWaveMole(i);
                m_uiSpawnMoleTimer = urand(40,50)*IN_MILLISECONDS;
            }else m_uiSpawnMoleTimer -= diff;

            // Set Ground phase
            if(m_creature->GetHealthPercent() <= 50)
                EnterGroundPhase();
        }

        if(Phase3)
        {
            // Fuse Armor
            if (m_uiFuse_Armor < diff)
            {
                DoCast(m_creature->getVictim(), SPELL_FUSE_ARMOR);
                m_uiFuse_Armor = 13000;
            }else m_uiFuse_Armor -= diff;

            // Flame Buffet
            if (m_uiFlame_Buffet < diff)
            {
                DoCast(m_creature, m_bIsRegularMode ? SPELL_FLAME_BUFFET : SPELL_FLAME_BUFFET_H);
                m_uiFlame_Buffet = 13000;
            }else m_uiFlame_Buffet -= diff;

            // Flame Breath
            if (m_uiFlame_Breath < diff)
            {
                DoCast(m_creature, m_bIsRegularMode ? SPELL_FLAME_BREATH : SPELL_FLAME_BREATH_H);
                m_uiFlame_Breath = 14000;
            }else m_uiFlame_Breath -= diff;
        }

        // Spawn npcs from mole
        if(m_uiDelayedNpcSpawn < diff && m_bDelayed)
        {
            for(uint8 i = 0; i < MAX_WAVE; ++i)
                SpawnWaveNPC(i);
            m_bDelayed = false;
        }else m_uiDelayedNpcSpawn -= diff;

        if(m_uiMoleDespawn < diff && m_bPrepareDespawn)
        {
            for(uint8 i = 0; i < MAX_WAVE; ++i)
                if(GameObject* pGO = m_vGOMoleMachine[i])
                    pGO->Delete();
            m_bPrepareDespawn = false;
        }else m_uiMoleDespawn -= diff;
        
        // Devouring Flame
        if (m_uiDevouring_Flame < diff && !Phase2)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(target, DEVOURING_FLAME_VISUAL);
            m_uiDevouring_Flame = 12000;
        }else m_uiDevouring_Flame -= diff;  

        if(Phase2)
        {
            // Stun when reaches the ground
            if(!m_creature->HasAura(SPELL_STUN))
                if(m_creature->GetPositionZ() < 393.0f)
                    m_creature->CastSpell(m_creature, SPELL_STUN, true);

            // Prepare to enter air phase
            if(m_uiBreakFree < diff)
            {
                if(m_creature->HasAura(SPELL_STUN))
                    m_creature->RemoveAurasDueToSpell(SPELL_STUN);
                if(Creature* pTemp = m_vNPCHarpoon[urand(0,3)])
                    m_creature->SetFacingToObject(pTemp);
                if (!m_creature->IsNonMeleeSpellCasted(false))
                {
                    if(m_uiPrepareFlyOffPhase == 0)
                        m_creature->CastSpell(m_creature, m_bIsRegularMode ? SPELL_FLAME_BREATH : SPELL_FLAME_BREATH_H, true);
                    else if(m_uiPrepareFlyOffPhase == 1)
                        m_creature->CastSpell(m_creature, SPELL_WING_BUFFET, true);
                    else if(m_uiPrepareFlyOffPhase == 2)
                        EnterAirPhase();
                    m_uiPrepareFlyOffPhase++;
                }
            }else m_uiBreakFree -= diff;
        }

        // Enrage
        if (m_uiBerserk < diff)
        {
            DoCast(m_creature, SPELL_BERSERK);
            m_uiBerserk = 10*IN_MILLISECONDS;
        }else m_uiBerserk -= diff;
        
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_razorscale(Creature* pCreature)
{
    return new boss_razorscaleAI(pCreature);
}

void AddSC_boss_razorscale()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "boss_razorscale";
    NewScript->GetAI = GetAI_boss_razorscale;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_devouring_flame_target";
    NewScript->GetAI = &GetAI_mob_devouring_flame_target;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_dark_rune_watcher";
    NewScript->GetAI = &GetAI_mob_dark_rune_watcher;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_dark_rune_sentinel";
    NewScript->GetAI = &GetAI_mob_dark_rune_sentinel;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_dark_rune_guardian";
    NewScript->GetAI = &GetAI_mob_dark_rune_guardian;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "npc_expedition_commander";
    NewScript->GetAI = &GetAI_npc_expedition_commander;
    NewScript->pGossipHello = &GossipHello_npc_expedition_commander;
    NewScript->pGossipSelect = &GossipSelect_npc_expedition_commander;
    NewScript->RegisterSelf();
}
/*
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
*/
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
SDName: boss_auriaya
SD%Complete: 0%
SDComment:
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "def_ulduar.h"

#define SP_BERSERK          47008
#define SP_TERRIFY          64386
#define SP_GUARDIAN_SWARM   64396
#define SP_SONIC_SCREECH    64422
#define H_SP_SONIC_SCREECH  64688
#define SP_SENTINEL_BLAST   64389
#define H_SP_SENTINEL_BLAST 64678

#define CR_FERAL_DEFENDER       34035
#define CR_SWARMING_GUARDIAN    34034

enum
{
    SAY_AGGRO                       = -1603111,
    SAY_SLAY1                       = -1603112,
    SAY_SLAY2                       = -1603113,
    SAY_BERSERK                     = -1603114,
    SAY_DEATH                       = -1603115,

    SPELL_SONIC_SCREECH             = 64422,
    H_SPELL_SONIC_SCREECH           = 64688,
    SPELL_SENTINEL_BLAST            = 64389,
    H_SPELL_SENTINEL_BLAST          = 64678,
    SPELL_TERRIFYING_SCREECH        = 64386,
    SPELL_GUARDIAN_SWARM            = 64396,
    SPELL_BERSERK                   = 47008,

    SPELL_ACTIVATE_FERAL_DEFENDER   = 64449,
    SPELL_FERAL_DEFENDER_TRIGGER    = 64448,

    // Feral Defender
    SPELL_FERAL_RUSH                = 64496,
    H_SPELL_FERAL_RUSH              = 64674,

    SPELL_FERAL_POUNCE              = 64478,
    H_SPELL_FERAL_POUNCE            = 64669,

    SPELL_FERAL_ESSENCE             = 64455,
    SPELL_FERAL_ESSENCE_SUMMON      = 64457,
    SPELL_FERAL_ESSENCE_REMOVE      = 64456,

    NPC_FERAL_DEFENDER              = 34035,
    NPC_FERAL_ESSENCE               = 34098,
    NPC_FERAL_DEFENDER_STALKER      = 34096,

    ACHIEVEMENT_CRAZY_CAT_LADY_10   = 3006,
    ACHIEVEMENT_CRAZY_CAT_LADY_25   = 3007,
    ACHIEVEMENT_NINE_LIVES_10       = 3076,
    ACHIEVEMENT_NINE_LIVES_25       = 3077,
};

struct MANGOS_DLL_DECL boss_auriaya_AI : public ScriptedAI
{
    boss_auriaya_AI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    ScriptedInstance* m_pInstance;

    uint32 m_uiSonicScreechTimer;
    uint32 m_uiTerrifyingScreechTimer;
    uint32 m_uiBerserkTimer;
    uint32 m_uiActivateDefenderTimer;
    uint32 m_uiGuardianSwarmTimer;
    bool m_bGuardianSummoned;
    bool m_bNeedCastSentinelBlast;

    void Reset()
    {
        m_uiSonicScreechTimer = 28000;
        m_uiTerrifyingScreechTimer = urand(34000, 40000);
        m_uiActivateDefenderTimer = 60000;
        m_uiBerserkTimer = 10*MINUTE*IN_MILLISECONDS;
        m_uiGuardianSwarmTimer = urand(50000, 60000);
        m_bGuardianSummoned = false;
        m_bNeedCastSentinelBlast = false;

        if (m_pInstance && m_creature->isAlive())
        {
            Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(DATA_SANCTUM_SENTRY_1)));
            if (pTemp)
            {
                if (pTemp->isAlive())
                {
                    if (pTemp->isInCombat())
                        pTemp->AI()->EnterEvadeMode();
                }
                else
                    pTemp->Respawn();
            }

            Creature* pTemp2 = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(DATA_SANCTUM_SENTRY_2)));
            if (pTemp2)
            {
                if (pTemp2->isAlive())
                {
                    if (pTemp2->isInCombat())
                        pTemp2->AI()->EnterEvadeMode();
                }
                else
                    pTemp2->Respawn();
            }

            Creature* pTemp3 = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(DATA_SANCTUM_SENTRY_3)));
            if (pTemp3)
            {
                if (pTemp3->isAlive())
                {
                    if (pTemp3->isInCombat())
                        pTemp3->AI()->EnterEvadeMode();
                }
                else
                    pTemp3->Respawn();
            }

            Creature* pTemp4 = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(DATA_SANCTUM_SENTRY_4)));
            if (pTemp4)
            {
                if (pTemp4->isAlive())
                {
                    if (pTemp4->isInCombat())
                        pTemp4->AI()->EnterEvadeMode();
                }
                else
                    pTemp4->Respawn();
            }
        }            
    }

    void Aggro(Unit*)
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_AURIAYA, IN_PROGRESS);

            Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(DATA_SANCTUM_SENTRY_1)));
            if (pTemp)
            {
                if (pTemp->isAlive())
                {
                    pTemp->SetInCombatWithZone();
                }
            }

            Creature* pTemp2 = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(DATA_SANCTUM_SENTRY_2)));
            if (pTemp2)
            {
                if (pTemp2->isAlive())
                {
                    pTemp2->SetInCombatWithZone();
                }
            }

            Creature* pTemp3 = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(DATA_SANCTUM_SENTRY_3)));
            if (pTemp3)
            {
                if (pTemp3->isAlive())
                {
                    pTemp3->SetInCombatWithZone();
                }
            }

            Creature* pTemp4 = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(DATA_SANCTUM_SENTRY_4)));
            if (pTemp4)
            {
                if (pTemp4->isAlive())
                {
                    pTemp4->SetInCombatWithZone();
                }
            }            
        }

        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustDied(Unit*)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_AURIAYA, DONE);

        DoScriptText(SAY_DEATH, m_creature);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_AURIAYA, FAIL);
    }

    void KilledUnit(Unit* pVictim)
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        if (urand(0, 1))
            DoScriptText(urand(0, 1) ? SAY_SLAY1 : SAY_SLAY2, m_creature);
    }

    void JustSummoned(Creature* pSummoned)
    {
        switch(pSummoned->GetEntry())
        {
        case NPC_FERAL_DEFENDER_STALKER:
            m_creature->CastSpell(NULL, SPELL_ACTIVATE_FERAL_DEFENDER, false);
            m_bGuardianSummoned = true;
            break;
        };
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiBerserkTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
            {
                DoScriptText(SAY_BERSERK, m_creature);
                m_uiBerserkTimer = 30*MINUTE*IN_MILLISECONDS;
            }
        }
        else
            m_uiBerserkTimer -= uiDiff;

        if (!m_bGuardianSummoned)
        {
            if (m_uiActivateDefenderTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_FERAL_DEFENDER_TRIGGER) == CAST_OK)
                    m_uiActivateDefenderTimer = 60000;
            }
            else
                m_uiActivateDefenderTimer -= uiDiff;
        }

        if (m_bNeedCastSentinelBlast)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SENTINEL_BLAST : H_SPELL_SENTINEL_BLAST) == CAST_OK)
                m_bNeedCastSentinelBlast = false;
        }

        if (m_uiSonicScreechTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_SONIC_SCREECH : H_SPELL_SONIC_SCREECH) == CAST_OK)
                m_uiSonicScreechTimer = urand(36000, 38000);
        }
        else
            m_uiSonicScreechTimer -= uiDiff;

        if (m_uiTerrifyingScreechTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_TERRIFYING_SCREECH) == CAST_OK)
            {
                m_uiTerrifyingScreechTimer = urand(32000, 40000);
                m_bNeedCastSentinelBlast = true;
            }
        }
        else
            m_uiTerrifyingScreechTimer -= uiDiff;

        if (m_uiGuardianSwarmTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_GUARDIAN_SWARM) == CAST_OK)
                    m_uiGuardianSwarmTimer = urand(35000, 42000);
            }
        }
        else
            m_uiGuardianSwarmTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_activate_defender_stalker_AI : public ScriptedAI
{
    mob_activate_defender_stalker_AI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void JustSummoned(Creature* pSummoned)
    {
        if(pSummoned->GetEntry() == NPC_FERAL_DEFENDER)
            pSummoned->SetInCombatWithZone();
    }

    void Reset() {}
    void MoveInLineOfSight(Unit*) {}
    void AttackStart(Unit*) {}
};

struct MANGOS_DLL_DECL mob_feral_defender_AI : public ScriptedAI
{
    mob_feral_defender_AI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    uint32 m_uiRushTimer;
    uint32 m_uiPounceTimer;
    uint32 m_uiReviveTimer;
    bool m_bFakeDeath;
    std::list<uint64> m_lVoidZones;

    void Reset()
    {
        m_uiRushTimer = 14000;
        m_uiPounceTimer = 7000;
        m_bFakeDeath = false;

        for(uint8 i = 0; i < 8; ++i)
            m_creature->CastSpell(m_creature, SPELL_FERAL_ESSENCE, true);

        DespawnVoidZones();
    }

    void DespawnVoidZones()
    {
        for(std::list<uint64>::const_iterator itr = m_lVoidZones.begin(); itr != m_lVoidZones.end(); ++itr)
        {
            if (Creature* pCreature = (Creature*)Unit::GetUnit(*m_creature, *itr))
                pCreature->ForcedDespawn();
        }

        m_lVoidZones.clear();
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
    {
        if (m_bFakeDeath)
        {
            uiDamage = 0;
            return;
        }

        if (m_creature->HasAura(SPELL_FERAL_ESSENCE) && m_creature->GetHealth() <= uiDamage)
        {
            m_creature->CastSpell(m_creature, SPELL_FERAL_ESSENCE_REMOVE, true);
            m_creature->CastSpell(NULL, SPELL_FERAL_ESSENCE_SUMMON, true);

            uiDamage = 0;
            m_creature->InterruptNonMeleeSpells(false);
            m_creature->SetHealth(0);
            m_creature->StopMoving();
            m_creature->RemoveAllAuras(AURA_REMOVE_BY_CANCEL);
            m_creature->ClearAllReactives();
            m_creature->ClearComboPointHolders();
            m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
            m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->SetUInt64Value(UNIT_FIELD_TARGET, 0);
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MoveIdle();
            m_creature->SetStandState(UNIT_STAND_STATE_DEAD);

            m_uiReviveTimer = 30000;
            m_bFakeDeath = true;
        }
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!pWho || m_bFakeDeath)
            return;

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho || m_bFakeDeath)
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_FERAL_ESSENCE)
            m_lVoidZones.push_back(pSummoned->GetGUID());
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_AURIAYA) != IN_PROGRESS)
        {
            DespawnVoidZones();
            m_creature->ForcedDespawn();
            return;
        }

        if (m_bFakeDeath)
        {
            if (m_uiReviveTimer < uiDiff)
            {
                m_bFakeDeath = false;
                m_creature->SetHealth(m_creature->GetMaxHealth());
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                DoResetThreat();

                if (Unit* pTarget = m_creature->getVictim())
                {
                    AttackStart(pTarget);
                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MoveChase(pTarget);
                    m_creature->AddThreat(pTarget);
                }
            }
            else
                m_uiReviveTimer -= uiDiff;

            return;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiRushTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_FERAL_RUSH : H_SPELL_FERAL_RUSH) == CAST_OK)
                    m_uiRushTimer = urand(10000, 18000);
            }
        }
        else
            m_uiRushTimer -= uiDiff;

        if (m_uiPounceTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_FERAL_POUNCE : H_SPELL_FERAL_POUNCE) == CAST_OK)
                    m_uiPounceTimer = urand(7000, 12000);
            }
        }
        else
            m_uiPounceTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_auriaya(Creature* pCreature)
{
    return new boss_auriaya_AI(pCreature);
}

CreatureAI* GetAI_mob_feral_defender(Creature* pCreature)
{
    return new mob_feral_defender_AI(pCreature);
}

CreatureAI* GetAI_mob_activate_defender_stalker(Creature* pCreature)
{
    return new mob_activate_defender_stalker_AI(pCreature);
}

void AddSC_boss_auriaya()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_auriaya";
    newscript->GetAI = &GetAI_boss_auriaya;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_feral_defender";
    newscript->GetAI = &GetAI_mob_feral_defender;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_activate_defender_stalker";
    newscript->GetAI = &GetAI_mob_activate_defender_stalker;
    newscript->RegisterSelf();
}

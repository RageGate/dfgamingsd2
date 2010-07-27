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
SDName: General Vezax
SD%Complete: 90
SDComment: TODO: yells
SDAuthor: Killerfrca
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

enum
{
    SPELL_SHADOW_CRASH                      = 62660,
    SPELL_AURA_OF_DESPAIR                   = 62692,
    SPELL_MARK_OF_FACELESS                  = 63276,
    SPELL_SEARING_FLAMES                    = 62661,
    SPELL_SURGE_OF_DARKNESS                 = 62662,
    SPELL_SARONITE_BARRIER                  = 63364,
    SPELL_SARONITE_VAPORS                   = 63323,
    SPELL_ENRAGE                            = 26662,

    SPELL_SUMMON_SARONITE_ANIMUS            = 63145,
    SPELL_SUMMON_SARONITE_VAPORS            = 63081,

    SPELL_SARONITE_ANIMUS_FORMATION         = 63319,
    SPELL_PROFOUND_DARKNESS                 = 63420,

    NPC_SARONITE_ANIMUS                     = 33524,
    NPC_SARONITE_VAPOR                      = 33488,
};

struct MANGOS_DLL_DECL boss_generalvezaxAI : public ScriptedAI
{
    boss_generalvezaxAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    std::list<Creature*> m_pVapors;

    float home[3];
    bool HardMode;
    bool AnimusAlive;
    bool SearingFlames;
    bool VaporKilled;
    bool PrepareAnimus;
    
    uint32 m_uiSearingFlames_Timer;
    uint32 m_uiImmunity;
    uint32 m_uiShadowCrash_Timer;
    uint32 m_uiMarkOfFaceless_Timer;
    uint32 m_uiSurgeOfDarkness_Timer;
    uint32 m_uiEnrage_Timer;
    uint32 m_uiSummonVapors_Timer;
    uint32 m_uiAnimus_Timer;

    uint32 Vapors_Count;

    Creature* pAnimus;

    void Reset()
    {
        m_uiSearingFlames_Timer = 8*IN_MILLISECONDS;
        m_uiShadowCrash_Timer = 10*IN_MILLISECONDS;
        m_uiMarkOfFaceless_Timer = urand(10,35)*IN_MILLISECONDS;
        m_uiSurgeOfDarkness_Timer = urand(20,40)*IN_MILLISECONDS;
        m_uiEnrage_Timer = 10*MINUTE*IN_MILLISECONDS;
        m_uiSummonVapors_Timer = 10*IN_MILLISECONDS;

        Vapors_Count = 0;

        home[0] = m_creature->GetPositionX();
        home[1] = m_creature->GetPositionY();
        home[2] = m_creature->GetPositionZ();

        if(pAnimus)
            pAnimus->ForcedDespawn();

        pAnimus = NULL;

        m_pVapors.clear();
        HardMode = false;
        SearingFlames = false;
        VaporKilled = false;
    }

    void KilledUnit(Unit *victim)
    {
    }

    void JustDied(Unit *victim)
    {
    }

    void Aggro(Unit* pWho)
    {
        m_creature->SetInCombatWithZone();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_VEZAX, IN_PROGRESS);

        // Mana-regen stop aura
        m_creature->CastSpell(m_creature, SPELL_AURA_OF_DESPAIR, true);
    }

    void PreActivateHardMode()
    {
        //m_creature->CastSpell(m_creature, SPELL_SUMMON_SARONITE_ANIMUS, true);
        //AnimusAlive = true;
        m_creature->CastSpell(m_creature, SPELL_SARONITE_BARRIER, true);

        GetCreatureListWithEntryInGrid(m_pVapors, m_creature, NPC_SARONITE_VAPOR, 200.0f);
    
        for(std::list<Creature*>::iterator iter = m_pVapors.begin(); iter != m_pVapors.end(); ++iter)
        {
            Creature* pVapor = *iter;
            pVapor->RemoveAllAuras();
            pVapor->DeleteThreatList();
            pVapor->setFaction(35);
            pVapor->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            pVapor->SetSpeedRate(MOVE_RUN, 3.5f, true);
            pVapor->GetMotionMaster()->MoveChase(m_creature);
            pVapor->GetMotionMaster()->MovePoint(0, home[0], home[1], home[2]);
           
        }
        PrepareAnimus = true;
        m_uiAnimus_Timer = 9*IN_MILLISECONDS;
        HardMode = true;
    }
    void ActivateHardMode()
    {
        if(pAnimus = m_creature->SummonCreature(NPC_SARONITE_ANIMUS, home[0], home[1], home[2], 0, TEMPSUMMON_DEAD_DESPAWN, 0))
            pAnimus->AI()->AttackStart(m_creature->getVictim());
        for(std::list<Creature*>::iterator iter = m_pVapors.begin(); iter != m_pVapors.end(); ++iter)
            (*iter)->ForcedDespawn();
        
        AnimusAlive = true;
    }

    void AnimusDied()
    {
        AnimusAlive = false;
        m_creature->RemoveAura(SPELL_SARONITE_BARRIER, EFFECT_INDEX_0);
    }

    Unit* TargetRandomPreferRanged()
    {
        Unit* target = NULL;
        bool found = false;
        std::list<Unit*> rangedTargetList;
        ThreatList const& tList = m_creature->getThreatManager().getThreatList();
        for (ThreatList::const_iterator itr = tList.begin();itr != tList.end(); ++itr)
        {
            Unit* pUnit = Unit::GetUnit((*m_creature), (*itr)->getUnitGuid());
            if (!pUnit || pUnit->GetTypeId() != TYPEID_PLAYER || pUnit->GetDistance(m_creature) < 15.0f)
                continue;

            rangedTargetList.push_back(pUnit);
            if(!found)
                found = true;
        }
        if(found)
        {
            std::list<Unit*>::iterator i = rangedTargetList.begin();
            std::advance(i, rand() % rangedTargetList.size());
            target = *i;
        }
        else
        {
            target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
        }

        return target;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(!m_creature->HasAura(SPELL_SARONITE_BARRIER) || !m_bIsRegularMode)
        {
            // Searing Flames and interrupt immunity
            if(m_uiSearingFlames_Timer < diff)
            {
                m_creature->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, false);
                DoCast(m_creature, SPELL_SEARING_FLAMES);
                m_uiSearingFlames_Timer = urand(6,9)*IN_MILLISECONDS;
                m_uiImmunity = 2*IN_MILLISECONDS;
                SearingFlames = true;
            }
            else 
            {
                m_uiSearingFlames_Timer -= diff;

                if(SearingFlames)
                    if(m_uiImmunity < diff)
                    {
                        m_creature->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);
                        SearingFlames = false;
                    }else m_uiImmunity -= diff;
            }
        }

        // Summon Vapors
        if(m_uiSummonVapors_Timer < diff && Vapors_Count < 6)
        {
            DoCast(m_creature, SPELL_SUMMON_SARONITE_VAPORS);
            m_uiSummonVapors_Timer = 30*IN_MILLISECONDS;
            Vapors_Count++;
            if(Vapors_Count == 6 && !VaporKilled)
                PreActivateHardMode();
        }else m_uiSummonVapors_Timer -= diff;

        if(m_uiAnimus_Timer < diff && PrepareAnimus)
        {
            ActivateHardMode();
            PrepareAnimus = false;
        }else m_uiAnimus_Timer -= diff;

        // Shadow Crash
        if(m_uiShadowCrash_Timer < diff)
        {
            if(Unit* target = TargetRandomPreferRanged())
                DoCast(target, SPELL_SHADOW_CRASH);
            m_uiShadowCrash_Timer = urand(20,25)*IN_MILLISECONDS;
        }else m_uiShadowCrash_Timer -= diff;

        // Mark Of Faceless
        if(m_uiMarkOfFaceless_Timer < diff)
        {
            if(Unit* target = TargetRandomPreferRanged())
                DoCast(target, SPELL_MARK_OF_FACELESS);
            m_uiMarkOfFaceless_Timer = urand(10,45)*IN_MILLISECONDS;
        }else m_uiMarkOfFaceless_Timer -= diff;

        // Surge Of Darkness
        if(m_uiSurgeOfDarkness_Timer < diff)
        {
            DoCast(m_creature, SPELL_SURGE_OF_DARKNESS);
            m_uiSurgeOfDarkness_Timer = urand(60,70)*IN_MILLISECONDS;		
        }else m_uiSurgeOfDarkness_Timer -= diff;

        // Enrage
        if(m_uiEnrage_Timer < diff)
        {
            DoCast(m_creature, SPELL_ENRAGE);
            m_uiEnrage_Timer = IN_MILLISECONDS;
        }else m_uiEnrage_Timer -= diff;

        DoMeleeAttackIfReady();

        EnterEvadeIfOutOfCombatArea(diff);
    }
};

struct MANGOS_DLL_DECL boss_saroniteanimusAI : public ScriptedAI
{
    boss_saroniteanimusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiProfoundDarkness_Timer;

    Creature* pVezax;
    void Reset()
    {
        m_uiProfoundDarkness_Timer = IN_MILLISECONDS;
        m_creature->CastSpell(m_creature, SPELL_SARONITE_ANIMUS_FORMATION, true);
        pVezax = (Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(DATA_VEZAX));
        if(!pVezax || !pVezax->getVictim())
            return;

        AttackStart(pVezax->getVictim());
    }

    void JustDied(Unit *victim)
    {
        if(pVezax)
            ((boss_generalvezaxAI*)pVezax->AI())->AnimusDied();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Profound Darkness
        if(m_uiProfoundDarkness_Timer < diff)
        {
            DoCast(m_creature, SPELL_PROFOUND_DARKNESS);
            m_uiProfoundDarkness_Timer = 1500;
        }else m_uiProfoundDarkness_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL npc_saronitevaporsAI : public ScriptedAI
{
    npc_saronitevaporsAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    Creature* pVezax;

    void Reset()
    {
        pVezax = (Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(DATA_VEZAX));
    }
    void AttackStart(Unit *pWho){return;}
    void DamageTaken(Unit* pDoneBy, uint32 &damage)
    {
        // Mana regen pool
        if(damage >= m_creature->GetHealth())
            m_creature->CastSpell(m_creature, SPELL_SARONITE_VAPORS, true);
    }

    void JustDied(Unit *victim)
    {
        if(pVezax)
            ((boss_generalvezaxAI*)pVezax->AI())->VaporKilled = true;
    }

    void UpdateAI(const uint32 diff){}
};

CreatureAI* GetAI_boss_generalvezax(Creature* pCreature)
{
    return new boss_generalvezaxAI(pCreature);
}
CreatureAI* GetAI_boss_saroniteanimus(Creature* pCreature)
{
    return new boss_saroniteanimusAI(pCreature);
}
CreatureAI* GetAI_npc_saronitevapors(Creature* pCreature)
{
    return new npc_saronitevaporsAI(pCreature);
}

void AddSC_boss_vezax()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_generalvezax";
    newscript->GetAI = &GetAI_boss_generalvezax;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_saroniteanimus";
    newscript->GetAI = &GetAI_boss_saroniteanimus;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_saronitevapors";
    newscript->GetAI = &GetAI_npc_saronitevapors;
    newscript->RegisterSelf();
}

/*
UPDATE `creature_template` SET `mechanic_immune_mask` = 650854271, `ScriptName` = 'boss_generalvezax' WHERE `entry` = 33271;
UPDATE `creature_template` SET `minlevel` = 83, `maxlevel` = 83, `minhealth` = 1575785, `maxhealth` = 1575785, `faction_A` = 16, `faction_H` = 16, `mindmg` = 412, `maxdmg` = 562, `attackpower` = 536, `mechanic_immune_mask` = 650854271, `ScriptName` = 'boss_saroniteanimus' WHERE `entry` = 33524;
UPDATE `creature_template` SET `minhealth` = 12600, `maxhealth` = 12600, `minlevel` = 80, `maxlevel` = 80, `faction_A` = 32, `faction_H` = 32, `ScriptName` = 'npc_saronitevapors' WHERE `entry` = 33488;
*/
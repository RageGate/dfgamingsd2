/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Boss_Thaddius
SD%Complete: 50
SDComment: Dev Phase /dev/FallenAngelX
SDCategory: Naxxramas
EndScriptData */

/* ToDo
Bosy mozna bic tylko na ich platformach musimy sprawdzac  czy bos zanjduje sie  blisko rozdzki jezeli nie testla coil -> dla wsyztkich 3 bosow
ogolne sprawdzenie wsyztkich speli
maski i inne duperele

Spele addow  dzialaja - > do sprawdzenia  hp event
+ sprawdzanie zasiegu od meisjca spawnu

Sumonowane Tesla Coil na  liste  i depsawn
*/

#include "precompiled.h"
#include "naxxramas.h"

enum
{
    // Stalagg
    SAY_STAL_AGGRO                = -1533023,
    SAY_STAL_SLAY                 = -1533024,
    SAY_STAL_DEATH                = -1533025,

    SPELL_POWERSURGE              = 28134,
    SPELL_POWERSURGE_H            = 54529,                    

    //Feugen
    SAY_FEUG_AGGRO                = -1533026,
    SAY_FEUG_SLAY                 = -1533027,
    SAY_FEUG_DEATH                = -1533028,

    SPELL_STATICFIELD             = 28135,
    SPELL_STATICFIELD_H           = 54528,
    SPELL_WARSTOMP                = 28125,

    //Thaddus
    SAY_GREET                     = -1533029,
    SAY_AGGRO1                    = -1533030,
    SAY_AGGRO2                    = -1533031,
    SAY_AGGRO3                    = -1533032,
    SAY_SLAY                      = -1533033,
    SAY_ELECT                     = -1533034,
    SAY_DEATH                     = -1533035,
    SAY_SCREAM1                   = -1533036,
    SAY_SCREAM2                   = -1533037,
    SAY_SCREAM3                   = -1533038,
    SAY_SCREAM4                   = -1533039,

    SPELL_CHARGE_POSITIVE_DMGBUFF = 29659,
    SPELL_CHARGE_POSITIVE_NEARDMG = 28059,
    SPELL_CHARGE_NEGATIVE_DMGBUFF = 29660,
    SPELL_CHARGE_NEGATIVE_NEARDMG = 28084,
    SPELL_BERSERK                 = 26662,
    SPELL_POLARITYSHIFT           = 28089,
    SPELL_BALL_LIGHTING           = 28299,
    SPELL_CHAIN_LIGHTNING         = 28167,
    SPELL_CHAIN_LIGHTNING_H       = 54531,

    //generic
    CREATURE_TESLA_COIL           = 16218                   //the coils (emotes "Tesla Coil overloads!")
};

// Tesla Coil AI
struct MANGOS_DLL_DECL mob_teslacoilAI : public Scripted_NoMovementAI
{
    mob_teslacoilAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    uint64 m_uiCreatureGUID;
    uint32 m_uiCheckRangeTimer;

    void Reset()
    {
        m_uiCreatureGUID    = 0;
        m_uiCheckRangeTimer = 1000;
    }    

    void SumonedBy(Unit* pCreature)
    {
        if(pCreature)
            m_uiCreatureGUID = pCreature->GetGUID();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        
        if (Unit* pCreature = Unit::GetUnit(*m_creature, m_uiCreatureGUID))
        {
            if (!pCreature ->isAlive() || !pCreature)
            {
                m_creature->ForcedDespawn();
                return;
            }

            if (m_uiCheckRangeTimer < uiDiff)
            {
                if (pCreature->isAlive() && !m_creature->IsWithinDistInMap(pCreature, 15))
                    if (m_creature->getVictim())
                        m_creature->CastSpell(m_creature->getVictim(), SPELL_BALL_LIGHTING, true);

                m_uiCheckRangeTimer = 2000;
            }else m_uiCheckRangeTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_mob_teslacoil(Creature* pCreature)
{
    return new mob_teslacoilAI(pCreature);
}

struct MANGOS_DLL_DECL boss_thaddiusAI : public ScriptedAI
{
    boss_thaddiusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool   m_bIsRegularMode;
    bool   m_bIsBerserk;
    bool   m_bIsPolarity;
    bool   m_bIsBallLighting;
    bool   m_bIsAggro;
    uint64 m_uiPlayerGUID;
    uint32 m_uiAggroTimer;
    uint32 m_uiScreamsTimer;
    uint32 m_uiPolarityDMGTimer;
    uint32 m_uiBerserkTimer;
    uint32 m_uiBallLightingTimer;
    uint32 m_uiChainLightingTimer;
    uint32 m_uiPolarityTimer;

    void Reset()
    {
        m_bIsAggro              = false;
        m_bIsBallLighting       = true;
        m_bIsPolarity           = false;
        m_bIsBerserk            = false;
        m_uiPlayerGUID          = 0;
        m_uiAggroTimer          = 5000;
        m_uiScreamsTimer        = 40000;
        m_uiPolarityDMGTimer    = 5000;
        m_uiBerserkTimer        = 300000;
        m_uiBallLightingTimer   = 1000;
        m_uiChainLightingTimer  = urand(13000,17000);
        m_uiPolarityTimer       = 30000;

        if (m_pInstance)
            m_pInstance->SetData(TYPE_THADDIUS, NOT_STARTED);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_THADDIUS, DONE);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(SAY_SLAY, m_creature);
    }

    void Aggro(Unit* pWho)
    {
        m_creature->SetInCombatWithZone();

        if (Creature* pTeslaCoil = m_creature->SummonCreature(CREATURE_TESLA_COIL, m_creature->GetPositionX()+urand(1,4), m_creature->GetPositionY()+urand(1,4), m_creature->GetPositionZ(), m_creature->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 1200000))
        {
            ((mob_teslacoilAI*)pTeslaCoil->AI())->SumonedBy(m_creature);
            pTeslaCoil->SetInCombatWithZone();
        }

        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_THADDIUS, IN_PROGRESS);
    }

    void StartEvent(Unit* pPlayer)
    {
        if(pPlayer)
            m_uiPlayerGUID = pPlayer->GetGUID();
    }

    void CheckDistance()
    {
        m_bIsBallLighting = true;
        std::list<HostileReference *> t_list = m_creature->getThreatManager().getThreatList();
        for (std::list<HostileReference *>::iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
        {
            Unit *pPlayer = Unit::GetUnit(*m_creature, (*itr)->getUnitGuid());  
            if (pPlayer && pPlayer->GetTypeId() == TYPEID_PLAYER && pPlayer->isAlive() && m_creature->IsWithinDistInMap(pPlayer, 6))
                m_bIsBallLighting = false;
        }
    
        if (m_bIsBallLighting)
            if (m_creature->getVictim())
                m_creature->CastSpell(m_creature->getVictim(), SPELL_BALL_LIGHTING, false);
    }   

    void CastPolarity()
    {
        std::list<HostileReference *> t_list = m_creature->getThreatManager().getThreatList();
        for (std::list<HostileReference *>::iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
        {
            Unit *pPlayer = Unit::GetUnit(*m_creature, (*itr)->getUnitGuid());  
            if (pPlayer && pPlayer->GetTypeId() == TYPEID_PLAYER && pPlayer->isAlive())
            {
                if (urand(0,1) == 0)
                    pPlayer->CastSpell(pPlayer, SPELL_CHARGE_POSITIVE_DMGBUFF, true);
                else
                    pPlayer->CastSpell(pPlayer, SPELL_CHARGE_NEGATIVE_DMGBUFF, true);
            }
        }

        m_bIsPolarity = true;
        m_uiPolarityDMGTimer = 5000;
    }

    void CastDMGPolarity()
    {
        std::list<HostileReference *> t_list = m_creature->getThreatManager().getThreatList();
        for (std::list<HostileReference *>::iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
        {
            Unit *pPlayer = Unit::GetUnit(*m_creature, (*itr)->getUnitGuid());  
            if (pPlayer && pPlayer->GetTypeId() == TYPEID_PLAYER && pPlayer->isAlive())
            {
                std::list<HostileReference *> t_list = m_creature->getThreatManager().getThreatList();
                for (std::list<HostileReference *>::iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                {
                    Unit *pVictim = Unit::GetUnit(*m_creature, (*itr)->getUnitGuid());  
                    if (pVictim && pVictim->GetTypeId() == TYPEID_PLAYER && pVictim->isAlive())
                    {
                        if (pPlayer->IsWithinDistInMap(pVictim,5))
                        {
                            if (pPlayer->HasAura(SPELL_CHARGE_POSITIVE_DMGBUFF))
                                pPlayer->CastSpell(pVictim, SPELL_CHARGE_POSITIVE_NEARDMG, true);
                            else
                                pPlayer->CastSpell(pVictim, SPELL_CHARGE_NEGATIVE_NEARDMG, true);
                        }
                    }
                }
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_STALAGG) == DONE && m_pInstance->GetData(TYPE_FEUGEN) == DONE && m_pInstance->GetData(TYPE_THADDIUS) == NOT_STARTED)
        {
            m_uiAggroTimer = 15000;
            m_bIsAggro = true; 
        }

        if (m_bIsAggro && m_uiAggroTimer < uiDiff)
        {
            if (Unit* pPlayer = Unit::GetUnit(*m_creature, m_uiPlayerGUID))
                m_creature->AI()->AttackStart(pPlayer);
            m_bIsAggro = false;
        }else m_uiAggroTimer -= uiDiff;
 
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiScreamsTimer < uiDiff)
        {
            switch(urand(0, 3))
            {
                case 0: DoScriptText(SAY_SCREAM1, m_creature); break;
                case 1: DoScriptText(SAY_SCREAM2, m_creature); break;
                case 2: DoScriptText(SAY_SCREAM3, m_creature); break;
                case 3: DoScriptText(SAY_SCREAM4, m_creature); break;
            }

            m_uiScreamsTimer = urand(30000,40000);
        }else m_uiScreamsTimer -= uiDiff;

        if (!m_bIsBerserk && m_uiBerserkTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_BERSERK);
            m_bIsBerserk = true;
        }else m_uiBerserkTimer -= uiDiff;

        if (m_uiChainLightingTimer < uiDiff)
        {
            if(Unit* pPlayer = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                m_creature->CastSpell(pPlayer, m_bIsRegularMode ? SPELL_CHAIN_LIGHTNING : SPELL_CHAIN_LIGHTNING_H, false);
            m_uiChainLightingTimer  = urand(13000,17000);
        }else m_uiChainLightingTimer -= uiDiff;

        if (m_uiBallLightingTimer < uiDiff)
        {
            CheckDistance();
            m_uiBallLightingTimer = 2000;
        }else m_uiBallLightingTimer -= uiDiff;

        if (m_uiPolarityTimer < uiDiff)
        {
            DoScriptText(SAY_ELECT, m_creature);
            CastPolarity();
            m_uiPolarityTimer = 30000;
        }else m_uiPolarityTimer -= uiDiff;

        if (m_bIsPolarity && m_uiPolarityDMGTimer < uiDiff)
        {
            //CastDMGPolarity();
            m_bIsPolarity = false;
        }else m_uiPolarityDMGTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_thaddius(Creature* pCreature)
{
    return new boss_thaddiusAI(pCreature);
}

//BOSS FEUGEN
struct MANGOS_DLL_DECL boss_stalaggAI : public ScriptedAI
{
    boss_stalaggAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool   m_bIsRegularMode;
    bool   m_bIsHPEvent;
    bool   m_bIsReset;
    bool   m_bIsEventNow;
    uint32 m_uiResetTimer;
    uint32 m_uiEnrageTimer;
    uint32 m_uiOutOfRangeTimer;
    uint32 m_uiMagneticPullTimer;
    uint32 m_uiWarStompTimer;

    void Reset()
    {
        m_bIsEventNow         = true;
        m_bIsReset            = false;
        m_bIsHPEvent          = false;
        m_uiResetTimer        = 10000;
        m_uiOutOfRangeTimer   = 1000;
        m_uiEnrageTimer       = urand(15000,25000);
        m_uiMagneticPullTimer = 30000;
        m_uiWarStompTimer     = 15000;

        if (m_pInstance)
            m_pInstance->SetData(TYPE_STALAGG, NOT_STARTED);
    }    

    void Aggro(Unit* pWho)
    {
        m_creature->SetInCombatWithZone();

        if (Creature* pTeslaCoil = m_creature->SummonCreature(CREATURE_TESLA_COIL, m_creature->GetPositionX()+urand(1,4), m_creature->GetPositionY()+urand(1,4), m_creature->GetPositionZ(), m_creature->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 1200000))
        {
            ((mob_teslacoilAI*)pTeslaCoil->AI())->SumonedBy(m_creature);
            pTeslaCoil->SetInCombatWithZone();
        }

        DoScriptText(SAY_FEUG_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_STALAGG, IN_PROGRESS);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(SAY_FEUG_SLAY, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_FEUG_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_STALAGG, DONE);

        if (m_pInstance && m_pInstance->GetData(TYPE_FEUGEN) == DONE)
        {
            if (Creature* pThaddius = ((Creature*)Unit::GetUnit(*m_creature, m_pInstance->GetData64(NPC_THADDIUS))))
            { 
                ((boss_thaddiusAI*)pThaddius->AI())->StartEvent(pKiller);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //HP event
        if (m_bIsReset)
            return;

        if (m_bIsEventNow)
        {
            if (m_bIsHPEvent && m_uiResetTimer < uiDiff)
            {
                if (m_pInstance && m_pInstance->GetData(TYPE_FEUGEN) == SPECIAL)
                {
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    m_bIsEventNow = false;    
                }
                else 
                {
                    m_bIsReset = true;
                    return;
                }
                return;
            }

            if (m_creature->GetHealth()*100 / m_creature->GetMaxHealth() < 5 && !m_bIsHPEvent)
            {
                if(m_pInstance)
                    m_pInstance->SetData(TYPE_STALAGG, SPECIAL);

                if (m_pInstance && m_pInstance->GetData(TYPE_FEUGEN) != SPECIAL)
                {
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                }
                else
                    m_bIsEventNow = false;

                m_uiResetTimer = 10000;
                m_bIsHPEvent = true;
                return;
            }
        }
        //End of HP Event

        if (m_uiOutOfRangeTimer < uiDiff)
        {
            m_uiOutOfRangeTimer= 30000;
        }else m_uiOutOfRangeTimer -= uiDiff;
  
        if (m_uiEnrageTimer < uiDiff)
        {
            m_creature->CastSpell(m_creature, m_bIsRegularMode ? SPELL_POWERSURGE_H : SPELL_POWERSURGE, false);
            m_uiEnrageTimer = urand(18000,23000);
        }else m_uiEnrageTimer -= uiDiff;

        if (m_uiMagneticPullTimer < uiDiff)
        {
            m_uiMagneticPullTimer = 30000;
        }else m_uiMagneticPullTimer -= uiDiff;

        if (m_uiWarStompTimer < uiDiff)
        {
            if(m_creature->getVictim())
                m_creature->CastSpell(m_creature->getVictim(), SPELL_WARSTOMP, false);
            m_uiWarStompTimer = 15000;
        }else m_uiWarStompTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_stalagg(Creature* pCreature)
{
    return new boss_stalaggAI(pCreature);
}

// BOSS STALAGG
struct MANGOS_DLL_DECL boss_feugenAI : public ScriptedAI
{
    boss_feugenAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool   m_bIsRegularMode;
    bool   m_bIsHPEvent;
    bool   m_bIsReset;
    bool   m_bIsEventNow;
    uint32 m_uiOutOfRangeTimer;
    uint32 m_uiResetTimer;
    uint32 m_uiWarStompTimer;
    uint32 m_uiStaticFieldTimer;

    void Reset()
    {
        m_bIsEventNow       = true;
        m_bIsReset          = false;
        m_bIsHPEvent        = false;
        m_uiOutOfRangeTimer = 1000;
        m_uiResetTimer      = 10000;
        m_uiStaticFieldTimer= 3000;
        m_uiWarStompTimer   = 15000;

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FEUGEN, NOT_STARTED);
    }    

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_FEUGEN, IN_PROGRESS);

        m_creature->SetInCombatWithZone();

        if (Creature* pTeslaCoil = m_creature->SummonCreature(CREATURE_TESLA_COIL, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), m_creature->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 1200000))
        {
            ((mob_teslacoilAI*)pTeslaCoil->AI())->SumonedBy(m_creature);
            pTeslaCoil->SetInCombatWithZone();
        }

        DoScriptText(SAY_STAL_AGGRO, m_creature);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(SAY_STAL_SLAY, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_STAL_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FEUGEN, DONE);

        if (m_pInstance && m_pInstance->GetData(TYPE_STALAGG) == DONE)
        {
            if (Creature* pThaddius = ((Creature*)Unit::GetUnit(*m_creature, m_pInstance->GetData64(NPC_THADDIUS))))
            { 
                ((boss_thaddiusAI*)pThaddius->AI())->StartEvent(pKiller);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //HP event
        if (m_bIsReset)
            return;

        if (m_bIsEventNow)
        {
            if (m_bIsHPEvent && m_uiResetTimer < uiDiff)
            {
                if (m_pInstance && m_pInstance->GetData(TYPE_STALAGG) == SPECIAL)
                {
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    m_bIsEventNow = false;    
                }
                else 
                {
                    m_bIsReset = true;
                    return;
                }
                return;
            }

            if (m_creature->GetHealth()*100 / m_creature->GetMaxHealth() < 5 && !m_bIsHPEvent)
            {
                if(m_pInstance)
                    m_pInstance->SetData(TYPE_FEUGEN, SPECIAL);

                if (m_pInstance && m_pInstance->GetData(TYPE_STALAGG) != SPECIAL)
                {
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                }
                else
                    m_bIsEventNow = false;

                m_uiResetTimer = 10000;
                m_bIsHPEvent = true;
                return;
            }
        }
        //End of HP Event

        if (m_uiOutOfRangeTimer < uiDiff)
        {
            m_uiOutOfRangeTimer= 1000;
        }else m_uiOutOfRangeTimer -= uiDiff;

        if (m_uiStaticFieldTimer < uiDiff)
        {
            m_creature->CastSpell(m_creature, m_bIsRegularMode ? SPELL_STATICFIELD : SPELL_STATICFIELD_H, false);
            m_uiStaticFieldTimer = 3000;
        }else m_uiStaticFieldTimer -= uiDiff;

        if (m_uiWarStompTimer < uiDiff)
        {
            if(m_creature->getVictim())
                m_creature->CastSpell(m_creature->getVictim(), SPELL_WARSTOMP, false);
            m_uiWarStompTimer = 15000;
        }else m_uiWarStompTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_feugen(Creature* pCreature)
{
    return new boss_feugenAI(pCreature);
}

void AddSC_boss_thaddius()
{
    Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_thaddius";
    NewScript->GetAI = &GetAI_boss_thaddius;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "boss_feugen";
    NewScript->GetAI = &GetAI_boss_feugen;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "boss_stalagg";
    NewScript->GetAI = &GetAI_boss_stalagg;
    NewScript->RegisterSelf();
    
    NewScript = new Script;
    NewScript->Name = "mob_teslacoil";
    NewScript->GetAI = &GetAI_mob_teslacoil;
    NewScript->RegisterSelf();
}

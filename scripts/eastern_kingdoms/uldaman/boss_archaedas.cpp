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
SDName: Boss_Archaedas
SD%Complete: 100
SDComment: 
SDCategory: Uldaman
EndScriptData */

#include "precompiled.h"
#include "uldaman.h"

enum
{
    FACTION_TITAN_UNFRIENDLY        = 415,
    MOB_CUSTODIAN                   = 7309,
    MOB_GUARDIAN                    = 7076,
    MOB_HALLSHAPER                  = 7077,
    MOB_VAULT_WARDER                = 10120,

    SPELL_GROUND_TREMMOR            = 6524,
    SPELL_STONED                    = 10255,
    SPELL_AWAKEN_EARTHEN_GUARDIAN   = 10252,
    SPELL_STONE_DWARF_AWAKEN_VISUAL = 10254,
    SPELL_AWAKEN_VAULT_WARDER       = 10258,
    SPELL_AWAKEN_EARTHEN_DWARF      = 10259,
    SPELL_ARCHAEDAS_AWAKEN_VISUAL   = 10347,
    SPELL_USE_ALTAR_VISUAL          = 11206,

    SAY_AGGRO                       = -1999821,
    SAY_AWAKE_GUARDIANS             = -1999822,
    SAY_AWAKE_WARDERS               = -1999823,
    SAY_UNIT_SLAIN                  = -1999824
};

struct MANGOS_DLL_DECL boss_archaedasAI : public ScriptedAI
{
    boss_archaedasAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    std::list<Creature*> lArchaedasAdds;
    uint32 m_uiIntroTimer;
    uint32 m_uiAwakeDwarfTimer;
    uint8 m_uiSubevent;
    bool bGuardiansAwaken;
    bool bWardersAwaken;

    void Reset()
    {
        // Fill Archaedas adds list. Maybe they should be deleted from DB and spawned during script execution?
        if (lArchaedasAdds.empty())
            AquireAddsList();
        // Respawn dead adds
        if (!lArchaedasAdds.empty())
            for (std::list<Creature*>::iterator itr = lArchaedasAdds.begin(); itr != lArchaedasAdds.end(); ++itr)
            {
                if ((*itr) && !(*itr)->isAlive())
                    (*itr)->Respawn();
            }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ARCHAEDAS_EVENT, NOT_STARTED);
        // restore orginal DB faction and unitflags at reseting
        // it should be not selectable, not attackable and not interract in any way with players
        m_creature->SetFlag(UNIT_FIELD_FLAGS, m_creature->GetCreatureInfo()->unit_flags);
        m_creature->setFaction(m_creature->GetCreatureInfo()->faction_A);

        m_uiIntroTimer = 1000;
        m_uiSubevent = 0;
        m_uiAwakeDwarfTimer = 10000;
        bGuardiansAwaken = false;
        bWardersAwaken = false;
        
    }
    void AquireAddsList()
    {
        //Aquire Custodians
        std::list<Creature*> lCreatureList;
        GetCreatureListWithEntryInGrid(lCreatureList, m_creature, MOB_CUSTODIAN, 100.0f);
        if (!lCreatureList.empty())
            for (std::list<Creature*>::iterator itr = lCreatureList.begin(); itr != lCreatureList.end(); ++itr)
            {
                lArchaedasAdds.push_back((*itr));
            }
        lCreatureList.clear();
        //Aquire Earthen Guardians
        GetCreatureListWithEntryInGrid(lCreatureList, m_creature, MOB_GUARDIAN, 100.0f);
        if (!lCreatureList.empty())
            for (std::list<Creature*>::iterator itr = lCreatureList.begin(); itr != lCreatureList.end(); ++itr)
            {
                lArchaedasAdds.push_back((*itr));
            }
        lCreatureList.clear();
        //Aquire Hallshapers
        GetCreatureListWithEntryInGrid(lCreatureList, m_creature, MOB_HALLSHAPER, 100.0f);
        if (!lCreatureList.empty())
            for (std::list<Creature*>::iterator itr = lCreatureList.begin(); itr != lCreatureList.end(); ++itr)
            {
                lArchaedasAdds.push_back((*itr));
            }
        lCreatureList.clear();
        //Aquire Vault Warders
        GetCreatureListWithEntryInGrid(lCreatureList, m_creature, MOB_VAULT_WARDER, 100.0f);
        if (!lCreatureList.empty())
            for (std::list<Creature*>::iterator itr = lCreatureList.begin(); itr != lCreatureList.end(); ++itr)
            {
                lArchaedasAdds.push_back((*itr));
            }
        lCreatureList.clear();
    }

    void Aggro(Unit *pWho)
    {
        if (!pWho || !m_pInstance)
            return;
        m_pInstance->SetData(TYPE_ARCHAEDAS_EVENT, IN_PROGRESS);
        ScriptedAI::Aggro(pWho);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(SAY_UNIT_SLAIN, m_creature, NULL);
    }

    void JustDied(Unit *pKiller)
    {
        // Despawn Adds
        if (!lArchaedasAdds.empty())
            for (std::list<Creature*>::iterator itr = lArchaedasAdds.begin(); itr != lArchaedasAdds.end(); ++itr)
            {
                if ((*itr))
                    (*itr)->ForcedDespawn();
            }

        if (!m_pInstance)
            return;

        // open door to vault (handled by instance script)
        m_pInstance->SetData(TYPE_ARCHAEDAS_EVENT, DONE);
    }

    void SetTargetableForCast(uint32 entry)
    {
        // lets make certain creatures vulnarable for spells effect before cast awakening spells with AoE effect
        for(std::list<Creature*>::iterator itr = lArchaedasAdds.begin(); itr != lArchaedasAdds.end(); ++itr)
        {
            if ((*itr) && (*itr)->GetEntry() == entry)
            {
                if ((*itr)->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                    (*itr)->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                if ((*itr)->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
                    (*itr)->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // OOC Intro part triggered by Altar activation
        if (m_pInstance && m_pInstance->GetData(TYPE_ARCHAEDAS_EVENT) == SPECIAL)
        {
            if (m_uiIntroTimer <= uiDiff)
            {
                switch(m_uiSubevent)
                {
                    case 0:
                        m_creature->RemoveAurasDueToSpell(SPELL_STONED);
                        DoCast(m_creature, SPELL_ARCHAEDAS_AWAKEN_VISUAL, false);
                        break;
                    case 1:
                        DoScriptText(SAY_AGGRO,m_creature,NULL);
                        break;
                    case 2:
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        m_creature->setFaction(FACTION_TITAN_UNFRIENDLY);
                        if (Unit* pUnit = Unit::GetUnit((*m_creature), m_pInstance->GetData64(DATA_EVENT_STARTER)))
                            AttackStart(pUnit);
                        else Reset();
                        break;
                    default: break;
                }
                ++m_uiSubevent;
                m_uiIntroTimer = 5000;
            }else m_uiIntroTimer -= uiDiff;
        }

        // Temp recurent call of ScriptedAI - without this creature is not evading - dunno why
        ScriptedAI::UpdateAI(uiDiff);

        if (!m_creature->getVictim() || !m_creature->SelectHostileTarget())
            return;

        // Awake random Dwarf
        if (m_creature->GetHealth() * 100 / m_creature->GetMaxHealth() >= 33)
        {
            if (m_uiAwakeDwarfTimer <= uiDiff)
            {
                DoCast(m_creature, SPELL_AWAKEN_EARTHEN_DWARF, false);
                m_uiAwakeDwarfTimer = urand(9000, 12000);
            }else m_uiAwakeDwarfTimer -= uiDiff;
        }

        //Awake Earthen Guardians
        if (!bGuardiansAwaken && m_creature->GetHealthPercent() <= 66)
        {
            Unit* pUnit = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);
            if (!pUnit)
                pUnit = m_creature->getVictim();
            if (!pUnit)
                return;
            m_pInstance->SetData64(DATA_EVENT_STARTER, pUnit->GetGUID());

            DoScriptText(SAY_AWAKE_GUARDIANS, m_creature, NULL);
            SetTargetableForCast(MOB_GUARDIAN);
            m_creature->InterruptNonMeleeSpells(false);
            DoCast(m_creature, SPELL_AWAKEN_EARTHEN_GUARDIAN, true);
            bGuardiansAwaken = true;
        }

        // Awake Warders
        if (!bWardersAwaken && m_creature->GetHealthPercent() <= 33)
        {
            Unit* pUnit = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);
            if (!pUnit)
                pUnit = m_creature->getVictim();
            if (!pUnit)
                return;
            m_pInstance->SetData64(DATA_EVENT_STARTER, pUnit->GetGUID());

            DoScriptText(SAY_AWAKE_WARDERS, m_creature, NULL);
            SetTargetableForCast(MOB_VAULT_WARDER);
            m_creature->InterruptNonMeleeSpells(false);
            DoCast(m_creature, SPELL_AWAKEN_VAULT_WARDER, true);
            bWardersAwaken = true;
        }

    }     
};

struct MANGOS_DLL_DECL mob_archaeras_addAI : public ScriptedAI
{
    mob_archaeras_addAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint32 m_uiIntroTimer;
    uint8 m_uiSubevent;
    bool bAwakening;

    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, m_creature->GetCreatureInfo()->unit_flags);
        m_creature->setFaction(m_creature->GetCreatureInfo()->faction_A);
        m_uiIntroTimer = 0;
        m_uiSubevent = 0;
        bAwakening = false;
    }

    void SpellHit(Unit *pCaster, const SpellEntry *pSpell)
    {
        if (!pSpell)
            return;

        switch (m_creature->GetEntry())
        {
            case MOB_CUSTODIAN:
            case MOB_HALLSHAPER:
                if (pSpell->Id == SPELL_AWAKEN_EARTHEN_DWARF)
                    bAwakening = true;
                break;
            case MOB_VAULT_WARDER:
                if (pSpell->Id == SPELL_AWAKEN_VAULT_WARDER)
                    bAwakening = true;
                break;
            case MOB_GUARDIAN:
                if (pSpell->Id == SPELL_AWAKEN_EARTHEN_GUARDIAN)
                    bAwakening = true;
                break;
            default: break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // OOC part triggered by awakening spell hit
        if (bAwakening)
        {
            if (m_uiIntroTimer <= uiDiff)
            {
                switch(m_uiSubevent)
                {
                case 0:
                    m_creature->RemoveAurasDueToSpell(SPELL_STONED);
                    DoCast(m_creature, SPELL_STONE_DWARF_AWAKEN_VISUAL, false);
                    break;
                case 1:
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    m_creature->setFaction(FACTION_TITAN_UNFRIENDLY);
                    if (Unit* pUnit = Unit::GetUnit((*m_creature), m_pInstance->GetData64(DATA_EVENT_STARTER)))
                    {
                        bAwakening = false;
                        AttackStart(pUnit);
                    }
                    else Reset();
                    break;
                default: break;
                }
                ++m_uiSubevent;
                m_uiIntroTimer = 2000;
            }else m_uiIntroTimer -= uiDiff;
        }

        // Temp recurent call of ScriptedAI - without this creature is not evading - dunno why
        ScriptedAI::UpdateAI(uiDiff);

        if (!m_creature->getVictim() || !m_creature->SelectHostileTarget())
            return;

        DoMeleeAttackIfReady();
    }
};

bool GOHello_go_altar_of_archaedas(Player* pPlayer, GameObject* pGo)
{
    ScriptedInstance* m_pInstance = (ScriptedInstance*)pGo->GetInstanceData();

    if (!pPlayer || !m_pInstance)
        return false;

    pPlayer->CastSpell(pPlayer, SPELL_USE_ALTAR_VISUAL,true);

    // begin OOC "intro" and store target for Archaedas to attack
    m_pInstance->SetData(TYPE_ARCHAEDAS_EVENT, SPECIAL);
    m_pInstance->SetData64(DATA_EVENT_STARTER, pPlayer->GetGUID());
    return true;
}

CreatureAI* GetAI_boss_archaedas(Creature* pCreature)
{
    return new boss_archaedasAI(pCreature);
}
CreatureAI* GetAI_mob_archaeras_add(Creature* pCreature)
{
    return new mob_archaeras_addAI(pCreature);
}

void AddSC_boss_archaedas()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_archaedas";
    newscript->GetAI = &GetAI_boss_archaedas;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_archaeras_add";
    newscript->GetAI = &GetAI_mob_archaeras_add;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_altar_of_archaedas";
    newscript->pGOHello = &GOHello_go_altar_of_archaedas;
    newscript->RegisterSelf();
}

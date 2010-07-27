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
SDName: Yogg-Saron
SD%Complete: 0
SDComment: PH.
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "def_ulduar.h"

#define CR_GUARDIAN_OF_YOGG_SARON   33136
#define CR_IMMORTAL_GUARDIAN        33988

struct MANGOS_DLL_DECL boss_yogg_saronAI : public ScriptedAI
{
    boss_yogg_saronAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Regular = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool Regular;
    ScriptedInstance *pInstance;

    void Reset()
    {
        if(pInstance) pInstance->SetData(TYPE_YOGG_SARON, NOT_STARTED);
    }

    void Aggro(Unit *who) 
    {
        if(pInstance) pInstance->SetData(TYPE_YOGG_SARON, IN_PROGRESS);
    }

    void JustDied(Unit *killer)
    {
        if(pInstance) pInstance->SetData(TYPE_YOGG_SARON, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_yogg_saron(Creature* pCreature)
{
    return new boss_yogg_saronAI(pCreature);
}

void AddSC_boss_yogg_saron()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_yogg_saron";
    newscript->GetAI = &GetAI_boss_yogg_saron;
    newscript->RegisterSelf();
}

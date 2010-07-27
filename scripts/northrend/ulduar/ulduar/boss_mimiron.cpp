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
SDName: Mimiron
SD%Complete: 0
SDComment: PH.
SDCategory: Ulduar
EndScriptData */
 
#include "precompiled.h"
#include "def_ulduar.h"

struct MANGOS_DLL_DECL boss_mimironAI : public ScriptedAI
{   
    boss_mimironAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Regular = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool Regular;
    ScriptedInstance *pInstance;

    void Reset()
    {
        if(pInstance) pInstance->SetData(TYPE_MIMIRON, NOT_STARTED);
    }

    void Aggro(Unit *who) 
    {
        if(pInstance) pInstance->SetData(TYPE_MIMIRON, IN_PROGRESS);
    }

    void JustDied(Unit *killer)
    {
        if(pInstance) pInstance->SetData(TYPE_MIMIRON, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_mimiron(Creature* pCreature)
{
    return new boss_mimironAI(pCreature);
}

void AddSC_boss_mimiron()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_mimiron";
    newscript->GetAI = &GetAI_boss_mimiron;
    newscript->RegisterSelf();
}

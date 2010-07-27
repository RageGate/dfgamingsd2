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
SDName: Algalon the Observer
SD%Complete: 0
SDComment: PH.
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "def_ulduar.h"

#define SP_HEAVENS              64487
#define SP_BIG_BANG             64443
#define H_SP_BIG_BANG           64584
#define SP_QUANTUM_STRIKE       64395
#define H_SP_QUANTUM_STRIKE     64592
#define SP_ENRAGE               47008

struct MANGOS_DLL_DECL boss_algalonAI : public ScriptedAI
{
    boss_algalonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Regular = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool Regular;
    ScriptedInstance *pInstance;
    uint32 QuantumStrikeTimer;
    uint32 EnrageTimer;

    void Reset()
    {
        if(pInstance) pInstance->SetData(TYPE_ALGALON, NOT_STARTED);
        QuantumStrikeTimer = 4000 + rand()%10000;
        EnrageTimer = 360000; //6 minutes
    }

    void Aggro(Unit *who) 
    {
        if(pInstance) pInstance->SetData(TYPE_ALGALON, IN_PROGRESS);
    }

    void JustDied(Unit *killer)
    {
        if(pInstance) pInstance->SetData(TYPE_ALGALON, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(QuantumStrikeTimer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), Regular ? SP_QUANTUM_STRIKE : H_SP_QUANTUM_STRIKE);
            QuantumStrikeTimer = 4000 + rand()%10000;
        }
        else QuantumStrikeTimer -= diff;

        if(EnrageTimer < diff)
        {
            DoCastSpellIfCan(m_creature, SP_ENRAGE, true);
            EnrageTimer = 30000;
        }
        else EnrageTimer -= diff;

        DoMeleeAttackIfReady();
        
        EnterEvadeIfOutOfCombatArea(diff);

    }
};

CreatureAI* GetAI_boss_algalon(Creature* pCreature)
{
    return new boss_algalonAI(pCreature);
}

void AddSC_boss_algalon()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_algalon";
    newscript->GetAI = &GetAI_boss_algalon;
    newscript->RegisterSelf();
}

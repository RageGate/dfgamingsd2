#include "precompiled.h"
#include "def_pit.h"

enum
{
        //common
        SPELL_BERSERK                           = 47008,
        //yells
        //summons
        //Abilities
        SPELL_FEAR                              = 68950
};

struct MANGOS_DLL_DECL boss_scourgelord_tyrannusAI : public ScriptedAI
{
    boss_scourgelord_tyrannusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance *pInstance;

    void Reset()
    {
        if(pInstance) pInstance->SetData(TYPE_TYRANNUS, NOT_STARTED);
    }

    void Aggro(Unit *who) 
    {
        if(pInstance) pInstance->SetData(TYPE_TYRANNUS, IN_PROGRESS);
    }

    void JustDied(Unit *killer)
    {
        if(pInstance) pInstance->SetData(TYPE_TYRANNUS, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_rimefang_posAI : public ScriptedAI
{
    mob_rimefang_posAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance *pInstance;

    void Reset()
    {
    }

    void Aggro(Unit *who) 
    {
    }

    void JustDied(Unit *killer)
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};


CreatureAI* GetAI_boss_scourgelord_tyrannus(Creature* pCreature)
{
    return new boss_scourgelord_tyrannusAI(pCreature);
}

CreatureAI* GetAI_mob_rimefang_pos(Creature* pCreature)
{
    return new mob_rimefang_posAI(pCreature);
}


void AddSC_boss_scourgelord_tyrannus()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_scourgelord_tyrannus";
    newscript->GetAI = &GetAI_boss_scourgelord_tyrannus;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_rimefang_pos";
    newscript->GetAI = &GetAI_mob_rimefang_pos;
    newscript->RegisterSelf();
}

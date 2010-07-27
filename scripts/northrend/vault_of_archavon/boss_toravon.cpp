/* ScriptData
SDName: Toravon the Ice Watcher
SDAuthor: Lutik
SD%Complete: 0%
SDComment:
SDCategory: Vault of Archavon
EndScriptData */

#include "precompiled.h"
#include "vault_of_archavon.h"

enum
{
    SP_WHITEOUT         = 72034,
    H_SP_WHITEOUT       = 72096,
    SP_FREEZING_GROUND  = 72090,
    H_SP_FREEZING_GROUND = 72104,
    SP_FROZEN_MALLET    = 71993,
    FROZEN_ORB_AURA     = 72081,

    CR_FROZEN_ORB       = 38456
};


struct MANGOS_DLL_DECL boss_toravonAI : public ScriptedAI
{
    boss_toravonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        orbsNum = m_bIsRegularMode ? 1 : 3;
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    bool m_bIsRegularMode;
    ScriptedInstance *pInstance;
    uint32 m_uiEvadeCheckCooldown;

    int orbsNum;
    uint32 WhiteoutTimer;
    uint32 OrbsTimer;
    uint32 FreezeTimer;

    void Reset()
    {
        m_uiEvadeCheckCooldown = 2000;
        WhiteoutTimer = 40000;
        OrbsTimer = 15000;
        FreezeTimer = 20000 + rand()%5000;

        if(pInstance)
            pInstance->SetData(TYPE_TORAVON, NOT_STARTED);
    }

    void Aggro(Unit *who)
    {
        DoCastSpellIfCan(m_creature, SP_FROZEN_MALLET);

        if(pInstance)
            pInstance->SetData(TYPE_TORAVON, IN_PROGRESS);
    }

    void JustDied(Unit *killer)
    {
        if(pInstance)
            pInstance->SetData(TYPE_TORAVON, DONE);
    }

    void JustSummoned(Creature *orb)
    {
        orb->CastSpell(orb, FROZEN_ORB_AURA, false);
        orb->SetInCombatWithZone();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiEvadeCheckCooldown < uiDiff)
        {
            if (m_creature->GetDistance2d(-43.68f, -289.02f) > 80.0f)
                EnterEvadeMode();
            m_uiEvadeCheckCooldown = 2000;
        }
        else
            m_uiEvadeCheckCooldown -= uiDiff;

        if(WhiteoutTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SP_WHITEOUT : H_SP_WHITEOUT);
            WhiteoutTimer = 40000;
        }
        else
            WhiteoutTimer -= uiDiff;

        if(OrbsTimer < uiDiff)
        {
            for(int i=0; i<orbsNum; ++i)
            {
                if(Unit *target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    m_creature->SummonCreature(CR_FROZEN_ORB, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 3000);
            }
            OrbsTimer = 40000;
        }
        else
            OrbsTimer -= uiDiff;

        if(FreezeTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SP_FREEZING_GROUND);
            FreezeTimer = 20000 + rand()%5000;
        }
        else
            FreezeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_toravon(Creature *pCreature)
{
    return new boss_toravonAI (pCreature);
};

void AddSC_boss_toravon()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_toravon";
    newscript->GetAI = &GetAI_boss_toravon;
    newscript->RegisterSelf();
};

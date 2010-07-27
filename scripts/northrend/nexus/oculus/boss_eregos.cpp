#include "precompiled.h"
#include "oculus.h"

/* /dev/ FallenangelX */

enum
{
	SPELL_ARCANE_BARRAGE    = 50804,
	SPELL_ARCANE_BARRAGE_H  = 59381,
	SPELL_ARCANE_VOLLEY     = 51153,
	SPELL_ARCANE_VOLLEY_H   = 59382,
	SPELL_ENRAGE            = 51170,
	SPELL_PLANAR_SHIFT      = 51162,
	SPELL_PLANAR_ANOMALIES  = 57959
};

struct MANGOS_DLL_DECL boss_eregosAI: public ScriptedAI
{
	boss_eregosAI(Creature* c): ScriptedAI(c)
	{
		m_pInstance = (ScriptedInstance*)m_creature->GetInstanceData();
		m_bIsRegularMode = m_creature->GetMap()->IsRegularDifficulty();
		Aggroed = false;
		Reset();
	}

	ScriptedInstance* m_pInstance;
	bool m_bIsRegularMode;

	uint32 ArcaneBarrageTimer;
	uint32 ArcaneVolleyTimer;
	uint32 EnrageTimer;
	uint8 PlanarShiftPhase;

	uint32 HeightEnrageTimer;

	bool Aggroed;
	bool Moving;

	void Reset()
	{
		ArcaneBarrageTimer = 2000;
		ArcaneVolleyTimer = 8000;
		EnrageTimer = urand(15000,20000);
		PlanarShiftPhase = 0;
		HeightEnrageTimer = 5000;
		Moving = false;
	}

	void JustDied(Unit* pKiller)
	{
		if(m_pInstance)
			m_pInstance->SetData(TYPE_EREGOS,DONE);
	}

	void Aggro(Unit*)
	{
		Aggroed = true;
	}

	void SpellHit(Unit* pCaster, const SpellEntry* spell)
	{
	}

	void UpdateAI(const uint32 diff)
	{
		if(!m_creature->getVictim() || !m_creature->SelectHostileTarget())
		{
			if(Aggroed)
			{
				Aggroed = false;
				EnterEvadeMode();
			}
			return;
		}

		if(!(m_creature->getVictim()->IsWithinDistInMap(m_creature,40.0f))) ///Nefachá, ale jak je to kurva možné?
		{
			if(!Moving)
			{
				m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
				Moving = true;
			}
		}
		else
		{
			if(Moving)
			{
				m_creature->GetMotionMaster()->MoveIdle();
				Moving = false;
			}
		}

		if(m_creature->GetPositionZ() < 645.0f)
		{
			if(HeightEnrageTimer <= diff)
			{
				DoCast(m_creature->getVictim(),5,true);
				DoCast(m_creature,23257,true);
				HeightEnrageTimer = 5000;
			} else HeightEnrageTimer -= diff;
		}
		else
			HeightEnrageTimer = 5000;

		if(ArcaneBarrageTimer <= diff)
		{
			if(DoCastSpellIfCan(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0),m_bIsRegularMode?SPELL_ARCANE_BARRAGE:SPELL_ARCANE_BARRAGE_H) == CAST_OK)
				ArcaneBarrageTimer = urand(2000,3000);
		} else ArcaneBarrageTimer -= diff;

		if(ArcaneVolleyTimer <= diff)
		{
			if(DoCastSpellIfCan(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0),m_bIsRegularMode?SPELL_ARCANE_VOLLEY:SPELL_ARCANE_VOLLEY_H) == CAST_OK)
				ArcaneVolleyTimer = urand(5000,6000);
		} else ArcaneVolleyTimer -= diff;

		if(EnrageTimer <= diff)
		{
			DoCast(m_creature,SPELL_ENRAGE,true);
			EnrageTimer = urand(20000,25000);
		} else EnrageTimer -= diff;
	}
};

CreatureAI* GetAI_boss_eregos(Creature* pCreature)
{
	return new boss_eregosAI(pCreature);
}

void AddSC_boss_eregos()
{
	Script* newscript;

	newscript = new Script;
	newscript->Name = "boss_eregos";
	newscript->GetAI = &GetAI_boss_eregos;
	newscript->RegisterSelf();
}

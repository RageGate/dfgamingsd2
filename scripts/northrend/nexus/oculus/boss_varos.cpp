// the Oculus: Varos Cloudstrider
// by Oj  /dev/ FallenangelX
 
#include "precompiled.h"
 
// ############################### Varos Cloudstrider

enum
{
	SPELL_ENERGIZE_CORES                   = 50785,
	H_SPELL_ENERGIZE_CORES                 = 59372,
	SPELL_ENERGIZE_CORES_VISUAL            = 61407,
 
	SAY_INTRO                              = -1595013,
	SAY_AIRSTRIKE_1                        = -1595014,
	SAY_AIRSTRIKE_2                        = -1595015,
	SAY_AIRSTRIKE_3                        = -1595016,
	EMOTE_AIRSTRIKE                        = -1595018,
	SAY_DEATH                              = -1595017
};
 
#define X   1286
#define Y   1070
#define Z   440
#define R   40
 
struct MANGOS_DLL_DECL boss_varos_cloudstriderAI : public ScriptedAI
{
	boss_varos_cloudstriderAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
		Reset();
	}
 
	ScriptedInstance* m_pInstance;
	bool m_bIsRegularMode;
 
	uint32 AzureRingCaptain_Timer;
	uint32 AzureRingCaptain_Cooldown;
	uint32 EnergizeCoresVisual_Timer;
	uint32 CastEnergizeCoresVisual_Timer;
	uint32 EnergizeCores_Timer;
	uint32 BackToFight_Timer;
	uint32 direction_number;
	bool Intro;
	bool AzureRingCaptain_80;
	bool AzureRingCaptain_60;
	bool AzureRingCaptain_40;
	bool AzureRingCaptain_20;
	Unit* pAggro;
	Creature* pDummyAggro;
 
	void Reset()
	{
		Intro = false;
		AzureRingCaptain_Timer = 30000;
		AzureRingCaptain_Cooldown = 0;
		EnergizeCoresVisual_Timer = 6000;
		direction_number = 1;
		CastEnergizeCoresVisual_Timer = 20000;
		EnergizeCores_Timer = 20000;
		BackToFight_Timer = 20000;
		AzureRingCaptain_80 = false;
		AzureRingCaptain_60 = false;
		AzureRingCaptain_40 = false;
		AzureRingCaptain_20 = false;
		pAggro = NULL;
		pDummyAggro = NULL;
	}
 
	void MoveInLineOfSight(Unit *who)
	{
		if (!m_creature->getVictim() && who->isTargetableForAttack() && m_creature->IsHostileTo(who) && who->isInAccessablePlaceFor(m_creature))
		{
			if (!m_creature->canFly() && m_creature->GetDistanceZ(who) > CREATURE_Z_ATTACK_RANGE)
				return;
 
			if (!Intro && m_creature->IsWithinDistInMap(who, m_creature->GetAttackDistance(who)))
			{
				DoScriptText(SAY_INTRO, m_creature);
				Intro = true;
            
				float attackRadius = m_creature->GetAttackDistance(who);
				if (m_creature->IsWithinDistInMap(who, attackRadius) && m_creature->IsWithinLOSInMap(who))
				{
					who->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
					AttackStart(who);
				}
			}
		}
	}
 
	void DamageTaken(Unit* pDoneBy, uint32 &damage)
	{
		if (!Intro)
		{
			DoScriptText(SAY_INTRO, m_creature);
			Intro = true;
		}
	}
 
	void JustDied(Unit* Killer)
	{
		DoScriptText(SAY_DEATH, m_creature);
	}
 
	void SpellHitTarget(Unit* target, const SpellEntry* spell)
	{
		if (spell->Id == 56251 && target->GetTypeId() == TYPEID_PLAYER)
		{
			target->RemoveAurasDueToSpell(56251);
		}
	}
        
	void CallAzureRingCaptain()
	{
		DoScriptText(EMOTE_AIRSTRIKE, m_creature);
		switch(urand(0, 2))
		{
			default: DoScriptText(SAY_AIRSTRIKE_1, m_creature); break;
			case 1: DoScriptText(SAY_AIRSTRIKE_2, m_creature); break;
			case 2: DoScriptText(SAY_AIRSTRIKE_3, m_creature); break;
		}
		float y, z;
		y = Y + 45;
		z = Z + 35;
		m_creature->SummonCreature(28236, X, y, z, 2, TEMPSUMMON_DEAD_DESPAWN, 15000);
		AzureRingCaptain_Timer = 30000;
		AzureRingCaptain_Cooldown = 13000;
	}
 
	void SpawnDummyAggro(uint32 direction)
	{
		pDummyAggro = NULL;
		pAggro = m_creature->getVictim();
        //      DoStopAttack();
		switch(direction)
		{
			default: pDummyAggro = m_creature->SummonCreature(10002, X, (Y + R), Z, 2, TEMPSUMMON_TIMED_DESPAWN, 1000); break;
			case 2: pDummyAggro = m_creature->SummonCreature(10002, (X - R), Y, Z, 2, TEMPSUMMON_TIMED_DESPAWN, 1000); break;
			case 3: pDummyAggro = m_creature->SummonCreature(10002, X, (Y - R), Z, 2, TEMPSUMMON_TIMED_DESPAWN, 1000); break;
			case 4: pDummyAggro = m_creature->SummonCreature(10002, (X + R), Y, Z, 2, TEMPSUMMON_TIMED_DESPAWN, 1000); break;
		}
		if (pDummyAggro)
		{
			m_creature->AddThreat(pDummyAggro, 1000000);
			//      pDummyAggro->MonsterSay("dummy aggro ok", LANG_UNIVERSAL, 0);
		}
	}
 
	void UpdateAI(const uint32 diff)
	{
		if (!m_creature->SelectHostileTarget() || (m_creature->getVictim() && m_creature->getVictim()->GetEntry() == 28183))
		{
			if (Intro)
			{
				EnterEvadeMode();
			}
			return;
		}
 
		if (CastEnergizeCoresVisual_Timer < diff)
		{
			DoCastSpellIfCan(m_creature, SPELL_ENERGIZE_CORES_VISUAL, true);
			if (pDummyAggro)
			{
				m_creature->DealDamage(pDummyAggro, pDummyAggro->GetHealth(), 0, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, 0, false);
				pDummyAggro = NULL;
			}
			CastEnergizeCoresVisual_Timer = 20000;
		} else CastEnergizeCoresVisual_Timer -= diff;
 
		if (BackToFight_Timer < diff)
		{
			if (pAggro)
				AttackStart(pAggro);
			BackToFight_Timer = 20000;
		} else BackToFight_Timer -= diff; 
 
		if (EnergizeCoresVisual_Timer < diff)
		{
			SpawnDummyAggro(direction_number);
			CastEnergizeCoresVisual_Timer = 1;
			EnergizeCores_Timer = m_bIsRegularMode ? 4000 : 4500;
			BackToFight_Timer = 1000;
			EnergizeCoresVisual_Timer = 20000;
		} else EnergizeCoresVisual_Timer -= diff;
 
		if (EnergizeCores_Timer < diff)
		{
			SpawnDummyAggro(direction_number);
			(direction_number < 4) ? direction_number++ : direction_number = 1;
			DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_ENERGIZE_CORES : H_SPELL_ENERGIZE_CORES);
			EnergizeCoresVisual_Timer = m_bIsRegularMode ? 1500 : 1000;
			BackToFight_Timer = (m_bIsRegularMode ? 1001 : 501);
			EnergizeCores_Timer = 20000;
		} else EnergizeCores_Timer -= diff;
 
		if (AzureRingCaptain_Cooldown < diff)
		{
			if (!AzureRingCaptain_80 && m_creature->GetHealthPercent() < 80.0f)
			{
				CallAzureRingCaptain();
				AzureRingCaptain_80 = true;
				return;
			}
			if (!AzureRingCaptain_60 && m_creature->GetHealthPercent() < 60.0f)
			{
				CallAzureRingCaptain();
				AzureRingCaptain_60 = true;
				return;
			}
			if (!AzureRingCaptain_40 && m_creature->GetHealthPercent() < 40.0f)
			{
				CallAzureRingCaptain();
				AzureRingCaptain_40 = true;
				return;
			}
			if (!AzureRingCaptain_20 && m_creature->GetHealthPercent() < 20.0f)
			{
				CallAzureRingCaptain();
				AzureRingCaptain_20 = true;
				return;
			}
			if (AzureRingCaptain_Timer < diff)
			{
				CallAzureRingCaptain();
			} else AzureRingCaptain_Timer -= diff;
		} else AzureRingCaptain_Cooldown -= diff;
 
		DoMeleeAttackIfReady();
	}
};
 
CreatureAI* GetAI_boss_varos_cloudstrider(Creature* pCreature)
{
	return new boss_varos_cloudstriderAI(pCreature);
}
 
// ############################################## Centrifuge Core _OK
struct MANGOS_DLL_DECL mob_centrifuge_core_varosAI : public ScriptedAI
{
	mob_centrifuge_core_varosAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
		Reset();
	}
 
	ScriptedInstance* m_pInstance;
	bool m_bIsRegularMode;
 
	void DamageTaken(Unit* pDoneBy, uint32 &damage)
	{
		damage = 0;
	}
 
	void Reset()
	{
		m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
	}
 
	void UpdateAI(const uint32 diff)
	{
		EnterEvadeMode();
	}
};
 
CreatureAI* GetAI_mob_centrifuge_core_varos(Creature* pCreature)
{
	return new mob_centrifuge_core_varosAI(pCreature);
}
 
// ###################################### Azure Ring Captain _OK
 
#define SPELL_ARCANE_BEAM_VISUAL                        51024
 
struct MANGOS_DLL_DECL mob_azure_ring_captain_varosAI : public ScriptedAI
{
	mob_azure_ring_captain_varosAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
		Reset();
	}
 
	ScriptedInstance* m_pInstance;
	bool m_bIsRegularMode;
 
	uint32 ArcaneBeam_Timer;
	uint32 SelfDestroy_Timer;
	bool Arcane_Beam;
	bool Self_Destroy;
	Unit* Dummy;
 
	void Reset()
	{
		m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
		Arcane_Beam = false;
		Self_Destroy = false;
		ArcaneBeam_Timer = 4500;
		SelfDestroy_Timer = 15000;
		Dummy = m_creature->SummonCreature(28239, X, Y, Z, 1, TEMPSUMMON_DEAD_DESPAWN, 15000);
		m_creature->GetMotionMaster()->MovePoint(0, (m_creature->GetPositionX()), (m_creature->GetPositionY()), (m_creature->GetPositionZ()));
	}
 
	void UpdateAI(const uint32 diff)
	{
		if (!Arcane_Beam)
		{
			if (ArcaneBeam_Timer < diff)
			{
				DoCastSpellIfCan(Dummy, SPELL_ARCANE_BEAM_VISUAL);
				Arcane_Beam = true;
			} else ArcaneBeam_Timer -= diff;
		}

		if (!Self_Destroy)
		{
			if (SelfDestroy_Timer < diff)
			{
				m_creature->SetVisibility(VISIBILITY_OFF);
				m_creature->DealDamage(m_creature, m_creature->GetHealth(), 0, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, 0, false);
				Self_Destroy = true;
			} else SelfDestroy_Timer -= diff;
		}
	}
};
 
CreatureAI* GetAI_mob_azure_ring_captain_varos(Creature* pCreature)
{
	return new mob_azure_ring_captain_varosAI(pCreature);
}
 
// #################################### Laser Dummy Target _OK
 
#define SPELL_ARCANE_BEAM_PERIODIC              51019
 
#define SPELL_AMPLIFY_MAGIC                     51054
#define H_SPELL_AMPLIFY_MAGIC                   59371
 
 
struct MANGOS_DLL_DECL mob_laser_dummy_varosAI : public ScriptedAI
{
	mob_laser_dummy_varosAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
		Reset();
    }
 
	ScriptedInstance* m_pInstance;
	bool m_bIsRegularMode;
 
	uint32 ArcaneBeam_Timer;
	uint32 SelfDestroy_Timer;
	bool Arcane_Beam;
	bool Self_Destroy;
 
	void Reset()
	{
		m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		Arcane_Beam = false;
		Self_Destroy = false;
		ArcaneBeam_Timer = 5000;
		SelfDestroy_Timer = 15000;
		//      m_creature->MonsterSay("Dummy_reset_ok", LANG_UNIVERSAL, 0);
	}
 
	void MoveInLineOfSight(Unit *who)
	{
		if (!m_creature->getVictim() && who->isTargetableForAttack() && m_creature->IsHostileTo(who) && who->isInAccessablePlaceFor(m_creature))
		{
			if (!m_creature->canFly() && m_creature->GetDistanceZ(who) > CREATURE_Z_ATTACK_RANGE)
				return;
 
			float attackRadius = m_creature->GetAttackDistance(who);
			if (m_creature->IsWithinDistInMap(who, attackRadius) && m_creature->IsWithinLOSInMap(who))
			{
				who->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
				DoCastSpellIfCan(who, m_bIsRegularMode ? SPELL_AMPLIFY_MAGIC : H_SPELL_AMPLIFY_MAGIC);
				AttackStart(who);
			}
		}
	}
 
	void UpdateAI(const uint32 diff)
	{
		if (!Arcane_Beam)
		{
			if (ArcaneBeam_Timer < diff)
			{
				m_creature->SetSpeedRate(MOVE_RUN, 0.35f); // unsure about the speed
				DoCastSpellIfCan(m_creature, SPELL_ARCANE_BEAM_PERIODIC);
				Arcane_Beam = true;
			} else ArcaneBeam_Timer -= diff;
		}

		if (!Self_Destroy)
		{
			if (SelfDestroy_Timer < diff)
			{
				m_creature->DealDamage(m_creature, m_creature->GetHealth(), 0, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, 0, false);
				Self_Destroy = true;
			} else SelfDestroy_Timer -= diff;
		}
	}
};
 
CreatureAI* GetAI_mob_laser_dummy_varos(Creature* pCreature)
{
	return new mob_laser_dummy_varosAI(pCreature);
}
 
// ######## add scripts
void AddSC_boss_varos()
{
	Script *newscript;
 
	newscript = new Script;
	newscript->Name = "boss_varos_cloudstrider";
	newscript->GetAI = &GetAI_boss_varos_cloudstrider;
	newscript->RegisterSelf();
 
	newscript = new Script;
	newscript->Name = "mob_centrifuge_core_varos";
	newscript->GetAI = &GetAI_mob_centrifuge_core_varos;
	newscript->RegisterSelf();
 
	newscript = new Script;
	newscript->Name = "mob_azure_ring_captain_varos";
	newscript->GetAI = &GetAI_mob_azure_ring_captain_varos;
	newscript->RegisterSelf();
 
	newscript = new Script;
	newscript->Name = "mob_laser_dummy_varos";
	newscript->GetAI = &GetAI_mob_laser_dummy_varos;
	newscript->RegisterSelf();
}

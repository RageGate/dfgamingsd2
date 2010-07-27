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
SDName: boss_anetheron
SD%Complete: 
SDComment:
SDCategory: Caverns of Time, Mount Hyjal
EndScriptData */

#include "precompiled.h"
#include "hyjal.h"

#define SP_AnetheronSleep        31298

enum
{
    // Yells: You are defenders of a doomed world. Flee here and perhaps you will prolong your pathetic lives. 
	SAY_AGGRO		= 10977,
    // Yells: The swarm is eager to feed.
	SAY_SWARM		= 10979,
    // Yells: Pestilence upon you! 
	SAY_SWARM2		= 11037,
    // Yells: You look tired.
	SAY_SLEEP		= 10978,
    // Yells: Sweet dreams.
	SAY_SLEEP2		= 11545,
    // Yells: Scream for me.
	SAY_SLAY		= 11038,
    // Yells: Pity, no time for a slow death.
	SAY_SLAY2		= 11039,
    // Yells: Your hopes, are lost. 
	SAY_SLAY3		= 10981,
    // Yells: Earth and Sky shall burn!
	SAY_INFERNO		= 11036,
    // Yells: Let fire rain from above! 
	SAY_INFERNO2	= 10980,
    // Yells: The clock... is still... ticking. 
	SAY_DEATH		= 10982,

	// Anetheron Spells
	SPELL_CARRION_SWARM     = 31306,
	SPELL_SLEEP             = 31298,  // affect all raid
	SPELL_VAMPIRIC_AURA     = 38196,  // working
	SPELL_INFERNO           = 31299,  // Summon Entry 17818
	SPELL_ENRAGE			= 26662,  // rage id
	SPELL_STUN				= 24647,  // - 2secs stun

	// Towering Infernal Spells
	SPELL_IMMOLATION        = 31304, //31303,   // 31304     
};

class MANGOS_DLL_DECL AnetheronSleep : public Aura
{
    public:
        AnetheronSleep(const SpellEntry *spell, SpellEffectIndex eff, int32 *bp, Unit *target, Unit *caster) : Aura(spell, eff, bp, target, caster, NULL)
            {}
};

struct MANGOS_DLL_DECL boss_anetheronAI : public ScriptedAI
{
	boss_anetheronAI(Creature *c) : ScriptedAI(c)
	{
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
        Reset();
    }
	
	ScriptedInstance* pInstance;

	uint32 CarrionSwarmTimer;
    uint32 SleepTimer;
    uint32 VampiricAuraTimer;
    uint32 InfernoTimer;
    uint32 EnrageTimer;

	bool Enraged;

	void Reset()
    {
        if (pInstance)
            pInstance->SetData(TYPE_ANETHERON, NOT_STARTED);

		CarrionSwarmTimer = 15000; // blizzlike
		SleepTimer = 35000+rand()%10000; // yotuube?
		VampiricAuraTimer = 20000+rand()%5000; // youtube?
		InfernoTimer = 60000; // blizzlike

		EnrageTimer = 10*60*1000; // blizzlike
		Enraged = false;
	}

	void Aggro(Unit *who)
    {
        DoPlaySoundToSet(m_creature, SAY_AGGRO);

		m_creature->SetInCombatWithZone();

		if (pInstance)
		{
            pInstance->SetData(TYPE_ANETHERON, IN_PROGRESS);
			error_log("Debug: Anetheron event IN_PROGRESS zainicjowany przez %u", who->GetGUID()); // usunac jesli wszystko bedzie OK
		}
    }

	void KilledUnit(Unit *victim)
    {
		switch (rand()%3)
		{
			case 0: DoPlaySoundToSet(m_creature, SAY_SLAY); break;
			case 1: DoPlaySoundToSet(m_creature, SAY_SLAY2); break;
			case 2: DoPlaySoundToSet(m_creature, SAY_SLAY3); break;
		}
    }

	void JustDied(Unit *victim)
    {
        DoPlaySoundToSet(m_creature, SAY_DEATH);

		if (pInstance)
		{
				pInstance->SetData(TYPE_ANETHERON, DONE); 
				error_log("Debug: Anetheron - event DONE");											// usunac jesli wszystko bedzie OK
		}
    }

    void CastSleep()
    {
        SpellEntry *spellInfo = (SpellEntry *)GetSpellStore()->LookupEntry(SPELL_SLEEP);
        if (spellInfo)
            for(uint8 j=0; j<3; ++j)
			{
                if(Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
				{
					for(uint32 i=0 ;i<3; ++i)
					{
						uint8 eff = spellInfo->Effect[i];
						if (eff>=TOTAL_SPELL_EFFECTS)
							continue;
						SpellEntry const *sp;
                        int bp;
						sp = (SpellEntry *)GetSpellStore()->LookupEntry(SP_AnetheronSleep);
                        bp = 8;
					    if(!target->HasAura(SP_AnetheronSleep, EFFECT_INDEX_0))
						    target->AddAura(new AnetheronSleep(sp, EFFECT_INDEX_0, &bp, target, target));
					}
				}
			}
    }

	void UpdateAI(const uint32 diff)
	{
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() )
			return;

		if(CarrionSwarmTimer < diff)
        {
			switch (rand()%2)
			{
				case 0: DoPlaySoundToSet(m_creature, SAY_SWARM); break;
				case 1: DoPlaySoundToSet(m_creature, SAY_SWARM2); break;
			}
			Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
			if(target)
			{
				m_creature->CastSpell(target, SPELL_CARRION_SWARM, false);
				CarrionSwarmTimer = 15000;
			}
        }CarrionSwarmTimer -= diff;


		if(SleepTimer < diff)
		{
			switch (rand()%2)
			{
				case 0: DoPlaySoundToSet(m_creature, SAY_SLEEP); break;
				case 1: DoPlaySoundToSet(m_creature, SAY_SLEEP2); break;
			}
			CastSleep();
			SleepTimer = 15000+rand()%10000;													//youtube timer
		}SleepTimer -= diff;

        if(VampiricAuraTimer < diff)
		{
			DoCastSpellIfCan(m_creature, SPELL_VAMPIRIC_AURA); 
			VampiricAuraTimer = 10000+rand()%5000;                                              //youtube timer
		}VampiricAuraTimer -= diff;

		if(InfernoTimer < diff)
		{
			switch (rand()%2)
			{
				case 0: DoPlaySoundToSet(m_creature, SAY_INFERNO); break;
				case 1: DoPlaySoundToSet(m_creature, SAY_INFERNO2); break;
			}
			Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
			if(target)
			{
				target->CastSpell(target, SPELL_STUN, true);
				m_creature->CastSpell(target, SPELL_INFERNO, true);
				InfernoTimer = 60000;
			}
		}InfernoTimer -= diff;

		if(!Enraged)
			if(EnrageTimer < diff)
			{
				DoCastSpellIfCan(m_creature, SPELL_ENRAGE);
				Enraged = true;
			}EnrageTimer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_boss_anetheron(Creature *_Creature)
{
    return new boss_anetheronAI (_Creature);
}

/*######
## Towering Infernal
######*/
                                   
struct MANGOS_DLL_DECL mob_towering_infernalAI : public ScriptedAI
{
    mob_towering_infernalAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 Immolation_Timer;
	bool Immolation;

    void Reset()
    {
        uint32 Immolation_Timer = 1000;
		Immolation = false;
    }

    void Aggro(Unit* who) {}

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if(!Immolation)
		{
			DoCastSpellIfCan(m_creature, SPELL_IMMOLATION);
			Immolation = true;
		}

		if(Immolation_Timer < diff)
		{
			if (Creature* Anetheron = (Creature*)Unit::GetUnit(*m_creature, m_pInstance->GetData64(DATA_ANETHERON)))
				if (m_creature->IsWithinDistInMap(Anetheron, 10.0f) && Anetheron->isAlive() && Anetheron->HasAura(SPELL_VAMPIRIC_AURA))
					DoCastSpellIfCan(m_creature, SPELL_VAMPIRIC_AURA);
			Immolation_Timer = 1000;
		}Immolation_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_towering_infernal(Creature* pCreature)
{
    return new mob_towering_infernalAI(pCreature);
}

void AddSC_boss_anetheron()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_anetheron";
    newscript->GetAI = &GetAI_boss_anetheron;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "mob_towering_infernal";
    newscript->GetAI = &GetAI_mob_towering_infernal;
    newscript->RegisterSelf();
}
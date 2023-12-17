/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
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
SDName: Elwynn_Forest
SD%Complete: 0
SDComment: Placeholder
SDCategory: Elwynn Forest
EndScriptData */

/* ContentData
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/BaseAI/CreatureAI.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{

	SPELL_NIBLE_REFLEXES = 6433,                 // removed after phase 1
	SPELL_SMITE_STOMP = 6432,
	SPELL_FLAME_BREATH = 9573,
	SPELL_TAIL_SWIPE = 15847,

	PHASE_1 = 1,
	PHASE_2 = 2,
	PHASE_3 = 3,
};


struct hoggerAI : public ScriptedAI
{
	hoggerAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

	uint32 m_uiResetTimer;
	// Let's do phases
	uint32 m_uiPhase;
    uint32 m_uiTransformTimer;
	uint32 m_uiBreathTimer;

	void Reset() override 
	{
		// Reset phase
		m_uiPhase = PHASE_1;
	}

	// Not used
	void DetectPlayerBehind(Player* pPlayer) {
		// Check if player is behind for Tail Swipe
		// Lets calculate their position
		// Player Coordinates
		float fPlayerX = pPlayer->GetPositionX();
		float fPlayerY = pPlayer->GetPositionY();
		float fPlayerZ = pPlayer->GetPositionZ();
		// Creature Coordinates
		float fCreatureX = m_creature->GetPositionX();
		float fCreatureY = m_creature->GetPositionY();
		float fCreatureZ = m_creature->GetPositionZ();

		float fPlayerAngle = atan2(fPlayerY - fCreatureY, fPlayerX - fCreatureX);
		float fPlayerAngleDegrees = fPlayerAngle * 180 / M_PI;
		float fCreatureAngle = m_creature->GetOrientation();
		float fCreatureAngleDegrees = fCreatureAngle * 180 / M_PI;
		float fAngleDifference = fCreatureAngleDegrees - fPlayerAngleDegrees;
		if (fAngleDifference < 0) {
			fAngleDifference = fAngleDifference + 360;
		}
		if (fAngleDifference > 180) {
			fAngleDifference = fAngleDifference - 360;
		}
		if (fAngleDifference < 0) {
			fAngleDifference = fAngleDifference * -1;
		}
		if (fAngleDifference > 90) {
			DoCastSpellIfCan(m_creature, SPELL_TAIL_SWIPE);
		}
	}

	void WatchAndChangeToPhase2() {
		//while (m_creature->GetHealthPercent() < 66.0f) {
		if (m_creature->GetHealthPercent() < 66.0f) {
			DoCastSpellIfCan(m_creature, SPELL_NIBLE_REFLEXES, CAST_AURA_NOT_PRESENT);
			if (m_creature->GetHealthPercent() < 20.0f) {
				m_uiPhase = PHASE_2;
			}
		}
	}

	bool TransformAndChangeToPhase3()
	{
		if (m_creature->GetHealthPercent() < 20.0f) {
			m_creature->SetDisplayId(6374);
			m_creature->SetMaxHealth(2000);
			m_creature->SetLevel(15);
			m_uiPhase = PHASE_3;
			return true;
		}
		return false;
	}

	void UpdateAI(const uint32 uiDiff) override
	{
		switch (m_uiPhase)
		{
			case PHASE_1: {
				WatchAndChangeToPhase2();
			}
			case PHASE_2: {
				if (m_creature->GetHealthPercent() < 20.0f) {
					if (DoCastSpellIfCan(m_creature, SPELL_SMITE_STOMP, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT) == CAST_OK) {
						m_creature->RemoveAurasDueToSpell(SPELL_NIBLE_REFLEXES);
						TransformAndChangeToPhase3();
					}
				}
			}
			case PHASE_3: {
				if (TransformAndChangeToPhase3() == true) {
					if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FLAME_BREATH) == CAST_OK) {
						DoCastSpellIfCan(nullptr, SPELL_TAIL_SWIPE);
					}
				}
			}
		}
		DoMeleeAttackIfReady();
	}
};

UnitAI* GetAI_Hogger(Creature* pCreature)
{
    return new hoggerAI(pCreature);
}


void AddSC_elwynn_forest()
{
	Script * pNewScript;

	pNewScript = new Script;
	pNewScript->Name = "npc_hogger";
    pNewScript->GetAI = &GetAI_Hogger;
    pNewScript->RegisterSelf();
}

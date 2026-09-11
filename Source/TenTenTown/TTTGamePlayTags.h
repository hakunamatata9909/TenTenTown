// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "TTTGamePlayTags.h"
#include "NativeGameplayTags.h"

namespace GASTAG
{
	//어빌리티
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Movement)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Movement_Jump)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Movement_Dash)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Movement_Sprint)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Fireball)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Fighter_Ultimate)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Fighter_UltimateAttack)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Fighter_Whirlwind)
	//이벤트
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Jump_Cost)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_LevelUP)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Fireball_Charging)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Fireball_Release)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Fighter_UltimateRelease)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Fighter_UltimateReleaseEnd)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Fighter_ComboStart)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Fighter_ComboEnd)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Confirm)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Cancel)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Fighter_Attack)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Fighter_WhirlwindAttack)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Character_Dead)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Character_Revive)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Archer_SkillBFire)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Archer_UltLocationConfirm)
	//스테이트
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Movement_IsJumping)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Movement_IsDoubleJumping)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Movement_IsDashing)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Movement_IsSprinting)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_IsChanelling)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_IsSelecting)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_UltimateOnGoing)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Block_Movement)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Block_Everything)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Invulnerable)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Fighter_Dizzy)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Character_Combat)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Character_Dead)
	//데이터
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Jump_Stamina)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Damage)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Enemy_Damage)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Duration)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Character_EXP)
	//게임플레이 큐
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Jump)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Dash)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Fireball_Charging)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Fireball_Cast)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Fireball_Explode)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Fighter_Ultimate)
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Enemy_Sound_Attack)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Enemy_Sound_Projectile)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Enemy_Sound_Burrow)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Enemy_Effect_Attack)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Enemy_Effect_Projectile)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Enemy_Effect_Burrow)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Enemy_Effect_Buffed)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Enemy_Effect_ExplodeSelf)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Enemy_Effect_Berserk)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Enemy_Effect_Guard)

	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Fighter_PunchRight)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Fighter_PunchLeft)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Fighter_PunchWhirlWind)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Fighter_PunchHit)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Fighter_PunchWhirlWindHit)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Fighter_KickWind)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Fighter_KickWindMiddle)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Fighter_WhirlWind)
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Archer_NormalAttackStart)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Archer_NormalAttackRelease)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Archer_SkillA)
	//쿨다운
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Dash)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Fireball)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Fighter_Ultimate)
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Enemy_Skill)

	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Fighter_Whirlwind)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Archer_NormalAttack)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Archer_SkillA)
	
	// ----- 구조물 -----
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Structure_Crossbow)

	// ----- [빌드 모드] -----
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_BuildMode)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Build_SelectStructure_1)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Build_SelectStructure_2)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Build_SelectStructure_3)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Build_SelectStructure_4)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Build_SelectStructure_5)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Build_SelectStructure_6)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Build_SelectStructure_7)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Build_SelectStructure_8)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Build_Upgrade)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Build_Sell)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Build_Repair)
	// ----------------------

	// ----- [데이터] -----
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Structure_SlowMagnitude)
	// ----------------------
	
	// ----- [게임 플레이 큐] -----
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Structure_Build)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Structure_Destroy)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Structure_Upgrade)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Structure_Crossbow_Fire)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Structure_IceTrap_Active)
	// --------------------------
	
	// --Enemy

	// ---Type
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Type)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Type_Orc)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Type_Dragon)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Type_Worm)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Type_Beholder)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Type_DemonKing)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Type_NagaWizard)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Type_BlackKnight)

	// ---State
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_State_Combat)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_State_Move)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_State_Dead)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_State_KnockBack)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_State_Burrowed)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_State_Berserk)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_State_Buffed)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_State_Counter)
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_State_Casting)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_State_Teleporting)

	// ---Ability
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Ability_Attack)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Ability_Dead)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Ability_Attack_Melee)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Ability_Attack_Range)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Ability_Attack_ExplodeSelf)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Ability_Burrow)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Ability_Berserk)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Ability_BuffNearBy)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Ability_Teleport)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Ability_Counter)


	// --- UI State Tags (UI 상태) ---
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_State_CharacterSelectOpen)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_State_MapSelectOpen)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_State_ShopOpen)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_State_ResultOpen)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_State_PingOpen)

	// --- Mode State Tags (레벨/모드 상태) ---
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Mode_Lobby)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Mode_Gameplay)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Role_Host)
	
}

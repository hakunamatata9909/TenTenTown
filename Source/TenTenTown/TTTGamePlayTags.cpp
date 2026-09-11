// Fill out your copyright notice in the Description page of Project Settings.

#include "TTTGamePlayTags.h"

//어빌리티
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Ability_Movement,"Ability.Movement")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Ability_Movement_Jump,"Ability.Movement.Jump")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Ability_Movement_Dash,"Ability.Movement.Dash")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Ability_Movement_Sprint,"Ability.Movement.Sprint")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Ability_Fireball,"Ability.Fireball")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Ability_Fighter_Ultimate,"Ability.Fighter.Ultimate")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Ability_Fighter_UltimateAttack,"Ability.Fighter.UltimateAttack")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Ability_Fighter_Whirlwind,"Ability.Fighter.Whirlwind")
//이벤트
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Jump_Cost,"Event.Jump.Cost")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_LevelUP,"Event.LevelUP")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Fireball_Charging,"Event.Fireball.Charging")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Fireball_Release,"Event.Fireball.Release")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Fighter_UltimateRelease,"Event.Fighter.UltimateRelease")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Fighter_UltimateReleaseEnd,"Event.Fighter.UltimateReleaseEnd")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Fighter_ComboStart,"Event.Fighter.ComboStart")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Fighter_ComboEnd,"Event.Fighter.ComboEnd")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Confirm, "Event.Selection.Confirm");
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Cancel,  "Event.Selection.Cancel");
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Fighter_Attack,"Event.Fighter.Attack")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Fighter_WhirlwindAttack,"Event.Fighter.WhirlwindAttack")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Character_Dead,"Event.Character.Dead")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Character_Revive,"Event.Character.Revive")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Archer_SkillBFire,"Event.Archer.SkillBFire")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Archer_UltLocationConfirm,"Event.Archer_UltLocationConfirm")

//스테이트
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_Movement_IsJumping,"State.Movement.IsJumping")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_Movement_IsDoubleJumping,"State.Movement.IsDoubleJumping")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_Movement_IsDashing,"State.Movement.IsDashing")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_Movement_IsSprinting,"State.Movement.IsSprinting")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_IsChanelling,"State.IsChannelling")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_IsSelecting, "State.IsSelecting");
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_UltimateOnGoing,"State.Fighter.UltimateOnGoing")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_Block_Movement,"State.Block.Movement")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_Block_Everything,"State.Block.Everything")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_Invulnerable,"State.Invulnerable")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_Fighter_Dizzy,"State.Fighter.Dizzy")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_Character_Combat,"State.Character.Combat")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_Character_Dead,"State.Character.Dead")
//데이터
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Data_Jump_Stamina,"Data.Jump.Stamina")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Data_Enemy_Damage,"Data.Enemy.Damage")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Data_Damage,"Data.Damage")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Data_Duration,"Data.Duration")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Data_Character_EXP,"Data.Character.EXP")
//게임 플레이 큐
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Jump,"GameplayCue.Jump")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Dash,"GameplayCue.Dash")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Fireball_Charging,"GameplayCue.Fireball.Charging")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Fireball_Cast,"GameplayCue.Fireball.Cast")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Fireball_Explode,"GameplayCue.Fireball.Explode")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Fighter_Ultimate,"GameplayCue.Fighter.Ultimate")

UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Enemy_Sound_Attack,"GameplayCue.Enemy.Sound.Attack")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Enemy_Sound_Projectile,"GameplayCue.Enemy.Sound.Burrow")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Enemy_Sound_Burrow,"GameplayCue.Enemy.Sound.Projectile")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Enemy_Effect_Attack,"GameplayCue.Enemy.Effect.Attack")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Enemy_Effect_Projectile,"GameplayCue.Enemy.Effect.Projectile")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Enemy_Effect_Burrow,"GameplayCue.Enemy.Effect.Burrow")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Enemy_Effect_Buffed,"GameplayCue.Enemy.Effect.Buffed")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Enemy_Effect_ExplodeSelf,"GameplayCue.Enemy.Effect.ExplodeSelf")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Enemy_Effect_Berserk,"GameplayCue.Enemy.Effect.Berserk")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Enemy_Effect_Guard,"GameplayCue.Enemy.Effect.Guard")

UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Fighter_PunchRight,"GameplayCue.Fighter.PunchRight")
UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Fighter_PunchLeft,"GameplayCue.Fighter.PunchLeft")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Fighter_PunchHit,"GameplayCue.Fighter.PunchHit")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Fighter_PunchWhirlWind,"GameplayCue.Fighter.PunchWhirlWind")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Fighter_PunchWhirlWindHit,"GameplayCue.Fighter.PunchWhirlWindHit")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Fighter_KickWind,"GameplayCue.Fighter.KickWind")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Fighter_KickWindMiddle,"GameplayCue.Fighter.KickWindMiddle")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Fighter_WhirlWind,"GameplayCue.Fighter.WhirlWind")

UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Archer_NormalAttackStart,"GameplayCue.Archer.NormalAttackStart")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Archer_NormalAttackRelease,"GameplayCue.Archer.NormalAttackRelease")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Archer_SkillA,"GameplayCue.Archer.SkillA")
//쿨다운
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Cooldown_Dash,"Cooldown.Dash")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Cooldown_Fireball,"Cooldown.Fireball")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Cooldown_Fighter_Ultimate,"Cooldown.Fighter.Ultimate")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Cooldown_Fighter_Whirlwind,"Cooldown.Fighter.Whirlwind")

UE_DEFINE_GAMEPLAY_TAG(GASTAG::Cooldown_Enemy_Skill,"Cooldown.Enemy.Skill")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Cooldown_Archer_NormalAttack,"Cooldown.Archer.NormalAttack")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Cooldown_Archer_SkillA,"Cooldown.Archer.SkillA");

// ---------- 구조물 --------------
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Cooldown_Structure_Crossbow, "Cooldown.Structure.Crossbow")

// 빌드 모드
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_BuildMode, "State.BuildMode");

UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Build_SelectStructure_1, "Event.Build.SelectStructure.1");
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Build_SelectStructure_2, "Event.Build.SelectStructure.2");
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Build_SelectStructure_3, "Event.Build.SelectStructure.3");
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Build_SelectStructure_4, "Event.Build.SelectStructure.4");
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Build_SelectStructure_5, "Event.Build.SelectStructure.5");
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Build_SelectStructure_6, "Event.Build.SelectStructure.6");
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Build_SelectStructure_7, "Event.Build.SelectStructure.7");
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Build_SelectStructure_8, "Event.Build.SelectStructure.8");

UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Build_Upgrade, "Event.Build.Upgrade");
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Build_Sell, "Event.Build.Sell");
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Build_Repair, "Event.Build.Repair");

// 데이터
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Data_Structure_SlowMagnitude, "Data.Structure.SlowMagnitude")

// 게임 플레이 큐
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Structure_Build, "GameplayCue.Structure.Build")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Structure_Destroy, "GameplayCue.Structure.Destroy")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Structure_Upgrade, "GameplayCue.Structure.Upgrade")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Structure_Crossbow_Fire, "GameplayCue.Structure.Crossbow.Fire")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Structure_IceTrap_Active, "GameplayCue.Structure.IceTrap.Active")

// --Enemy
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_Type,"Enemy.Type")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_Type_Orc,"Enemy.Type.Orc")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_Type_Dragon,"Enemy.Type.Dragon")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_Type_Worm,"Enemy.Type.Worm")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_Type_Beholder,"Enemy.Type.Beholder")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_Type_DemonKing,"Enemy.Type.DemonKing")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_Type_NagaWizard,"Enemy.Type.NagaWizard")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_Type_BlackKnight,"Enemy.Type.BlackKnight")

UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_State_Combat,"Enemy.State.Combat")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_State_Move,"Enemy.State.Move")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_State_Dead,"Enemy.State.Dead")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_State_KnockBack,"Enemy.State.KnockBack")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_State_Burrowed,"Enemy.State.Burrowed")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_State_Berserk,"Enemy.State.Berserk")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_State_Buffed,"Enemy.State.Buffed")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_State_Counter,"Enemy.State.Counter")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_State_Casting,"Enemy.State.Casting")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_State_Teleporting,"Enemy.State.Teleporting")


UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_Ability_Attack, "Enemy.Ability.Attack")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_Ability_Dead,"Enemy.Ability.Dead")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_Ability_Attack_Melee,"Enemy.Ability.Attack.Melee")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_Ability_Attack_Range,"Enemy.Ability.Attack.Range")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_Ability_Attack_ExplodeSelf,"Enemy.Ability.Attack.ExplodeSelf")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_Ability_Burrow,"Enemy.Ability.Burrow")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_Ability_Berserk,"Enemy.Ability.Berserk")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_Ability_BuffNearBy,"Enemy.Ability.BuffNearBy")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_Ability_Teleport,"Enemy.Ability.Teleport")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_Ability_Counter,"Enemy.Ability.Counter")


// --- 새로 추가된 UI/모드 태그 구현 ---
UE_DEFINE_GAMEPLAY_TAG(GASTAG::UI_State_CharacterSelectOpen, "UI.State.CharacterSelectOpen");
UE_DEFINE_GAMEPLAY_TAG(GASTAG::UI_State_MapSelectOpen, "UI.State.MapSelectOpen"); 
UE_DEFINE_GAMEPLAY_TAG(GASTAG::UI_State_ShopOpen, "UI.State.ShopOpen");
UE_DEFINE_GAMEPLAY_TAG(GASTAG::UI_State_ResultOpen, "UI.State.ResultOpen");
UE_DEFINE_GAMEPLAY_TAG(GASTAG::UI_State_PingOpen, "UI.State.PingOpen");

UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_Mode_Lobby, "State.Mode.Lobby");
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_Mode_Gameplay, "State.Mode.Gameplay");
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_Role_Host, "State.Role.Host");


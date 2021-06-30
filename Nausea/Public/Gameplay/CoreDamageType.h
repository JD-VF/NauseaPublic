// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "Gameplay/StatusType.h"
#include "CoreDamageType.generated.h"

class AActor;
class UStatusEffectBase;

UENUM(BlueprintType)
enum class EApplicationLogic : uint8
{
	Enemy, //Apply to enemies, but can friendly fire.
	EnemyOnly, //Apply to enemies only. Useful for enemy-only status effects.
	AllyOnly, //Apply to allies only. Useful for ally-only status effects.
	All //Apply to everyone.
};

UENUM(BlueprintType)
enum class EApplicationResult : uint8
{
	Full, //Event is fully applicable to target.
	FriendlyFire, //Event is friendly fire on target.
	None //Event should not apply to target.
};

/**
 * 
 */
UCLASS()
class NAUSEA_API UCoreDamageType : public UDamageType
{
	GENERATED_UCLASS_BODY()
	
public:
	float GetDamageAmount() const { return DamageAmount; }

	float GetWeakpointDamageMultiplier() const { return WeakpointDamageMultiplier; }

	bool ShouldScaleStatusEffectPowerByDamage() const { return bScaleStatusEffectPowerByDamage; }

	bool ShouldScaleStatusEffectPowerByFrameDeltaTime() const { return bScaleStatusEffectPowerByFrameDeltaTime; }

	const TMap<TSoftClassPtr<UStatusEffectBase>, float>& GetStatusEffectMap() const { return StatusEffectMap; }

	const TMap<EStatusType, float>& GetGenericStatusEffectMap() const { return GenericStatusEffectMap; }

	EApplicationLogic GetDamageApplicationLogic() const { return DamageApplicationLogic; }
	EApplicationLogic GetStatusApplicationLogic() const { return StatusApplicationLogic; }

	EApplicationResult GetDamageApplicationResult(AActor* Instigator, AActor* Target) const;
	EApplicationResult GetStatusApplicationResult(AActor* Instigator, AActor* Target) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = StatusEffect)
	EApplicationLogic DamageApplicationLogic = EApplicationLogic::Enemy;
	UPROPERTY(EditDefaultsOnly, Category = StatusEffect)
	EApplicationLogic StatusApplicationLogic = EApplicationLogic::EnemyOnly;

	UPROPERTY(EditDefaultsOnly, Category = StatusEffect)
	float DamageAmount = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = StatusEffect)
	float WeakpointDamageMultiplier = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = StatusEffect)
	bool bScaleStatusEffectPowerByDamage = false;

	//Should this effect power be multiplied by frame delta time?
	UPROPERTY(EditDefaultsOnly, Category = StatusEffect)
	bool bScaleStatusEffectPowerByFrameDeltaTime = false;

	//Specific effect classes and the power that will be applied on hit of this damage type.
	UPROPERTY(EditDefaultsOnly, Category = StatusEffect)
	TMap<TSoftClassPtr<UStatusEffectBase>, float> StatusEffectMap;

	//Generic effect type and the power that will be applied on hit of this damage type. The specific effect class for this status type is dependent on the UStatusComponentConfigObject.
	UPROPERTY(EditDefaultsOnly, Category = StatusEffect)
	TMap<EStatusType, float> GenericStatusEffectMap;
};

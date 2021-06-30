// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Weapon/WeaponTypes.h"
#include "CoreCharacterAnimInstanceTypes.generated.h"

class UAnimSequence;
class UAnimMontage;
class UBlendSpace;

/**
 * 
 */
USTRUCT(BlueprintType)
struct FLocomotionAnimationContainer
{
	GENERATED_USTRUCT_BODY()

	FLocomotionAnimationContainer() {}

public:
	bool IsValid() const { return IdleAnimation != nullptr; }

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	UAnimSequence* IdleAnimation = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	UBlendSpace* WalkBlendSpace = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	UBlendSpace* RunBlendSpace = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	UAnimSequence* CrouchIdleAnimation = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	UBlendSpace* CrouchWalkBlendSpace = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Jumping)
	UAnimSequence* JumpStartAnimation = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Jumping)
	UAnimSequence* JumpLoopAnimation = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Jumping)
	UAnimSequence* JumpEndAnimation = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Jumping)
	UAnimSequence* RunningJumpStartAnimation = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Jumping)
	UAnimSequence* RunningJumpLoopAnimation = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Jumping)
	UAnimSequence* RunningJumpEndAnimation = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Torso)
	UAnimSequence* TorsoIdleAnimation = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Torso)
	UAnimSequence* TorsoWalkAnimation = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Torso)
	UAnimSequence* TorsoRunAnimation = nullptr;
};

USTRUCT(BlueprintType)
struct FWeaponMontagePair
{
	GENERATED_USTRUCT_BODY()

	FWeaponMontagePair() {}

public:
	bool HasPlayerMontage() const { return PlayerMontage != nullptr; }
	bool HasWeaponMontage() const { return WeaponMontage != nullptr; }

	bool IsValid() const { return PlayerMontage != nullptr || WeaponMontage != nullptr; }

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	UAnimMontage* PlayerMontage = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	UAnimMontage* WeaponMontage = nullptr;

	static FWeaponMontagePair InvalidMontagePair;
};

UCLASS(BlueprintType, Blueprintable)
class NAUSEA_API UAnimationObject : public UObject
{
	GENERATED_UCLASS_BODY()

#if WITH_EDITOR
//~ Begin UObject Interface
public:
	virtual void PostEditImport() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
//~ End UObject Interface
protected:
	void UpdateVisibleProperties();
#endif //WITH_EDITOR

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	FLocomotionAnimationContainer LocomontionAnimations;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Animation Set")
	FWeaponMontagePair EquipMontage = FWeaponMontagePair();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Animation Set")
	float EquipMontageAnimRate = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Animation Set")
	FWeaponMontagePair PutDownMontage = FWeaponMontagePair();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Animation Set")
	float PutDownMontageAnimRate = 1.f;

	UFUNCTION(BlueprintCallable, Category = "Weapon Animation Set")
	const FWeaponMontagePair& GetFireMontage(EFireMode FireMode) const;
	UFUNCTION(BlueprintCallable, Category = "Weapon Animation Set")
	float GetFireMontageAnimRate(EFireMode FireMode) const;

	UFUNCTION(BlueprintCallable, Category = "Weapon Animation Set")
	const FWeaponMontagePair& GetReloadMontage(EFireMode FireMode) const;
	UFUNCTION(BlueprintCallable, Category = "Weapon Animation Set")
	float GetReloadMontageAnimRate(EFireMode FireMode) const;

	UFUNCTION(BlueprintCallable, Category = "Weapon Animation Set")
	bool PlayEquipMontage(USkeletalMeshComponent* PlayerMesh, USkeletalMeshComponent* WeaponMesh, float Duration) const;
	UFUNCTION(BlueprintCallable, Category = "Weapon Animation Set")
	bool PlayPutDownMontage(USkeletalMeshComponent* PlayerMesh, USkeletalMeshComponent* WeaponMesh, float Duration) const;
	
	UFUNCTION(BlueprintCallable, Category = "Weapon Animation Set")
	bool PlayFireMontage(USkeletalMeshComponent* PlayerMesh, USkeletalMeshComponent* WeaponMesh, float Duration, EFireMode FireMode) const;
	UFUNCTION(BlueprintCallable, Category = "Weapon Animation Set")
	bool PlayReloadMontage(USkeletalMeshComponent* PlayerMesh, USkeletalMeshComponent* WeaponMesh, float Duration, EFireMode FireMode) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Animation Set")
	TMap<EFireMode, FWeaponMontagePair> FireMontage = TMap<EFireMode, FWeaponMontagePair>();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Animation Set")
	TMap<EFireMode, float> FireMontageAnimRate = TMap<EFireMode, float>();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Animation Set")
	TMap<EFireMode, FWeaponMontagePair> ReloadMontage = TMap<EFireMode, FWeaponMontagePair>();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Animation Set")
	TMap<EFireMode, float> ReloadMontageAnimRate = TMap<EFireMode, float>();

//Looping reloads (made for single reload weapons but can support any kind of looping reload).
public:
	UFUNCTION(BlueprintCallable, Category = "Weapon Animation Set")
	const FWeaponMontagePair& GetLoopingReloadMontage(EFireMode FireMode) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Looping Reload Animation Set")
	bool bHasLoopingReload = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Looping Reload Animation Set", meta = (EditCondition = "bHasLoopingReload", EditConditionHides))
	TMap<EFireMode, FWeaponMontagePair> LoopingReloadMontage = TMap<EFireMode, FWeaponMontagePair>();
};
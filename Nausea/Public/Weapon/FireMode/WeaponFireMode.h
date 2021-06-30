// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/FireMode/ReplicatedFireMode.h"
#include "WeaponFireMode.generated.h"

class UWeaponDamageType;

UENUM(BlueprintType)
enum class EFireType : uint8
{
	SemiAuto,
	Automatic,
	MAX = 255
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FFireModeSpreadUpdateSignature, UWeaponFireMode*, FireMode, float, Spread);

/**
 * 
 */
UCLASS()
class NAUSEA_API UWeaponFireMode : public UReplicatedFireMode
{
	GENERATED_UCLASS_BODY()

//~ Begin FTickableGameObject Interface
protected:
	virtual void Tick(float DeltaTime) override;
public:
	virtual bool IsTickable() const { return !IsPendingKill() && (bTickEnabled || CurrentSpread > 0.f); }
//~ End FTickableGameObject Interface

//~ Begin UFireMode Interface
public:
	virtual void RegisterOwningWeaponClass(const UWeapon* Weapon) override;
	virtual void Initialize(UWeapon* Weapon) override;
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual bool IsFiring_Implementation() const override;
	virtual bool CanPutDown() const override;
	virtual bool BlockAction(const UFireMode* InstigatorFireMode) const override;
	virtual bool CanFire() const override;
	virtual bool Fire(float WorldTimeOverride = -1.f) override;
	virtual void StopFire(float WorldTimeOverride = -1.f) override;
	virtual bool Reload() override;
	virtual bool StopReload() override;
protected:
	virtual void OnFireCosmetic() override;
	virtual void FireComplete() override;
	virtual void BindWeaponEvents() override;
	virtual void UnBindWeaponEvents() override;
	virtual void Client_Reliable_FailedFire_Implementation() override;
//~ End UFireMode Interface

public:
	UFUNCTION(BlueprintCallable, Category = FireMode)
	float GetFireRate() const;

	UFUNCTION(BlueprintCallable, Category = FireMode)
	float GetFireSpread() const;

	UFUNCTION(BlueprintCallable, Category = FireMode)
	TSubclassOf<UWeaponDamageType> GetDamageType() const { return WeaponDamageType; }

	UFUNCTION(BlueprintCallable, Category = FireMode)
	UAmmo* GetAmmo() const { return Ammo; }

	template<class T>
	const T* GetAmmoCDO() const;

	UFUNCTION(BlueprintCallable, Category = FireMode)
	virtual bool CanRefire() const;

	UFUNCTION(BlueprintCallable, Category = FireMode)
	virtual bool CanConsumeAmmo() const;

	UFUNCTION(BlueprintCallable, Category = FireMode)
	virtual float GetDamage() const { return WeaponDamage; }

protected:
	UFUNCTION()
	virtual bool ConsumeAmmo();

	UFUNCTION()
	virtual void ApplyRecoil();

	virtual void UpdateFireCounter() override;

public:
	UPROPERTY(BlueprintAssignable, Category = FireMode)
	FFireModeSpreadUpdateSignature OnFireModeSpreadUpdate;

protected:
	UPROPERTY(EditDefaultsOnly)
	float FireRate = 1.f;
	
	UPROPERTY(EditDefaultsOnly)
	EFireType FireType = EFireType::SemiAuto;

	UPROPERTY(EditDefaultsOnly, Instanced, Replicated)
	class UAmmo* Ammo = nullptr;

	UPROPERTY()
	FTimerHandle FireTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = Recoil)
	FVector2D RecoilStrength;
	UPROPERTY(EditDefaultsOnly, Category = Recoil)
	FVector2D RecoilPitchVariance = FVector2D(0.9f, 1.1f);
	UPROPERTY(EditDefaultsOnly, Category = Recoil)
	FVector2D RecoilYawVariance = FVector2D(-1.f, 1.f);
	UPROPERTY(EditDefaultsOnly, Category = Recoil)
	float RecoilDuration = 0.175f;

	UPROPERTY(EditDefaultsOnly, Category = Spread)
	FVector2D SpreadRange = FVector2D(0.f, 4.f);
	UPROPERTY(EditDefaultsOnly, Category = Spread)
	float SpreadPercentIncreasePerShot = 0.1f;
	UPROPERTY(EditDefaultsOnly, Category = Spread)
	float SpreadDecayRate = 0.5f;
	UPROPERTY(Transient)
	float CurrentSpread = 0.f;
	UPROPERTY(Transient)
	float CurrentSpreadDecayRate = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = Damage)
	TSubclassOf<UWeaponDamageType> WeaponDamageType = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = Damage)
	float WeaponDamage = 0.f;
};

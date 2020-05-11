// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Weapon/FireMode/ReplicatedFireMode.h"
#include "WeaponFireMode.generated.h"

UENUM(BlueprintType)
enum class EFireType : uint8
{
	SemiAuto,
	Automatic,
	MAX = 255
};

/**
 * 
 */
UCLASS()
class NAUSEA_API UWeaponFireMode : public UReplicatedFireMode
{
	GENERATED_UCLASS_BODY()

//~ Begin UFireMode Interface
public:
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual bool IsFiring() const override;
	virtual bool CanFire() const override;
	virtual bool Fire(float WorldTimeOverride = -1.f) override;
	virtual void StopFire(float WorldTimeOverride = -1.f) override;
protected:
	virtual void FireComplete() override;
//~ End UFireMode Interface

public:
	UFUNCTION(BlueprintCallable, Category = FireMode)
	UAmmo* GetAmmo() const { return Ammo; }

	UFUNCTION(BlueprintCallable, Category = FireMode)
	virtual bool CanRefire() const;

	UFUNCTION(BlueprintCallable, Category = FireMode)
	virtual bool CanConsumeAmmo() const;

protected:
	UFUNCTION()
	virtual bool ConsumeAmmo();

	virtual void UpdateFireCounter() override;

protected:
	UPROPERTY(EditDefaultsOnly)
	float FireRate = 1.f;
	
	UPROPERTY(EditDefaultsOnly)
	EFireType FireType = EFireType::SemiAuto;

	UPROPERTY(EditDefaultsOnly)
	float Capacity = 100.f;

	UPROPERTY(EditDefaultsOnly, Instanced, Replicated)
	class UAmmo* Ammo = nullptr;

protected:
	UPROPERTY()
	FTimerHandle FireTimerHandle;
};

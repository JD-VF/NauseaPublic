// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Tickable.h"
#include "WeaponTypes.h"
#include "FireMode.generated.h"

class UWeapon;
class ACoreCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBeginFireSignature, UFireMode*, FireMode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEndFireSignature, UFireMode*, FireMode);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced, AutoExpandCategories = (Default))
class NAUSEA_API UFireMode : public UObject, public FTickableGameObject
{
	GENERATED_UCLASS_BODY()

//~ Begin UObject Interface
public:
	virtual void PostInitProperties() override;
	virtual void BeginDestroy() override;
	virtual UWorld* GetWorld() const override final { return (WorldPrivate ? WorldPrivate : GetWorld_Uncached()); } //UActorComponent's implementation
//~ End UObject Interface

//~ Begin FTickableGameObject Interface
protected:
	virtual void Tick(float DeltaTime) override { K2_Tick(DeltaTime); }
public:
	virtual ETickableTickType GetTickableTickType() const { return TickType; }
	virtual bool IsTickable() const { return bTickEnabled && !IsPendingKill(); }
	virtual TStatId GetStatId() const { return TStatId(); }
	virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }
//~ End FTickableGameObject Interface

public:
	virtual void Initialize(UWeapon* Weapon);

	UFUNCTION(BlueprintCallable, Category = FireMode)
	FORCEINLINE UWeapon* GetOwningWeapon() const { return OwningWeapon; }

	UFUNCTION(BlueprintCallable, Category = FireMode)
	FORCEINLINE ACoreCharacter* GetOwningCharacter() const { return OwningCharacter; }

public:
	virtual bool ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags) { return false; }

	//Used to know if we're a replicated firemode without casting to the replicated firemode every time.
	UFUNCTION(BlueprintCallable, Category = FireMode)
	virtual bool IsReplicated() const { return false; }

	UFUNCTION(BlueprintCallable, Category = FireMode)
	FORCEINLINE bool IsHoldingFire() const { return bHoldingFire; }

	UFUNCTION(BlueprintCallable, Category = FireMode)
	virtual bool IsFiring() const;
	//If returns false if put away should be blocked from occuring. If this can ever be false firemode should callback to weapon when its task is complete.
	UFUNCTION(BlueprintCallable, Category = FireMode)
	virtual bool CanPutDown() const;

	UFUNCTION(BlueprintCallable, Category = FireMode)
	virtual bool CanFire() const;

	UFUNCTION()
	virtual bool Fire(float WorldTimeOverride = -1.f);
	UFUNCTION()
	virtual void StopFire(float WorldTimeOverride = -1.f);

	UFUNCTION()
	virtual void ForceEndFire();
	
	UFUNCTION()
	void ClearHoldingFire() { bHoldingFire = false; }

public:
	UPROPERTY(BlueprintAssignable, Category = FireMode)
	FBeginFireSignature OnFireStart;
	UPROPERTY(BlueprintAssignable, Category = FireMode)
	FEndFireSignature OnFireComplete;

protected:
	UFUNCTION()
	virtual void FireComplete();

	UFUNCTION()
	virtual void BindWeaponEvents();
	UFUNCTION()
	virtual void UnBindWeaponEvents();

	UFUNCTION()
	virtual void WeaponEquipComplete(UWeapon* Weapon);

	UFUNCTION()
	virtual void WeaponPutDownStart(UWeapon* Weapon);

	UFUNCTION(BlueprintImplementableEvent, Category = FireMode, meta = (DisplayName="Can Fire",ScriptName="CanFire"))
	void K2_CanFire(bool& bCanFire) const;
	UFUNCTION(BlueprintImplementableEvent, Category = FireMode, meta = (DisplayName="On Fire",ScriptName="OnFire"))
	void K2_OnFire();
	UFUNCTION(BlueprintImplementableEvent, Category = FireMode, meta = (DisplayName="On Stop Fire",ScriptName="OnStopFire"))
	void K2_OnStopFire();	
	UFUNCTION(BlueprintImplementableEvent, Category = FireMode, meta = (DisplayName="On Fire Complete",ScriptName="OnFireComplete"))
	void K2_OnFireComplete();
	UFUNCTION(BlueprintImplementableEvent, Category = FireMode, meta = (DisplayName="Tick",ScriptName="Tick"))
	void K2_Tick(float DeltaTime);
	
	UFUNCTION(BlueprintCallable, Category = FireMode)
	void SetTickEnabled(bool bInTickEnabled) { bTickEnabled = bInTickEnabled; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = FireMode)
	bool bCanEverTick = false;
	UPROPERTY(EditDefaultsOnly, Category = FireMode)
	bool bNeverTickOnDedicatedServer = false;
	UPROPERTY(EditDefaultsOnly, Category = FireMode)
	bool bStartWithTickEnabled = false;
	UPROPERTY(Transient)
	bool bTickEnabled = false;

private:
	UWorld* GetWorld_Uncached() const;

private:
	UWorld* WorldPrivate = nullptr;

	UPROPERTY(Transient)
	UWeapon* OwningWeapon = nullptr;
	UPROPERTY(Transient)
	ACoreCharacter* OwningCharacter = nullptr;

	ETickableTickType TickType = ETickableTickType::Never;

	UPROPERTY(Transient)
	bool bHoldingFire = false;
};

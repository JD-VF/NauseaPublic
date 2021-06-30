// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Ammo.generated.h"

class UFireMode;
class ACoreCharacter;
class UAmmoUserWidget;

USTRUCT(BlueprintType)
struct FReloadHistory
{
	GENERATED_USTRUCT_BODY()

	FReloadHistory() {}

	FReloadHistory(float InReloadTimeStamp, float InReloadAmount)
	{
		ReloadTimeStamp = InReloadTimeStamp;
		ReloadAmount = InReloadAmount;
	}

	UPROPERTY()
	float ReloadTimeStamp = -1.f;

	UPROPERTY()
	float ReloadAmount = -1.f;

	UPROPERTY()
	bool bHasApplied = false;

	UPROPERTY()
	bool bHasFailed = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAmmoChangedSignature, UAmmo*, Ammo, float, Amount);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FReloadBeginSignature, UAmmo*, Ammo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FReloadEndSignature, UAmmo*, Ammo);

/**
 * Object that contains ammunition information for a firemode. 
 */
UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced, AutoExpandCategories = (Default))
class NAUSEA_API UAmmo : public UObject
{
	GENERATED_UCLASS_BODY()
		
//~ Begin UObject Interface
public:
	virtual void PostInitProperties() override;
	virtual void BeginDestroy() override;
	virtual UWorld* GetWorld() const override final { return (WorldPrivate ? WorldPrivate : GetWorld_Uncached()); } //UActorComponent's implementation
protected:
	virtual bool IsSupportedForNetworking() const { return true; }
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack) override;
//~ End UObject Interface

public:
	virtual void Initialize(UFireMode* FireMode);

	//This getter must be used instead of the conventional GetClass()->GetDefaultObject() due to the way these are inlined on UWeaponFireMode.
	template<class T>
	const T* GetDefaultObject() const;

	UFUNCTION()
	virtual void UpdateAmmoCapacity(bool bFirstInitialization);

	UFUNCTION(BlueprintCallable, Category = Ammo)
	UFireMode* GetOwningFireMode() const { return OwningFireMode; }
	UFUNCTION(BlueprintCallable, Category = Ammo)
	UWeapon* GetOwningWeapon() const;
	UFUNCTION(BlueprintCallable, Category = FireMode)
	FORCEINLINE ACoreCharacter* GetOwningCharacter() const;

	UFUNCTION(BlueprintCallable, Category = Ammo)
	TSoftClassPtr<UAmmoUserWidget> GetAmmoWidget() const { return AmmoWidget; }

	UFUNCTION(BlueprintCallable, Category = Ammo)
	FORCEINLINE float GetAmmoAmount() const { return AmmoAmount; }

	UFUNCTION(BlueprintCallable, Category = Ammo)
	FORCEINLINE float GetMaxAmmo() const { return MaxAmmoAmount; }

	UFUNCTION(BlueprintCallable, Category = Ammo)
	FORCEINLINE float GetAmmoPercent() const { return AmmoAmount / MaxAmmoAmount; }

	UFUNCTION(BlueprintCallable, Category = Ammo)
	virtual bool ConsumeAmmo(float Amount = 1.f);

	UFUNCTION(BlueprintCallable, Category = Ammo)
	virtual bool CanConsumeAmmo(float Amount = 1.f) const;

	virtual void ApplyAmmoCorrection(float Amount = 1.f);

	UFUNCTION(Client, Reliable)
	void Client_Reliable_SendAmmoDeltaCorrection(float Amount = 1.f);


	UFUNCTION(BlueprintCallable, Category = Ammo)
	virtual bool CanReload() const;
	UFUNCTION()
	virtual bool Reload(float WorldTimeOverride = -1.f) { return false; }
	UFUNCTION()
	virtual bool StopReload(float WorldTimeOverride = -1.f) { return false; }
	UFUNCTION(BlueprintCallable, Category = Ammo)
	virtual bool IsReloading() const { return false; }

	//If true, the current reload action is nearly complete and can be forced to completion to allow for other firemode/ammo actions.
	//IMPORTANT: This should only return true in cases where a reload is nearly complete on non-locally owned authority.
	virtual bool IsReloadNearlyComplete() const { return false; }

	//Called by owning firemode's UFireMode::CanPutDown.
	UFUNCTION()
	virtual bool CanPutDown() const;

	//Called by owning firemode's UFireMode::BlockAction.
	UFUNCTION()
	virtual bool BlockAction(const UFireMode* InstigatorFireMode = nullptr) const;

public:
	UPROPERTY(BlueprintAssignable, Category = Ammo)
	FAmmoChangedSignature OnAmmoChanged;

	UPROPERTY(BlueprintAssignable, Category = Ammo)
	FReloadBeginSignature OnReloadBegin;
	UPROPERTY(BlueprintAssignable, Category = Ammo)
	FReloadEndSignature OnReloadComplete;

protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Reliable_Reload(float WorldTimeOverride = -1.f);
	
	UFUNCTION(Client, Reliable)
	void Client_Reliable_ReloadFailed(float WorldTimeOverride);
	
	UFUNCTION()
	virtual void OnReloadCosmetic() {}

	UFUNCTION()
	virtual void ReloadComplete(float ReloadStartTime) {}

	UFUNCTION()
	void OnRep_AmmoAmount(float PreviousAmount);

	UFUNCTION()
	void OnRep_InitialAmount();

protected:
	UPROPERTY(Replicated, EditDefaultsOnly, Category = Ammo)
	float MaxAmmoAmount = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = Ammo)
	float DefaultAmmoAmount = 50.f;
	
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSoftClassPtr<UAmmoUserWidget> AmmoWidget = nullptr;

private:
	UWorld* GetWorld_Uncached() const;

protected:
	UPROPERTY(Transient)
	bool bDoneFirstInitialization = false;
	
	UPROPERTY(Transient, ReplicatedUsing = OnRep_AmmoAmount)
	float AmmoAmount = -1.f;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_InitialAmount)
	float InitialAmmo = -1.f;

	UPROPERTY()
	TArray<FReloadHistory> ReloadHistory;

private:
	UWorld* WorldPrivate = nullptr;

	UPROPERTY()
	UFireMode* OwningFireMode = nullptr;
};

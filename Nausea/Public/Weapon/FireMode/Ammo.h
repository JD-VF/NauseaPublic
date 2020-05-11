// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Ammo.generated.h"

class UFireMode;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAmmoChangedSignature, UAmmo*, Ammo, float, Amount);

/**
 * Object that contains ammunition information for a weapon. 
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
	virtual int32 GetFunctionCallspace(UFunction* Function, void* Parameters, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack) override;
//~ End UObject Interface

public:
	virtual void Initialize(UFireMode* FireMode);

	UFUNCTION(BlueprintCallable, Category = FireMode)
	UFireMode* GetOwningFireMode() const { return OwningFireMode; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetAmmoAmount() const { return AmmoAmount; }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = FireMode)
	virtual bool ConsumeAmmo(float Amount = 1.f);

	UFUNCTION(BlueprintCallable, Category = FireMode)
	virtual bool CanConsumeAmmo(float Amount = 1.f) const;

	UFUNCTION(Client, Reliable)
	void Client_Reliable_SendAmmoDeltaCorrection(float Amount = 1.f);

public:
	UPROPERTY(BlueprintAssignable)
	FAmmoChangedSignature OnAmmoChanged;

protected:
	UFUNCTION()
	void OnRep_AmmoAmount(float PreviousAmount);

	UFUNCTION()
	void OnRep_InitialAmount();

protected:
	UPROPERTY(EditDefaultsOnly, Category = Ammo)
	float MaxAmmo = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = Ammo)
	float DefaultAmmoAmount = 50.f;

private:
	UWorld* GetWorld_Uncached() const;

private:
	UPROPERTY(Transient)
	bool bDoneFirstInitialization = false;

	UWorld* WorldPrivate = nullptr;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_AmmoAmount)
	float AmmoAmount = -1.f;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_InitialAmount)
	float InitialAmmo = -1.f;

	UPROPERTY()
	UFireMode* OwningFireMode = nullptr;
};

// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/FireMode.h"
#include "ReplicatedFireMode.generated.h"

/**
 * 
 */
UCLASS()
class NAUSEA_API UReplicatedFireMode : public UFireMode
{
	GENERATED_UCLASS_BODY()
		
//~ Begin UObject Interface
protected:
	virtual bool IsSupportedForNetworking() const { return true; }
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack) override;
//~ End UObject Interface

//~ Begin UFireMode Interface 
public:
	virtual bool IsReplicated() const override { return true; }
public:
	virtual bool Fire(float WorldTimeOverride = -1.f) override;
	virtual void StopFire(float WorldTimeOverride = -1.f) override;
//~ End UFireMode Interface 

protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Reliable_Fire(float WorldTimeOverride = -1.f);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Reliable_StopFire(float WorldTimeOverride = -1.f);

	UFUNCTION(Client, Reliable)
	void Client_Reliable_FailedFire();
	
	UFUNCTION()
	virtual void OnRep_FireCounter();

	UFUNCTION()
	virtual void UpdateFireCounter() {}

protected:
	UPROPERTY(ReplicatedUsing = OnRep_FireCounter)
	int32 FireCounter = 0;

	int32 LocalFireCounter = 0;
};

// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PlayerClassObject.generated.h"

class UPlayerClassComponent;

/**
 * 
 */
UCLASS()
class NAUSEA_API UPlayerClassObject : public UObject
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
	bool IsInitialized() const { return OwningPlayerClassComponent != nullptr; }

	virtual void Initialize(UPlayerClassComponent* OwningComponent);

	UFUNCTION(BlueprintCallable, Category = PlayerClass)
	UPlayerClassComponent* GetPlayerClassComponent() const { return OwningPlayerClassComponent; }

	FORCEINLINE bool IsReplicated() const { return bReplicates; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = Replication)
	bool bReplicates = true;

private:
	UWorld* GetWorld_Uncached() const;

private:
	UWorld* WorldPrivate = nullptr;
	
	UPROPERTY()
	UPlayerClassComponent* OwningPlayerClassComponent = nullptr;
};

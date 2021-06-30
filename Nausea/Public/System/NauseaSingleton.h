// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Tickable.h"
#include "NauseaSingleton.generated.h"

class UGameInstance;

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSingletonTickSignature, float, DeltaTime);

/**
 * 
 */
UCLASS()
class NAUSEA_API UNauseaSingleton : public UObject, public FTickableGameObject
{
	GENERATED_UCLASS_BODY()
	
//~ Begin FTickableGameObject Interface
protected:
	virtual void Tick(float DeltaTime) override;
public:
	virtual ETickableTickType GetTickableTickType() const { return ETickableTickType::Always; }
	virtual bool IsTickable() const { return !IsPendingKill(); }
	virtual TStatId GetStatId() const { return TStatId(); }
	virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }
//~ End FTickableGameObject Interface

public:
	UFUNCTION(BlueprintCallable, Category = Singleton, meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext))
	static void BindToSingletonTick(const UObject* WorldContextObject, FOnSingletonTickSignature Delegate);
	UFUNCTION(BlueprintCallable, Category = Singleton, meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext))
	static void UnbindFromSingletonTick(const UObject* WorldContextObject, FOnSingletonTickSignature Delegate);

private:
	UPROPERTY(Transient, DuplicateTransient)
	TArray<FOnSingletonTickSignature> SingletonTickCallbackList;

	UPROPERTY()
	TWeakObjectPtr<UGameInstance> GameInstance = nullptr;
};

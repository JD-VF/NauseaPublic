// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "CoreGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMatchStateChanged, ACoreGameState*, GameState, FName, MatchState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerArrayChangeSignature, ACorePlayerState*, PlayerState);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayerAliveChangedSignature, ACorePlayerState*, PlayerState, bool, bAlive);

/**
 * 
 */
UCLASS()
class NAUSEA_API ACoreGameState : public AGameState
{
	GENERATED_UCLASS_BODY()
	
//~ Begin AGameStateBase Interface
protected:
	virtual void BeginPlay() override;
public:
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	virtual void OnRep_MatchState() override;
//~ End AGameStateBase Interface

public:
	UFUNCTION(BlueprintCallable, Category = GameState)
	bool IsWaitingToStart() const;
	UFUNCTION(BlueprintCallable, Category = GameState)
	bool IsInProgress() const;
	UFUNCTION(BlueprintCallable, Category = GameState)
	bool IsWaitingPostMatch() const;

	UFUNCTION(BlueprintCallable, Category = GameState)
	const TArray<TSubclassOf<UPlayerClassComponent>>& GetPlayerClassList() const { return PlayerClassList; }
	//Returns true if a given player can EVER select this class.
	UFUNCTION()
	virtual bool CanPlayerSelectPlayerClass(const ACorePlayerController* PlayerController, TSubclassOf<UPlayerClassComponent> PlayerClass) const;

	UFUNCTION()
	virtual void HandleDamageApplied(AActor* Actor, float& DamageAmount, FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION()
	virtual void HandleStatusApplied(AActor* Actor, float& EffectPower, FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable, Category = GameState)
	bool AreThereAnyAlivePlayers() const;

	UFUNCTION(BlueprintCallable, Category = GameState)
	int32 GetNumberOfPlayers() const;
	UFUNCTION(BlueprintCallable, Category = GameState)
	int32 GetNumberOfAlivePlayers() const;

public:
	UPROPERTY(BlueprintAssignable, Category = Objective)
	FMatchStateChanged OnMatchStateChanged;

	UPROPERTY(BlueprintAssignable, Category = Objective)
	FPlayerArrayChangeSignature OnPlayerStateAdded;
	UPROPERTY(BlueprintAssignable, Category = Objective)
	FPlayerArrayChangeSignature OnPlayerStateRemoved;

	UPROPERTY(BlueprintAssignable, Category = Objective)
	FPlayerAliveChangedSignature OnPlayerAliveChanged;

protected:
	UFUNCTION()
	virtual void ApplyFriendlyFireDamageMultiplier(AActor* Actor, float& DamageAmount, FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);
	UFUNCTION()
	virtual void ApplyFriendlyFireEffectPowerMultiplier(AActor* Actor, float& EffectPower, FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION()
	void OnRep_PlayerClassList();
	UFUNCTION()
	void OnPlayerAliveUpdate(ACorePlayerState* PlayerState, bool bAlive);

protected:
	UPROPERTY(EditDefaultsOnly, Category = GameState)
	TArray<TSoftClassPtr<UPlayerClassComponent>> DefaultPlayerClassList;
	
	UPROPERTY(EditDefaultsOnly, Category = GameState)
	float FriendlyFireDamageMultiplier = 0.01f;

	UPROPERTY(EditDefaultsOnly, Category = GameState)
	float FriendlyFireEffectPowerMultiplier = 0.f;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_PlayerClassList)
	TArray<TSubclassOf<UPlayerClassComponent>> PlayerClassList;
};

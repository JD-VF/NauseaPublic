// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CoreGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FPlayerKilledSignature, AController*, Killer, AController*, Killed, ACoreCharacter*, KilledCharacter, const struct FDamageEvent&, DamageEvent);

/**
 * 
 */
UCLASS()
class NAUSEA_API ACoreGameMode : public AGameMode
{
	GENERATED_UCLASS_BODY()
	
//~ Begin AGameModeBase Interface
public:
	virtual void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation) override;
//~ End AGameModeBase Interface

public:
	UFUNCTION()
	void PlayerKilled(UStatusComponent* Component, float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION()
	virtual void NotifyKilled(AController* Killer, AController* Killed, ACoreCharacter* KilledCharacter, const struct FDamageEvent& DamageEvent);

public:
	UPROPERTY(BlueprintAssignable, Category = GameMode)
	FPlayerKilledSignature OnPlayerKilled;
};

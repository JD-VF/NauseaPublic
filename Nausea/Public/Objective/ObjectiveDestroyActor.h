// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Objective/Objective.h"
#include "ObjectiveDestroyActor.generated.h"

/**
 * 
 */
UCLASS()
class NAUSEA_API UObjectiveDestroyActor : public UObjective
{
	GENERATED_UCLASS_BODY()
	
//~ Begin UObjective Interface
public:
	virtual void SetObjectiveState(EObjectiveState State) override;
	virtual FString DescribeObjectiveToGameplayDebugger() const override;
//~ End UObjective Interface

protected:
	UFUNCTION()
	void BindObjectiveEvents();
	UFUNCTION()
	void UnBindObjectiveEvents();

	UFUNCTION()
	void CharacterDestroyed(AActor* Character);
	UFUNCTION()
	void OnActorDied(UStatusComponent* Component, float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION()
	void UpdateObjectiveProgress();

protected:
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> ActorList;
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> RemainingActorList;

	UPROPERTY()
	int32 StartingActorCount = -1;
};

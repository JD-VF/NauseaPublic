// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AIPerceptionTypes.h"
#include "AI/EnemySelectionComponent.h"
#include "PerceptionEnemyComponent.generated.h"

class UNauseaAIPerceptionComponent;

/**
 * 
 */
UCLASS()
class NAUSEA_API UPerceptionEnemyComponent : public UEnemySelectionComponent
{
	GENERATED_UCLASS_BODY()
	
//~ Begin UActorComponent Interface
protected:
	virtual void BeginPlay() override;
//~ End UActorComponent Interface

//~ Begin UEnemySelectionComponent Interface
protected:
	virtual AActor* FindBestEnemy() const override;
//~ End UEnemySelectionComponent Interface

public:
	UFUNCTION(BlueprintCallable, Category = EnemySelectionComponent)
	UNauseaAIPerceptionComponent* GetPerceptionComponent() const { return OwningPerceptionComponent; }

protected:
	UFUNCTION()
	virtual void GainedVisibilityOfActor(UNauseaAIPerceptionComponent* PerceptionComponent, AActor* Actor);
	UFUNCTION()
	virtual void LostVisiblityOfActor(UNauseaAIPerceptionComponent* PerceptionComponent, AActor* Actor);
	UFUNCTION()
	virtual void HeardNoiseFromActor(UNauseaAIPerceptionComponent* PerceptionComponent, AActor* Actor);
	UFUNCTION()
	virtual void DamageReceivedFromActor(UNauseaAIPerceptionComponent* PerceptionComponent, AActor* Actor);

private:
	UPROPERTY(Transient)
	UNauseaAIPerceptionComponent* OwningPerceptionComponent = nullptr;
};

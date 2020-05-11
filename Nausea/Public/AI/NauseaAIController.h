// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NauseaAIController.generated.h"

class UActionBrainComponent;
class UEnemySelectionComponent;
class ACoreCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAIPawnUpdatedSignature, ANauseaAIController*, AIController, ACoreCharacter*, Pawn);

/**
 * 
 */
UCLASS()
class NAUSEA_API ANauseaAIController : public AAIController
{
	GENERATED_UCLASS_BODY()

//~ Begin AActor Interface
protected:
	virtual void BeginPlay() override;
//~ End AActor Interface

//~ Begin AController Interface
public:
	virtual void SetPawn(APawn* InPawn) override;
//~ End AController Interface

//~ Begin AController Interface
public:
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
//~ End AController Interface

public:
	UFUNCTION(BlueprintCallable, Category = AIController)
	UActionBrainComponent* GetActionBrainComponent() const { return ActionBrainComponent; }
	UFUNCTION(BlueprintCallable, Category = AIController)
	UEnemySelectionComponent* GetEnemySelectionComponent() const { return EnemySelectionComponent; }

public:
	UPROPERTY(BlueprintAssignable)
	FAIPawnUpdatedSignature OnPawnUpdated;

protected:
	UPROPERTY(EditDefaultsOnly, Category = AIController)
	TSubclassOf<UEnemySelectionComponent> EnemySelectionComponentClass = nullptr;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = BrainComponent)
	UActionBrainComponent* ActionBrainComponent = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = BrainComponent)
	UEnemySelectionComponent* EnemySelectionComponent = nullptr;
};
// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NauseaAIController.generated.h"

class UActionBrainComponent;
class UEnemySelectionComponent;
class URoutineManagerComponent;
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
	virtual void PostInitializeComponents() override;
//~ End AActor Interface

//~ Begin AController Interface
public:
	virtual void SetPawn(APawn* InPawn) override;
//~ End AController Interface

//~ Begin AAIController Interface
public:
	virtual bool RunBehaviorTree(UBehaviorTree* BTAsset) override;
	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn) override;
//~ End AAIController Interface

public:
	UFUNCTION(BlueprintCallable, Category = AIController)
	UActionBrainComponent* GetActionBrainComponent() const { return ActionBrainComponent; }
	UFUNCTION(BlueprintCallable, Category = AIController)
	UEnemySelectionComponent* GetEnemySelectionComponent() const { return EnemySelectionComponent; }
	UFUNCTION(BlueprintCallable, Category = AIController)
	URoutineManagerComponent* GetRoutineManagerComponent() const { return RoutineManagerComponent; }

	UFUNCTION(BlueprintCallable, Category = AIController)
	float GetMaxRotationRate() const;

public:
	UPROPERTY(BlueprintAssignable)
	FAIPawnUpdatedSignature OnPawnUpdated;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = BrainComponent)
	UActionBrainComponent* ActionBrainComponent = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = BrainComponent)
	UEnemySelectionComponent* EnemySelectionComponent = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = BrainComponent)
	URoutineManagerComponent* RoutineManagerComponent = nullptr;
};
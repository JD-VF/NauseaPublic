// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/AIControllerComponent.h"
#include "RoutineManagerComponent.generated.h"

class URoutine;
class URoutineAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRoutineManagerRoundCompleteSignature, URoutineManagerComponent*, RoutineManager, URoutine*, Routine);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRoutineManagerActionCompleteSignature, URoutineManagerComponent*, RoutineManager, URoutine*, Routine, URoutineAction*, RoutineAction);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class NAUSEA_API URoutineManagerComponent : public UAIControllerComponent
{
	GENERATED_UCLASS_BODY()

//~ Begin UAIControllerComponent Interface
public:
	virtual void OnPawnUpdated(ANauseaAIController* AIController, ACoreCharacter* InCharacter) override;
//~ End UAIControllerComponent Interface

public:
	UFUNCTION()
	URoutine* GetCurrentRoutine() const { return CurrentRoutine; }
	UFUNCTION()
	URoutineAction* GetCurrentRoutineAction() const;

	UFUNCTION(BlueprintCallable, Category = RoutineManager)
	virtual URoutine* CreateRoutine(TSubclassOf<URoutine> RoutineClass, bool bAutoStart = true);
	UFUNCTION(BlueprintCallable, Category = RoutineManager)
	virtual void StartRoutine(URoutine* Routine);
	UFUNCTION(BlueprintCallable, Category = RoutineManager)
	virtual void EndRoutine();

	virtual FString DescribeRoutineManagerToGameplayDebugger() const;

public:
	UPROPERTY(BlueprintAssignable, Category = RoutineManager)
	FRoutineManagerRoundCompleteSignature OnRoutineComplete;
	UPROPERTY(BlueprintAssignable, Category = RoutineManager)
	FRoutineManagerActionCompleteSignature OnRoutineActionComplete;

protected:
	UFUNCTION()
	virtual void BindToRoutine(URoutine* Routine);
	UFUNCTION()
	virtual void UnbindToRoutine(URoutine* Routine);

	UFUNCTION()
	virtual void RoutineCompleted(URoutine* Routine);
	UFUNCTION(BlueprintImplementableEvent, Category = Routine, meta = (DisplayName = "On Routine Completed", ScriptName = "RoutineCompleted"))
	void K2_RoutineCompleted(URoutine* Routine);

	UFUNCTION()
	virtual void RoutineActionCompleted(URoutine* Routine, URoutineAction* RoutineAction);

protected:
	UPROPERTY(EditDefaultsOnly, Category = Routine)
	TSubclassOf<URoutine> DefaultRoutineClass;
	UPROPERTY()
	URoutine* CurrentRoutine = nullptr;
};

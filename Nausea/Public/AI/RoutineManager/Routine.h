// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Routine.generated.h"

class ANauseaAIController;
class URoutineManagerComponent;

class UActionBrainDataObject;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRoutineCompleteSignature, URoutine*, Routine);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRoutineActionCompleteSignature, URoutine*, Routine, URoutineAction*, RoutineAction);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class NAUSEA_API URoutine : public UObject
{
	GENERATED_UCLASS_BODY()
	
//~ Begin UObject Interface
public:
	virtual void PostInitProperties() override;
	virtual void BeginDestroy() override;
	virtual UWorld* GetWorld() const override final { return (WorldPrivate ? WorldPrivate : GetWorld_Uncached()); } //UActorComponent's implementation
//~ End UObject Interface

private:
	UWorld* GetWorld_Uncached() const;
private:
	UWorld* WorldPrivate = nullptr;

public:
	UFUNCTION()
	URoutineManagerComponent* GetRoutineManager() const { return RoutineManager; }
	UFUNCTION()
	ANauseaAIController* GetAIController() const { return AIController; }
	UFUNCTION()
	URoutineAction* GetCurrentRoutineAction() const { return CurrentRoutineAction; }
	
	UFUNCTION(BlueprintPure, Category = Routine)
	bool IsRoutineStarted() const { return bRoutineStarted; }
	UFUNCTION(BlueprintPure, Category = Routine)
	bool IsRoutineCompleted() const { return bRoutineCompleted; }

	UFUNCTION(BlueprintCallable, Category = Routine)
	virtual void StartRoutine();
	UFUNCTION(BlueprintCallable, Category = Routine)
	virtual void EndRoutine();

	virtual FString DescribeRoutineToGameplayDebugger() const;

public:
	UPROPERTY(BlueprintAssignable, Category = Routine)
	FRoutineCompleteSignature OnRoutineComplete;
	UPROPERTY(BlueprintAssignable, Category = Routine)
	FRoutineActionCompleteSignature OnRoutineActionComplete;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = Routine, meta = (DisplayName = "On Start Routine", ScriptName = "StartRoutine"))
	void K2_StartRoutine();
	UFUNCTION(BlueprintImplementableEvent, Category = Routine, meta = (DisplayName = "On End Routine", ScriptName = "EndRoutine"))
	void K2_EndRoutine();

	UFUNCTION()
	virtual void BindToRoutineAction(URoutineAction* RoutineAction);
	UFUNCTION()
	virtual void UnbindToRoutineAction(URoutineAction* RoutineAction);

	UFUNCTION()
	virtual URoutineAction* CreateRoutineAction(TSubclassOf<URoutineAction> RoutineActionClass, bool bAutoStart = true);
	UFUNCTION()
	virtual void StartRoutineAction(URoutineAction* RoutineAction);
	UFUNCTION()
	virtual void StopCurrentRoutineAction();


	UFUNCTION()
	virtual void RoutineActionCompleted(URoutineAction* RoutineAction);
	UFUNCTION(BlueprintImplementableEvent, Category = Routine, meta = (DisplayName = "On Routine Action Completed", ScriptName = "RoutineActionCompleted"))
	void K2_RoutineActionCompleted(URoutineAction* RoutineAction);

protected:
	UPROPERTY()
	bool bRoutineStarted = false;
	UPROPERTY()
	bool bRoutineCompleted = false;

private:
	UPROPERTY()
	URoutineAction* CurrentRoutineAction = nullptr;
	UPROPERTY()
	URoutineManagerComponent* RoutineManager = nullptr;
	UPROPERTY()
	ANauseaAIController* AIController = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActionCompleteSignature, URoutineAction*, RoutineAction);

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class NAUSEA_API URoutineAction : public UObject
{
	GENERATED_UCLASS_BODY()

//~ Begin UObject Interface
public:
	virtual void PostInitProperties() override;
	virtual void BeginDestroy() override;
	virtual UWorld* GetWorld() const override final { return (WorldPrivate ? WorldPrivate : GetWorld_Uncached()); } //UActorComponent's implementation
//~ End UObject Interface

private:
	UWorld* GetWorld_Uncached() const;
private:
	UWorld* WorldPrivate = nullptr;

public:
	UFUNCTION(BlueprintPure, Category = RoutineAction)
	URoutine* GetOwningRoutine() const { return OwningRoutine; }
	UFUNCTION(BlueprintPure, Category = RoutineAction)
	ANauseaAIController* GetAIController() const { return AIController; }

	UFUNCTION(BlueprintPure, Category = RoutineAction)
	bool IsActionStarted() const { return bActionStarted; }
	UFUNCTION(BlueprintPure, Category = RoutineAction)
	bool IsActionCompleted() const { return bActionCompleted; }

	UFUNCTION()
	virtual void StartAction();
	UFUNCTION()
	virtual void EndAction();

	virtual FString DescribeRoutineActionToGameplayDebugger() const;

public:
	UPROPERTY(BlueprintAssignable, Category = RoutineAction)
	FActionCompleteSignature OnActionCompleted;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = RoutineAction, meta = (DisplayName = "On Action Start", ScriptName = "OnActionStart"))
	void K2_StartAction();
	UFUNCTION(BlueprintImplementableEvent, Category = RoutineAction, meta = (DisplayName = "On Action End", ScriptName = "OnActionEnd"))
	void K2_EndAction();

protected:
	UPROPERTY()
	bool bActionStarted = false;
	UPROPERTY()
	bool bActionCompleted = false;

private:
	UPROPERTY()
	URoutine* OwningRoutine = nullptr;
	UPROPERTY()
	ANauseaAIController* AIController = nullptr;
};
// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "BrainComponent.h"
#include "ActionStack.h"
#include "ActionBrainComponent.generated.h"

class ANauseaAIController;
class ACoreCharacter;
class UActionBrainComponentAction;

USTRUCT()
struct NAUSEA_API FActionEvent
{
	GENERATED_USTRUCT_BODY()

	// used for marking FPawnActionEvent instances created solely for comparisons uses
	static const int32 FakeActionIndex = INDEX_NONE;

	UPROPERTY()
	UActionBrainComponentAction* Action;

	EPawnActionEventType::Type EventType;

	EAIRequestPriority::Type Priority;

	// used to maintain order of equally-important messages
	uint32 Index;

	FActionEvent() : Action(NULL), EventType(EPawnActionEventType::Invalid), Priority(EAIRequestPriority::MAX), Index(uint32(-1))
	{}

	FActionEvent(UActionBrainComponentAction* Action, EPawnActionEventType::Type EventType, uint32 Index);

	bool operator==(const FActionEvent& Other) const { return (Action == Other.Action) && (EventType == Other.EventType) && (Priority == Other.Priority); }
};

NAUSEA_API DECLARE_LOG_CATEGORY_EXTERN(LogActionBrain, Warning, All);

/**
 * UActionBrainComponent is a version of UPawnActionsComponent but is rewritten to be a more standardized UBrainComponent.
 */
UCLASS()
class NAUSEA_API UActionBrainComponent : public UBrainComponent
{
	GENERATED_UCLASS_BODY()
	
public:
	UFUNCTION()
	ACoreCharacter* GetPawn() const { return Pawn; }

//~ Begin UActorComponent Interface
public:
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
//~ End UActorComponent Interface

public:
	UFUNCTION(BlueprintCallable, Category = ActionBrainComponent)
	void StartLogic();
//~ Begin UBrainComponent Interface
public:
	virtual bool IsRunning() const override;
	virtual bool IsPaused() const override;
	virtual void StopLogic(const FString& Reason) override;
	virtual void RestartLogic() override;
	virtual void Cleanup() override;
	virtual void PauseLogic(const FString& Reason) override;
	virtual EAILogicResuming::Type ResumeLogic(const FString& Reason) override;
	virtual FString GetDebugInfoString() const override;
//~ End UBrainComponent Interface

public:
	UFUNCTION(BlueprintCallable, Category = ActionBrainComponent)
	bool PerformAction(UActionBrainComponentAction* Action, TEnumAsByte<EAIRequestPriority::Type> Priority = EAIRequestPriority::HardScript, UObject* Instigator = nullptr);

	bool OnEvent(UActionBrainComponentAction* Action, EPawnActionEventType::Type Event);
	bool PushAction(UActionBrainComponentAction* NewAction, EAIRequestPriority::Type Priority, UObject* Instigator);

	EPawnActionAbortState::Type AbortAction(UActionBrainComponentAction* ActionToAbort);
	EPawnActionAbortState::Type ForceAbortAction(UActionBrainComponentAction* ActionToAbort);

protected:
	UFUNCTION()
	void OnPawnUpdated(ANauseaAIController* AIController, ACoreCharacter* Character);

	void RemoveEventsForAction(UActionBrainComponentAction* Action);
	void UpdateCurrentAction();

protected:
	UPROPERTY(EditDefaultsOnly, Category = ActionBrainComponent)
	TSubclassOf<UActionBrainComponentAction> DefaultActionClass = nullptr;

private:
	UPROPERTY(Transient)
	TArray<FActionStack> ActionStacks;

	UPROPERTY(Transient)
	TArray<FActionEvent> ActionEvents;

	UPROPERTY(Transient)
	UActionBrainComponentAction* CurrentAction;

	UPROPERTY()
	bool bRunning = false;

	UPROPERTY()
	bool bPaused = false;

	UPROPERTY()
	ACoreCharacter* Pawn = nullptr;

	uint32 ActionEventIndex = 0;
};
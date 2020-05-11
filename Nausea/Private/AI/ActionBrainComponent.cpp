// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#include "ActionBrainComponent.h"
#include "ActionBrainComponentAction.h"
#include "NauseaAIController.h"
#include "CoreCharacter.h"
#include "VisualLogger/VisualLogger.h"

DEFINE_LOG_CATEGORY(LogActionBrain);

namespace
{
	FString GetEventName(int64 Value)
	{
		static const UEnum* Enum = StaticEnum<EPawnActionEventType::Type>();
		check(Enum);
		return Enum->GetNameStringByValue(Value);
	}

	FString GetPriorityName(int64 Value)
	{
		static const UEnum* Enum = StaticEnum<EAIRequestPriority::Type>();
		check(Enum);
		return Enum->GetNameStringByValue(Value);
	}

	FString GetActionSignature(UActionBrainComponentAction* Action)
	{
		if (Action == NULL)
		{
			return TEXT("NULL");
		}

		return FString::Printf(TEXT("[%s, %s]"), *Action->GetName(), *GetPriorityName(Action->GetPriority()));
	}

	struct FActionEvenSort
	{
		FORCEINLINE bool operator()(const FActionEvent& A, const FActionEvent& B) const
		{
			return A.Priority < B.Priority
				|| (A.Priority == B.Priority
					&& (A.EventType < B.EventType
						|| (A.EventType == B.EventType && A.Index < B.Index)));
		}
	};
}

FActionEvent::FActionEvent(UActionBrainComponentAction* InAction, EPawnActionEventType::Type InEventType, uint32 InIndex)
	: Action(InAction), EventType(InEventType), Index(InIndex)
{
	check(InAction);
	Priority = InAction->GetPriority();
}

UActionBrainComponent::UActionBrainComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;

	ActionStacks.AddZeroed(EAIRequestPriority::MAX);
}

void UActionBrainComponent::OnPawnUpdated(ANauseaAIController* AIController, ACoreCharacter* Character)
{
	if (Character)
	{
		Pawn = Character;
		StartLogic();
	}
	else
	{
		Cleanup();
		Pawn = nullptr;
	}
}

void UActionBrainComponent::OnRegister()
{
	Super::OnRegister();

	ANauseaAIController* AIController = Cast<ANauseaAIController>(AIOwner);

	if (!AIController)
	{
		return;
	}

	if (!AIController->OnPawnUpdated.IsAlreadyBound(this, &UActionBrainComponent::OnPawnUpdated))
	{
		AIController->OnPawnUpdated.AddDynamic(this, &UActionBrainComponent::OnPawnUpdated);
	}

	if (AIController && AIController->GetPawn())
	{
		OnPawnUpdated(AIController, Cast<ACoreCharacter>(AIController->GetPawn()));
	}
}

void UActionBrainComponent::OnUnregister()
{
	Super::OnUnregister();

	ANauseaAIController* AIController = Cast<ANauseaAIController>(AIOwner);

	if (!AIController)
	{
		return;
	}

	if (AIController->OnPawnUpdated.IsAlreadyBound(this, &UActionBrainComponent::OnPawnUpdated))
	{
		AIController->OnPawnUpdated.RemoveDynamic(this, &UActionBrainComponent::OnPawnUpdated);
	}
}

void UActionBrainComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GetPawn())
	{
		SetComponentTickEnabled(false);
		return;
	}

	if (ActionEvents.Num() > 1)
	{
		ActionEvents.Sort(FActionEvenSort());
	}

	if (ActionEvents.Num() > 0)
	{
		UE_VLOG(AIOwner, LogActionBrain, Log, TEXT("Processing Action Events:"));
		for (int32 EventIndex = 0; EventIndex < ActionEvents.Num(); ++EventIndex)
		{
			FActionEvent& Event = ActionEvents[EventIndex];

			if (Event.Action == nullptr)
			{
				UE_VLOG(AIOwner, LogActionBrain, Warning, TEXT("%i> NULL action encountered during ActionEvents processing. May result in some notifies not being sent out."), EventIndex);
				continue;
			}

			UE_VLOG(AIOwner, LogActionBrain, Log, TEXT("%i> %s has sent event %s"), EventIndex, *Event.Action->GetName(), *GetEventName(Event.EventType));

			switch (Event.EventType)
			{
			case EPawnActionEventType::InstantAbort:
				// can result in adding new ActionEvents (from child actions) and reallocating data in ActionEvents array
				// because of it, we need to operate on copy instead of reference to memory address
				{
					FActionEvent EventCopy(Event);
					EventCopy.Action->Abort(EAIForceParam::Force);
					ActionStacks[EventCopy.Priority].PopAction(EventCopy.Action);
				}
				break;
			case EPawnActionEventType::FinishedAborting:
			case EPawnActionEventType::FinishedExecution:
			case EPawnActionEventType::FailedToStart:
				ActionStacks[Event.Priority].PopAction(Event.Action);
				break;
			case EPawnActionEventType::Push:
				ActionStacks[Event.Priority].PushAction(Event.Action);
				break;
			default:
				break;
			}
		}

		ActionEvents.Reset();

		UpdateCurrentAction();
	}

	if (CurrentAction)
	{
		CurrentAction->TickAction(DeltaTime);
	}

	// it's possible we got new events with CurrentAction's tick
	if (!IsRunning() && ActionEvents.Num() == 0 && (CurrentAction == NULL || CurrentAction->WantsTick() == false))
	{
		SetComponentTickEnabled(false);
	}
}

void UActionBrainComponent::StartLogic()
{
	bRunning = true;
	SetComponentTickEnabled(true);

	if (UActionBrainComponentAction* Action = UActionBrainComponentAction::CreateAction(this, DefaultActionClass))
	{
		PerformAction(Action, Action->GetActionPriority());
	}
}

bool UActionBrainComponent::IsRunning() const
{
	return bRunning;
}

bool UActionBrainComponent::IsPaused() const
{
	return bPaused;
}

void UActionBrainComponent::StopLogic(const FString& Reason)
{
	for (int32 PriorityIndex = 0; PriorityIndex < EAIRequestPriority::MAX; ++PriorityIndex)
	{
		UActionBrainComponentAction* Action = ActionStacks[PriorityIndex].GetTop();
		while (Action)
		{
			Action->Abort(EAIForceParam::Force);
			Action = Action->ParentAction;
		}
	}

	MessagesToProcess.Reset();
	bRunning = false;
}

void UActionBrainComponent::RestartLogic()
{
	if (IsRunning())
	{
		StopLogic("Restart");
	}

	StartLogic();
}

void UActionBrainComponent::Cleanup()
{
	StopLogic("Cleanup");
}

void UActionBrainComponent::PauseLogic(const FString& Reason)
{
	
}

EAILogicResuming::Type UActionBrainComponent::ResumeLogic(const FString& Reason)
{
	EAILogicResuming::Type Type = Super::ResumeLogic(Reason);

	switch (Type)
	{
	case EAILogicResuming::Type::RestartedInstead:
		RestartLogic();
		return EAILogicResuming::Type::RestartedInstead;
	}

	return Type;
}

FString UActionBrainComponent::GetDebugInfoString() const
{
	FString DebugInfo;

	for (int32 Priority = int32(EAIRequestPriority::Ultimate); Priority >= 0; Priority--)
	{
		FString PriorityName = UEnum::GetValueAsString<EAIRequestPriority::Type>(EAIRequestPriority::Type(Priority));
		PriorityName.RemoveFromStart("EAIRequestPriority::");

		if (ActionStacks[Priority].IsEmpty())
		{
			DebugInfo += FString::Printf(TEXT("\n%s actions:\n   (None)\n"), *PriorityName);
			continue;
		}

		const UActionBrainComponentAction* ParentmostAction = ActionStacks[Priority].GetTop();

		while (ParentmostAction->GetParentAction())
		{
			ParentmostAction = ParentmostAction->GetParentAction();
		}

		DebugInfo += FString::Printf(TEXT("\n%s actions: \n"), *PriorityName);
		DebugInfo += ParentmostAction->GetDebugInfoString(1);
	}

	return DebugInfo;
}

bool UActionBrainComponent::PerformAction(UActionBrainComponentAction* Action, TEnumAsByte<EAIRequestPriority::Type> Priority, UObject* Instigator)
{
	return PushAction(Action, Priority, Instigator);
}

bool UActionBrainComponent::OnEvent(UActionBrainComponentAction* Action, EPawnActionEventType::Type Event)
{
	if (!Action)
	{
		UE_VLOG(AIOwner, LogActionBrain, Warning, TEXT("Null Action in Action Event: Event %s")
			, *GetEventName(Event));
		return false;
	}

	bool bResult = false;

	const FActionEvent ActionEvent(Action, Event, ActionEventIndex++);

	if (Event != EPawnActionEventType::Invalid && ActionEvents.Find(ActionEvent) == INDEX_NONE)
	{
		ActionEvents.Add(ActionEvent);

		//Beginng ticking if this is the first event added and we have a pawn (UActionBrainComponent::StartLogic is called on pawn possession and will do this).
		if (ActionEvents.Num() == 1 && GetPawn())
		{
			SetComponentTickEnabled(true);
		}

		bResult = true;
	}
	else if (Event == EPawnActionEventType::Invalid)
	{
		UE_VLOG(AIOwner, LogActionBrain, Warning, TEXT("Ignoring Action Event: Action %s Event %s")
			, *Action->GetName(), *GetEventName(Event));
	}
	else
	{
		UE_VLOG(AIOwner, LogActionBrain, Warning, TEXT("Ignoring duplicate Action Event: Action %s Event %s")
			, *Action->GetName(), *GetEventName(Event));
	}

	return bResult;
}

bool UActionBrainComponent::PushAction(UActionBrainComponentAction* NewAction, EAIRequestPriority::Type Priority, UObject* Instigator)
{
	if (!NewAction)
	{
		return false;
	}

	if (NewAction->HasBeenStarted() == false || NewAction->IsFinished() == true)
	{
		NewAction->ExecutionPriority = Priority;
		NewAction->SetOwnerComponent(this);
		NewAction->SetInstigator(Instigator);
		return OnEvent(NewAction, EPawnActionEventType::Push);
	}

	return false;
}

EPawnActionAbortState::Type UActionBrainComponent::AbortAction(UActionBrainComponentAction* ActionToAbort)
{
	if (!ActionToAbort)
	{
		return EPawnActionAbortState::AbortDone;
	}

	const EPawnActionAbortState::Type AbortState = ActionToAbort->Abort(EAIForceParam::DoNotForce);
	if (AbortState == EPawnActionAbortState::NeverStarted)
	{
		// this is a special case. It's possible someone tried to abort an action that 
		// has just requested to be pushed and the push event has not been processed yet.
		// in such a case we'll look through the awaiting action events and remove a push event 
		// for given ActionToAbort
		RemoveEventsForAction(ActionToAbort);
	}
	return AbortState;
}

EPawnActionAbortState::Type UActionBrainComponent::ForceAbortAction(UActionBrainComponentAction* ActionToAbort)
{
	if (!ActionToAbort)
	{
		return EPawnActionAbortState::AbortDone;
	}

	const EPawnActionAbortState::Type AbortState = ActionToAbort->Abort(EAIForceParam::Force);
	RemoveEventsForAction(ActionToAbort);
	return AbortState;
}

void UActionBrainComponent::RemoveEventsForAction(UActionBrainComponentAction* Action)
{
	for (int32 ActionIndex = ActionEvents.Num() - 1; ActionIndex >= 0; --ActionIndex)
	{
		if (ActionEvents[ActionIndex].Action == Action)
		{
			ActionEvents.RemoveAtSwap(ActionIndex, /*Count=*/1, /*bAllowShrinking=*/false);
		}
	}
}

void UActionBrainComponent::UpdateCurrentAction()
{
	UE_VLOG(AIOwner, LogActionBrain, Log, TEXT("Picking new current actions. Old CurrentAction %s")
		, *GetActionSignature(CurrentAction));

	// find the highest priority action available
	UActionBrainComponentAction* NewCurrentAction = NULL;
	int32 Priority = EAIRequestPriority::MAX - 1;
	do
	{
		NewCurrentAction = ActionStacks[Priority].GetTop();

	} while (NewCurrentAction == NULL && --Priority >= 0);

	// if it's a new Action then enable it
	if (CurrentAction != NewCurrentAction)
	{
		UE_VLOG(AIOwner, LogActionBrain, Log, TEXT("New action: %s")
			, *GetActionSignature(NewCurrentAction));

		if (CurrentAction != NULL && CurrentAction->IsActive())
		{
			CurrentAction->Pause(NewCurrentAction);
		}
		CurrentAction = NewCurrentAction;
		bool bNewActionStartedSuccessfully = true;
		if (CurrentAction != NULL)
		{
			bNewActionStartedSuccessfully = CurrentAction->Activate();
		}

		if (bNewActionStartedSuccessfully == false)
		{
			UE_VLOG(AIOwner, LogActionBrain, Warning, TEXT("CurrentAction %s failed to activate. Removing and re-running action selection")
				, *GetActionSignature(NewCurrentAction));

			CurrentAction = NULL;
		}
	}
	else
	{
		if (CurrentAction == NULL)
		{
			UE_VLOG(AIOwner, LogActionBrain, Warning, TEXT("Doing nothing."));
		}
		else if (CurrentAction->IsFinished())
		{
			UE_VLOG(AIOwner, LogActionBrain, Warning, TEXT("Re-running same action"));
			CurrentAction->Activate();
		}
		else
		{
			UE_VLOG(AIOwner, LogActionBrain, Warning, TEXT("Still doing the same action"));
		}
	}
}
// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#include "EnemySelectionComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "NauseaAIController.h"
#include "SelectableEnemyInterface.h"
#include "CoreCharacter.h"

UEnemySelectionComponent::UEnemySelectionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UEnemySelectionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UEnemySelectionComponent::OnPawnUpdated(ANauseaAIController* AIController, ACoreCharacter* InCharacter)
{
	if (InCharacter)
	{
		CurrentCharacter = InCharacter;
		SetEnemy(nullptr);
	}
	else
	{
		ClearEnemy();
		CurrentCharacter = nullptr;
	}
}

AActor* UEnemySelectionComponent::GetEnemy() const
{
	return Cast<AActor>(CurrentEnemy.GetObject());
}

AActor* UEnemySelectionComponent::FindBestEnemy() const
{
	if (!GetOwningCharacter())
	{
		return nullptr;
	}

	float ClosestDistanceSq = MAX_FLT;
	AActor* ClosestActor = nullptr;

	//Default behaviour is to just find closest pawn to us.
	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		APawn* Pawn = It->Get();

		if (ETeamAttitude::Type::Hostile != FGenericTeamId::GetAttitude(GetOwner(), Pawn))
		{
			continue;
		}

		const float DistanceSq = GetOwningCharacter()->GetSquaredDistanceTo(Pawn);

		if (DistanceSq >= ClosestDistanceSq)
		{
			continue;
		}

		ClosestDistanceSq = DistanceSq;
		ClosestActor = Pawn;
	}

	return ClosestActor;
}


ISelectableEnemyInterface* UEnemySelectionComponent::GetEnemyInterface() const
{
	if (!CurrentEnemy.GetObject())
	{
		return nullptr;
	}

	return (ISelectableEnemyInterface*)CurrentEnemy.GetInterface();
}

bool UEnemySelectionComponent::SetEnemy(AActor* NewEnemy, bool bForce)
{
	if (!NewEnemy)
	{
		NewEnemy = FindBestEnemy();
	}

	if (GetEnemy() == NewEnemy)
	{
		return true;
	}

	if (!bForce && !CanChangeEnemy())
	{
		return false;
	}

	if (!CanTargetEnemy(NewEnemy))
	{
		return false;
	}

	AActor* PreviousEnemy = GetEnemy();
	CurrentEnemy = NewEnemy;
	EnemyChanged(PreviousEnemy);
	return true;
}

void UEnemySelectionComponent::ClearEnemy()
{
	AActor* PreviousEnemy = GetEnemy();
	CurrentEnemy = nullptr;
	EnemyChanged(PreviousEnemy);
}

bool UEnemySelectionComponent::CanChangeEnemy() const
{
	if (GetEnemy() && IsEnemyChangeCooldownActive())
	{
		return false;
	}

	return true;
}

bool UEnemySelectionComponent::CanTargetEnemy(AActor* NewEnemy) const
{
	ISelectableEnemyInterface* NewEnemyInterface = Cast<ISelectableEnemyInterface>(NewEnemy);

	if (!NewEnemyInterface)
	{
		return false;
	}

	if (!NewEnemyInterface->IsTargetable(GetOwner()))
	{
		return false;
	}

	return true;
}

void UEnemySelectionComponent::EnemyChanged(AActor* PreviousEnemy)
{
	if (bEnemyChangeCooldown)
	{
		GetWorld()->GetTimerManager().SetTimer(EnemyCooldownTimerHandle, EnemyChangeCooldown, false);
	}

	if (ISelectableEnemyInterface* PreviousEnemyInterface = Cast<ISelectableEnemyInterface>(PreviousEnemy))
	{
		PreviousEnemyInterface->GetTargetableStateChangedDelegate().RemoveDynamic(this, &UEnemySelectionComponent::OnEnemyTargetableStateChanged);
		PreviousEnemyInterface->OnEndTarget(GetOwner());
	}

	if (ISelectableEnemyInterface* NewEnemyInterface = GetEnemyInterface())
	{
		NewEnemyInterface->GetTargetableStateChangedDelegate().AddDynamic(this, &UEnemySelectionComponent::OnEnemyTargetableStateChanged);
		NewEnemyInterface->OnBecomeTarget(GetOwner());
	}
	
	OnEnemyChanged.Broadcast(this, GetEnemy(), PreviousEnemy);
	K2_OnEnemyChanged(PreviousEnemy);
}

float UEnemySelectionComponent::GetEnemyChangeCooldownRemaining() const
{
	return GetWorld()->GetTimerManager().GetTimerRemaining(EnemyCooldownTimerHandle);
}

bool UEnemySelectionComponent::IsEnemyChangeCooldownActive() const
{
	return GetWorld()->GetTimerManager().IsTimerActive(EnemyCooldownTimerHandle);
}

void UEnemySelectionComponent::OnEnemyTargetableStateChanged(AActor* Actor, bool bTargetable)
{
	if (Actor != GetEnemy())
	{
		return;
	}

	//Although we are being told the new targetable state of the actor, we should still double check.
	if (GetEnemyInterface()->IsTargetable(GetOwner()))
	{
		return;
	}

	SetEnemy(nullptr, true);
}
// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#include "NauseaAIController.h"
#include "ActionBrainComponent.h"
#include "EnemySelectionComponent.h"
#include "NauseaAIPerceptionComponent.h"

ANauseaAIController::ANauseaAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActionBrainComponent = CreateDefaultSubobject<UActionBrainComponent>(TEXT("ActionBrainComponent"));
	BrainComponent = ActionBrainComponent;

	PerceptionComponent = CreateDefaultSubobject<UNauseaAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
}

void ANauseaAIController::BeginPlay()
{
	bStopAILogicOnUnposses = false;

	if (!EnemySelectionComponentClass)
	{
		EnemySelectionComponentClass = UEnemySelectionComponent::StaticClass();
	}

	EnemySelectionComponent = NewObject<UEnemySelectionComponent>(this, EnemySelectionComponentClass, TEXT("EnemySelectionComponent"));
	EnemySelectionComponent->RegisterComponent();

	Super::BeginPlay();
}

void ANauseaAIController::SetPawn(APawn* InPawn)
{
	const APawn* PreviousPawn = GetPawn();

	Super::SetPawn(InPawn);

	if (GetPawn() != PreviousPawn)
	{
		OnPawnUpdated.Broadcast(this, Cast<ACoreCharacter>(InPawn));
	}
}

ETeamAttitude::Type ANauseaAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (GetGenericTeamId() == FGenericTeamId::NoTeam)
	{
		return ETeamAttitude::Hostile;
	}

	return Super::GetTeamAttitudeTowards(Other);
}
// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#include "PerceptionEnemyComponent.h"
#include "GameFramework/Pawn.h"
#include "Perception/AISense_Sight.h"
#include "NauseaAIController.h"
#include "NauseaAIPerceptionComponent.h"
#include "CoreCharacter.h"

UPerceptionEnemyComponent::UPerceptionEnemyComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UPerceptionEnemyComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningPerceptionComponent = Cast<UNauseaAIPerceptionComponent>(GetAIController()->GetAIPerceptionComponent());
	
	if (!GetPerceptionComponent()->OnGainedSightOfActor.IsAlreadyBound(this, &UPerceptionEnemyComponent::GainedVisibilityOfActor))
	{
		GetPerceptionComponent()->OnGainedSightOfActor.AddDynamic(this, &UPerceptionEnemyComponent::GainedVisibilityOfActor);
	}

	if (!GetPerceptionComponent()->OnLostSightOfActor.IsAlreadyBound(this, &UPerceptionEnemyComponent::LostVisiblityOfActor))
	{
		GetPerceptionComponent()->OnLostSightOfActor.AddDynamic(this, &UPerceptionEnemyComponent::LostVisiblityOfActor);
	}

	if (!GetPerceptionComponent()->OnHeardNoiseFromActor.IsAlreadyBound(this, &UPerceptionEnemyComponent::HeardNoiseFromActor))
	{
		GetPerceptionComponent()->OnHeardNoiseFromActor.AddDynamic(this, &UPerceptionEnemyComponent::HeardNoiseFromActor);
	}

	if (!GetPerceptionComponent()->OnGainedSightOfActor.IsAlreadyBound(this, &UPerceptionEnemyComponent::DamageReceivedFromActor))
	{
		GetPerceptionComponent()->OnGainedSightOfActor.AddDynamic(this, &UPerceptionEnemyComponent::DamageReceivedFromActor);
	}

	ensure(GetPerceptionComponent());
}

AActor* UPerceptionEnemyComponent::FindBestEnemy() const
{
	if (!GetOwningCharacter() || !GetPerceptionComponent())
	{
		return nullptr;
	}

	float ClosestDistanceSq = MAX_FLT;
	AActor* ClosestActor = nullptr;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		APawn* Pawn = It->Get();

		if (ETeamAttitude::Type::Hostile != FGenericTeamId::GetAttitude(GetOwner(), Pawn))
		{
			continue;
		}

		const bool bCanSee = GetPerceptionComponent()->HasSeenActor(Pawn);

		const float DistanceSq = bCanSee ? GetOwningCharacter()->GetSquaredDistanceTo(Pawn) * 0.5f : GetOwningCharacter()->GetSquaredDistanceTo(Pawn);

		if (DistanceSq >= ClosestDistanceSq)
		{
			continue;
		}

		ClosestDistanceSq = DistanceSq;
		ClosestActor = Pawn;
	}

	return ClosestActor;
}

void UPerceptionEnemyComponent::GainedVisibilityOfActor(UNauseaAIPerceptionComponent* PerceptionComponent, AActor* Actor)
{
	if (Actor == GetEnemy())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Gained visibility of %s"), *Actor->GetName())

	if (!GetEnemy())
	{
		SetEnemy(Actor);
		return;
	}

	APawn* Pawn = GetAIController()->GetPawn();

	const float DistanceToOldTarget = Pawn->GetDistanceTo(GetEnemy());
	const float DistanceToNewTarget = Pawn->GetDistanceTo(Actor);

	//If we can see our enemy, the newly seen actor must be half the distance of the current enemy.
	if (GetPerceptionComponent()->HasSeenActor(GetEnemy()))
	{
		if (DistanceToNewTarget > DistanceToOldTarget * 0.5f)
		{
			return;
		}
	}
	else
	{
		if (DistanceToNewTarget > DistanceToOldTarget * 2.f)
		{
			return;
		}
	}

	SetEnemy(Actor);
}

void UPerceptionEnemyComponent::LostVisiblityOfActor(UNauseaAIPerceptionComponent* PerceptionComponent, AActor* Actor)
{
	if (Actor != GetEnemy())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Lost visibility of %s"), *Actor->GetName());

	TArray<AActor*> Actors;
	GetPerceptionComponent()->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), Actors);

	float ClosestDistanceSq = MAX_FLT;
	AActor* ClosestActor = nullptr;

	const FVector Location = GetAIController()->GetPawn()->GetActorLocation();

	for (AActor* PerceivedActor : Actors)
	{
		if (!PerceivedActor)
		{
			continue;
		}

		const float DistanceSq = FVector::DistSquared(Location, PerceivedActor->GetActorLocation());

		if (ClosestDistanceSq > DistanceSq)
		{
			ClosestDistanceSq = DistanceSq;
			ClosestActor = PerceivedActor;
		}
	}

	const float EnemyDistanceSq = FVector::DistSquared(Location, GetEnemy()->GetActorLocation());

	if (EnemyDistanceSq < ClosestDistanceSq)
	{
		return;
	}

	SetEnemy(ClosestActor);
}

void UPerceptionEnemyComponent::HeardNoiseFromActor(UNauseaAIPerceptionComponent* PerceptionComponent, AActor* Actor)
{
	if (!GetPerceptionComponent()->HasSeenActor(Actor))
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Heard noise by %s"), *Actor->GetName());

	APawn* Pawn = GetAIController()->GetPawn();

	const float DistanceToOldTarget = Pawn->GetDistanceTo(GetEnemy());
	const float DistanceToNewTarget = Pawn->GetDistanceTo(Actor);

	if (DistanceToNewTarget > DistanceToOldTarget * 0.5f)
	{
		return;
	}

	SetEnemy(Actor);
}

void UPerceptionEnemyComponent::DamageReceivedFromActor(UNauseaAIPerceptionComponent* PerceptionComponent, AActor* Actor)
{
	if (!GetPerceptionComponent()->HasSeenActor(Actor))
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Received damage from %s"), *Actor->GetName());

	APawn* Pawn = GetAIController()->GetPawn();

	const float DistanceToOldTarget = Pawn->GetDistanceTo(GetEnemy());
	const float DistanceToNewTarget = Pawn->GetDistanceTo(Actor);

	if (DistanceToNewTarget > DistanceToOldTarget * 1.2f)
	{
		return;
	}

	SetEnemy(Actor);
}
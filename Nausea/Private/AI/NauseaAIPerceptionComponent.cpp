// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#include "NauseaAIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Damage.h"
#include "AIController.h"

UNauseaAIPerceptionComponent::UNauseaAIPerceptionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AISenseSightID = UAISense::GetSenseID<UAISense_Sight>();
	AISenseHearingID = UAISense::GetSenseID<UAISense_Hearing>();
	AISenseDamageID = UAISense::GetSenseID<UAISense_Damage>();
}

void UNauseaAIPerceptionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (!OnTargetPerceptionUpdated.IsAlreadyBound(this, &UNauseaAIPerceptionComponent::OnPerceptionUpdate))
	{
		OnTargetPerceptionUpdated.AddDynamic(this, &UNauseaAIPerceptionComponent::OnPerceptionUpdate);
	}
}

float UNauseaAIPerceptionComponent::GetMostRecentActiveStimulusAge(const FActorPerceptionInfo* PerceptionInfo) const
{
	if (!PerceptionInfo)
	{
		return FAIStimulus::NeverHappenedAge;
	}

	float BestAge = FAIStimulus::NeverHappenedAge;

	for (const FAIStimulus& Stimulus : PerceptionInfo->LastSensedStimuli)
	{
		if (!UNauseaAIPerceptionComponent::IsUsableStimulus(Stimulus))
		{
			continue;
		}

		if (Stimulus.GetAge() >= BestAge)
		{
			continue;
		}
		
		BestAge = Stimulus.GetAge();
	}

	return BestAge;
}

bool UNauseaAIPerceptionComponent::HasPerceivedActor(AActor* Actor, float MaxAge) const
{
	if (MaxAge < 0.f)
	{
		if (const FActorPerceptionInfo* PerceptionInfo = GetActorPerceptionInfo(Actor))
		{
			for (const FAIStimulus& Stimulus : PerceptionInfo->LastSensedStimuli)
			{
				if (!UNauseaAIPerceptionComponent::IsUsableStimulus(Stimulus))
				{
					continue;
				}

				if (Stimulus.GetAge() != FAIStimulus::NeverHappenedAge)
				{
					return true;
				}
			}
		}
	}
	else
	{
		if (const FActorPerceptionInfo* PerceptionInfo = GetActorPerceptionInfo(Actor))
		{
			for (const FAIStimulus& Stimulus : PerceptionInfo->LastSensedStimuli)
			{
				if (!UNauseaAIPerceptionComponent::IsUsableStimulus(Stimulus))
				{
					continue;
				}

				if (Stimulus.GetAge() > MaxAge)
				{
					continue;
				}

				return true;
			}
		}
	}

	return false;
}

FORCEINLINE float GetStimulusAge(const FActorPerceptionInfo* PerceptionInfo, FAISenseID Sense)
{
	if (!PerceptionInfo || !PerceptionInfo->LastSensedStimuli.IsValidIndex(Sense))
	{
		return FAIStimulus::NeverHappenedAge;
	}

	const FAIStimulus& Stimulus = PerceptionInfo->LastSensedStimuli[Sense];

	if (!Stimulus.IsValid())
	{
		return FAIStimulus::NeverHappenedAge;
	}

	if (!Stimulus.IsActive() || Stimulus.IsExpired())
	{
		return FAIStimulus::NeverHappenedAge;
	}

	return Stimulus.GetAge();
}

bool UNauseaAIPerceptionComponent::HasSeenActor(AActor* Actor, float MaxAge) const
{
	if (MaxAge < 0.f)
	{
		return GetStimulusAge(GetActorPerceptionInfo(Actor), AISenseSightID) != FAIStimulus::NeverHappenedAge;
	}

	return GetStimulusAge(GetActorPerceptionInfo(Actor), AISenseSightID) <= MaxAge;
}

bool UNauseaAIPerceptionComponent::HasRecentlyHeardActor(AActor* Actor, float MaxAge) const
{
	if (MaxAge < 0.f)
	{
		return GetStimulusAge(GetActorPerceptionInfo(Actor), AISenseHearingID) != FAIStimulus::NeverHappenedAge;
	}

	return GetStimulusAge(GetActorPerceptionInfo(Actor), AISenseHearingID) <= MaxAge;
}

bool UNauseaAIPerceptionComponent::HasRecentlyReceivedDamageFromActor(AActor* Actor, float MaxAge) const
{
	if (MaxAge < 0.f)
	{
		return GetStimulusAge(GetActorPerceptionInfo(Actor), AISenseDamageID) != FAIStimulus::NeverHappenedAge;
	}

	return GetStimulusAge(GetActorPerceptionInfo(Actor), AISenseDamageID) <= MaxAge;
}

void UNauseaAIPerceptionComponent::OnPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor || !AIOwner || !AIOwner->GetPawn())
	{
		return;
	}

	if (Stimulus.Type == AISenseSightID)
	{
		if (Stimulus.IsActive())
		{
			OnGainedSightOfActor.Broadcast(this, Actor);
		}
		else
		{
			OnLostSightOfActor.Broadcast(this, Actor);
		}
	}
	else if (Stimulus.Type == AISenseHearingID)
	{
		if (Stimulus.IsActive())
		{
			OnHeardNoiseFromActor.Broadcast(this, Actor);
		}
	}
	else if (Stimulus.Type == AISenseDamageID)
	{
		if (Stimulus.IsActive())
		{
			OnReceivedDamageFromActor.Broadcast(this, Actor);
		}
	}
}
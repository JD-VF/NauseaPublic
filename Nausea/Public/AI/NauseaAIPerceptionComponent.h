// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Perception/AIPerceptionComponent.h"
#include "NauseaAIPerceptionComponent.generated.h"

static FAISenseID AISenseSightID;
static FAISenseID AISenseHearingID;
static FAISenseID AISenseDamageID;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGainedSightOfActorSignature, UNauseaAIPerceptionComponent*, PerceptionComponent, AActor*, Actor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FLostSightOfActorSignature, UNauseaAIPerceptionComponent*, PerceptionComponent, AActor*, Actor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHeardNoiseFromActorSignature, UNauseaAIPerceptionComponent*, PerceptionComponent, AActor*, Actor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FReceivedDamageFromActorSignature, UNauseaAIPerceptionComponent*, PerceptionComponent, AActor*, Actor);

/**
 * 
 */
UCLASS()
class NAUSEA_API UNauseaAIPerceptionComponent : public UAIPerceptionComponent
{
	GENERATED_UCLASS_BODY()

//~ Begin UActorComponent Interface
public:
	virtual void BeginPlay() override;
//~ End UActorComponent Interface

public:
	float GetMostRecentActiveStimulusAge(const FActorPerceptionInfo* PerceptionInfo) const;
	UFUNCTION(BlueprintCallable, Category = Perception)
	bool HasPerceivedActor(AActor* Actor, float MaxAge = 0.f) const;
	UFUNCTION(BlueprintCallable, Category = Perception)
	bool HasSeenActor(AActor* Actor, float MaxAge = 0.f) const;
	UFUNCTION(BlueprintCallable, Category = Perception)
	bool HasRecentlyHeardActor(AActor* Actor, float MaxAge = 5.f) const;
	UFUNCTION(BlueprintCallable, Category = Perception)
	bool HasRecentlyReceivedDamageFromActor(AActor* Actor, float MaxAge = 7.f) const;

	UFUNCTION(BlueprintCallable, Category = Perception)
	static bool IsUsableStimulus(const FAIStimulus& Stimulus) { return Stimulus.IsValid() && Stimulus.IsActive() && !Stimulus.IsExpired(); }

	const FActorPerceptionInfo* GetActorPerceptionInfo(const AActor* Actor) const { return GetPerceptualData().Find(reinterpret_cast<uint64>(Actor)); }

public:
	UPROPERTY(BlueprintAssignable)
	FGainedSightOfActorSignature OnGainedSightOfActor;
	UPROPERTY(BlueprintAssignable)
	FLostSightOfActorSignature OnLostSightOfActor;
	UPROPERTY(BlueprintAssignable)
	FHeardNoiseFromActorSignature OnHeardNoiseFromActor;
	UPROPERTY(BlueprintAssignable)
	FReceivedDamageFromActorSignature OnReceivedDamageFromActor;

protected:
	UFUNCTION()
	virtual void OnPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus);
};
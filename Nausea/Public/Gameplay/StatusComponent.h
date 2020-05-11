// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StatusType.h"
#include "StatusComponent.generated.h"

class AActor;
class IStatusInterface;

USTRUCT(BlueprintType)
struct FStatStruct
{
	GENERATED_USTRUCT_BODY()

	FStatStruct()
	{

	}

	FStatStruct(const float& InValue, const float& InMaxValue)
		: DefaultValue(InValue), DefaultMaxValue(InMaxValue) {}

	FStatStruct(const float& InMaxValue)
		: FStatStruct(InMaxValue, InMaxValue) {}

	FStatStruct& operator= (const float& InValue) { this->SetValue(InValue); return *this; }
	operator float() const { return GetValue(); }

public:
	FStatStruct Initialize() { MaxValue = DefaultMaxValue; SetValue(DefaultValue); return *this; }

	float GetValue() const { return Value; }
	float GetPercentValue() const { return Value / MaxValue; }

	float SetValue(float InValue)
	{
		Value = FMath::Clamp(InValue, 0.f, MaxValue);	
		return Value;
	}

	float AddValue(float DeltaValue) { SetValue(Value + DeltaValue); return Value; }
	float SetMaxValue(float InMaxValue) { MaxValue = InMaxValue; return SetValue(Value); }

public:
	UPROPERTY()
	float Value = 0.f;
	UPROPERTY()
	float MaxValue = MAX_FLT;
	
	UPROPERTY(NotReplicated, EditAnywhere, BlueprintReadOnly)
	float DefaultValue = 0.f;
	UPROPERTY(NotReplicated, EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Max Value"))
	float DefaultMaxValue = MAX_FLT;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHealthChangedSignature, UStatusComponent*, Component, float, Health);

UCLASS( ClassGroup=(Custom), BlueprintType, HideCategories = (ComponentTick, Collision, Tags, Variable, Activation, ComponentReplication, Cooking, Sockets, UserAssetData))
class NAUSEA_API UStatusComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

//~ Begin UActorComponent Interface 
protected:
	virtual void BeginPlay() override;
//~ End UActorComponent Interface

public:
	UFUNCTION(BlueprintCallable, Category = StatusComponent)
	TScriptInterface<IStatusInterface> GetOwnerInterface() const { return StatusInterface; }

	UFUNCTION(BlueprintCallable, Category = StatusInterface)
	float GetHealth() const { return Health; }
	UFUNCTION(BlueprintCallable, Category = StatusInterface)
	float GetHealthPercent() const { return Health.GetPercentValue(); }

	UFUNCTION(BlueprintCallable, Category = StatusInterface)
	float IsDead() const { return Health <= 0.f; }

public:
	UPROPERTY(BlueprintAssignable, Category = StatusInterface)
	FHealthChangedSignature OnHealthChanged;

protected:
	UFUNCTION()
	virtual void InitializeStats();

	float SetHealth(float InHealth);
	float SetMaxHealth(float InMaxHealth);

	UFUNCTION()
	virtual void TakeDamage(AActor* Actor, float& DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION()
	virtual void OnRep_Health();

private:
	UPROPERTY(ReplicatedUsing = OnRep_Health, EditDefaultsOnly, Category = StatusComponent)
	FStatStruct Health;

private:
	UPROPERTY(Transient)
	TScriptInterface<IStatusInterface> StatusInterface = nullptr;
};
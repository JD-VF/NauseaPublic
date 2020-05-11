// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#include "StatusComponent.h"
#include "Net/UnrealNetwork.h"
#include "StatusInterface.h"

UStatusComponent::UStatusComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	SetIsReplicated(true);

	Health = FStatStruct(100.f);
}

void UStatusComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UStatusComponent, Health);
}

void UStatusComponent::BeginPlay()
{
	StatusInterface = GetOwner();

	TInlineComponentArray<UStatusComponent*> StatusComponents(GetOwner());
	ensure(StatusComponents.Num() <= 1);
	ensure(StatusInterface && StatusInterface.GetObject());

	InitializeStats();

	if (IsDead())
	{
		StatusInterface->Died(0.f, FDamageEvent(), nullptr, nullptr);
		Super::BeginPlay();
		return;
	}

	StatusInterface->GetTakeDamageDelegate().AddDynamic(this, &UStatusComponent::TakeDamage);

	Super::BeginPlay();
}

void UStatusComponent::InitializeStats()
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		return;
	}

	Health.Initialize();
}

float UStatusComponent::SetHealth(float InHealth)
{
	if (Health == InHealth)
	{
		return Health;
	}

	const float CurrentHealth = Health.SetValue(InHealth);
	OnRep_Health();
	return CurrentHealth;
}

float UStatusComponent::SetMaxHealth(float InMaxHealth)
{
	const float PreviousHealth = Health.GetValue();
	const float CurrentHealth = Health.SetMaxValue(InMaxHealth);

	if (CurrentHealth != PreviousHealth)
	{
		OnRep_Health();
	}

	return CurrentHealth;
}

void UStatusComponent::TakeDamage(AActor* Actor, float& DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		return;
	}

	if (IsDead())
	{
		DamageAmount = 0.f;
		return;
	}

	SetHealth(Health - DamageAmount);

	if (Health <= 0.f)
	{
		StatusInterface->Died(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	}
}

void UStatusComponent::OnRep_Health()
{
	OnHealthChanged.Broadcast(this, Health);
}
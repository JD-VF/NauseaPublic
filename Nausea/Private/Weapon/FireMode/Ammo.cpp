// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#include "Ammo.h"
#include "Net/UnrealNetwork.h"
#include "FireMode.h"
#include "Weapon.h"
#include "CoreCharacter.h"

UAmmo::UAmmo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UAmmo::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UAmmo, AmmoAmount, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(UAmmo, InitialAmmo, COND_OwnerOnly);
}

void UAmmo::PostInitProperties()
{
	Super::PostInitProperties();
	
	if (HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))
	{
		return;
	}

	Initialize(GetTypedOuter<UFireMode>());
}

void UAmmo::BeginDestroy()
{
	OwningFireMode = nullptr;
	WorldPrivate = nullptr;

	Super::BeginDestroy();
}

int32 UAmmo::GetFunctionCallspace(UFunction* Function, void* Parameters, FFrame* Stack)
{
	check(GetOuter());
	return GetOuter()->GetFunctionCallspace(Function, Parameters, Stack);
}

bool UAmmo::CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack)
{
	//RPCs to ammunition is only supported if attached to a firemode.
	if (!GetOwningFireMode())
	{
		return false;
	}

	bool bProcessed = false;

	if (AActor* MyOwner = GetOwningFireMode()->GetOwningCharacter())
	{
		FWorldContext* const Context = GEngine->GetWorldContextFromWorld(GetWorld());
		if (Context != nullptr)
		{
			for (FNamedNetDriver& Driver : Context->ActiveNetDrivers)
			{
				if (Driver.NetDriver != nullptr && Driver.NetDriver->ShouldReplicateFunction(MyOwner, Function))
				{
					Driver.NetDriver->ProcessRemoteFunction(MyOwner, Function, Parameters, OutParms, Stack, this);
					bProcessed = true;
				}
			}
		}
	}
	return bProcessed;
}

void UAmmo::Initialize(UFireMode* FireMode)
{
	//We've been reparented to something that isn't a firemode, attempt to establish some basic initialization.
	if (!FireMode)
	{
		OwningFireMode = nullptr;

		if (AActor* Actor = GetTypedOuter<AActor>())
		{
			WorldPrivate = Actor->GetWorld();
		}
		return;
	}

	OwningFireMode = FireMode;
	WorldPrivate = OwningFireMode->GetWorld();

	if (!GetTypedOuter<AActor>() || GetTypedOuter<AActor>()->Role != ROLE_Authority)
	{
		return;
	}

	if (bDoneFirstInitialization)
	{
		return;
	}

	InitialAmmo = DefaultAmmoAmount;
	OnRep_InitialAmount(); //Server and owning client are expected to run the same code path here in order to maintain cohesion.

	bDoneFirstInitialization = true;
}

bool UAmmo::ConsumeAmmo(float Amount)
{
	if (!GetOwningFireMode())
	{
		return false;
	}

	if (GetOwningFireMode()->GetOwningWeapon()->IsSimulatedProxy())
	{
		return true;
	}

	if (!CanConsumeAmmo(Amount))
	{
		return false;
	}

	const float PreviousAmount = AmmoAmount;
	AmmoAmount -= Amount;
	OnRep_AmmoAmount(PreviousAmount);
	return true;
}

bool UAmmo::CanConsumeAmmo(float Amount) const
{
	return Amount <= AmmoAmount;
}

void UAmmo::Client_Reliable_SendAmmoDeltaCorrection_Implementation(float Amount)
{
	const float PreviousAmount = AmmoAmount;
	AmmoAmount += Amount;
	OnRep_AmmoAmount(PreviousAmount);
}

void UAmmo::OnRep_AmmoAmount(float PreviousAmount)
{
	OnAmmoChanged.Broadcast(this, AmmoAmount);
}

void UAmmo::OnRep_InitialAmount()
{
	if (UWeapon* Weapon = GetOwningFireMode() ? GetOwningFireMode()->GetOwningWeapon() : nullptr)
	{
		if (Weapon->IsSimulatedProxy())
		{
			return;
		}
	}

	AmmoAmount = InitialAmmo;

	OnRep_AmmoAmount(AmmoAmount);
}

UWorld* UAmmo::GetWorld_Uncached() const
{
	UWorld* ObjectWorld = nullptr;

	const UObject* Owner = GetOwningFireMode();

	if (Owner && !Owner->HasAnyFlags(RF_ClassDefaultObject))
	{
		ObjectWorld = Owner->GetWorld();
	}

	if (ObjectWorld == nullptr)
	{
		if (AActor* Actor = GetTypedOuter<AActor>())
		{
			ObjectWorld = Owner->GetWorld();
		}
		else
		{
			ObjectWorld = Cast<UWorld>(GetOuter());
		}
	}

	return ObjectWorld;
}
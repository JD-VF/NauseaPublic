// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#include "ReplicatedFireMode.h"
#include "UnrealNetwork.h"
#include "GameFramework/GameStateBase.h"
#include "CoreCharacter.h"
#include "Weapon.h"

UReplicatedFireMode::UReplicatedFireMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UReplicatedFireMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UReplicatedFireMode, FireCounter, COND_SkipOwner);
}

int32 UReplicatedFireMode::GetFunctionCallspace(UFunction* Function, void* Parameters, FFrame* Stack)
{
	check(GetOuter());
	return GetOuter()->GetFunctionCallspace(Function, Parameters, Stack);
}

bool UReplicatedFireMode::CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack)
{
	bool bProcessed = false;

	if (AActor* MyOwner = GetOwningCharacter())
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

bool UReplicatedFireMode::Fire(float WorldTimeOverride)
{
	if (!Super::Fire(WorldTimeOverride))
	{
		return false;
	}

	if (GetOwningWeapon()->IsLocallyOwnedRemote() && GetWorld()->GetGameState())
	{
		Server_Reliable_Fire(GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
	}

	return true;
}

void UReplicatedFireMode::StopFire(float WorldTimeOverride)
{
	Super::StopFire(WorldTimeOverride);

	if (GetOwningWeapon()->IsLocallyOwnedRemote() && GetWorld()->GetGameState())
	{
		Server_Reliable_StopFire(GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
	}
}

bool UReplicatedFireMode::Server_Reliable_Fire_Validate(float WorldTimeOverride)
{
	return true;
}

void UReplicatedFireMode::Server_Reliable_Fire_Implementation(float WorldTimeOverride)
{
	if (!GetWorld()->GetGameState())
	{
		return;
	}

	if (GetOwningWeapon()->IsEquipping())
	{
		GetOwningWeapon()->ForceEquip();
	}

	WorldTimeOverride = FMath::Min(WorldTimeOverride, GetWorld()->GetGameState()->GetServerWorldTimeSeconds());

	if (!Fire(WorldTimeOverride))
	{
		Client_Reliable_FailedFire();
	}

	FireCounter++;
}

bool UReplicatedFireMode::Server_Reliable_StopFire_Validate(float WorldTimeOverride)
{
	return true;
}

void UReplicatedFireMode::Server_Reliable_StopFire_Implementation(float WorldTimeOverride)
{
	if (!GetWorld()->GetGameState())
	{
		return;
	}

	WorldTimeOverride = FMath::Min(WorldTimeOverride, GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
	StopFire(WorldTimeOverride);
}

void UReplicatedFireMode::Client_Reliable_FailedFire_Implementation()
{

}

void UReplicatedFireMode::OnRep_FireCounter()
{
	if (!GetOwningWeapon()->HasBegunPlay())
	{
		LocalFireCounter = FireCounter;
		return;
	}

	if (FireCounter <= 0)
	{
		LocalFireCounter = FireCounter;
		return;
	}

	if (GetOwningWeapon()->IsInactive())
	{
		LocalFireCounter = FireCounter;
		return;
	}

	UpdateFireCounter();
}
// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#include "WeaponFireMode.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "GameFramework/GameStateBase.h"
#include "Weapon.h"
#include "Ammo.h"

UWeaponFireMode::UWeaponFireMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UWeaponFireMode::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponFireMode, Ammo);
}

bool UWeaponFireMode::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	if (!Ammo)
	{
		return false;
	}

	return Channel->ReplicateSubobject(Ammo, *Bunch, *RepFlags);
}

bool UWeaponFireMode::IsFiring() const
{
	return GetWorld()->GetTimerManager().IsTimerActive(FireTimerHandle);
}

bool UWeaponFireMode::CanFire() const
{
	if (GetOwningWeapon()->IsSimulatedProxy())
	{
		return true;
	}

	if (!Super::CanFire())
	{
		return false;
	}

	if (!CanConsumeAmmo())
	{
		return false;
	}

	return true;
}

bool UWeaponFireMode::Fire(float WorldTimeOverride)
{
	if (!Super::Fire(WorldTimeOverride))
	{
		return false;
	}

	if (!ConsumeAmmo())
	{
		return false;
	}

	if (FireType != EFireType::Automatic && GetOwningWeapon()->IsLocallyOwned())
	{
		ClearHoldingFire();
	}

	const float FireDuration = WorldTimeOverride == -1.f ? FireRate : FMath::Max3(FireRate - (GetWorld()->GetGameState()->GetServerWorldTimeSeconds() - WorldTimeOverride), FireRate * 0.5f, 0.01f);

	GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, FTimerDelegate::CreateUObject(this, &UWeaponFireMode::FireComplete), FireDuration, false);
	return true;
}

void UWeaponFireMode::StopFire(float WorldTimeOverride)
{
	Super::StopFire(WorldTimeOverride);
}

void UWeaponFireMode::FireComplete()
{
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);

	Super::FireComplete();

	if (GetOwningWeapon()->IsSimulatedProxy())
	{
		UpdateFireCounter();
		return;
	}

	if (!GetOwningWeapon()->IsLocallyOwned())
	{
		return;
	}

	if (CanRefire())
	{
		Fire();
	}
}

bool UWeaponFireMode::CanRefire() const
{
	if (!CanFire())
	{
		return false;
	}

	return IsHoldingFire();
}

bool UWeaponFireMode::CanConsumeAmmo() const
{
	if (!GetAmmo())
	{
		return true;
	}

	return GetAmmo()->CanConsumeAmmo();
}

bool UWeaponFireMode::ConsumeAmmo()
{
	if (!GetAmmo())
	{
		return true;
	}

	return GetAmmo()->ConsumeAmmo();
}

void UWeaponFireMode::UpdateFireCounter()
{
	if (GetOwningWeapon()->IsPuttingDown() || GetOwningWeapon()->IsInactive())
	{
		return;
	}

	if (LocalFireCounter == FireCounter)
	{
		return;
	}

	LocalFireCounter++;
	LocalFireCounter = FMath::Min(LocalFireCounter, FireCounter);

	if (LocalFireCounter != FireCounter)
	{
		const int32 MaxDiff = FMath::CeilToInt((FireRate + 0.5f) / FireRate);
		LocalFireCounter = FMath::Max(FireCounter - MaxDiff, LocalFireCounter);
	}

	Fire();
}
// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#include "FireMode.h"
#include "Weapon.h"
#include "CoreCharacter.h"

UFireMode::UFireMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UFireMode::PostInitProperties()
{
	Super::PostInitProperties();

	if (HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))
	{
		TickType = ETickableTickType::Never;
		bTickEnabled = false;
		return;
	}

	//Non authority running this will not have a weapon as the outer.
	if (UWeapon* Weapon = Cast<UWeapon>(GetOuter()))
	{
		Initialize(Weapon);
	}
}

void UFireMode::BeginDestroy()
{
	UnBindWeaponEvents();
	OwningWeapon = nullptr;
	OwningCharacter = nullptr;
	WorldPrivate = nullptr;
	bTickEnabled = false;

	Super::BeginDestroy();
}

void UFireMode::Initialize(UWeapon* Weapon)
{
	ensure(Weapon);

	if (OwningWeapon)
	{
		ensure(Weapon == OwningWeapon); //Fire modes do not support ownership transferring and as such should never allow a change of ownership.
		return;
	}

	OwningWeapon = Weapon;
	WorldPrivate = GetOwningWeapon()->GetWorld();
	OwningCharacter = OwningWeapon->GetTypedOuter<ACoreCharacter>();

	BindWeaponEvents();

	const bool bShouldTick = bCanEverTick && !(bNeverTickOnDedicatedServer && GetOwningCharacter()->IsNetMode(NM_DedicatedServer));

	TickType = bShouldTick ? ETickableTickType::Conditional : ETickableTickType::Never;
	bTickEnabled = bShouldTick && bStartWithTickEnabled;
}

bool UFireMode::IsFiring() const
{
	return false;
}

bool UFireMode::CanPutDown() const
{
	return !IsFiring();
}

bool UFireMode::CanFire() const
{
	if (GetOwningWeapon()->IsSimulatedProxy())
	{
		return true;
	}

	if (!GetOwningWeapon()->IsActive())
	{
		return false;
	}

	if (IsFiring())
	{
		return false;
	}

	bool bCanFire = true;
	K2_CanFire(bCanFire);
	return bCanFire;
}

bool UFireMode::Fire(float WorldTimeOverride)
{
	bHoldingFire = true;

	if (!CanFire())
	{
		return false;
	}

	K2_OnFire();
	OnFireStart.Broadcast(this);

	return true;
}

void UFireMode::StopFire(float WorldTimeOverride)
{
	bHoldingFire = false;

	K2_OnStopFire();
}

void UFireMode::ForceEndFire()
{
	if (!GetOwningWeapon()->IsNonOwningAuthority())
	{
		return;
	}

	if (IsHoldingFire())
	{
		StopFire();
	}

	if (IsFiring())
	{
		FireComplete();
	}
}

void UFireMode::FireComplete()
{
	K2_OnFireComplete();
	OnFireComplete.Broadcast(this);
}

void UFireMode::BindWeaponEvents()
{
	if (OwningWeapon->OnWeaponEquipComplete.IsAlreadyBound(this, &UFireMode::WeaponEquipComplete))
	{
		return;
	}

	OwningWeapon->OnWeaponEquipComplete.AddDynamic(this, &UFireMode::WeaponEquipComplete);
	OwningWeapon->OnWeaponPutDownStart.AddDynamic(this, &UFireMode::WeaponPutDownStart);
}

void UFireMode::UnBindWeaponEvents()
{
	if (!OwningWeapon || OwningWeapon->IsPendingKill() || !OwningWeapon->OnWeaponEquipComplete.IsAlreadyBound(this, &UFireMode::WeaponEquipComplete))
	{
		return;
	}

	OwningWeapon->OnWeaponEquipComplete.RemoveDynamic(this, &UFireMode::WeaponEquipComplete);
	OwningWeapon->OnWeaponPutDownStart.RemoveDynamic(this, &UFireMode::WeaponPutDownStart);
}

void UFireMode::WeaponEquipComplete(UWeapon* Weapon)
{
	if (GetOwningWeapon()->IsLocallyOwned() && IsHoldingFire() && CanFire())
	{
		Fire();
	}
}

void UFireMode::WeaponPutDownStart(UWeapon* Weapon)
{
	//...
}

UWorld* UFireMode::GetWorld_Uncached() const
{
	UWorld* ObjectWorld = nullptr;

	const UObject* Owner = GetOwningWeapon();

	if (Owner && !Owner->HasAnyFlags(RF_ClassDefaultObject))
	{
		ObjectWorld = Owner->GetWorld();
	}

	if (ObjectWorld == nullptr)
	{
		if (AActor* Actor = GetTypedOuter<AActor>())
		{
			ObjectWorld = Actor->GetWorld();
		}
		else
		{
			ObjectWorld = Cast<UWorld>(GetOuter());
		}
	}

	return ObjectWorld;
}
// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#include "Weapon.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "CoreCharacter.h"
#include "InventoryManagerComponent.h"
#include "FireMode.h"

UWeapon::UWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FireModeList.SetNum(MAXFIREMODES);
}

void UWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeapon, FireModeList);
	DOREPLIFETIME_CONDITION(UWeapon, WeaponState, COND_SkipOwner);
}

void UWeapon::BeginPlay()
{
	UpdateFireModeList();

	Super::BeginPlay();
}

void UWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

bool UWeapon::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = false;

	for (UFireMode* FireMode : FireModeList)
	{
		if (!FireMode)
		{
			continue;
		}

		bWroteSomething |= FireMode->ReplicateSubobjects(Channel, Bunch, RepFlags);
		bWroteSomething |= Channel->ReplicateSubobject(FireMode, *Bunch, *RepFlags);
	}

	return Super::ReplicateSubobjects(Channel, Bunch, RepFlags) || bWroteSomething;
}

bool UWeapon::IsCurrentlyEquippedWeapon() const
{
	return GetOwningInventoryManager()->GetCurrentWeapon() == this;
}

bool UWeapon::IsFiring() const
{
	for (const UFireMode* FireMode : FireModeList)
	{
		if (!FireMode)
		{
			continue;
		}

		if (!FireMode->IsFiring())
		{
			continue;
		}

		return true;
	}

	return false;
}

bool UWeapon::Equip()
{
	if (!CanEquip())
	{
		return false;
	}

	ClearPendingPutDown();

	GetWorld()->GetTimerManager().SetTimer(EquipTimer, FTimerDelegate::CreateUObject(this, &UWeapon::EquipComplete), PutDownTime, false);
	K2_OnEquip();
	SetWeaponState(EWeaponState::Equipping);
	return true;
}

bool UWeapon::CanEquip() const
{
	if (IsNonOwningAuthority() || IsSimulatedProxy())
	{
		return true;
	}

	if (IsEquipping())
	{
		return false;
	}

	return true;
}

void UWeapon::EquipComplete()
{
	GetWorld()->GetTimerManager().ClearTimer(EquipTimer);
	K2_OnEquipComplete();
	SetWeaponState(EWeaponState::Active);
}

bool UWeapon::PutDown()
{
	if (!CanPutDown())
	{
		if (IsLocallyOwned())
		{
			bPendingPutDown = true;
		}
		return false;
	}

	ClearPendingPutDown();

	GetWorld()->GetTimerManager().SetTimer(PutDownTimer, FTimerDelegate::CreateUObject(this, &UWeapon::PutDownComplete), PutDownTime, false);
	K2_OnPutDown();
	SetWeaponState(EWeaponState::PuttingDown);
	return true;
}

bool UWeapon::CanPutDown() const
{
	if (IsNonOwningAuthority() || IsSimulatedProxy())
	{
		return true;
	}

	for (UFireMode* FireMode : FireModeList)
	{
		if (!FireMode)
		{
			continue;
		}

		if (!FireMode->CanPutDown())
		{
			return false;
		}
	}

	if (!IsActive())
	{
		return false;
	}

	return true;
}

void UWeapon::ClearPendingPutDown()
{
	bPendingPutDown = false;
}

void UWeapon::PutDownComplete()
{
	GetWorld()->GetTimerManager().ClearTimer(PutDownTimer);
	K2_OnPutDownComplete();

	SetWeaponState(EWeaponState::Inactive);
}

void UWeapon::AbortPutDown()
{
	if (!IsNonOwningAuthority())
	{
		return;
	}

	if (!IsPuttingDown())
	{
		return;
	}

	PutDownComplete();
}

void UWeapon::AbortEquip()
{
	if (!IsNonOwningAuthority())
	{
		return;
	}

	if (!IsEquipping())
	{
		return;
	}

	EquipComplete();
	PutDown();
	PutDownComplete();
}

void UWeapon::ForcePutDown()
{
	if (!IsNonOwningAuthority())
	{
		return;
	}

	if (IsPuttingDown())
	{
		PutDownComplete();
		return;
	}

	if (IsEquipping())
	{
		AbortEquip();
		return;
	}

	for (UFireMode* FireMode : FireModeList)
	{
		if (!FireMode || FireMode->CanPutDown())
		{
			continue;
		}

		FireMode->ForceEndFire();
	}

	ensure(CanPutDown());

	PutDown();
	PutDownComplete();
}

void UWeapon::ForceEquip()
{
	if (!IsNonOwningAuthority())
	{
		return;
	}

	if (IsEquipping())
	{
		EquipComplete();
		return;
	}

	if (IsPuttingDown())
	{
		AbortPutDown();
		return;
	}

	ensure(CanEquip());

	Equip();
	EquipComplete();
}

FORCEINLINE bool UWeapon::ShouldPlayEffects1P() const
{
	return IsLocallyOwned();
}

FORCEINLINE bool UWeapon::ShouldPlayEffects3P() const
{
	return !IsNetMode(NM_DedicatedServer) && !IsLocallyOwned();
}

FORCEINLINE USkeletalMeshComponent* UWeapon::GetMesh1P() const
{
	return GetOwningCharacter()->GetMesh1P();
}

FORCEINLINE USkeletalMeshComponent* UWeapon::GetWeaponMesh1P() const
{
	return GetOwningCharacter()->GetWeaponMesh1P();
}

FORCEINLINE USkeletalMeshComponent* UWeapon::GetMesh3P() const
{
	return GetOwningCharacter()->GetMesh();
}

FORCEINLINE USkeletalMeshComponent* UWeapon::GetWeaponMesh3P() const
{
	return GetOwningCharacter()->GetWeaponMesh3P();
}

bool UWeapon::Fire(EFireMode Mode)
{
	if (UFireMode* FireMode = GetFireMode(Mode))
	{
		return FireMode->Fire();
	}

	return false;
}

bool UWeapon::StopFire(EFireMode Mode)
{
	if (UFireMode* FireMode = GetFireMode(Mode))
	{
		FireMode->StopFire();
		return true;
	}

	return false;
}

void UWeapon::SetWeaponState(EWeaponState State)
{
	if (GetOwnerRole() < ROLE_AutonomousProxy)
	{
		return;
	}

	EWeaponState PreviousState = WeaponState;

	WeaponState = State;

	OnRep_WeaponState(PreviousState);
}

FORCEINLINE UFireMode* UWeapon::GetFireMode(EFireMode Mode) const
{
	return FireModeList[uint8(Mode)];
}

void UWeapon::OnRep_WeaponState(EWeaponState PreviousState)
{
	if (IsSimulatedProxy())
	{
		switch (WeaponState)
		{
		case EWeaponState::Equipping:
			Equip();
			break;
		case EWeaponState::PuttingDown:
			PutDown();
			break;
		case EWeaponState::Active:
			if (PreviousState == EWeaponState::Equipping) { EquipComplete(); }
			break;
		case EWeaponState::Inactive:
			if (PreviousState == EWeaponState::PuttingDown) { PutDownComplete(); }
			break;
		}
	}

	OnWeaponStateChanged.Broadcast(this, WeaponState, PreviousState);

	switch (WeaponState)
	{
	case EWeaponState::Equipping:
		OnWeaponEquipStart.Broadcast(this);
		break;
	case EWeaponState::PuttingDown:
		OnWeaponPutDownStart.Broadcast(this);
		break;
	case EWeaponState::Active:
		if (PreviousState == EWeaponState::Equipping) { OnWeaponEquipComplete.Broadcast(this); }
		break;
	case EWeaponState::Inactive:
		if (PreviousState == EWeaponState::PuttingDown) { OnWeaponPutDownComplete.Broadcast(this); }
		break;
	}
}

void UWeapon::OnRep_FireModeList()
{
	//Auth already generated these.
	if (IsAuthority())
	{
		return;
	}

	UpdateFireModeList();
}

void UWeapon::UpdateFireModeList()
{
	for (TPair<EFireMode, TSubclassOf<UFireMode>>& FireModeEntry : WeaponFireModeList)
	{
		if (!FireModeEntry.Value)
		{
			continue;
		}

		UFireMode*& FireMode = FireModeList[uint8(FireModeEntry.Key)];

		if (FireMode)
		{
			continue;
		}

		const UFireMode* FireModeCDO = FireModeEntry.Value->GetDefaultObject<UFireMode>();

		//Remote authority doesn't care about non-replicating firemodes.
		if (IsNonOwningAuthority() && !FireModeCDO->IsReplicated())
		{
			continue;
		}

		//Non authority will receive replicating firemodes from authority.
		if (!IsAuthority() && FireModeCDO->IsReplicated())
		{
			continue;
		}

		FireMode = NewObject<UFireMode>(this, FireModeEntry.Value);
	}

	for (UFireMode* FireMode : FireModeList)
	{
		if (!FireMode)
		{
			continue;
		}
		
		FireMode->Initialize(this);

		if (!FireMode->OnFireComplete.IsAlreadyBound(this, &UWeapon::FireCompleted))
		{
			FireMode->OnFireComplete.AddDynamic(this, &UWeapon::FireCompleted);
		}
	}

	if (IsAuthority())
	{
		OnRep_FireModeList();
	}
}

void UWeapon::FireCompleted(UFireMode* FireMode)
{
	OnWeaponFireComplete.Broadcast(this);
}
// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#include "InventoryManagerComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/GameStateBase.h"
#include "Components/InputComponent.h"
#include "Inventory.h"
#include "Weapon.h"

UInventoryManagerComponent::UInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	SetIsReplicated(true);

	WeaponGroupMap.Reserve(MAXWEAPONGROUP);
}

void UInventoryManagerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryManagerComponent, InventoryList);
}

void UInventoryManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (IsAuthority())
	{
		InitializeInventory();
	}
}

void UInventoryManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UInventoryManagerComponent::InitializeInventory()
{
	for (TSubclassOf<UInventory> Inventory : DefaultInventoryList)
	{
		AddInventory(Inventory, false);
	}

	OnRep_InventoryList();
}

bool UInventoryManagerComponent::AddInventory(TSubclassOf<UInventory> InventoryClass, bool bDispatchOnRep)
{
	if (!InventoryClass)
	{
		return false;
	}

	for (UInventory* Inventory : InventoryList)
	{
		if (!Inventory)
		{
			continue;
		}

		if (Inventory->GetClass() == InventoryClass)
		{
			return false;
		}
	}

	UInventory* Inventory = NewObject<UInventory>(GetOwner(), InventoryClass);

	if (Inventory)
	{
		InventoryList.Add(Inventory);
		Inventory->RegisterComponent();
		GetOwner()->ForceNetUpdate();
		if (bDispatchOnRep) { OnRep_InventoryList(); }
	}

	return true;
}

bool UInventoryManagerComponent::RemoveInventory(TSubclassOf<UInventory> InventoryClass, bool bDispatchOnRep)
{
	if (!InventoryClass)
	{
		return false;
	}

	for (UInventory* Inventory : InventoryList)
	{
		if (!Inventory || Inventory->GetClass() == InventoryClass)
		{
			continue;
		}

		Inventory->DestroyComponent();
		GetOwner()->ForceNetUpdate();
		if (bDispatchOnRep) { OnRep_InventoryList(); }
		return true;
	}

	return false;
}

#define BIND_NEXT_WEAPON(Type)\
InputComponent->BindAction("Next"#Type##"Weapon", EInputEvent::IE_Pressed, this, &UInventoryManagerComponent::EquipNextWeapon<EWeaponGroup::Type>);\

#define BIND_WEAPON_FIRE(Type)\
InputComponent->BindAction(#Type##"Fire", EInputEvent::IE_Pressed, this, &UInventoryManagerComponent::StartFire<EFireMode::Type>);\
InputComponent->BindAction(#Type##"Fire", EInputEvent::IE_Released, this, &UInventoryManagerComponent::StopFire<EFireMode::Type>);\

void UInventoryManagerComponent::SetupInputComponent(UInputComponent* InputComponent)
{
	CurrentInputComponent = InputComponent;

	check(InputComponent);

	BIND_NEXT_WEAPON(Melee);
	BIND_NEXT_WEAPON(Pistol);
	BIND_NEXT_WEAPON(SMG);
	BIND_NEXT_WEAPON(Rifle);
	BIND_NEXT_WEAPON(Special);
	BIND_NEXT_WEAPON(Utility);

	BIND_WEAPON_FIRE(Primary);
	BIND_WEAPON_FIRE(Secondary);
	BIND_WEAPON_FIRE(Tertiary);
	BIND_WEAPON_FIRE(Quaternary);
	BIND_WEAPON_FIRE(Quinary);
}

void UInventoryManagerComponent::EquipNextWeapon(EWeaponGroup Group)
{
	if (!WeaponGroupMap.Contains(Group) || WeaponGroupMap[Group].WeaponArray.Num() == 0)
	{
		return;
	}

	FWeaponGroupArray& WeaponArray = WeaponGroupMap[Group];

	const UWeapon* ComparedWeapon = GetPendingWeapon() ? GetPendingWeapon() : GetCurrentWeapon();

	bool bFoundCurrentWeapon = false;
	for (TWeakObjectPtr<UWeapon> Weapon : WeaponArray)
	{
		if (bFoundCurrentWeapon)
		{
			SetCurrentWeapon(Weapon.Get());
			return;
		}

		if (!Weapon.IsValid())
		{
			continue;
		}

		if (Weapon != ComparedWeapon)
		{
			continue;
		}

		bFoundCurrentWeapon = true;
	}

	SetCurrentWeapon(WeaponArray.WeaponArray[0].Get());
}

void UInventoryManagerComponent::StartFire(EFireMode FireMode)
{
	if (GetCurrentWeapon())
	{
		GetCurrentWeapon()->Fire(FireMode);
	}
}

void UInventoryManagerComponent::StopFire(EFireMode FireMode)
{
	if (GetCurrentWeapon())
	{
		GetCurrentWeapon()->StopFire(FireMode);
	}
}

UWeapon* UInventoryManagerComponent::GetNextBestWeapon() const
{
	float Rating = -MAX_FLT;
	UWeapon* BestWeapon = nullptr;

	for (UInventory* Inventory : InventoryList)
	{
		UWeapon* Weapon = Cast<UWeapon>(Inventory);

		if (!Weapon || Weapon == CurrentWeapon)
		{
			continue;
		}

		if (!Weapon->CanEquip())
		{
			continue;
		}

		if (Rating > Weapon->GetWeaponRating())
		{
			continue;
		}
		
		BestWeapon = Weapon;
	}

	return BestWeapon;
}

bool UInventoryManagerComponent::ContainsInventory(UInventory* Inventory) const
{
	return InventoryList.Contains(Inventory);
}

bool UInventoryManagerComponent::CanEquipWeapon(UWeapon* Weapon) const
{
	return ContainsInventory(Weapon);
}

void UInventoryManagerComponent::SetCurrentWeapon(UWeapon* DesiredWeapon)
{
	if (!DesiredWeapon)
	{
		return;
	}

	if (!CanEquipWeapon(DesiredWeapon))
	{
		return;
	}

	if (!DesiredWeapon->CanEquip())
	{
		//Handle this condition where we cannot equip because we're already equipping (this specific case will fail UWeapon::CanEquip)
		if (GetCurrentWeapon() == DesiredWeapon && DesiredWeapon->IsEquipping())
		{
			SetPendingWeapon(nullptr);
			GetCurrentWeapon()->ClearPendingPutDown();
		}
		return;
	}

	if (GetCurrentWeapon() == DesiredWeapon && !GetCurrentWeapon()->IsPuttingDown())
	{
		SetPendingWeapon(nullptr);
		GetCurrentWeapon()->ClearPendingPutDown();
		return;
	}

	SetPendingWeapon(DesiredWeapon);

	if (GetCurrentWeapon() == DesiredWeapon && !GetCurrentWeapon()->IsInactive())
	{
		return;
	}

	if (!GetCurrentWeapon())
	{
		if (IsLocallyOwnedRemote())
		{
			Server_Reliable_SetCurrentWeapon(DesiredWeapon);
		}

		ChangedWeapon();
		return;
	}

	if(!GetCurrentWeapon()->IsPuttingDown())
	{
		if (GetCurrentWeapon()->PutDown())
		{
			if (IsLocallyOwnedRemote())
			{
				Server_Reliable_SetCurrentWeapon(DesiredWeapon);
			}
		}
	}
}

void UInventoryManagerComponent::WeaponEquipComplete(UWeapon* Weapon)
{
	if (!Weapon || Weapon != GetCurrentWeapon())
	{
		return;
	}

	if (GetCurrentWeapon()->IsPendingPutDown() && GetPendingWeapon())
	{
		SetCurrentWeapon(GetPendingWeapon());
	}
}

void UInventoryManagerComponent::WeaponPutDownComplete(UWeapon* Weapon)
{
	if (Weapon != GetCurrentWeapon())
	{
		return;
	}

	ChangedWeapon();
}

void UInventoryManagerComponent::ChangedWeapon()
{
	if (GetCurrentWeapon() && !GetCurrentWeapon()->IsInactive())
	{
		GetCurrentWeapon()->ForcePutDown();

		if (!GetCurrentWeapon()->IsInactive())
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UInventoryManagerComponent::ChangedWeapon));
			return;
		}
	}

	if (!PendingWeapon || !PendingWeapon->CanEquip())
	{
		PendingWeapon = GetCurrentWeapon() ? GetCurrentWeapon() : GetNextBestWeapon();

		//This is not good. Maybe ask again next tick or something.
		if (!ensure(PendingWeapon && PendingWeapon->CanEquip()))
		{
			return;
		}
	}

	UnBindWeaponEvents();

	CurrentWeapon = PendingWeapon;
	SetPendingWeapon(nullptr);

	BindWeaponEvents();

	CurrentWeapon->Equip();

	OnCurrentWeaponUpdate.Broadcast(CurrentWeapon);

	if (IsLocallyOwnedRemote())
	{
		Server_Reliable_WeaponEquipped(CurrentWeapon);
	}
}

void UInventoryManagerComponent::BindWeaponEvents()
{
	if (!CurrentWeapon)
	{
		return;
	}

	if (CurrentWeapon->OnWeaponPutDownComplete.IsAlreadyBound(this, &UInventoryManagerComponent::WeaponPutDownComplete))
	{
		return;
	}

	CurrentWeapon->OnWeaponEquipComplete.AddDynamic(this, &UInventoryManagerComponent::WeaponEquipComplete);
	CurrentWeapon->OnWeaponPutDownComplete.AddDynamic(this, &UInventoryManagerComponent::WeaponPutDownComplete);
	CurrentWeapon->OnWeaponFireComplete.AddDynamic(this, &UInventoryManagerComponent::WeaponFireComplete);
}

void UInventoryManagerComponent::UnBindWeaponEvents()
{
	if (!CurrentWeapon)
	{
		return;
	}

	if (!CurrentWeapon->OnWeaponPutDownComplete.IsAlreadyBound(this, &UInventoryManagerComponent::WeaponPutDownComplete))
	{
		return;
	}

	CurrentWeapon->OnWeaponEquipComplete.RemoveDynamic(this, &UInventoryManagerComponent::WeaponEquipComplete);
	CurrentWeapon->OnWeaponPutDownComplete.RemoveDynamic(this, &UInventoryManagerComponent::WeaponPutDownComplete);
	CurrentWeapon->OnWeaponFireComplete.RemoveDynamic(this, &UInventoryManagerComponent::WeaponFireComplete);
}

bool UInventoryManagerComponent::Server_Reliable_SetCurrentWeapon_Validate(UWeapon* DesiredWeapon)
{
	return true;
}

void UInventoryManagerComponent::Server_Reliable_SetCurrentWeapon_Implementation(UWeapon* DesiredWeapon)
{
	if (!DesiredWeapon)
	{
		return;
	}

	SetCurrentWeapon(DesiredWeapon);
}

bool UInventoryManagerComponent::Server_Reliable_WeaponEquipped_Validate(UWeapon* DesiredWeapon)
{
	return true;
}

void UInventoryManagerComponent::Server_Reliable_WeaponEquipped_Implementation(UWeapon* DesiredWeapon)
{
	if (!DesiredWeapon || !CanEquipWeapon(DesiredWeapon))
	{
		return;
	}

	if (!GetCurrentWeapon())
	{
		//If there is no current weapon, run through SetCurrentWeapon immediately.
		SetCurrentWeapon(DesiredWeapon);
		return;
	}

	if (GetCurrentWeapon() == DesiredWeapon)
	{
		//If current weapon is the desired one but is pending a put down, do what we can to abort that.
		if (GetCurrentWeapon()->IsPuttingDown())
		{
			SetPendingWeapon(DesiredWeapon);
			GetCurrentWeapon()->AbortPutDown();
		}
		return;
	}

	//Attempt to play catch up with the client if we're still holding onto an older weapon.
	if (GetCurrentWeapon() != DesiredWeapon)
	{
		SetCurrentWeapon(DesiredWeapon);

		if (GetCurrentWeapon() != DesiredWeapon)
		{
			GetCurrentWeapon()->ForcePutDown();
			return;
		}
	}
}

void UInventoryManagerComponent::SetPendingWeapon(UWeapon* DesiredWeapon)
{
	PendingWeapon = DesiredWeapon;
	OnPendingWeaponUpdate.Broadcast(PendingWeapon);
}

void UInventoryManagerComponent::WeaponFireComplete(UWeapon* Weapon)
{
	if (!IsLocallyOwned())
	{
		return;
	}

	if (!GetPendingWeapon())
	{
		return;
	}

	if (!GetCurrentWeapon()->IsPendingPutDown())
	{
		return;
	}

	if (!GetCurrentWeapon()->CanPutDown())
	{
		return;
	}

	SetCurrentWeapon(GetPendingWeapon());
}

void UInventoryManagerComponent::OnRep_InventoryList()
{
	EquipInitialWeapon();

	if (PreviousInventoryList.Num() > InventoryList.Num())
	{
		//Our currently equipped weapon is going through some sort of garbage collection
		if (!CurrentWeapon || !CurrentWeapon->IsValidLowLevelFast() || !CurrentWeapon->IsBeingDestroyed() || !InventoryList.Contains(CurrentWeapon))
		{
			SetCurrentWeapon(GetPendingWeapon() ? GetPendingWeapon() : GetNextBestWeapon());
		}
	}

	for (UInventory* Inventory : InventoryList)
	{
		if (!Inventory)
		{
			continue;
		}

		if (!PreviousInventoryList.Contains(Inventory))
		{
			if (Inventory->OnComponentEndPlay.IsAlreadyBound(this, &UInventoryManagerComponent::OnInventoryEndPlay))
			{
				Inventory->OnComponentEndPlay.AddDynamic(this, &UInventoryManagerComponent::OnInventoryEndPlay);
			}

			OnInventoryAdded.Broadcast(Inventory);
		}
	}

	UpdateWeaponGroupMap();
	OnInventoryUpdate.Broadcast(this);
	PreviousInventoryList = InventoryList;
}

void UInventoryManagerComponent::EquipInitialWeapon()
{
	if (IsLocallyOwned() && !GetCurrentWeapon() && InventoryList.Num() > 0)
	{
		UWeapon* Weapon = NULL;

		for (UInventory* Inventory : InventoryList)
		{
			Weapon = Cast<UWeapon>(Inventory);

			if (!Weapon || !Weapon->CanEquip())
			{
				continue;
			}

			//Retry initial equip next frame if these weapons haven't begun play yet.
			if (!Weapon->HasBegunPlay())
			{
				GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UInventoryManagerComponent::EquipInitialWeapon));
				break;
			}

			SetCurrentWeapon(Weapon);
			break;
		}
	}
}

void UInventoryManagerComponent::UpdateWeaponGroupMap()
{
	TArray<EWeaponGroup> GroupUpdateList;

	TMap<EWeaponGroup, FWeaponGroupArray> NewWeaponGroupMap;
	NewWeaponGroupMap.Reserve(MAXWEAPONGROUP);

	for (UInventory* Inventory : InventoryList)
	{
		UWeapon* Weapon = Cast<UWeapon>(Inventory);
		EWeaponGroup Group = Weapon ? Weapon->GetWeaponGroup() : EWeaponGroup::None;

		if (Group == EWeaponGroup::None)
		{
			continue;
		}

		NewWeaponGroupMap.FindOrAdd(Group).WeaponArray.Add(TWeakObjectPtr<UWeapon>(Weapon));
	}

	for (TPair<EWeaponGroup, FWeaponGroupArray>& GroupEntry : WeaponGroupMap)
	{
		if (!NewWeaponGroupMap.Contains(GroupEntry.Key))
		{
			GroupUpdateList.Add(GroupEntry.Key);
			continue;
		}

		bool bFoundNullWeapon = false;
		for (TWeakObjectPtr<UWeapon> Weapon : GroupEntry.Value)
		{
			if (!Weapon.IsValid())
			{
				bFoundNullWeapon = true;
				break;
			}
		}

		if (bFoundNullWeapon)
		{
			GroupUpdateList.Add(GroupEntry.Key);
			continue;
		}

		bool bFoundNewWeapon = false;
		const TArray<TWeakObjectPtr<UWeapon>>& NewWeaponList = NewWeaponGroupMap[GroupEntry.Key].WeaponArray;

		for (const TWeakObjectPtr<UWeapon>& Weapon : NewWeaponList)
		{
			if (!GroupEntry.Value.WeaponArray.Contains(Weapon))
			{
				bFoundNewWeapon = true;
				break;
			}
		}

		if (bFoundNewWeapon)
		{
			GroupUpdateList.Add(GroupEntry.Key);
			continue;
		}
	}
	
	for (const EWeaponGroup& Group : GroupUpdateList)
	{
		OnInventoryGroupUpdate.Broadcast(Group);
	}

	WeaponGroupMap = MoveTemp(NewWeaponGroupMap);
}

void UInventoryManagerComponent::OnInventoryEndPlay(UCoreCharacterComponent* Component, EEndPlayReason::Type Reason)
{
	UInventory* Inventory = Cast<UInventory>(Component);

	if (!Inventory)
	{
		return;
	}

	OnInventoryRemoved.Broadcast(Inventory);
}

void UInventoryManagerComponent::GetArrayFromWeaponGroup(const FWeaponGroupArray& WeaponGroup, TArray<UWeapon*>& Array)
{
	Array.Reserve(WeaponGroup.WeaponArray.Num());

	for (TWeakObjectPtr<UWeapon> Weapon : WeaponGroup)
	{
		if (!Weapon.IsValid())
		{
			continue;
		}

		Array.Add(Weapon.Get());
	}

	Array.Shrink();
}
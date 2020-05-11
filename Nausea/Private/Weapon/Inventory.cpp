// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#include "Inventory.h"
#include "CoreCharacter.h"
#include "InventoryManagerComponent.h"

UInventory::UInventory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicated(true);
}

void UInventory::BeginPlay()
{
	OwningInventoryManager = GetOwningCharacter()->GetInventoryManager();

	Super::BeginPlay();
}
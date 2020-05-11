// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "CoreCharacterComponent.h"
#include "Inventory.generated.h"


UCLASS(Blueprintable, BlueprintType, HideCategories = (ComponentTick, Collision, Tags, Variable, Activation, ComponentReplication, Cooking))
class NAUSEA_API UInventory : public UCoreCharacterComponent
{
	GENERATED_UCLASS_BODY()

//~ Begin UActorComponent Interface
protected:
	virtual void BeginPlay() override;
//~ End UActorComponent Interface

public:
	UFUNCTION(BlueprintCallable, Category = Inventory)
	FORCEINLINE UInventoryManagerComponent* GetOwningInventoryManager() const { return OwningInventoryManager; }

protected:
	UPROPERTY()
	UInventoryManagerComponent* OwningInventoryManager = nullptr;
};

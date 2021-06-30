// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/CoreCharacterComponent.h"
#include "Inventory.generated.h"


UCLASS(Blueprintable, BlueprintType, HideCategories = (ComponentTick, Collision, Tags, Variable, Activation, ComponentReplication, Cooking))
class NAUSEA_API UInventory : public UCoreCharacterComponent
{
	GENERATED_UCLASS_BODY()

//~ Begin UObject Interface
public:
	virtual void PostInitProperties() override;
//~ End UObject Interface

public:
	UFUNCTION(BlueprintCallable, Category = Inventory)
	FORCEINLINE UInventoryManagerComponent* GetOwningInventoryManager() const { return OwningInventoryManager; }
	
	//Is used as a multiplier for the player's speed. NOTE: UInventoryManagerComponent::RequestMovementSpeedModifierUpdate MUST BE CALLED IF THIS VALUE CHANGES.
	UFUNCTION(BlueprintCallable, Category = Inventory)
	virtual float GetMovementSpeedModifier() const { return MovementSpeedModifier; }

protected:
	UPROPERTY()
	UInventoryManagerComponent* OwningInventoryManager = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	float MovementSpeedModifier = 1.f;
};

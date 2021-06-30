// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Player/PlayerOwnershipInterface.h"
#include "CoreCharacterComponent.generated.h"

class ACoreCharacter;
class AController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FComponentEndPlaySignature, UCoreCharacterComponent*, Component, EEndPlayReason::Type, Reason);

/*
 * Component for player-owned components. Contains helpful API and convenience.
*/
UCLASS()
class NAUSEA_API UCoreCharacterComponent : public UActorComponent, public IPlayerOwnershipInterface
{
	GENERATED_UCLASS_BODY()

//~ Begin UObject Interface
public:
	virtual void PostInitProperties() override;
//~ End UObject Interface

//~ Begin UActorComponent Interface.
protected:
	virtual void BeginPlay() override;
public:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
//~ End UActorComponent Interface.

//~ Begin IPlayerOwnershipInterface Interface.
public:
	virtual ACorePlayerState* GetOwningPlayerState() const override;
	virtual AController* GetOwningController() const override;
	virtual APawn* GetOwningPawn() const override;
//~ End IPlayerOwnershipInterface Interface.

public:
	UFUNCTION(BlueprintCallable, Category = Component)
	bool IsLocallyOwnedRemote() const;

	UFUNCTION(BlueprintCallable, Category = Component)
	bool IsLocallyOwned() const;

	UFUNCTION(BlueprintCallable, Category = Component)
	bool IsSimulatedProxy() const;

	UFUNCTION(BlueprintCallable, Category = Component)
	bool IsAuthority() const;

	UFUNCTION(BlueprintCallable, Category = Component)
	bool IsNonOwningAuthority() const;

	UFUNCTION(BlueprintCallable, Category = Component)
	ACoreCharacter* GetOwningCharacter() const;

public:
	UPROPERTY(BlueprintAssignable, Category = Component)
	FComponentEndPlaySignature OnComponentEndPlay;

protected:
	void UpdateOwningCharacter();

private:
	UPROPERTY()
	ACoreCharacter* OwningCharacter = nullptr;
};
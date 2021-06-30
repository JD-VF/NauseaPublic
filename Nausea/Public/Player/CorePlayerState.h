// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GenericTeamAgentInterface.h"
#include "Player/PlayerOwnershipInterface.h"
#include "Player/PlayerClass/PlayerClassTypes.h"
#include "CorePlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAliveChangedSignature, ACorePlayerState*, PlayerState, bool, bAlive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTeamChangedSignature, ACorePlayerState*, PlayerState, const FGenericTeamId&, NewTeam);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayerClassChangedSignature, ACorePlayerState*, PlayerState, UPlayerClassComponent*, InPlayerClassComponent);

/**
 * 
 */
UCLASS()
class NAUSEA_API ACorePlayerState : public APlayerState, public IGenericTeamAgentInterface, public IPlayerOwnershipInterface
{
	GENERATED_UCLASS_BODY()
	
//~ Begin AActor Interface
public:
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
//~ End AActor Interface
	
//~ Begin IGenericTeamAgentInterface Interface
public:
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamID; }
//~ End IGenericTeamAgentInterface Interface

//~ Begin IPlayerOwnershipInterface Interface
public:
	virtual ACorePlayerState* GetOwningPlayerState() const override { return const_cast<ACorePlayerState*>(this); }
	virtual UPlayerClassComponent* GetPlayerClassComponent() const override { return PlayerClassComponent; }
	virtual FGenericTeamId GetOwningTeamId() const override { return GetGenericTeamId(); }
//~ End IPlayerOwnershipInterface Interface
	

public:
	UFUNCTION(BlueprintCallable, Category = PlayerState)
	bool IsAlive() const { return bIsAlive; }

	void SetIsAlive(bool bInIsAlive);

	virtual void SetPlayerClassComponent(TSubclassOf<UPlayerClassComponent> InPlayerClassComponentClass, EPlayerClassVariant Variant);
	virtual void SetPlayerClassComponent(UPlayerClassComponent* InPlayerClassComponent);

public:
	UPROPERTY(BlueprintAssignable, Category = PlayerState)
	FAliveChangedSignature OnAliveChanged;

	UPROPERTY(BlueprintAssignable, Category = PlayerState)
	FTeamChangedSignature OnTeamChanged;

	UPROPERTY(BlueprintAssignable, Category = PlayerState)
	FPlayerClassChangedSignature OnPlayerClassChanged;

protected:
	//Used internally to set the default value of TeamID in a constructor.
	UFUNCTION()
	void SetDefaultGenericTeamId(const FGenericTeamId& NewTeamID);

	UFUNCTION()
	virtual void OnRep_IsAlive();

	UFUNCTION()
	virtual void OnRep_TeamID();

private:
	//Is this player alive.
	UPROPERTY(ReplicatedUsing = OnRep_IsAlive)
	bool bIsAlive = false;

	UPROPERTY(ReplicatedUsing = OnRep_TeamID)
	FGenericTeamId TeamID;

	UPROPERTY()
	UPlayerClassComponent* PlayerClassComponent = nullptr;
};

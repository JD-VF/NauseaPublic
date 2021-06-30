// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "Player/PlayerOwnershipInterface.h"
#include "Player/PlayerClass/PlayerClassTypes.h"
#include "CorePlayerController.generated.h"

class ANauseaPlayerCameraManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FReceivedPlayerStateSignature, ACorePlayerController*, PlayerController, ACorePlayerState*, PlayerState);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayerPawnUpdatedSignature, ACorePlayerController*, PlayerController, ACoreCharacter*, Pawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FPlayerPawnKilledSignature, ACoreCharacter*, KilledCharacter, TSubclassOf<UDamageType>, DamageType, float, Damage, APlayerState*, KillerPlayerState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FPlayerClassSelectionResponse, ACorePlayerController*, PlayerController, TSubclassOf<UPlayerClassComponent>, RequestedPlayerClass, EPlayerClassVariant, RequestedVariant, EPlayerClassSelectionResponse, RequestResponse, uint8, RequestID);

class UInventory;

USTRUCT()
struct NAUSEA_API FInventorySelectionArray
{
	GENERATED_USTRUCT_BODY()

public:
	typedef TArray<TSoftClassPtr<UInventory>> InventoryArrayType;

	FInventorySelectionArray() {}

	FInventorySelectionArray(const FInventorySelectionArray& InWeaponSelection)
	{
		ConfigArray = InWeaponSelection.ConfigArray;
	}

	FInventorySelectionArray(const InventoryArrayType& InArray)
	{
		ConfigArray = InArray;
	}

private:
	UPROPERTY(Config)
	TArray<TSoftClassPtr<UInventory>> ConfigArray;

public:
	TArray<TSubclassOf<UInventory>> GetInventorySelection() const;
};

USTRUCT()
struct NAUSEA_API FPlayerClassSelectionMap
{
	GENERATED_USTRUCT_BODY()

public:
	FPlayerClassSelectionMap() {}

	FPlayerClassSelectionMap(const FPlayerClassSelectionMap& InSelectionMap)
	{
		VariantSelectionMap = InSelectionMap.VariantSelectionMap;
	}

	FPlayerClassSelectionMap(const TMap<EPlayerClassVariant, FInventorySelectionArray>& InMap)
	{
		VariantSelectionMap = InMap;
	}

public:
	UPROPERTY(Config)
	TMap<EPlayerClassVariant, FInventorySelectionArray> VariantSelectionMap;

public:
	FInventorySelectionArray& operator[] (EPlayerClassVariant Variant) { return VariantSelectionMap[Variant]; }
	const FInventorySelectionArray& operator[] (EPlayerClassVariant Variant) const { return VariantSelectionMap[Variant]; }

	bool Contains(EPlayerClassVariant Variant) const { return VariantSelectionMap.Contains(Variant); }
	int32 Remove(EPlayerClassVariant Variant) { return VariantSelectionMap.Remove(Variant); }
	FInventorySelectionArray& FindOrAdd(EPlayerClassVariant Variant) { return VariantSelectionMap.FindOrAdd(Variant); }
};

/**
 * 
 */
UCLASS(Config=Game)
class NAUSEA_API ACorePlayerController : public APlayerController, public IGenericTeamAgentInterface, public IPlayerOwnershipInterface
{
	GENERATED_UCLASS_BODY()
	
//~ Begin AActor Interface	
protected:
	virtual void BeginPlay() override;
public:
	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;
//~ End AActor Interface

//~ Begin AController Interface
public:
	virtual void OnRep_Pawn() override;
	virtual void SetPawn(APawn* InPawn) override;
protected:
	virtual void OnRep_PlayerState() override;
//~ End AController Interface
	
//~ Begin APlayerController Interface
public:
	virtual void AcknowledgePossession(APawn* InPawn) override;
	virtual void SpawnPlayerCameraManager() override;
	virtual void SetCameraMode(FName NewCamMode) override;
	virtual void ClientSetCameraMode_Implementation(FName NewCamMode) override { SetCameraMode(NewCamMode); }
	virtual void AutoManageActiveCameraTarget(AActor* SuggestedTarget) override;
protected:
	virtual void SetupInputComponent() override;
//~ End APlayerController Interface

//~ Begin IGenericTeamAgentInterface Interface
public:
	virtual FGenericTeamId GetGenericTeamId() const override;
//~ End IGenericTeamAgentInterface Interface

//~ Begin IPlayerOwnershipInterface Interface
public:
	virtual ACorePlayerState* GetOwningPlayerState() const override;
//~ End IPlayerOwnershipInterface Interface

public:
	virtual EPlayerClassSelectionResponse SelectPlayerClass(TSubclassOf<UPlayerClassComponent> PlayerClass, EPlayerClassVariant Variant, uint8& RequestID);

	UFUNCTION(BlueprintCallable, Category = PlayerClass)
	virtual EPlayerClassSelectionResponse CanSelectPlayerClass(TSubclassOf<UPlayerClassComponent> PlayerClass, EPlayerClassVariant Variant) const;

	UFUNCTION(BlueprintCallable, Category = PlayerController)
	virtual void SetInventorySelection(TSubclassOf<UPlayerClassComponent> PlayerClass, EPlayerClassVariant Variant, const TArray<TSubclassOf<UInventory>>& InInventorySelection);
	
	UFUNCTION(BlueprintCallable, Category = PlayerController)
	TArray<TSubclassOf<UInventory>> GetInitialInventorySelection(TSubclassOf<UPlayerClassComponent> PlayerClass, EPlayerClassVariant Variant) const;


	UFUNCTION(BlueprintCallable, Category = PlayerController)
	ACorePlayerCameraManager* GetPlayerCameraManager() const { return CorePlayerCameraManager; }

	void ForceCameraMode(FName NewCameraMode);

	UFUNCTION(exec)
	void ExecSetCameraMode(const FString& NewCameraMode);

public:
	UPROPERTY(BlueprintAssignable, Category = PlayerController)
	FReceivedPlayerStateSignature OnReceivedPlayerState;

	UPROPERTY(BlueprintAssignable, Category = PlayerController)
	FPlayerPawnUpdatedSignature OnPawnUpdated;
	UPROPERTY(BlueprintAssignable, Category = PlayerController)
	FPlayerPawnKilledSignature OnPlayerPawnKilled;

	UPROPERTY(BlueprintAssignable, Category = PlayerController)
	FPlayerClassSelectionResponse OnPlayerClassSelectionResponse;

protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Reliable_SelectInitialPlayerClass(TSubclassOf<UPlayerClassComponent> PlayerClass, EPlayerClassVariant Variant);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Reliable_SendInitialInventorySelection(const TMap<TSoftClassPtr<UPlayerClassComponent>, FPlayerClassSelectionMap>& InitialWeaponSelectionConfig);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Reliable_SendInventorySelection(TSubclassOf<UPlayerClassComponent> PlayerClass, EPlayerClassVariant Variant, const TArray<TSubclassOf<UInventory>>& InInventorySelection);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Reliable_SelectPlayerClass(TSubclassOf<UPlayerClassComponent> PlayerClass, EPlayerClassVariant Variant, uint8 ReqID);

	UFUNCTION(Client, Reliable)
	void Client_Reliable_SelectPlayerClassResponse(TSubclassOf<UPlayerClassComponent> PlayerClass, EPlayerClassVariant Variant, uint8 ReqID, EPlayerClassSelectionResponse Response);

	UFUNCTION()
	void PlayerPawnKilled(UStatusComponent* Component, float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION(Client, Reliable)
	void Client_Reliable_PlayerPawnKilled(ACoreCharacter* KilledCharacter, TSubclassOf<UDamageType> DamageType, float Damage, APlayerState* KillerPlayerState);

	UFUNCTION()
	void ValidateInitialInventorySelection(const TMap<TSoftClassPtr<UPlayerClassComponent>, FPlayerClassSelectionMap>& InInventorySelectionConfig);
	UFUNCTION()
	void UpdateInventorySelectionConfig(const TMap<TSubclassOf<UPlayerClassComponent>, FPlayerClassSelectionMap>& InInventorySelectionConfig);

private:
	UPROPERTY(EditDefaultsOnly, Category = Nausea)
	TSubclassOf<UPlayerClassComponent> DefaultPlayerClass = nullptr;

	UPROPERTY()
	TMap<TSubclassOf<UPlayerClassComponent>, FPlayerClassSelectionMap> PlayerInventorySelection;
	UPROPERTY(GlobalConfig)
	TMap<TSoftClassPtr<UPlayerClassComponent>, FPlayerClassSelectionMap> InventorySelectionConfig;

	UPROPERTY()
	ACorePlayerCameraManager* CorePlayerCameraManager = nullptr;
};

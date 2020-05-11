// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NauseaPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayerPawnUpdatedSignature, ANauseaPlayerController*, PlayerController, ACoreCharacter*, Pawn);

class ANauseaPlayerCameraManager;

/**
 * 
 */
UCLASS()
class NAUSEA_API ANauseaPlayerController : public APlayerController
{
	GENERATED_UCLASS_BODY()

//~ Begin AActor Interface	
public:
	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;
//~ End AActor Interface

//~ Begin AController Interface
public:
	virtual void SetPawn(APawn* InPawn) override;
//~ End AController Interface

//~ Begin APlayerController Interface
public:
	virtual void AcknowledgePossession(APawn* InPawn) override;
	virtual void SpawnPlayerCameraManager() override;
	virtual void SetCameraMode(FName NewCamMode) override;
	virtual void ClientSetCameraMode_Implementation(FName NewCamMode) { SetCameraMode(NewCamMode); }
//~ End APlayerController Interface

public:
	UFUNCTION(BlueprintCallable, Category = PlayerController)
	ANauseaPlayerCameraManager* GetPlayerCameraManager() const;

	void ForceCameraMode(FName NewCameraMode);

	UFUNCTION(exec)
	void ExecSetCameraMode(const FString& NewCameraMode);

public:
	UPROPERTY(BlueprintAssignable, Category = PlayerController)
	FPlayerPawnUpdatedSignature OnPawnUpdated;

private:
	UPROPERTY()
	ANauseaPlayerCameraManager* NauseaPlayerCameraManager = nullptr;
};

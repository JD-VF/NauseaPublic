// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#include "NauseaPlayerController.h"
#include "CoreCharacter.h"
#include "NauseaPlayerCameraManager.h"

ANauseaPlayerController::ANauseaPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerCameraManagerClass = ANauseaPlayerCameraManager::StaticClass();
}

void ANauseaPlayerController::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	if (IsLocalController() && GetPawnOrSpectator())
	{
		GetPawnOrSpectator()->CalcCamera(DeltaTime, OutResult);
		return;
	}

	Super::CalcCamera(DeltaTime, OutResult);
}

void ANauseaPlayerController::SetPawn(APawn* InPawn)
{
	const APawn* PreviousPawn = GetPawn();

	Super::SetPawn(InPawn);

	bool bBroadcastUpdate = false;

	switch (Role)
	{
	case ROLE_Authority:
		bBroadcastUpdate = GetPawn() != PreviousPawn;
		break;
	//Catches non-auth change of pawn to null. All other changes are broadcasted through ANauseaPlayerController::AcknowledgePossession 
	default:
		bBroadcastUpdate = GetPawn() == nullptr;
		break;
	}

	if (bBroadcastUpdate)
	{
		OnPawnUpdated.Broadcast(this, Cast<ACoreCharacter>(InPawn));
	}
}

void ANauseaPlayerController::AcknowledgePossession(APawn* InPawn)
{
	if (Cast<ULocalPlayer>(Player) == nullptr)
	{
		return;
	}

	Super::AcknowledgePossession(InPawn);

	if (Role != ROLE_Authority)
	{
		OnPawnUpdated.Broadcast(this, Cast<ACoreCharacter>(InPawn));
	}
}

void ANauseaPlayerController::SpawnPlayerCameraManager()
{
	Super::SpawnPlayerCameraManager();

	NauseaPlayerCameraManager = Cast<ANauseaPlayerCameraManager>(PlayerCameraManager);
}

void ANauseaPlayerController::SetCameraMode(FName NewCamMode)
{
	//Avoid redundant updates
	if (PlayerCameraManager && PlayerCameraManager->CameraStyle == NewCamMode)
	{
		return;
	}

	if (NauseaPlayerCameraManager)
	{
		NauseaPlayerCameraManager->SetCameraStyle(NewCamMode);
	}

	if (!IsLocalPlayerController())
	{
		ClientSetCameraMode(NewCamMode);
	}
}

ANauseaPlayerCameraManager* ANauseaPlayerController::GetPlayerCameraManager() const
{
	return NauseaPlayerCameraManager;
}

void ANauseaPlayerController::ForceCameraMode(FName NewCameraMode)
{
	Super::SetCameraMode(NewCameraMode);
}

void ANauseaPlayerController::ExecSetCameraMode(const FString& NewCameraMode)
{
	SetCameraMode(FName(*NewCameraMode));
}
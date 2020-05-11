// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#include "NauseaCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "NauseaPlayerController.h"
#include "NauseaPlayerCameraManager.h"
#include "NauseaCameraComponent.h"

ANauseaCharacter::ANauseaCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	FirstPersonCamera = CreateDefaultSubobject<UNauseaCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->RelativeLocation = FVector(0, 0, 64.f); // Position the camera
	FirstPersonCamera->bUsePawnControlRotation = true;

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("FollowCameraBoom"));
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SetupAttachment(GetMesh());

	FollowCamera = CreateDefaultSubobject<UNauseaCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(GetCameraBoom());
	FollowCamera->bUsePawnControlRotation = false;
}

void ANauseaCharacter::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	Super::GetActorEyesViewPoint(OutLocation, OutRotation);
}

void ANauseaCharacter::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult)
{
	if (!GetViewingPlayerController())
	{
		FollowCamera->GetCameraView(DeltaTime, OutResult);
		return;
	}

	if (GetViewingPlayerController()->GetPlayerCameraManager() && GetViewingPlayerController()->GetPlayerCameraManager()->IsFirstPersonCamera())
	{
		FirstPersonCamera->GetCameraView(DeltaTime, OutResult);
		return;
	}

	FollowCamera->GetCameraView(DeltaTime, OutResult);
}
// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#include "CoreCharacter.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "NauseaPlayerController.h"
#include "NauseaPlayerCameraManager.h"
#include "StatusComponent.h"
#include "InventoryManagerComponent.h"

ACoreCharacter::ACoreCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	InventoryManager = CreateDefaultSubobject<UInventoryManagerComponent>(TEXT("InventoryManagerComponent"));

	StatusComponent = CreateDefaultSubobject<UStatusComponent>(TEXT("StatusComponent"));
}

void ACoreCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsNetMode(NM_DedicatedServer))
	{
		CacheMeshList();
	}
}

void ACoreCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ACoreCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	OnTakeDamage.Broadcast(this, DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	return DamageAmount;
}

void ACoreCharacter::BecomeViewTarget(APlayerController* PC)
{
	Super::BecomeViewTarget(PC);

	if (PC && PC->IsLocalController())
	{
		ViewingPlayerController = Cast<ANauseaPlayerController>(PC);
	}

	if (ANauseaPlayerCameraManager* NauseaPlayerCameraManager = GetViewingPlayerController() ? GetViewingPlayerController()->GetPlayerCameraManager() : nullptr)
	{
		if (NauseaPlayerCameraManager->IsFirstPersonCamera())
		{
			SetMeshVisibility(true);
		}
		else
		{
			SetMeshVisibility(false);
		}
	}
}

void ACoreCharacter::EndViewTarget(APlayerController* PC)
{
	Super::EndViewTarget(PC);

	if (PC && PC->IsLocalController())
	{
		ViewingPlayerController = nullptr;
	}

	ResetMeshVisibility();
}

void ACoreCharacter::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult)
{
	Super::CalcCamera(DeltaTime, OutResult);
}

void ACoreCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACoreCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACoreCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ACoreCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACoreCharacter::LookUpAtRate);

	check(InventoryManager);
	InventoryManager->SetupInputComponent(PlayerInputComponent);
}

FRotator ACoreCharacter::GetViewRotation() const
{
	if (GetController() != nullptr)
	{
		return GetController()->GetControlRotation();
	}

	if (Role < ROLE_Authority && ViewingPlayerController && ViewingPlayerController->GetPlayerCameraManager() && !ViewingPlayerController->GetPlayerCameraManager()->IsFirstPersonCamera())
	{
		return ViewingPlayerController->GetControlRotation();
	}

	return GetBaseAimRotation();
}

void ACoreCharacter::Died(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bHasDied)
	{
		return;
	}

	bHasDied = true;

	OnTargetableStateChanged.Broadcast(this, false);
}

bool ACoreCharacter::IsDead() const
{
	if (bHasDied)
	{
		return true;
	}

	return GetStatusComponent()->IsDead();
}

ANauseaPlayerController* ACoreCharacter::GetPlayerController() const
{
	return GetController<ANauseaPlayerController>();
}

ANauseaPlayerController* ACoreCharacter::GetViewingPlayerController() const
{
	return ViewingPlayerController;
}

void ACoreCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACoreCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ACoreCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ACoreCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(Direction, Value);
	}
}

void ACoreCharacter::CacheMeshList()
{
	TInlineComponentArray<UPrimitiveComponent*> Components(this);
	FirstPersonMeshList.Reserve(Components.Num());
	ThirdPersonMeshList.Reserve(Components.Num());

	for (UPrimitiveComponent* Component : Components)
	{
		if (!Component)
		{
			continue;
		}

		if (Component->bOwnerNoSee)
		{
			ThirdPersonMeshList.Add(Component);
			continue;
		}

		if (Component->bOnlyOwnerSee)
		{
			FirstPersonMeshList.Add(Component);
			continue;
		}
	}

	FirstPersonMeshList.Shrink();
	ThirdPersonMeshList.Shrink();
}

void ACoreCharacter::SetMeshVisibility(bool bFirstPerson)
{
	for (TWeakObjectPtr<UPrimitiveComponent> Component : FirstPersonMeshList)
	{
		Component->SetOnlyOwnerSee(bFirstPerson);
		Component->SetOwnerNoSee(!bFirstPerson);
	}

	for (TWeakObjectPtr<UPrimitiveComponent> Component : ThirdPersonMeshList)
	{
		Component->SetOnlyOwnerSee(!bFirstPerson);
		Component->SetOwnerNoSee(bFirstPerson);
	}
}

void ACoreCharacter::ResetMeshVisibility()
{
	for (TWeakObjectPtr<UPrimitiveComponent> Component : FirstPersonMeshList)
	{
		Component->SetOnlyOwnerSee(true);
		Component->SetOwnerNoSee(false);
	}

	for (TWeakObjectPtr<UPrimitiveComponent> Component : ThirdPersonMeshList)
	{
		Component->SetOnlyOwnerSee(false);
		Component->SetOwnerNoSee(true);
	}
}
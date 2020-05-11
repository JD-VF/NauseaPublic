// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "PilotCharacter.h"
#include "NauseaCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS(config=Game)
class ANauseaCharacter : public APilotCharacter
{
	GENERATED_UCLASS_BODY()
		
//~ Begin AActor Interface
public:
	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;
	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;
//~ End AActor Interface

public:
	UFUNCTION(BlueprintCallable, Category = Character)
	UCameraComponent* GetFirstPersonCamera() const { return FirstPersonCamera; }
	UFUNCTION(BlueprintCallable, Category = Character)
	USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	UFUNCTION(BlueprintCallable, Category = Character)
	UCameraComponent* GetFollowCamera() const { return FollowCamera; }


private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCamera = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera = nullptr;
};


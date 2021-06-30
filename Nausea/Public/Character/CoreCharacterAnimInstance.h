// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimInstanceProxy.h"
#include "Character/CoreCharacterAnimInstanceTypes.h"
#include "CoreCharacterAnimInstance.generated.h"

class ACoreCharacter;
class UCoreCharacterMovementComponent;
class UWeapon;
class UAnimSequence;
class UAnimMontage;

USTRUCT(BlueprintType)
struct FCoreCharacterAnimInstanceProxy : public FAnimInstanceProxy
{
	GENERATED_USTRUCT_BODY()

	FCoreCharacterAnimInstanceProxy()
		: FAnimInstanceProxy()
	{

	}

	FCoreCharacterAnimInstanceProxy(UAnimInstance* Instance);

public:
    UPROPERTY(Transient, BlueprintReadOnly, Category = CoreCharacterAnimInstanceProxy)
    float MovementDirection = 0.f;
    UPROPERTY(Transient, BlueprintReadOnly, Category = CoreCharacterAnimInstanceProxy)
    float MovementSpeed = 0.f;

    UPROPERTY(Transient, BlueprintReadOnly, Category = CoreCharacterAnimInstanceProxy)
    bool bMoving = false;
    UPROPERTY(Transient, BlueprintReadOnly, Category = CoreCharacterAnimInstanceProxy)
    bool bRunning = false;

    UPROPERTY(Transient, BlueprintReadOnly, Category = CoreCharacterAnimInstanceProxy)
    bool bCrouching = false;

    UPROPERTY(Transient, BlueprintReadOnly, Category = CoreCharacterAnimInstanceProxy)
    bool bJumping = false;
    UPROPERTY(Transient, BlueprintReadOnly, Category = CoreCharacterAnimInstanceProxy)
    bool bRunJumping = false;


protected:
	virtual void PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds) override;
};

/**
 * 
 */
UCLASS()
class NAUSEA_API UCoreCharacterAnimInstance : public UAnimInstance
{
	GENERATED_UCLASS_BODY()

public:	
	UFUNCTION(BlueprintCallable, Category = Animation)
	ACoreCharacter* GetCharacter() const;

	UFUNCTION(BlueprintCallable, Category = Animation)
	const FLocomotionAnimationContainer& GetLocomotionAnimation() const;
	
	UFUNCTION()
	void SetLocomotionAnimation(const FLocomotionAnimationContainer& InLocomationAnimations);

	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override;
	virtual void DestroyAnimInstanceProxy(FAnimInstanceProxy* InProxy) override;

public:
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	float MinMoveSpeed = 20.f;
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	float MinRunMoveSpeed = 20.f;

protected:
	virtual void NativeInitializeAnimation() override;

	UFUNCTION()
	void OnWeaponChanged(UWeapon* Weapon);

	UFUNCTION()
	void PerformWeaponEquip(UWeapon* Weapon);
	UFUNCTION()
	void PerformWeaponPutDown(UWeapon* Weapon);

	void SetCoreCharacterMovementComponent(const UCoreCharacterMovementComponent* InCoreCharacterMovementComponent)
	{
		CoreCharacterMovementComponent = InCoreCharacterMovementComponent;
	}

	const UCoreCharacterMovementComponent* GetCoreCharacterMovementComponent() const
	{
		return CoreCharacterMovementComponent;
	}

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	bool bIsThirdPerson = false;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	TSubclassOf<UAnimationObject> DefaultLocomationAnimation = nullptr;
	UPROPERTY()
	FLocomotionAnimationContainer DefaultLocomationAnimations;
	UPROPERTY()
	FLocomotionAnimationContainer WeaponLocomationAnimations;

	//Used by anim BP. Set by FCoreCharacterAnimInstanceProxy::PreUpdate() to comply with async work.
	UPROPERTY(Transient, BlueprintReadOnly, Category = Animation)
	FLocomotionAnimationContainer LocomationAnimations;

protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	FCoreCharacterAnimInstanceProxy CoreCharacterProxy;
	
	UPROPERTY(transient)
	const UCoreCharacterMovementComponent* CoreCharacterMovementComponent;
};
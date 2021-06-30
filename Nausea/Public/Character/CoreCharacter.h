// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "Player/PlayerOwnershipInterface.h"
#include "Gameplay/StatusInterface.h"
#include "AI/EnemySelection/AITargetInterface.h"
#include "CoreCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UPrimitiveComponent;
class USkeletalMeshComponent;
class UCoreCharacterComponent;
class UInventoryManagerComponent;
class ACorePlayerController;
class ACorePlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCharacterPossessedSignature, ACoreCharacter*, Character, AController*, Controller);

UCLASS()
class NAUSEA_API ACoreCharacter : public ACharacter, public IGenericTeamAgentInterface, public IPlayerOwnershipInterface, public IStatusInterface, public IAITargetInterface
{
	GENERATED_UCLASS_BODY()

//~ Begin AActor Interface
protected:
	virtual void BeginPlay() override;
public:
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;
	virtual void BecomeViewTarget(APlayerController* PC) override;
	virtual void EndViewTarget(APlayerController* PC) override;
	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;
//~ End AActor Interface

//~ Begin APawn Interface
public:
	virtual void PossessedBy(AController* NewController) override;
	virtual void SetPlayerDefaults() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual FRotator GetViewRotation() const override;
	virtual void RecalculateBaseEyeHeight() override;
//~ End APawn Interface

//~ Begin ACharacter Interface
public:
	virtual void FaceRotation(FRotator NewControlRotation, float DeltaTime) override;
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
//~ End ACharacter Interface

//~ Begin IGenericTeamAgentInterface Interface
public:
	virtual FGenericTeamId GetGenericTeamId() const override;
//~ End IGenericTeamAgentInterface Interface

//~ Begin IPlayerOwnershipInterface Interface
public:
	virtual ACorePlayerState* GetOwningPlayerState() const override;
//~ End IPlayerOwnershipInterface Interface

//~ Begin IStatusInterface Interface
public:
	virtual UStatusComponent* GetStatusComponent() const override { return StatusComponent; }
	virtual void Died(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual bool IsDead() const override;
protected:
	UPROPERTY(VisibleDefaultsOnly, Category = Character)
	UStatusComponent* StatusComponent = nullptr;
//~ End IStatusInterface Interface

//~ Begin IAITargetInterface Interface
public:
	virtual bool IsTargetable(const AActor* Targeter = nullptr) const override { return !IsDead(); }
	virtual FTargetableStateChanged& GetTargetableStateChangedDelegate() override { return OnTargetableStateChanged; }
protected:
	UPROPERTY()
	FTargetableStateChanged OnTargetableStateChanged;
//~ End IAITargetInterface Interface

public:
	UFUNCTION(BlueprintCallable, Category = Character)
	ACorePlayerController* GetPlayerController() const;
	//Returns the player controller locally viewing this pawn (if they are viewing it).
	UFUNCTION(BlueprintCallable, Category = Character)
	ACorePlayerController* GetViewingPlayerController() const;
	UFUNCTION(BlueprintCallable, Category = Character)
	bool IsCurrentlyViewedPawn() const;
	UFUNCTION(BlueprintCallable, Category = Character)
	bool IsCurrentlyFirstPersonViewedPawn() const;
	
	UFUNCTION(BlueprintCallable, Category = Character)
	bool IsCurrentlyReplayingMoves() const { return bClientUpdating; }

	UFUNCTION(BlueprintCallable, Category = Character)
	bool IsFalling() const;
	
	UFUNCTION(BlueprintCallable, Category = Character)
	UCoreCharacterMovementComponent* GetCoreMovementComponent() const { return CoreMovementComponent; }
	UFUNCTION(BlueprintCallable, Category = Character)
	UInventoryManagerComponent* GetInventoryManager() const { return InventoryManager; }
	
	UFUNCTION(BlueprintCallable, Category = Character)
	USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	UFUNCTION(BlueprintCallable, Category = Character)
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	UFUNCTION(BlueprintCallable, Category = Character)
	USkeletalMeshComponent* GetWeaponMesh1P() const { return WeaponMesh1P; }

	UFUNCTION(BlueprintCallable, Category = Character)
	UCameraComponent* GetFirstPersonCamera() const { return FirstPersonCamera; }
	UFUNCTION(BlueprintCallable, Category = Character)
	USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	UFUNCTION(BlueprintCallable, Category = Character)
	UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	void MoveForward(float Value);
	void MoveRight(float Value);

	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	virtual void StartCrouch();
	virtual void StopCrouch();

	UFUNCTION()
	virtual float GetCrouchRate() const { return CrouchRate; }
	UFUNCTION()
	virtual void TickCrouch(float DeltaTime);

protected:
	//Used to cache which meshes are third person meshes and which ones are first person.
	UFUNCTION()
	virtual void CacheMeshList();
	UFUNCTION()
	virtual void SetMeshVisibility(bool bFirstPerson);
	UFUNCTION()
	virtual void ResetMeshVisibility();

public:
	UPROPERTY(BlueprintAssignable, Category = Character)
	FCharacterPossessedSignature OnCharacterPossessed;

protected:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	float BaseLookUpRate;

	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<UPrimitiveComponent>> ThirdPersonMeshList;
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<UPrimitiveComponent>> FirstPersonMeshList;

	UPROPERTY(Transient)
	bool bHasDied = false;

	UPROPERTY(EditDefaultsOnly, Category=Crouch)
	float CrouchRate = 1.f;
	UPROPERTY(Transient)
	float CrouchAmount = 0.f;

private:
	UPROPERTY(Transient)
	UCoreCharacterMovementComponent* CoreMovementComponent = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	UInventoryManagerComponent* InventoryManager = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMesh = nullptr;
	UPROPERTY(VisibleDefaultsOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P = nullptr;
	UPROPERTY(VisibleDefaultsOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMesh1P = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCamera = nullptr;
	UPROPERTY(VisibleDefaultsOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom = nullptr;
	UPROPERTY(VisibleDefaultsOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera = nullptr;
	
	//Local client only. Used to track the controller currently viewing this actor.
	UPROPERTY()
	ACorePlayerController* ViewingPlayerController = nullptr;
};

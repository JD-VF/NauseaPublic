// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "StatusInterface.h"
#include "SelectableEnemyInterface.h"
#include "CoreCharacter.generated.h"

class UPrimitiveComponent;
class USkeletalMeshComponent;
class UInventoryManagerComponent;
class ANauseaPlayerController;

UCLASS()
class NAUSEA_API ACoreCharacter : public ACharacter, public IStatusInterface, public ISelectableEnemyInterface
{
	GENERATED_UCLASS_BODY()

//~ Begin AActor Interface
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void BecomeViewTarget(APlayerController* PC) override;
	virtual void EndViewTarget(APlayerController* PC) override;
	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;
//~ End AActor Interface

//~ Begin APawn Interface
public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual FRotator GetViewRotation() const override;
//~ End APawn Interface

//~ Begin IHeathInterface Interface
public:
	virtual UStatusComponent* GetStatusComponent() const override { return StatusComponent; }
	virtual FActorTakeDamage& GetTakeDamageDelegate() override { return OnTakeDamage; }
	virtual void Died(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual bool IsDead() const override;
protected:
	UPROPERTY(VisibleDefaultsOnly, Category = Character)
	UStatusComponent* StatusComponent = nullptr;
	UPROPERTY()
	FActorTakeDamage OnTakeDamage;
//~ End IHeathInterface Interface

//~ Begin ISelectableEnemyInterface Interface
public:
	virtual bool IsTargetable(const AActor* Targeter = nullptr) const override { return !IsDead(); }
	virtual FTargetableStateChanged& GetTargetableStateChangedDelegate() override { return OnTargetableStateChanged; }
protected:
	UPROPERTY()
	FTargetableStateChanged OnTargetableStateChanged;
//~ End ISelectableEnemyInterface Interface

public:
	UFUNCTION(BlueprintCallable, Category = Character)
	ANauseaPlayerController* GetPlayerController() const;
	
	//Returns the player controller locally viewing this pawn.
	UFUNCTION(BlueprintCallable, Category = Character)
	ANauseaPlayerController* GetViewingPlayerController() const;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

public:
	UFUNCTION(BlueprintCallable, Category = Character)
	USkeletalMeshComponent* GetMesh1P() const { return nullptr; }
	UFUNCTION(BlueprintCallable, Category = Character)
	USkeletalMeshComponent* GetWeaponMesh1P() const { return nullptr; }
	UFUNCTION(BlueprintCallable, Category = Character)
	USkeletalMeshComponent* GetWeaponMesh3P() const { return nullptr; }

	UFUNCTION(BlueprintCallable, Category = Character)
	UInventoryManagerComponent* GetInventoryManager() const { return InventoryManager; }

protected:
	//Used to cache which meshes are third person meshes and which ones are first person.
	UFUNCTION()
	virtual void CacheMeshList();
	UFUNCTION()
	virtual void SetMeshVisibility(bool bFirstPerson);
	UFUNCTION()
	virtual void ResetMeshVisibility();

protected:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<UPrimitiveComponent>> ThirdPersonMeshList;
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<UPrimitiveComponent>> FirstPersonMeshList;

	UPROPERTY(Transient)
	bool bHasDied = false;

protected:
	UPROPERTY(VisibleDefaultsOnly, Category = Character)
	UInventoryManagerComponent* InventoryManager = nullptr;

	//Local client only. Used to track the controller currently viewing this actor.
	UPROPERTY()
	ANauseaPlayerController* ViewingPlayerController = nullptr;
};

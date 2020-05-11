// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Weapon/Inventory.h"
#include "WeaponTypes.h"
#include "Weapon.generated.h"

class UFireMode;
class USkeletalMeshComponent;
class UTexture;
class UUserWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FWeaponStateChangedSignature, UWeapon*, Weapon, EWeaponState, State, EWeaponState, PreviousState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponEquipSignature, UWeapon*, Weapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponPutDownSignature, UWeapon*, Weapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponFireCompleteSignature, UWeapon*, Weapon);

/**
 * 
 */
UCLASS()
class NAUSEA_API UWeapon : public UInventory
{
	GENERATED_UCLASS_BODY()
	
//~ Begin UActorComponent Interface.
protected:
	virtual void BeginPlay() override;
public:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

//Disable all activation-related functionality.
private:
	virtual void Activate(bool bReset = false) override {}
	virtual void Deactivate() override {}
	virtual void SetActive(bool bNewActive, bool bReset = false) override {}
	virtual void ToggleActive() override {}
//~ End UActorComponent Interface.

public:
	UFUNCTION(BlueprintCallable, Category = Weapon)
	EWeaponState GetWeaponState() const { return WeaponState; }

	virtual bool IsActive() const override { return WeaponState == EWeaponState::Active; }
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool IsInactive() const { return WeaponState == EWeaponState::Inactive; }

	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool IsCurrentlyEquippedWeapon() const;
	
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool IsEquipping() const { return WeaponState == EWeaponState::Equipping; }
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool IsPuttingDown() const { return WeaponState == EWeaponState::PuttingDown; }
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool IsPendingPutDown() const { return bPendingPutDown; }
	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual bool IsFiring() const;

	//Mesh and firing effects
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool ShouldPlayEffects1P() const;
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool ShouldPlayEffects3P() const;
	UFUNCTION(BlueprintCallable, Category = Weapon)
	USkeletalMeshComponent* GetMesh1P() const;
	UFUNCTION(BlueprintCallable, Category = Weapon)
	USkeletalMeshComponent* GetWeaponMesh1P() const;
	UFUNCTION(BlueprintCallable, Category = Weapon)
	USkeletalMeshComponent* GetMesh3P() const;
	UFUNCTION(BlueprintCallable, Category = Weapon)
	USkeletalMeshComponent* GetWeaponMesh3P() const;

	//UI and general display info
	UFUNCTION(BlueprintCallable, Category = Weapon)
	TAssetSubclassOf<UTexture> GetInventoryItemImage() const { return InventoryItemImage; }
	UFUNCTION(BlueprintCallable, Category = Weapon)
	TAssetSubclassOf<UUserWidget> GetInventoryItemWidget() const { return InventoryItemWidget; }
	UFUNCTION(BlueprintCallable, Category = Weapon)
	TAssetSubclassOf<UUserWidget> GetCrosshairWidget() const { return CrosshairWidget; }
	UFUNCTION(BlueprintCallable, Category = Weapon)
	TAssetSubclassOf<UUserWidget> GetAmmoWidget() const { return AmmoWidget; }
		
	UFUNCTION(BlueprintCallable, Category = Weapon)
	uint8 GetWeaponPriority() const { return WeaponPriority; }
	UFUNCTION(BlueprintCallable, Category = Weapon)
	EWeaponGroup GetWeaponGroup() const { return WeaponGroup; }

	UFUNCTION()
	virtual bool Fire(EFireMode Mode);
	UFUNCTION()
	virtual bool StopFire(EFireMode Mode);

	UFUNCTION()
	virtual bool Equip();
	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual bool CanEquip() const;
	UFUNCTION()
	virtual void EquipComplete();
	UFUNCTION()
	virtual bool PutDown();
	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual bool CanPutDown() const;
	UFUNCTION()
	virtual void ClearPendingPutDown();
	UFUNCTION()
	virtual void PutDownComplete();
	
	UFUNCTION()
	virtual float GetWeaponRating() const { return 1.f; }

	//----
	//Server synchronization functions. Used to catch the server up to the client. All of these should be only called on NON OWNING AUTHORITY ONLY.
	UFUNCTION()
	virtual void AbortPutDown();
	UFUNCTION()
	virtual void AbortEquip();
	UFUNCTION()
	virtual void ForcePutDown();
	UFUNCTION()
	virtual void ForceEquip();


public:
	UPROPERTY(BlueprintAssignable, Category = Weapon)
	FWeaponStateChangedSignature OnWeaponStateChanged;
	UPROPERTY(BlueprintAssignable, Category = Weapon)
	FWeaponEquipSignature OnWeaponEquipStart;
	UPROPERTY(BlueprintAssignable, Category = Weapon)
	FWeaponEquipSignature OnWeaponEquipComplete;
	UPROPERTY(BlueprintAssignable, Category = Weapon)
	FWeaponPutDownSignature OnWeaponPutDownStart;
	UPROPERTY(BlueprintAssignable, Category = Weapon)
	FWeaponPutDownSignature OnWeaponPutDownComplete;
	UPROPERTY(BlueprintAssignable, Category = Weapon)
	FWeaponFireCompleteSignature OnWeaponFireComplete;

protected:
	UFUNCTION()
	virtual void SetWeaponState(EWeaponState State);

	UFUNCTION()
	virtual void OnRep_WeaponState(EWeaponState PreviousState);

	UFUNCTION()
	UFireMode* GetFireMode(EFireMode Mode) const;

	UFUNCTION()
	void UpdateFireModeList();

	UFUNCTION()
	void FireCompleted(UFireMode* FireMode);


	UFUNCTION(BlueprintImplementableEvent, Category = FireMode, meta = (DisplayName = "On Equip", ScriptName = "OnEquip"))
	void K2_OnEquip();
	UFUNCTION(BlueprintImplementableEvent, Category = FireMode, meta = (DisplayName = "On Equip Complete", ScriptName = "OnEquipComplete"))
	void K2_OnEquipComplete();
	UFUNCTION(BlueprintImplementableEvent, Category = FireMode, meta = (DisplayName = "On Put Down", ScriptName = "OnPutDown"))
	void K2_OnPutDown();
	UFUNCTION(BlueprintImplementableEvent, Category = FireMode, meta = (DisplayName = "On Put Down Complete", ScriptName = "OnPutDownComplete"))
	void K2_OnPutDownComplete();

protected:
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	float EquipTime = 1.f;	
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	float PutDownTime = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = Equip)
	uint8 WeaponPriority = 0;
	UPROPERTY(EditDefaultsOnly, Category = Equip)
	EWeaponGroup WeaponGroup = EWeaponGroup::None;

	UPROPERTY(EditDefaultsOnly, Category = UI)
	TAssetSubclassOf<UTexture> InventoryItemImage;
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TAssetSubclassOf<UUserWidget> InventoryItemWidget;
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TAssetSubclassOf<UUserWidget> CrosshairWidget;
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TAssetSubclassOf<UUserWidget> AmmoWidget;

private:
	UFUNCTION()
	void OnRep_FireModeList();

protected:
	//Publicly-facing firemode list for configuration purposes.
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TMap<EFireMode, TSubclassOf<UFireMode>> WeaponFireModeList;

private:
	UPROPERTY(Transient, ReplicatedUsing = OnRep_WeaponState)
	EWeaponState WeaponState = EWeaponState::Inactive;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_FireModeList)
	TArray<UFireMode*> FireModeList;

	UPROPERTY(Transient)
	bool bPendingPutDown = false;

	UPROPERTY(Transient)
	FTimerHandle EquipTimer;
	UPROPERTY(Transient)
	FTimerHandle PutDownTimer;
};

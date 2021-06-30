// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Player/PlayerClass/PlayerClassTypes.h"
#include "PlayerClassComponent.generated.h"

class ACorePlayerState;

class UPlayerClassSkill;
class UPlayerClassLevelRequirement;

class ACoreCharacter;
class UInventory;
class UWeapon;
class UFireMode;
class UAmmo;
class ULoadedAmmo;

USTRUCT(BlueprintType)
struct FSkillEntry
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString SkillName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
	UPlayerClassSkill* Skill = nullptr;
};

USTRUCT(BlueprintType)
struct FRequirementEntry
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly)
	FString RequirementName;

	UPROPERTY(EditDefaultsOnly, Instanced)
	UPlayerClassLevelRequirement* Requirement = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FLevelChangedSignature, UPlayerClassComponent*, PlayerClass, int32, Level);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSkillListUpdatedSignature, UPlayerClassComponent*, PlayerClass, const TArray<UPlayerClassSkill*>&, SkillList);

DECLARE_DYNAMIC_DELEGATE_OneParam(FSelectedPlayerClassUpdatedSignature, TSubclassOf<UPlayerClassComponent>, SelectedClass);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FSelectedPlayerClassVariantUpdatedSignature, TSubclassOf<UPlayerClassComponent>, SelectedClass, EPlayerClassVariant, Variant);

UCLASS(Blueprintable, BlueprintType/*, HideCategories = (ComponentTick, Collision, Tags, Variable, Activation, ComponentReplication, Cooking)*/)
class NAUSEA_API UPlayerClassComponent : public UActorComponent, public IPlayerOwnershipInterface
{
	GENERATED_UCLASS_BODY()
		
//~ Begin UObject Interface
protected:
	virtual void PostNetReceive() override;
//~ Begin UObject Interface

//~ Begin UActorComponent Interface
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
//~ End UActorComponent Interface

//~ Begin IPlayerOwnershipInterface Interface.
public:
	virtual ACorePlayerState* GetOwningPlayerState() const override;
	virtual AController* GetOwningController() const override;
	virtual APawn* GetOwningPawn() const override;
//~ End IPlayerOwnershipInterface Interface.

public:
	UFUNCTION(BlueprintCallable, Category = Class)
	FText GetTitle() const { return PlayerClassTitle; }

	UFUNCTION(BlueprintCallable, Category = Class)
	FText GetDescription() const { return PlayerClassDescription; }

	UFUNCTION(BlueprintCallable, Category = Class)
	const FLinearColor& GetPlayerClassColour() const;

	UFUNCTION(BlueprintCallable, Category = Class)
	int32 GetLevel() const { return Level; }
	UFUNCTION(BlueprintCallable, Category = Class)
	int32 GetMaxLevel() const { return MaxLevel; }

	UFUNCTION(BlueprintCallable, Category = Class)
	int32 SetLevel(int32 InLevel);

	UFUNCTION(BlueprintCallable, Category = Class)
	EPlayerClassVariant GetVariant() const { return Variant; }
	UFUNCTION()
	void SetVariant(EPlayerClassVariant InVariant);

	UFUNCTION(BlueprintCallable, Category = Class)
	float GetExperienceAmount() const { return 0.f; }

	//Gets percent experience to next left.
	UFUNCTION(BlueprintCallable, Category = Class)
	float GetPercentExperience() const { return 0.f; }

	UFUNCTION(BlueprintCallable, Category = Class)
	const TArray<UPlayerClassSkill*>& GetCoreSkillList() const { return CoreSkills; }
	UFUNCTION(BlueprintCallable, Category = Class)
	const TArray<UPlayerClassSkill*>& GetPrimaryVariantSkillList() const { return PrimarySkills; }
	UFUNCTION(BlueprintCallable, Category = Class)
	const TArray<UPlayerClassSkill*>& GetAlternativeVariantSkillList() const { return AlternativeSkills; }

	UFUNCTION(BlueprintCallable, Category = Class)
	TArray<UPlayerClassSkill*> GetActiveSkillList() const;

	UFUNCTION(BlueprintCallable, Category = Class)
	const TArray<UPlayerClassLevelRequirement*>& GetLevelRequirementList() const { return PlayerClassRequirements; }

	UFUNCTION(BlueprintCallable, Category = Class)
	TArray<TSoftClassPtr<UInventory>> GetDefaultInventoryList() const;

	UFUNCTION(BlueprintCallable, Category = Class)
	void ProcessInitialInventoryList(TArray<TSubclassOf<UInventory>>& InventoryClassList) const;

protected:
	UFUNCTION()
	void OnRep_Level();

	UFUNCTION()
	void OnRep_Variant();
	
	UFUNCTION()
	void OnRep_CoreSkills();
	UFUNCTION()
	void OnRep_PrimarySkills();
	UFUNCTION()
	void OnRep_AlternativeSkills();

	UFUNCTION()
	void AttemptPlayerClassInitialization();

	UFUNCTION()
	void InitializePlayerClass();

public:
	UPROPERTY(BlueprintAssignable, Category = PlayerClass)
	FLevelChangedSignature OnLevelChanged;

	UPROPERTY(BlueprintAssignable, Category = PlayerClass)
	FSkillListUpdatedSignature OnCoreSkillListUpdated;
	UPROPERTY(BlueprintAssignable, Category = PlayerClass)
	FSkillListUpdatedSignature OnPrimarySkillListUpdated;
	UPROPERTY(BlueprintAssignable, Category = PlayerClass)
	FSkillListUpdatedSignature OnAlternativeSkillListUpdated;

//Player class bonus hooks.
public:
	DECLARE_EVENT_TwoParams(UPlayerClassComponent, FWeaponValueModifierSignature, const UWeapon*, float&)
	FWeaponValueModifierSignature OnProcessEquipTime;
	FWeaponValueModifierSignature OnProcessPutDownTime;

	DECLARE_EVENT_ThreeParams(UPlayerClassComponent, FWeaponFireModeValueModifierSignature, const UWeapon*, const UFireMode*, float&)
	FWeaponFireModeValueModifierSignature OnProcessFireRate;

	//float RecoilX, float RecoilY, float RecoilDuration
	DECLARE_EVENT_FiveParams(UPlayerClassComponent, FWeaponRecoilValueModifierSignature, const UWeapon*, const UFireMode*, float&, float&, float&)
	FWeaponRecoilValueModifierSignature OnProcessRecoil;

	DECLARE_EVENT_ThreeParams(UPlayerClassComponent, FWeaponAmmoValueModifierSignature, const UWeapon*, const UAmmo*, float&)
	FWeaponAmmoValueModifierSignature OnProcessAmmoCapacity;

	DECLARE_EVENT_ThreeParams(UPlayerClassComponent, FWeaponLoadedAmmoValueModifierSignature, const UWeapon*, const ULoadedAmmo*, float&)
	FWeaponLoadedAmmoValueModifierSignature OnProcessLoadedAmmoCapacity;
	FWeaponLoadedAmmoValueModifierSignature OnProcessReloadRate;

	DECLARE_EVENT_ThreeParams(UPlayerClassComponent, FWeaponDamageValueModifierSignature, AActor*, const struct FDamageEvent&, float&)
	FWeaponDamageValueModifierSignature OnProcessDamageDealt;
	FWeaponDamageValueModifierSignature OnProcessDamageTaken;

	DECLARE_EVENT_TwoParams(UPlayerClassComponent, FPlayerBasicStatValueModifierSignature, AActor*, float&)
	FPlayerBasicStatValueModifierSignature OnProcessOwnedActorMaxHealth;
	FPlayerBasicStatValueModifierSignature OnProcessOwnedActorMaxArmour;

	//float ArmourAbsorption, float ArmourDecay
	DECLARE_EVENT_FiveParams(UPlayerClassComponent, FArmourDamageModifierSignature, float&, float&, const struct FDamageEvent&, AController*, AActor*)
	FArmourDamageModifierSignature OnProcessArmourDamage;

	DECLARE_EVENT_TwoParams(UPlayerClassComponent, FMovementSpeedModifierSignature, const ACoreCharacter*, float&)
	FMovementSpeedModifierSignature OnProcessMovementSpeed;

protected:
	UPROPERTY(EditDefaultsOnly, Category = UI)
	FText PlayerClassTitle;
	UPROPERTY(EditDefaultsOnly, Category = UI)
	FText PlayerClassDescription;

	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSoftObjectPtr<UTexture2D> PlayerClassIcon;
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSoftObjectPtr<UTexture2D> PlayerClassIconLarge;

	UPROPERTY(EditDefaultsOnly, Category = UI)
	FLinearColor PlayerClassCoreColour;
	UPROPERTY(EditDefaultsOnly, Category = UI)
	FLinearColor PlayerClassColourPrimaryVariant;
	UPROPERTY(EditDefaultsOnly, Category = UI)
	FLinearColor PlayerClassColourAlternativeVariant;

	UPROPERTY(EditDefaultsOnly, Category = UI)
	UMaterialInterface* PlayerClassPrimaryUIBackplate = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = UI)
	UMaterialInterface* PlayerClassAlternativeUIBackplate = nullptr;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_Variant)
	EPlayerClassVariant Variant = EPlayerClassVariant::Invalid;

	UPROPERTY(EditDefaultsOnly, meta=(TitleProperty = "SkillName"), Category = "Core")
	TArray<FSkillEntry> CoreSkillList;
	UPROPERTY(EditDefaultsOnly, meta=(TitleProperty = "SkillName"), Category = "Primary Variant")
	TArray<FSkillEntry> PrimarySkillList;
	UPROPERTY(EditDefaultsOnly, meta=(TitleProperty = "SkillName"), Category = "Alternative Variant")
	TArray<FSkillEntry> AlternativeSkillList;

	UPROPERTY(EditDefaultsOnly, Category = "Core")
	TArray<TSoftClassPtr<UInventory>> CoreDefaultInventory;
	UPROPERTY(EditDefaultsOnly, Category = "Primary Variant")
	TArray<TSoftClassPtr<UInventory>> PrimaryDefaultInventory;
	UPROPERTY(EditDefaultsOnly, Category = "Alternative Variant")
	TArray<TSoftClassPtr<UInventory>> AlternateDefaultInventory;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_CoreSkills)
	TArray<UPlayerClassSkill*> CoreSkills;
	UPROPERTY(Transient, ReplicatedUsing = OnRep_PrimarySkills)
	TArray<UPlayerClassSkill*> PrimarySkills;
	UPROPERTY(Transient, ReplicatedUsing = OnRep_AlternativeSkills)
	TArray<UPlayerClassSkill*> AlternativeSkills;

	//Internal array of all replicating skills for iterating through for replication;
	UPROPERTY(Transient)
	TArray<UPlayerClassSkill*> ReplicatingPlayerClassSkills;

	UPROPERTY(EditDefaultsOnly, meta=(TitleProperty = "RequirementName"), Category = Requirements)
	TArray<FRequirementEntry> LevelRequirementList;

	UPROPERTY(Transient, Replicated)
	TArray<UPlayerClassLevelRequirement*> PlayerClassRequirements;

private:
	UPROPERTY(Transient, ReplicatedUsing = OnRep_Level)
	int32 Level = -1;

	UPROPERTY(EditDefaultsOnly, Category = Class)
	int32 MaxLevel = 10;

	UPROPERTY()
	ACoreCharacter* LastPossessedCharacter = nullptr;

	UPROPERTY(Transient)
	ACorePlayerState* OwningPlayerState = nullptr;

	//Edited on the CDO to save player class selection.
	UPROPERTY(GlobalConfig)
	mutable TSoftClassPtr<UPlayerClassComponent> SelectedPlayerClass;
	UPROPERTY(GlobalConfig)
	mutable TMap<TSoftClassPtr<UPlayerClassComponent>, EPlayerClassVariant> SelectedPlayerClassVariantMap;

	//List of delegates to fire when selection data is changed. Only used on CDO.
	UPROPERTY(Transient)
	mutable TArray<FSelectedPlayerClassUpdatedSignature> SelectedPlayerClassDelegateList;
	UPROPERTY(Transient)
	mutable TArray<FSelectedPlayerClassVariantUpdatedSignature> SelectedPlayerClassVariantDelegateList;

	UPROPERTY(Transient)
	bool bReceivedCoreSkills = false;
	UPROPERTY(Transient)
	bool bReceivedPrimarySkills = false;
	UPROPERTY(Transient)
	bool bReceivedAlternativeSkills = false;

public:
	UFUNCTION(BlueprintCallable, Category = PlayerClass)
	static TArray<UPlayerClassSkill*> GetCoreSkillListOfClass(TSubclassOf<UPlayerClassComponent> PlayerClass);

	UFUNCTION(BlueprintCallable, Category = PlayerClass)
	static TArray<UPlayerClassSkill*> GetVariantSkillListOfClass(TSubclassOf<UPlayerClassComponent> PlayerClass, EPlayerClassVariant PlayerClassVariant);
	
	UFUNCTION(BlueprintCallable, Category = Class)
	static TArray<UPlayerClassSkill*> GetActiveSkillListListOfClass(TSubclassOf<UPlayerClassComponent> PlayerClass, EPlayerClassVariant PlayerClassVariant);

	UFUNCTION(BlueprintCallable, Category = PlayerClass)
	static FText GetPlayerClassTitle(TSubclassOf<UPlayerClassComponent> PlayerClass);
	UFUNCTION(BlueprintCallable, Category = PlayerClass)
	static FText GetVariantName(EPlayerClassVariant PlayerClassVariant);

	UFUNCTION(BlueprintCallable, Category = PlayerClass)
	static FText GetPlayerClassDescription(TSubclassOf<UPlayerClassComponent> PlayerClass);

	UFUNCTION(BlueprintCallable, Category = PlayerClass)
	static TSoftObjectPtr<UTexture2D> GetPlayerClassIcon(TSubclassOf<UPlayerClassComponent> PlayerClass);
	UFUNCTION(BlueprintCallable, Category = PlayerClass)
	static TSoftObjectPtr<UTexture2D> GetPlayerClassIconLarge(TSubclassOf<UPlayerClassComponent> PlayerClass);
	UFUNCTION(BlueprintCallable, Category = PlayerClass)
	static const FLinearColor& GetPlayerClassCoreColour(TSubclassOf<UPlayerClassComponent> PlayerClass);
	UFUNCTION(BlueprintCallable, Category = PlayerClass)
	static const FLinearColor& GetPlayerClassVariantColour(TSubclassOf<UPlayerClassComponent> PlayerClass, EPlayerClassVariant PlayerClassVariant);

	UFUNCTION(BlueprintCallable, Category = PlayerClass)
	static void SetSelectedPlayerClassData(TSubclassOf<UPlayerClassComponent> PlayerClass, EPlayerClassVariant PlayerClassVariant);
	UFUNCTION(BlueprintCallable, Category = PlayerClass)
	static TSoftClassPtr<UPlayerClassComponent> GetSelectedPlayerClass();
	UFUNCTION(BlueprintCallable, Category = PlayerClass)
	static EPlayerClassVariant GetSelectedPlayerClassVariant(TSubclassOf<UPlayerClassComponent> PlayerClass);

	static void CleanCDODelegateList();

	UFUNCTION(BlueprintCallable, Category = PlayerClass)
	static void BindToSelectedPlayerClassUpdate(FSelectedPlayerClassUpdatedSignature Delegate);
	UFUNCTION(BlueprintCallable, Category = PlayerClass)
	static void BindToSelectedPlayerClassVariantUpdate(FSelectedPlayerClassVariantUpdatedSignature Delegate);

	UFUNCTION(BlueprintCallable, Category = PlayerClass, meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext))
	static UMaterialInterface* GetPlayerClassBackplateMaterial(TSubclassOf<UPlayerClassComponent> PlayerClass, EPlayerClassVariant PlayerClassVariant);

	UFUNCTION(BlueprintCallable, Category = PlayerClass, meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext))
	static TArray<TSoftClassPtr<UInventory>> GetDefaultInventoryListFromClass(TSubclassOf<UPlayerClassComponent> PlayerClass, EPlayerClassVariant PlayerClassVariant);
};
// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tickable.h"
#include "Gameplay/StatusType.h"
#include "Player/PlayerClass/PlayerClassTypes.h"
#include "StatusEffectBase.generated.h"

class ANauseaPlayerState;
class UStatusComponent;
class UStatusEffectUserWidget;

UENUM(BlueprintType)
enum class EBasicStatusEffectType : uint8
{
	Instant,
	Cumulative
};

UENUM(BlueprintType)
enum class EStatusEffectStatModifier : uint8
{
	MovementSpeed,
	RotationRate,
	DamageTaken,
	DamageDealt,
	StatusPowerTaken,
	StatusPowerDealt,
	ActionDisabled
};

USTRUCT(BlueprintType)
struct FStatusEffectDelegateEntry
{
	GENERATED_USTRUCT_BODY()

	FStatusEffectDelegateEntry() {}

public:
	FDelegateHandle StatusEffectDelegate;
	UPROPERTY()
	float Value = -1.f;
};

class UStatusEffectBase;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStatusEffectBeginSignature, UStatusEffectBase*, StatusEffect, EStatusBeginType, BeginType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStatusEffectEndSignature, UStatusEffectBase*, StatusEffect, EStatusEndType, EndType);

//Base of all status effects.
UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced, AutoExpandCategories = (StatusEffectObject))
class NAUSEA_API UStatusEffectBase : public UObject
{
	GENERATED_UCLASS_BODY()

//~ Begin UObject Interface
public:
	virtual void PostInitProperties() override;
	virtual void PreDestroyFromReplication() override;
	virtual void BeginDestroy() override;
	virtual UWorld* GetWorld() const override final { return (WorldPrivate ? WorldPrivate : GetWorld_Uncached()); } //UActorComponent's implementation
protected:
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParams, FFrame* Stack) override;
//~ End UObject Interface

public:
	virtual void Initialize(UStatusComponent* StatusComponent, ANauseaPlayerState* Instigator, float Power = -1.f);
	bool IsInitialized() const { return OwningStatusComponent != nullptr; }

	virtual void OnDestroyed();

	virtual bool CanActivateStatus(ANauseaPlayerState* Instigator, float Power) const;
	virtual bool CanRefreshStatus(ANauseaPlayerState* Instigator, float Power) const;

	virtual void AddEffectPower(ANauseaPlayerState* Instigator, float Power = -1.f);
	virtual void OnActivated(EStatusBeginType BeginType);
	virtual void OnDeactivated(EStatusEndType EndType);

	UFUNCTION(BlueprintCallable, Category = StatusEffect)
	bool IsAuthority() const;
	UFUNCTION(BlueprintCallable, Category = StatusEffect)
	bool IsAutonomousProxy() const;
	UFUNCTION(BlueprintCallable, Category = StatusEffect)
	bool IsSimulatedProxy() const;

	UFUNCTION(BlueprintCallable, Category = StatusEffect)
	UStatusComponent* GetOwningStatusComponent() const { return OwningStatusComponent; }

	UFUNCTION(BlueprintCallable, Category = StatusEffect)
	EStatusType GetStatusType() const { return StatusType; }
	UFUNCTION(BlueprintCallable, Category = StatusEffect)
	FText GetStatusEffectName() const { return Name; }
	UFUNCTION(BlueprintCallable, Category = StatusEffect)
	FText GetStatusEffectDescription() const { return Description; }

	UFUNCTION(BlueprintCallable, Category = StatusEffect)
	TSoftClassPtr<UStatusEffectUserWidget> GetStatusEffectWidget() const { return StatusEffectWidget; }
	UFUNCTION(BlueprintCallable, Category = StatusEffect)
	TSoftObjectPtr<UTexture2D> GetStatusEffectIcon() const { return StatusEffectIcon; }

	//Generic progress meter. Can describe how long until a status expires, how long until a stack decreases, how long until a new stack is applied, etc.
	UFUNCTION(BlueprintCallable, Category = StatusEffect)
	virtual float GetStatusEffectProgress() const { return -1.f; }

	UFUNCTION(BlueprintCallable, Category = StatusEffect)
	void SetStatModifier(EStatusEffectStatModifier Stat, float InModifier);

	UFUNCTION(BlueprintCallable, Category = StatusEffect)
	float GetStatModifier(EStatusEffectStatModifier Stat) const;

	UFUNCTION(BlueprintCallable, Category = StatusEffect)
	void ClearStatModifiers();

	UFUNCTION(BlueprintCallable, Category = StatusEffect)
	void RequestMovementSpeedUpdate();
	UFUNCTION(BlueprintCallable, Category = StatusEffect)
	void RequestRotationRateUpdate();

public:
	UPROPERTY(BlueprintAssignable, Category = StatusEffect)
	FStatusEffectBeginSignature OnEffectBegin;
	UPROPERTY(BlueprintAssignable, Category = StatusEffect)
	FStatusEffectEndSignature OnEffectEnd;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = StatusEffect, meta=(DisplayName="On Activated",ScriptName="OnActivated"))
	void K2_OnActivated(EStatusBeginType BeginType);
	UFUNCTION(BlueprintImplementableEvent, Category = StatusEffect, meta=(DisplayName="On Deactivated",ScriptName="OnDeactivated"))
	void K2_OnDeactivated(EStatusEndType EndType);
	UFUNCTION(BlueprintImplementableEvent, Category = StatusEffect, meta=(DisplayName="On Owner Died",ScriptName="OnOwnerDied"))
	void K2_OnOwnerDied(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	UFUNCTION(BlueprintImplementableEvent, Category = StatusEffect, meta=(DisplayName="On Power Changed",ScriptName="OnPowerChanged"))
	void K2_OnPowerChanged(float Power);

	void BindStatModifier(EStatusEffectStatModifier Stat, FStatusEffectDelegateEntry& StatusEffectModifierEntry);
	void UpdateStatModifier(EStatusEffectStatModifier Stat);
	void UnbindStatModifier(EStatusEffectStatModifier Stat, FStatusEffectDelegateEntry& StatusEffectModifierEntry);

	UFUNCTION()
	virtual void OnOwnerDied(UStatusComponent* StatusComponent, float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION()
	virtual void OnRep_RefreshCounter();

protected:
	UPROPERTY(EditDefaultsOnly)
	EStatusType StatusType = EStatusType::Invalid;

	UPROPERTY(EditDefaultsOnly)
	FText Name;
	UPROPERTY(EditDefaultsOnly)
	FText Description;

	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UStatusEffectUserWidget> StatusEffectWidget;
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UTexture2D> StatusEffectIcon;

	UPROPERTY(ReplicatedUsing = OnRep_RefreshCounter)
	uint8 RefreshCounter = 0;

	UPROPERTY()
	TMap<EStatusEffectStatModifier, FStatusEffectDelegateEntry> StatusModificationMap;

private:
	UWorld* GetWorld_Uncached() const;

private:
	UWorld* WorldPrivate = nullptr;

	UPROPERTY()
	UStatusComponent* OwningStatusComponent = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FStatusTimeUpdateSignature, UStatusEffectBase*, StatusEffect, float, StatusStartTime, float, StatusEndTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPowerUpdateSignature, UStatusEffectBase*, StatusEffect, float, Power);

/**
 * 
 */
UCLASS()
class NAUSEA_API UStatusEffectBasic : public UStatusEffectBase, public FTickableGameObject
{
	GENERATED_UCLASS_BODY()

//~ Begin UObject Interface
public:
	virtual void PostInitProperties() override;
	virtual void BeginDestroy() override;
//~ End UObject Interface

//~ Begin UStatusEffectBase Interface
public:
	virtual void Initialize(UStatusComponent* StatusComponent, ANauseaPlayerState* Instigator, float Power) override;
	virtual void OnDestroyed() override;
	virtual void OnActivated(EStatusBeginType BeginType) override;
	virtual void OnDeactivated(EStatusEndType EndType) override;
	virtual bool CanActivateStatus(ANauseaPlayerState* Instigator, float Power) const override;
	virtual bool CanRefreshStatus(ANauseaPlayerState* Instigator, float Power) const override;
	virtual void AddEffectPower(ANauseaPlayerState* Instigator, float Power) override;
//~ End UStatusEffectBase Interface

//~ Begin FTickableGameObject Interface
protected:
	virtual void Tick(float DeltaTime) override;
public:
	virtual ETickableTickType GetTickableTickType() const { return TickType; }
	virtual bool IsTickable() const { return bTickEnabled && !IsPendingKill(); }
	virtual TStatId GetStatId() const { return TStatId(); }
	virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }
//~ End FTickableGameObject Interface

public:
	UFUNCTION(BlueprintCallable, Category = StatusEffect)
	float GetPowerRequirement() const { return EffectPowerRange.X; }

	UFUNCTION()
	float GetDurationAtCurrentPower() const;

	UFUNCTION(BlueprintCallable, Category = StatusEffect)
	float GetPowerPercent() const;

	UFUNCTION(BlueprintCallable, Category = StatusEffect)
	float GetCurrentPower() const { return CurrentPower; }
	UFUNCTION(BlueprintCallable, Category = StatusEffect)
	float GetMaximumPower() const { return EffectPowerRange.Y; }

	UFUNCTION(BlueprintCallable, Category = StatusEffect)
	void GetStatusTime(float& StartTime, float& EndTime) const;

	UFUNCTION(BlueprintCallable, Category = StatusEffect)
	EBasicStatusEffectType GetStatusEffectType() const { return StatusEffectType; }

	UFUNCTION(BlueprintCallable, Category = StatusEffect)
	bool IsCriticalPointReached() const;

public:
	UPROPERTY(BlueprintAssignable, Category = StatusEffect)
	FStatusTimeUpdateSignature OnStatusTimeUpdate;

	UPROPERTY(BlueprintAssignable, Category = StatusEffect)
	FPowerUpdateSignature OnPowerUpdate;

protected:
	UFUNCTION()
	void OnRep_StatusTime();

	UFUNCTION()
	virtual void OnRep_CurrentPower();

	UFUNCTION()
	void SetCriticalPointReached(bool bReached);

	UFUNCTION()
	void OnRep_CriticalPointReached();

	UFUNCTION(BlueprintImplementableEvent, Category = StatusEffect)
	void OnCriticalPointReached(bool bReached);

protected:
	UPROPERTY(ReplicatedUsing = OnRep_StatusTime)
	FVector2D StatusTime = -1.f;

	UPROPERTY()
	FTimerHandle StatusTimer;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentPower)
	float CurrentPower = -1.f;

	UPROPERTY(EditDefaultsOnly)
	EBasicStatusEffectType StatusEffectType = EBasicStatusEffectType::Instant;

	UPROPERTY(EditDefaultsOnly)
	FVector2D EffectDuration = 1.f;
	UPROPERTY(EditDefaultsOnly)
	FVector2D EffectPowerRange = 1.f;

	UPROPERTY(EditDefaultsOnly)
	uint8 bNotifyWhenCriticalPointReached:1;

	UPROPERTY(ReplicatedUsing = OnRep_CriticalPointReached)
	uint8 bCriticalPointReached:1;

	UPROPERTY(EditDefaultsOnly)
	float PowerDecayDelay = 0.f;
	UPROPERTY(EditDefaultsOnly)
	float PowerDecayRate = 0.f;

	UPROPERTY()
	FTimerHandle PowerDecayTimer;

	UPROPERTY(Transient)
	bool bTickEnabled = false;

private:
	ETickableTickType TickType = ETickableTickType::Never;
};
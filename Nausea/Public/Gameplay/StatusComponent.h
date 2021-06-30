// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GenericTeamAgentInterface.h"
#include "Player/PlayerOwnershipInterfaceTypes.h"
#include "StatusType.h"
#include "StatusComponent.generated.h"

class IStatusInterface;
class UStatusEffectBase;
class AActor;
class AController;
class ANauseaPlayerState;
class ACoreCharacter;
class UDamageType;
class UNauseaDamageType;

//Helper struct for health and anything else that would want to be clamped between 0 and a specified maximum. 
USTRUCT(BlueprintType)
struct FStatStruct
{
	GENERATED_USTRUCT_BODY()

	FStatStruct()
	{

	}

	FStatStruct(const float& InValue, const float& InMaxValue)
		: DefaultValue(InValue), DefaultMaxValue(InMaxValue) {}

	FStatStruct(const float& InMaxValue)
		: FStatStruct(InMaxValue, InMaxValue) {}

	operator float() const { return GetValue(); }

	FStatStruct& operator= (const float& InValue) { this->SetValue(InValue); return *this; }
	float operator+ (const float& InValue) { return this->GetValue() + InValue; }
	FStatStruct& operator+= (const float& InValue) { this->SetValue(this->GetValue() + InValue); return *this; }
	float operator- (const float& InValue) { return this->GetValue() - InValue; }
	FStatStruct& operator-= (const float& InValue) { this->SetValue(this->GetValue() - InValue); return *this; }

public:
	FStatStruct Initialize() { MaxValue = DefaultMaxValue; SetValue(DefaultValue < 0.f ? MaxValue : DefaultValue); return *this; }

	float GetValue() const { return Value; }
	float GetMaxValue() const { return MaxValue; }
	float GetPercentValue() const { return Value / MaxValue; }

	float SetValue(float InValue)
	{
		Value = FMath::Clamp(InValue, 0.f, MaxValue);	
		return Value;
	}

	float AddValue(float Delta) { this->SetValue(this->GetValue() + Delta); return Value; }
	float SetMaxValue(float InMaxValue) { MaxValue = InMaxValue; return SetValue(Value); }

	bool IsValid() const { return DefaultValue == -1.f && DefaultMaxValue == -1.f; }

protected:
	UPROPERTY()
	float Value = 0.f;
	UPROPERTY()
	float MaxValue = 100.f;
	
public:
	UPROPERTY(NotReplicated, EditAnywhere, BlueprintReadOnly)
	float DefaultValue = 0.f;
	UPROPERTY(NotReplicated, EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Max Value"))
	float DefaultMaxValue = 100.f;

	static FStatStruct InvalidStat;
};

USTRUCT(BlueprintType)
struct FPartStatStruct
{
	GENERATED_USTRUCT_BODY()

public:
	FPartStatStruct()
	{

	}

	FPartStatStruct(const float& InValue, const float& InMaxValue)
		: StatStruct(InValue, InMaxValue) {}

	operator float() const { return StatStruct; }

	FPartStatStruct& operator= (const float& InValue) { this->StatStruct.SetValue(InValue); return *this; }
	FPartStatStruct& operator+ (const float& InValue) { this->StatStruct.SetValue(this->StatStruct.GetValue() + InValue); return *this; }
	FPartStatStruct& operator+= (const float& InValue) { this->StatStruct.SetValue(this->StatStruct.GetValue() + InValue); return *this; }
	FPartStatStruct& operator- (const float& InValue) { this->StatStruct.SetValue(this->StatStruct.GetValue() - InValue); return *this; }
	FPartStatStruct& operator-= (const float& InValue) { this->StatStruct.SetValue(this->StatStruct.GetValue() - InValue); return *this; }

	float GetValue() const { return this->StatStruct.GetValue(); }
	float GetMaxValue() const { return this->StatStruct.GetMaxValue(); }
	float GetPercentValue() const { return this->StatStruct.GetPercentValue(); }

	bool IsValid() const { return PartName != NAME_None; }

public:
	//Part name returned by UStatusComponent::GetHitBodyPart when provided a hit bone.
	UPROPERTY(NotReplicated, EditAnywhere, BlueprintReadOnly)
	FName PartName = NAME_None;

	//List of bones that will correspond to the part above.
	UPROPERTY(NotReplicated, EditAnywhere, BlueprintReadOnly)
	TArray<FName> BoneList = TArray<FName>();

	//Multiplies incoming damage for this part as well as general health.
	UPROPERTY(NotReplicated, EditAnywhere, BlueprintReadOnly)
	float DamageMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FStatStruct StatStruct = FStatStruct();

	static FPartStatStruct InvalidPartStat;
};

USTRUCT(BlueprintType)
struct FDeathEvent
{
	GENERATED_USTRUCT_BODY()

	FDeathEvent() {}

public:
	UPROPERTY()
	TSubclassOf<UDamageType> DamageType = nullptr;
	UPROPERTY()
	float Damage = 0.f;
	UPROPERTY()
	FVector_NetQuantize HitLocation;
	UPROPERTY()
	FVector_NetQuantize HitMomentum;
};



DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FHealthChangedSignature, UStatusComponent*, Component, float, Health, float, PreviousHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FMaxHealthChangedSignature, UStatusComponent*, Component, float, MaxHealth, float, PreviousMaxHealth);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FArmourChangedSignature, UStatusComponent*, Component, float, Armour, float, PreviousArmour);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FMaxArmourChangedSignature, UStatusComponent*, Component, float, MaxArmour, float, PreviousMaxArmour);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FPartHealthChangedSignature, UStatusComponent*, Component, const FName&, PartName, float, Health, float, PreviousHealth);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FStatusComponentEffectBeginSignature, UStatusComponent*, Component, UStatusEffectBase*, StatusEffect, EStatusBeginType, BeginType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FStatusComponentEffectEndSignature, UStatusComponent*, Component, UStatusEffectBase*, StatusEffect, EStatusEndType, EndType);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FDiedSignature, UStatusComponent*, Component, float, Damage, struct FDamageEvent const&, DamageEvent, AController*, EventInstigator, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FDeathEventSignature, UStatusComponent*, Component, TSubclassOf<UDamageType>, DamageType, float, Damage, FVector_NetQuantize, HitLocation, FVector_NetQuantize, HitMomentum);

UCLASS( ClassGroup=(Custom), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent), HideCategories = (ComponentTick, Collision, Tags, Variable, Activation, ComponentReplication, Cooking, Sockets, UserAssetData))
class NAUSEA_API UStatusComponent : public UActorComponent, public IGenericTeamAgentInterface
{
	GENERATED_UCLASS_BODY()

	//Configuration will perform the initialization so it needs access to all the internals of this class.
	friend class UStatusComponentConfigObject;

//~ Begin UActorComponent Interface 
protected:
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
public:
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
//~ End UActorComponent Interface

//~ Begin IGenericTeamAgentInterface Interface
public:
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }
//~ End IGenericTeamAgentInterface Interface

public:
	UFUNCTION()
	virtual void SetPlayerDefaults();
	UFUNCTION()
	virtual void InitializeStatusComponent();

	UFUNCTION(BlueprintCallable, Category = StatusComponent)
	TScriptInterface<IStatusInterface> GetOwnerInterface() const { return StatusInterface; }

	UFUNCTION(BlueprintCallable, Category = StatusComponent)
	float GetHealth() const { return Health; }
	UFUNCTION(BlueprintCallable, Category = StatusComponent)
	float GetHealthMax() const { return Health.GetMaxValue(); }
	UFUNCTION(BlueprintCallable, Category = StatusComponent)
	float GetHealthPercent() const { return Health.GetPercentValue(); }

	UFUNCTION(BlueprintCallable, Category = StatusComponent)
	bool IsHealthBelowPercent(float Percent) const { return Health.GetPercentValue() < Percent; }

	UFUNCTION(BlueprintCallable, Category = StatusComponent)
	float GetArmour() const { return Armour; }
	UFUNCTION(BlueprintCallable, Category = StatusComponent)
	float GetArmourMax() const { return Armour.GetMaxValue(); }
	UFUNCTION(BlueprintCallable, Category = StatusComponent)
	float GetArmourPercent() const { return Armour.GetPercentValue(); }

	UFUNCTION(BlueprintCallable, Category = StatusComponent)
	bool IsDead() const { return Health <= 0.f; }

	UFUNCTION(BlueprintCallable, Category = StatusComponent)
	const FDeathEvent& GetDeathEvent() const { return DeathEvent; }

	UFUNCTION(BlueprintCallable, Category = StatusComponent)
	float GetMovementSpeedModifier() const;

	UFUNCTION(BlueprintCallable, Category = StatusComponent)
	float GetRotationRateModifier() const;

	UFUNCTION()
	virtual void Kill(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION()
	virtual void Died(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable, Category = StatusComponent)
	FName GetHitBodyPartName(const FName& BoneName) const;

	UFUNCTION(BlueprintCallable, Category = StatusComponent)
	FPartStatStruct& GetPartHealthForBone(const FName& BoneName);

	UFUNCTION()
	int32 GetPartHealthIndexForBone(const FName& BoneName) const;

	void RequestMovementSpeedUpdate() { bUpdateMovementSpeedModifier = true; }
	void RequestRotationRateUpdate() { bUpdateRotationRateModifier = true; }

public:
	UPROPERTY(BlueprintAssignable, Category = StatusComponent)
	FHealthChangedSignature OnHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = StatusComponent)
	FMaxHealthChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = StatusComponent)
	FArmourChangedSignature OnArmourChanged;
	UPROPERTY(BlueprintAssignable, Category = StatusComponent)
	FMaxArmourChangedSignature OnMaxArmourChanged;
	
	UPROPERTY(BlueprintAssignable, Category = StatusComponent)
	FPartHealthChangedSignature OnPartHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = StatusComponent)
	FDiedSignature OnDied;

	UPROPERTY(BlueprintAssignable, Category = StatusComponent)
	FDeathEventSignature OnDeathEvent;

	UPROPERTY(BlueprintAssignable, Category = StatusComponent)
	FStatusComponentEffectBeginSignature OnStatusEffectBegin;
	UPROPERTY(BlueprintAssignable, Category = StatusComponent)
	FStatusComponentEffectEndSignature OnStatusEffectEnd;
	
//Status effect hooks.
public:
	DECLARE_EVENT_TwoParams(UStatusComponent, FStatusComponentValueModifierSignature, const UStatusComponent*, float&)
	FStatusComponentValueModifierSignature OnProcessMovementSpeed;
	FStatusComponentValueModifierSignature OnProcessRotationRate;

	DECLARE_EVENT_ThreeParams(UStatusComponent, FStatusComponentDamageModifierSignature, AActor*, const struct FDamageEvent&, float&)
	FStatusComponentDamageModifierSignature OnProcessDamageTaken;
	FStatusComponentDamageModifierSignature OnProcessDamageDealt;

	DECLARE_EVENT_FourParams(UStatusComponent, FStatusComponentStatusPowerModifierSignature, AActor*, const struct FDamageEvent&, EStatusType, float&)
	FStatusComponentStatusPowerModifierSignature OnProcessStatusPowerTaken;
	FStatusComponentStatusPowerModifierSignature OnProcessStatusPowerDealt;

	DECLARE_EVENT_TwoParams(UStatusComponent, FStatusComponentBooleanValueModifierSignature, const UStatusComponent*, bool&)
	FStatusComponentBooleanValueModifierSignature OnProcessActionDisabled;

protected:
	float SetHealth(float InHealth);
	float SetMaxHealth(float InMaxHealth);
	float SetArmour(float InArmour);
	float SetMaxArmour(float InMaxArmour);

	UFUNCTION()
	virtual void TakeDamage(AActor* Actor, float& DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION()
	virtual void HandlePointDamage(AActor* Actor, float& DamageAmount, struct FPointDamageEvent const& PointDamageEvent, AController* EventInstigator, AActor* DamageCauser);
	UFUNCTION()
	virtual void HandleRadialDamage(AActor* Actor, float& DamageAmount, struct FRadialDamageEvent const& RadialDamageEvent, AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION(BlueprintImplementableEvent, Category = StatusComponent, meta = (DisplayName="Handle Point Damage"))
	void K2_HandlePointDamage(AActor* Actor, float DamageAmount, float& Return, const struct FPointDamageEvent& PointDamageEvent, AController* EventInstigator, AActor* DamageCauser);
	UFUNCTION(BlueprintImplementableEvent, Category = StatusComponent, meta = (DisplayName = "Handle Radial Damage"))
	void K2_HandleRadialDamage(AActor* Actor, float DamageAmount, float& Return, const struct FRadialDamageEvent& PointDamageEvent, AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION()
	virtual void HandleArmourDamage(float& DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	virtual void HandleDamageTypeStatus(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = StatusComponent)
	virtual UStatusEffectBase* AddStatusEffect(TSubclassOf<UStatusEffectBase> StatusEffectClass, struct FDamageEvent const& DamageEvent, AController* EventInstigator, float Power = -1.f);

	UFUNCTION()
	virtual void ReceivedStatusEffectEnd(UStatusEffectBase* StatusEffect, EStatusEndType EndType);

	UFUNCTION()
	virtual void UpdateDeathEvent(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION()
	virtual void OnRep_Health();
	UFUNCTION()
	virtual void OnRep_PartHealthList();

	UFUNCTION()
	virtual void OnRep_Armour();

	UFUNCTION()
	virtual void OnRep_ReplicatedStatusEffectList();
	UFUNCTION()
	void OnRep_ReplicatedStatusListUpdateCounter();

	UFUNCTION()
	virtual void OnRep_TeamId();

	UFUNCTION()
	virtual void OnRep_DeathEvent();

protected:
	UPROPERTY(Transient, ReplicatedUsing = OnRep_Health)
	FStatStruct Health;
	UPROPERTY(Transient, Replicated)
	FVector2D HealthMovementSpeedModifier;
	UPROPERTY(Transient)
	FStatStruct PreviousHealth;


	UPROPERTY(ReplicatedUsing = OnRep_PartHealthList)
	TArray<FPartStatStruct> PartHealthList;

	UPROPERTY(Transient)
	TArray<FPartStatStruct> PreviousPartHealthList;

	UPROPERTY(Transient)
	TMap<FName, int32> BonePartIndexMap;
	

	UPROPERTY(Transient, ReplicatedUsing = OnRep_Armour)
	FStatStruct Armour;
	UPROPERTY(Transient)
	FStatStruct PreviousArmour;
	UPROPERTY(Transient, Replicated)
	FVector2D ArmourAbsorption = FVector2D(1.f, 0.5f);
	UPROPERTY(Transient, Replicated)
	FVector2D ArmourDecay = FVector2D(0.667f, 0.333f);

	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedStatusEffectList)
	TArray<UStatusEffectBase*> ReplicatedStatusEffectList;
	UPROPERTY()
	TArray<TWeakObjectPtr<UStatusEffectBase>> PreviousReplicatedStatusEffectList;

	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedStatusListUpdateCounter)
	uint8 ReplicatedStatusListUpdateCounter = 0;

	UPROPERTY()
	TMap<EStatusType, TSubclassOf<UStatusEffectBase>> GenericStatusEffectMap;

	UPROPERTY(EditDefaultsOnly, Category = StatusComponent)
	TSubclassOf<UStatusComponentConfigObject> StatusConfig;

	//Internal property.
	UPROPERTY()
	bool bHideStatusComponentTeam = false;

	UPROPERTY(EditDefaultsOnly, Category = StatusComponent, meta = (HideCondition = "bHideStatusComponentTeam"))
	ETeam StatusComponentTeam;

	UPROPERTY(ReplicatedUsing = OnRep_TeamId)
	FGenericTeamId TeamId = FGenericTeamId::NoTeam;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_DeathEvent)
	FDeathEvent DeathEvent;

	UPROPERTY(Transient)
	bool bInitializeOnBeginPlay = true;

	UPROPERTY(Transient)
	mutable float CachedMovementSpeedModifier = 1.f;
	UPROPERTY(Transient)
	mutable bool bUpdateMovementSpeedModifier = false;

	UPROPERTY(Transient)
	mutable float CachedRotationRateModifier = 1.f;
	UPROPERTY(Transient)
	mutable bool bUpdateRotationRateModifier = false;

private:
	UPROPERTY(Transient)
	TScriptInterface<IStatusInterface> StatusInterface = nullptr;
};

//Configuration object of a given UStatusComponent. Gives us the ability to leverage OOP principles while configuring/designing UStatusComponent configurations.
UCLASS(BlueprintType, Blueprintable, AutoExpandCategories = (Default, Basic))
class NAUSEA_API UStatusComponentConfigObject : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	virtual void ConfigureStatusComponent(UStatusComponent* StatusComponent) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = Basic)
	FStatStruct Health = FStatStruct(100.f, 100.f);
	UPROPERTY(EditDefaultsOnly, Category = Basic)
	FVector2D HealthMovementSpeedModifier = FVector2D(1.f, 1.f);

	UPROPERTY(EditDefaultsOnly, Category = Basic)
	FStatStruct Armour = FStatStruct(100.f, 100.f);
	UPROPERTY(EditDefaultsOnly, Category = Basic)
	FVector2D ArmourAbsorption = FVector2D(1.f, 0.5f);
	UPROPERTY(EditDefaultsOnly, Category = Basic)
	FVector2D ArmourDecay = FVector2D(0.667f, 0.333f);

	UPROPERTY(EditDefaultsOnly, Category = Advanced)
	TArray<FPartStatStruct> PartHealthList;
	
	UPROPERTY(EditDefaultsOnly, Category = Status)
	TMap<EStatusType, TSubclassOf<UStatusEffectBase>> GenericStatusEffectMap;
};
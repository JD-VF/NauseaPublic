// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Player/PlayerClass/PlayerClassObject.h"
#include "Player/PlayerClass/PlayerClassTypes.h"
#include "PlayerClassSkill.generated.h"

class UPlayerClassComponent;
class UWeapon;
class UFireMode;
class UAmmo;
class ULoadedAmmo;

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced, AutoExpandCategories = (Default))
class NAUSEA_API UPlayerClassSkill : public UPlayerClassObject
{
	GENERATED_UCLASS_BODY()

//~ Begin UPlayerClassObject Interface
public:
	virtual void Initialize(UPlayerClassComponent* OwningComponent) override;
//~ End UPlayerClassObject Interface

public:
	UFUNCTION(BlueprintNativeEvent, Category = Skill)
	void InitializeSkill(UPlayerClassComponent* PlayerClass);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Skill)
	FText GetSkillName() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Skill)
	FText GetSkillDescription() const;

protected:
	UPROPERTY(EditDefaultsOnly)
	FText SkillName = FText();
};

UCLASS(EditInlineNew, AutoExpandCategories = (Default, PassiveSkill))
class NAUSEA_API UPlayerClassSimplePassiveSkill : public UPlayerClassSkill
{
	GENERATED_UCLASS_BODY()

//~ Begin UPlayerClassSkill Interface
public:
	virtual void InitializeSkill_Implementation(UPlayerClassComponent* PlayerClass) override;
	virtual FText GetSkillDescription_Implementation() const override;

//~ End UPlayerClassSkill Interface

	UFUNCTION(BlueprintCallable, Category = PassiveSkill)
	float GetSkillPower() const;

	UFUNCTION(BlueprintCallable, Category = PassiveSkill)
	FText GetSkillPowerForSkillDescription() const;

protected:
	void ApplySkillToValue(float& Value) const;

	UFUNCTION()
	void ProcessWeaponEvent(const UWeapon* Weapon, float& Value);

	UFUNCTION()
	void ProcessFireModeEvent(const UWeapon* Weapon, const UFireMode* FireMode, float& Value);

	UFUNCTION()
	void ProcessRecoilEvent(const UWeapon* Weapon, const UFireMode* FireMode, float& RecoilX, float& RecoilY, float& Rate);

	UFUNCTION()
	void ProcessAmmoEvent(const UWeapon* Weapon, const UAmmo* FireMode, float& Value);

	UFUNCTION()
	void ProcessLoadedAmmoEvent(const UWeapon* Weapon, const ULoadedAmmo* FireMode, float& Value);

	UFUNCTION()
	void ProcessDamageValueEvent(AActor* Target, const struct FDamageEvent& DamageEvent, float& Value);

	UFUNCTION()
	void ProcessCharacterEvent(const ACoreCharacter* Weapon, float& Value);

protected:
	UPROPERTY(EditDefaultsOnly)
	ESimplePassiveSkill PassiveSkill = ESimplePassiveSkill::Invalid;

	UPROPERTY(EditDefaultsOnly)
	FText SkillDescription;
	//Used as an alternative to SkillDescription when the simple passive skill is described as a reduction (determined by ESimplePassiveSkill).
	UPROPERTY(EditDefaultsOnly)
	FText ReductionSkillDescription;

	UPROPERTY(EditDefaultsOnly)
	float BasePower = 0.f;
	UPROPERTY(EditDefaultsOnly)
	float PowerPerLevel = 0.f;

	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* CurveBonus = nullptr;

	UPROPERTY()
	bool bPowerAsReduction = false;

public:
	UFUNCTION(BlueprintCallable, Category = PassiveSkill)
	static FText GetNameOfSimplePassiveSkill(ESimplePassiveSkill SimplePassiveSkill);
};

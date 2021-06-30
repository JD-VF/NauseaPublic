// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Engine/EngineTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StatusInterface.generated.h"

class AActor;
class AController;
class UStatusComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FActorTakeDamage, AActor*, Actor, float&, Damage, struct FDamageEvent const&, DamageEvent, AController*, EventInstigator, AActor*, DamageCauser);

UINTERFACE(Blueprintable)
class UStatusInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class NAUSEA_API IStatusInterface
{
	GENERATED_IINTERFACE_BODY()

public:
	virtual UStatusComponent* GetStatusComponent() const { return K2_GetStatusComponent(); }

	virtual void Died(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) { K2_Died(Damage, DamageEvent, EventInstigator, DamageCauser); }

	virtual bool IsDead() const;

	virtual void Kill(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = StatusInterface, meta = (DisplayName="Get Status Component"))
	UStatusComponent* K2_GetStatusComponent() const;

	UFUNCTION(BlueprintImplementableEvent, Category = StatusInterface, meta = (DisplayName="Died"))
	void K2_Died(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
};

UCLASS(Abstract, MinimalAPI)
class UStatusInterfaceStatics : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = StatusInterface)
	static UStatusComponent* GetStatusComponent(TScriptInterface<IStatusInterface> Target);

	UFUNCTION(BlueprintCallable, Category = StatusInterface)
	static bool IsDead(TScriptInterface<IStatusInterface> Target);
};
// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StatusInterface.generated.h"

class AActor;
class AController;
class UStatusComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FActorTakeDamage, AActor*, Actor, float&, Damage, struct FDamageEvent const&, DamageEvent, AController*, EventInstigator, AActor*, DamageCauser);

UINTERFACE(meta = (CannotImplementInterfaceInBlueprint))
class UStatusInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class NAUSEA_API IStatusInterface
{
	GENERATED_IINTERFACE_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = StatusInterface)
	virtual UStatusComponent* GetStatusComponent() const = 0;

	virtual FActorTakeDamage& GetTakeDamageDelegate() = 0;

	virtual void Died(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) {}

	UFUNCTION(BlueprintCallable, Category = StatusInterface)
	virtual bool IsDead() const;
};
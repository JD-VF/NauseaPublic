// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AITargetInterface.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTargetableStateChanged, AActor*, Actor, bool, bIsTargetable);

UINTERFACE(meta = (CannotImplementInterfaceInBlueprint))
class UAITargetInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class NAUSEA_API IAITargetInterface
{
	GENERATED_IINTERFACE_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = AITargetInterface)
	virtual bool IsTargetable(const AActor* Targeter = nullptr) const { return true; }

	//Must broadcast when implementer can nolonger be targeted.
	virtual FTargetableStateChanged& GetTargetableStateChangedDelegate() = 0;

	//Called when this actor has become a target.
	virtual void OnBecomeTarget(AActor* Targeter) {}
	//Called when this actor is no longer a target.
	virtual void OnEndTarget(AActor* Targeter) {}
};

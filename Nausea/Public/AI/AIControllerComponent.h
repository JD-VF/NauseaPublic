// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIControllerComponent.generated.h"

class ANauseaAIController;
class ACoreCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NAUSEA_API UAIControllerComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

//~ Begin UActorComponent Interface
public:
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
//~ End UActorComponent Interface

public:
	UFUNCTION(BlueprintCallable, Category = AIControllerComponent)
	ANauseaAIController* GetAIController() const { return OwningAIController; }

protected:
	UFUNCTION()
	virtual void OnPawnUpdated(ANauseaAIController* AIController, ACoreCharacter* InCharacter) {}

private:
	UPROPERTY(Transient)
	ANauseaAIController* OwningAIController = nullptr;
};

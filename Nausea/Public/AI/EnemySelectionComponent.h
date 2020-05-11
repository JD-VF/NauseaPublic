// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "AIControllerComponent.h"
#include "EnemySelectionComponent.generated.h"

class ISelectableEnemyInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FEnemyChangedSignature, UEnemySelectionComponent*, EnemySelectionComponent, AActor*, NewEnemy, AActor*, PreviousEnemy);

/*
* Base class for all enemy selection AI systems.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NAUSEA_API UEnemySelectionComponent : public UAIControllerComponent
{
	GENERATED_UCLASS_BODY()

//~ Begin UActorComponent Interface
protected:
	virtual void BeginPlay() override;
//~ End UActorComponent Interface
	
//~ Begin UAIControllerComponent Interface
protected:
	virtual void OnPawnUpdated(ANauseaAIController* AIController, ACoreCharacter* InCharacter) override;
//~ End UAIControllerComponent Interface

public:
	UFUNCTION(BlueprintCallable, Category = EnemySelectionComponent)
	ACoreCharacter* GetOwningCharacter() const { return CurrentCharacter; }

	UFUNCTION(BlueprintCallable, Category = EnemySelectionComponent)
	AActor* GetEnemy() const;

	UFUNCTION()
	virtual AActor* FindBestEnemy() const;

	ISelectableEnemyInterface* GetEnemyInterface() const;

	//Attempts to set enemy to New Enemy. Returns false if fails.
	//If nullptr is passed as New Enemy, will attempt to set enemy as result of FindBestEnemy() instead.
	UFUNCTION(BlueprintCallable, Category = EnemySelectionComponent)
	virtual bool SetEnemy(AActor* NewEnemy, bool bForce = false);
	//Will clear out current enemy. Does not guarantee enemy will stay cleared, however.
	UFUNCTION(BlueprintCallable, Category = EnemySelectionComponent)
	virtual void ClearEnemy();
	UFUNCTION(BlueprintCallable, Category = EnemySelectionComponent)
	virtual bool CanChangeEnemy() const;
	UFUNCTION(BlueprintCallable, Category = EnemySelectionComponent)
	virtual bool CanTargetEnemy(AActor* NewEnemy) const;
	UFUNCTION()
	virtual void EnemyChanged(AActor* PreviousEnemy);
	UFUNCTION(BlueprintImplementableEvent, Category = EnemySelectionComponent, meta = (DisplayName = "On Enemy Changed"))
	void K2_OnEnemyChanged(AActor* PreviousEnemy);

	UFUNCTION(BlueprintCallable, Category = EnemySelectionComponent)
	float GetEnemyChangeCooldownRemaining() const;
	UFUNCTION(BlueprintCallable, Category = EnemySelectionComponent)
	bool IsEnemyChangeCooldownActive() const;

public:
	UPROPERTY(BlueprintAssignable, Category = EnemySelectionComponent)
	FEnemyChangedSignature OnEnemyChanged;

protected:
	UFUNCTION()
	virtual void OnEnemyTargetableStateChanged(AActor* Actor, bool bTargetable);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EnemySelectionComponent, meta = (PinHiddenByDefault, InlineEditConditionToggle))
	bool bEnemyChangeCooldown = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EnemySelectionComponent, meta = (EditCondition = bEnemyChangeCooldown))
	float EnemyChangeCooldown = 4.f; 

private:
	UPROPERTY()
	ACoreCharacter* CurrentCharacter = nullptr;

	UPROPERTY(Transient)
	TScriptInterface<ISelectableEnemyInterface> CurrentEnemy = TScriptInterface<ISelectableEnemyInterface>(nullptr);

	UPROPERTY()
	FTimerHandle EnemyCooldownTimerHandle;
};

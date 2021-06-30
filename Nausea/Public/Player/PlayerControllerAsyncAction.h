// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "PlayerControllerAsyncAction.generated.h"

class ACorePlayerController;

/**
 * 
 */
UCLASS()
class NAUSEA_API UPlayerControllerAsyncAction : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()
	
//~ Begin UBlueprintAsyncActionBase Interface
	virtual void Activate() override;
//~ End UBlueprintAsyncActionBase Interface

protected:
	//Generic failure state. Can be called via UPlayerControllerAsyncAction::Activate if WorldContextObject or OwningPlayerController is not valid.
	virtual void OnFailed();

protected:
	UPROPERTY()
	TWeakObjectPtr<ACorePlayerController> OwningPlayerController;

	UPROPERTY()
	bool bFailed = false;
};

class UPlayerClassComponent;
enum class EPlayerClassSelectionResponse : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPlayerClassSelectedResult, TSubclassOf<UPlayerClassComponent>, PlayerClass, EPlayerClassVariant, Variant, EPlayerClassSelectionResponse, Result);

UCLASS()
class USelectPlayerClassAsyncAction : public UPlayerControllerAsyncAction
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintAssignable)
	FOnPlayerClassSelectedResult OnSelectionResult;

	// Shows the login UI for the currently active online subsystem, if the subsystem supports a login UI.
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Online")
	static USelectPlayerClassAsyncAction* SelectPlayerClass(ACorePlayerController* PlayerController, TSubclassOf<UPlayerClassComponent> PlayerClass, EPlayerClassVariant Variant);

	// UBlueprintAsyncActionBase interface
	virtual void Activate() override;
	// End of UBlueprintAsyncActionBase interface

protected:
	virtual void OnFailed() override;

	void OnComplete(EPlayerClassSelectionResponse Response);

private:
	UFUNCTION()
	void OnPlayerClassSelectionResult(ACorePlayerController* PlayerController, TSubclassOf<UPlayerClassComponent> RequestedPlayerClass, EPlayerClassVariant RequestedVariant, EPlayerClassSelectionResponse RequestResponse, uint8 RequestID);

protected:
	UPROPERTY()
	TSubclassOf<UPlayerClassComponent> RequestedPlayerClass = nullptr;
	UPROPERTY()
	EPlayerClassVariant RequestedPlayerClassVariant;
	UPROPERTY()
	uint8 SelectionRequestID;
};
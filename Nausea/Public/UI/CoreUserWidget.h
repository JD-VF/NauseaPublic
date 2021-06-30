// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CoreUserWidget.generated.h"

class AActor;
class ACorePlayerController;
class ACorePlayerState;
class UPlayerClassComponent;
class ACoreCharacter;
class UNauseaWidgetComponent;

/**
 * 
 */
UCLASS()
class NAUSEA_API UCoreUserWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()
	
//~ Begin UUserWidget Interface
public:
	virtual bool Initialize() override;
//~ End UUserWidget Interface

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Widget")
	ACorePlayerController* GetOwningCorePlayerController() const;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Widget")
	ACoreCharacter* GetOwningPlayerCharacter() const;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Widget")
	void K2_SetWidgetMinimumDesiredSize(const FVector2D& InMinimumDesiredSize);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Widget")
	void OnReceivedPlayerState(ACorePlayerState* PlayerState);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Widget")
	void OnReceivedPlayerClass(UPlayerClassComponent* PlayerClass);

	virtual void InitializeWidgetComponent(UNauseaWidgetComponent* OwningComponent);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Widget")
	void OnReceivedWidgetComponent(UNauseaWidgetComponent* WidgetComponent);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Widget")
	UNauseaWidgetComponent* GetWidgetComponent() const;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Widget")
	bool IsWidgetComponentWidget() const;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Widget")
	AActor* GetWidgetComponentOwner() const;

protected:
	UFUNCTION()
	void ReceivedPlayerState(ACorePlayerController* PlayerController, ACorePlayerState* PlayerState);

	UFUNCTION()
	void ReceivedPlayerClassChange(ACorePlayerState* PlayerState, UPlayerClassComponent* PlayerClass);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	bool bNotifyOnReceivePlayerState = false;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	bool bNotifyOnPlayerClassChanged = false;

private:
	UPROPERTY(Transient)
	UNauseaWidgetComponent* OwningWidgetComponent = nullptr;
};

// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "UI/CoreUserWidget.h"
#include "PawnUserWidget.generated.h"

class ANauseaPlayerController;
class ACoreCharacter;

/**
 * 
 */
UCLASS()
class NAUSEA_API UPawnUserWidget : public UCoreUserWidget
{
	GENERATED_UCLASS_BODY()
	
//~ Begin UUserWidget Interface
public:
	virtual bool Initialize() override;
//~ End UUserWidget Interface

protected:
	UFUNCTION()
	virtual void PossessedPawn(ANauseaPlayerController* PlayerController, ACoreCharacter* Pawn);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = Widget, meta = (DisplayName = "On Possessed Pawn", ScriptName = "OnPossessedPawn"))
	void K2_OnPossessedPawn(ACoreCharacter* Pawn);
};

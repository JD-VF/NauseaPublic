// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CoreUserWidget.generated.h"

class ANauseaPlayerController;

/**
 * 
 */
UCLASS()
class NAUSEA_API UCoreUserWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Widget")
	ANauseaPlayerController* GetOwningNauseaPlayerController() const;
};

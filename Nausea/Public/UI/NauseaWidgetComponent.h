// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "NauseaWidgetComponent.generated.h"

/**
 * Adds context information so that UCoreUserWidgets are able to generically know if they are WidgetComponent widgets and who their owners are.
 */
UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class NAUSEA_API UNauseaWidgetComponent : public UWidgetComponent
{
	GENERATED_UCLASS_BODY()

//~ Begin UWidgetComponent Interface
public:
	virtual void InitWidget() override;
//~ Begin UWidgetComponent Interface
};

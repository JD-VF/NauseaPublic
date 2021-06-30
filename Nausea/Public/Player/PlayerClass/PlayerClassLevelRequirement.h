// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Player/PlayerClass/PlayerClassObject.h"
#include "PlayerClassLevelRequirement.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced, AutoExpandCategories = (Default))
class NAUSEA_API UPlayerClassLevelRequirement : public UPlayerClassObject
{
	GENERATED_UCLASS_BODY()

public:
};

UCLASS()
class NAUSEA_API UPlayerClassSimpleLevelRequirement : public UPlayerClassLevelRequirement
{
	GENERATED_UCLASS_BODY()

};

// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NauseaGameplayStatics.generated.h"

/**
 * 
 */
UCLASS()
class NAUSEA_API UNauseaGameplayStatics : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = GameplayStatics)
	static FString GetNetRoleNameForActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = GameplayStatics)
	static FString GetNetRoleName(ENetRole Role);
};

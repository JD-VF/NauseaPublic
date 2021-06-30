// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "System/NauseaGameMode.h"
#include "FreerunGameMode.generated.h"

/**
 * 
 */
UCLASS()
class NAUSEA_API AFreerunGameMode : public ANauseaGameMode
{
	GENERATED_UCLASS_BODY()
	
//~ Begin AGameModeBase Interface
public:
	virtual void ResetLevel() override { ensure(false); } //AFreerunGameState::ResetLevel now does this behaviour via match states.
//~ End AGameModeBase Interface
};

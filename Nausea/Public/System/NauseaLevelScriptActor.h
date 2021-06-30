// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "NauseaLevelScriptActor.generated.h"

class UMapDataAsset;

/**
 * 
 */
UCLASS()
class NAUSEA_API ANauseaLevelScriptActor : public ALevelScriptActor
{
	GENERATED_UCLASS_BODY()
	
public:
	//Generates level missions. Returns starting missing.
	UFUNCTION(BlueprintImplementableEvent, Category = LevelScriptActor)
	class UMissionComponent* GenerateLevelMissions();

	/** Returns the current GameStateBase or Null if it can't be retrieved */
	UFUNCTION(BlueprintPure, Category="Game", meta=(WorldContext="WorldContextObject"))
	static class ANauseaGameState* GetNauseaGameState(const UObject* WorldContextObject);
};

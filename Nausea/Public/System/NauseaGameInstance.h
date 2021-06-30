// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "NauseaGameInstance.generated.h"

class UWorld;
class UMapDataAsset;

/**
 * 
 */
UCLASS()
class NAUSEA_API UNauseaGameInstance : public UGameInstance
{
	GENERATED_UCLASS_BODY()
	
//~ Begin UGameInstance Interface
protected:
	virtual void Init() override;
public:
	virtual void Shutdown() override;
//~ End UGameInstance Interface

public:
	UFUNCTION()
	void LoadMapDataList(bool bImmediate);

	const TMap<TSoftObjectPtr<UWorld>, UMapDataAsset*>& GetMapDataAssetMap() const { return MapDataAssetMap; }
	UMapDataAsset* GetMapDataAsset(TSoftObjectPtr<UWorld> World) const;
	UMapDataAsset* GetMapDataAssetForCurrentWorld() const;

	UFUNCTION(BlueprintCallable, Category = GameInstance, meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext))
	static const TMap<TSoftObjectPtr<UWorld>, UMapDataAsset*>& GetMapDataAssetMap(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = GameInstance, meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext))
	static TArray<UMapDataAsset*> GetMapDataAssetList(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = GameInstance, meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext))
	static UMapDataAsset* GetCurrentMapDataAsset(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = GameInstance, meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext))
	static UMapDataAsset* GetMapDataAsset(const UObject* WorldContextObject, TSoftObjectPtr<UWorld> World);

protected:
	UPROPERTY()
	bool bIsMapDataListReady = false;

	UPROPERTY()
	TMap<TSoftObjectPtr<UWorld>, UMapDataAsset*> MapDataAssetMap;

	UPROPERTY()
	TArray<FPrimaryAssetId> PendingMapDataAssetList;

	static const TMap<TSoftObjectPtr<UWorld>, UMapDataAsset*> InvalidMapDataAssetMap;
};

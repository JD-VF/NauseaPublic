// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "NauseaCameraComponent.generated.h"

/**
 * 
 */
UCLASS()
class NAUSEA_API UNauseaCameraComponent : public UCameraComponent
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;
};

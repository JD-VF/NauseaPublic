// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "NauseaPlayerCameraManager.generated.h"

//Redefinition of APlayerCameraManager's camera styles in a way that is accessible
namespace CameraStyleName
{
	extern NAUSEA_API const FName NAME_Default;
	extern NAUSEA_API const FName NAME_Fixed;
	extern NAUSEA_API const FName NAME_ThirdPerson;
	extern NAUSEA_API const FName NAME_FreeCam;
	extern NAUSEA_API const FName NAME_FreeCam_Default;
	extern NAUSEA_API const FName NAME_FirstPerson;
}

class ANauseaPlayerController;

/**
 * 
 */
UCLASS()
class NAUSEA_API ANauseaPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_UCLASS_BODY()
		
//~ Begin ANauseaPlayerCameraManager Interface
public:
	virtual void SetViewTarget(class AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams = FViewTargetTransitionParams()) override;
	virtual void AssignViewTarget(AActor* NewTarget, FTViewTarget& VT, struct FViewTargetTransitionParams TransitionParams = FViewTargetTransitionParams()) override;
protected:
	virtual void UpdateViewTarget(FTViewTarget& OutViewTarget, float DeltaTime) override;
//~ End ANauseaPlayerCameraManager Interface

public:
	UFUNCTION(BlueprintCallable, Category = PlayerCameraManager)
	bool IsDefaultCamera() const { return CameraStyle == CameraStyleName::NAME_Fixed; }
	UFUNCTION(BlueprintCallable, Category = PlayerCameraManager)
	bool IsFixedCamera() const { return CameraStyle == CameraStyleName::NAME_Fixed; }
	UFUNCTION(BlueprintCallable, Category = PlayerCameraManager)
	bool IsThirdPersonCamera() const { return CameraStyle == CameraStyleName::NAME_ThirdPerson; }
	UFUNCTION(BlueprintCallable, Category = PlayerCameraManager)
	bool IsFreeCamera() const { return CameraStyle == CameraStyleName::NAME_FreeCam; }
	UFUNCTION(BlueprintCallable, Category = PlayerCameraManager)
	bool IsDefaultFreeCamera() const { return CameraStyle == CameraStyleName::NAME_FreeCam_Default; }
	UFUNCTION(BlueprintCallable, Category = PlayerCameraManager)
	bool IsFirstPersonCamera() const { return CameraStyle == CameraStyleName::NAME_FirstPerson; }

	UFUNCTION(BlueprintCallable, Category = PlayerCameraManager)
	ANauseaPlayerController* GetPlayerController() const;

	UFUNCTION()
	void SetCameraStyle(const FName& InCameraStyle);

protected:
	UFUNCTION()
	void PostUpdateViewTarget(FTViewTarget& OutViewTarget, bool bApplyModifiers, float DeltaTime);
};

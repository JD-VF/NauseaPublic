// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#include "CoreUserWidget.h"
#include "NauseaPlayerController.h"

UCoreUserWidget::UCoreUserWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

FORCEINLINE ANauseaPlayerController* UCoreUserWidget::GetOwningNauseaPlayerController() const
{
	return GetOwningPlayer<ANauseaPlayerController>();
}
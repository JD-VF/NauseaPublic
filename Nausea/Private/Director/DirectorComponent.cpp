// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#include "DirectorComponent.h"

UDirectorComponent::UDirectorComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}
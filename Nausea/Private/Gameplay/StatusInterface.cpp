// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#include "StatusInterface.h"
#include "StatusComponent.h"

UStatusInterface::UStatusInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

bool IStatusInterface::IsDead() const
{
	return GetStatusComponent()->IsDead();
}
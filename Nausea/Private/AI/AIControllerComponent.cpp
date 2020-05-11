// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#include "AIControllerComponent.h"
#include "NauseaAIController.h"

UAIControllerComponent::UAIControllerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UAIControllerComponent::OnRegister()
{
	Super::OnRegister();

	OwningAIController = Cast<ANauseaAIController>(GetOwner());

	ensure(OwningAIController);

	OwningAIController->OnPawnUpdated.AddDynamic(this, &UAIControllerComponent::OnPawnUpdated);
}

void UAIControllerComponent::OnUnregister()
{
	Super::OnUnregister();

	if (!OwningAIController)
	{
		OwningAIController = Cast<ANauseaAIController>(GetOwner());

		if (!OwningAIController)
		{
			return;
		}
	}

	OwningAIController->OnPawnUpdated.RemoveDynamic(this, &UAIControllerComponent::OnPawnUpdated);
}
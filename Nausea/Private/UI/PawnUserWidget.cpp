// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#include "PawnUserWidget.h"
#include "NauseaPlayerController.h"
#include "CoreCharacter.h"

UPawnUserWidget::UPawnUserWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

bool UPawnUserWidget::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (!GetPlayerContext().IsValid() || !GetPlayerContext().IsInitialized())
	{
		return true;
	}

	if (!ensure(GetOwningNauseaPlayerController()))
	{
		return true;
	}

	GetOwningNauseaPlayerController()->OnPawnUpdated.AddDynamic(this, &UPawnUserWidget::PossessedPawn);

	if (ACoreCharacter* Pawn = Cast<ACoreCharacter>(GetOwningPlayer()->GetPawn()))
	{
		if (Pawn == GetOwningPlayer()->AcknowledgedPawn)
		{
			PossessedPawn(GetOwningNauseaPlayerController(), Pawn);
		}
	}

	return true;
}

void UPawnUserWidget::PossessedPawn(ANauseaPlayerController* PlayerController, ACoreCharacter* Pawn)
{
	K2_OnPossessedPawn(Pawn);
}
// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#include "CoreCharacterComponent.h"
#include "CoreCharacter.h"

UCoreCharacterComponent::UCoreCharacterComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCoreCharacterComponent::PostInitProperties()
{
	Super::PostInitProperties();

	if (HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))
	{
		return;
	}

	//Some things want to know what this is very early so we have to handle that.
	OwningCharacter = GetTypedOuter<ACoreCharacter>();
}

void UCoreCharacterComponent::BeginPlay()
{
	OwningCharacter = GetTypedOuter<ACoreCharacter>();

	Super::BeginPlay();
}

void UCoreCharacterComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	OnComponentEndPlay.Broadcast(this, EndPlayReason);
}

bool UCoreCharacterComponent::IsLocallyOwnedRemote() const
{
	return GetOwnerRole() == ROLE_AutonomousProxy;
}

bool UCoreCharacterComponent::IsSimulatedProxy() const
{
	return GetOwnerRole() < ROLE_AutonomousProxy;
}

bool UCoreCharacterComponent::IsAuthority() const
{
	return GetOwnerRole() == ROLE_Authority;
}

bool UCoreCharacterComponent::IsNonOwningAuthority() const
{
	return IsAuthority() && !IsLocallyOwned();
}

ACoreCharacter* UCoreCharacterComponent::GetOwningCharacter() const
{
	return OwningCharacter;
}

bool UCoreCharacterComponent::IsLocallyOwned() const
{
	return IsLocallyOwnedRemote() || (GetOwningCharacter() ? GetOwningCharacter()->IsLocallyControlled() : false);
}
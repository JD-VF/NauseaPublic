// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.

#pragma once

#if WITH_EDITOR
#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "UObject/WeakObjectPtr.h"
#include "Layout/Visibility.h"
#include "IDetailCustomization.h"

/**
 * 
 */
class NAUSEA_API FNauseaDetails : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

//~ Begin IDetailCustomization Interface 
public:
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;
//~ End IDetailCustomization Interface 

};
#endif //WITH_EDITOR
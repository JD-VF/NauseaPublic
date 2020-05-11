// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#pragma once

#include "StatusType.generated.h"

UENUM(BlueprintType)
enum class EStatusType : uint8
{
	Slow,
	Stun,
	Flinch,
	Burn,
	Test,
	MAX
};
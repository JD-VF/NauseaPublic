#pragma once

#include "StatusType.generated.h"

UENUM(BlueprintType)
enum class EStatusType : uint8
{
	Invalid,
	Slow,
	Stun,
	Flinch,
	Burn,
	Poison,
	NonGeneric,
	MAX
};

UENUM(BlueprintType)
enum class EStatusBeginType : uint8
{
	Invalid,
	Initial,
	Refresh
};

UENUM(BlueprintType)
enum class EStatusEndType : uint8
{
	Invalid,
	Expired,
	Interrupted
};
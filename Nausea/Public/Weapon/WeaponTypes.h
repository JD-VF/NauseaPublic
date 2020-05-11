// Copyright 2019-2020 Jean-David Veilleux-Foppiano. All Rights Reserved.


#pragma once

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	Active, //Idle
	Inactive, //Down
	Equipping, //Equipping
	PuttingDown, //Putting Away
	Custom, //Other - Used with UWeapon::CustomWeaponState.
	None = 255
};

UENUM(BlueprintType)
enum class EFireMode : uint8
{
	Primary,
	Secondary,
	Tertiary,
	Quaternary,
	Quinary,
	MAX
};

const int32 MAXFIREMODES = int32(EFireMode::MAX);

UENUM(BlueprintType)
enum class EWeaponGroup : uint8
{
	Melee,
	Pistol,
	SMG,
	Rifle,
	Special,
	Utility,
	None
};

const int32 MAXWEAPONGROUP = int32(EWeaponGroup::None);

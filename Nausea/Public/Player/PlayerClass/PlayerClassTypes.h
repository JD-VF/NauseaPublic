#pragma once

UENUM(BlueprintType)
enum class EPlayerClassVariant : uint8
{
	Invalid,
	Primary,
	Alternative,
	Special
};

UENUM(BlueprintType)
enum class EPlayerClassSelectionResponse : uint8
{
	//Failed Responses
	Invalid,
	NotAvailable,
	//Success Responses
	Success,
	Pending,
	AlreadySelected
};

UENUM(BlueprintType)
enum class ESimplePassiveSkill : uint8
{
	Invalid,
	EquipRate,
	FireRate,
	Recoil,
	ReloadRate,
	AmmoCapacity,
	LoadedAmmoCapacity,
	MovementSpeed,
	DamageTaken,
	DamageDealt,
	StatusPowerTaken,
	StatusPowerDealt
};
#pragma once
UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EAT_9MM		UMETA(DisplayName = "9mm Ammo"),
	EAT_AR		UMETA(DisplayName = "Assault Rifle Ammo"),

	EAT_MAX		UMETA(DisplayName = "Default MAX"),

};
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "AmmoTypes.h"
#include "Engine/DataTable.h"
#include "WeaponType.h"
#include "Weapon.generated.h"

/**
 * 
 */
class ThrowWeaponTimer;
class USoundCue;


USTRUCT(BlueprintType)
struct FWeaponDataTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAmmoType AmmoType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WeaponAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MagazineCapacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* PickupSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* EquipSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetComponent* WidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* ItemMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* InventoryIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* AmmoIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance* MaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaterialIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MagBoneName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ReloadMontageName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> AnimBP;

	//Centerdot of Crosshair
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairMiddle;
	//Left Part of Crosshair
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairLeft;
	//Right Part of Crosshair
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairRight;
	//Bottom Part of Crosshair
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairBottom;
	//Top part of Crosshair
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairTop;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AutoFireRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* MuzzleFlash;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* FireSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BoneToHide;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAutomatic;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HeadshotDamage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinCrosshairSpread;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InAirCrosshairSpread;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AimingCrosshairSpread;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireingCrosshairSpread;
};
UCLASS()
class SHOOTER_API AWeapon : public AItem
{
	GENERATED_BODY()
private:
	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime;
	
	bool bIsFalling;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	int32 Ammo;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	EAmmoType AmmoType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	int32 MagazineCapacity;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	bool bMovingMag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	FName ReloadMontageSection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	FName MagBoneName;

	//DataTable for Weapon Properties
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	UDataTable* WeaponDataTable;

	int32 PreviousMaterialIndex;

	//Centerdot of Crosshair
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
		UTexture2D* CrosshairMiddle;
	//Left Part of Crosshair
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
		UTexture2D* CrosshairLeft;
	//Right Part of Crosshair
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
		UTexture2D* CrosshairRight;
	//Bottom Part of Crosshair
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
		UTexture2D* CrosshairBottom;
	//Top part of Crosshair
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
		UTexture2D* CrosshairTop;

	//Speed at which automatic fire happens
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	float AutoFireRate;
	//Partile system spawn at barrel socket
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
		UParticleSystem* MuzzleFlash;
	//Sound weapon makes when fireing
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	USoundCue* FireSound;

	//Name of the bone to hide on the weapon mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	FName BoneToHide;

	//Amount when slide is pushed buck during pistol fire
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pistol", meta =(AllowPrivateAccess = "true"))
	float SlideDisplacement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* SlideDisplacementCurve;
	//Timer Handle for updating slide displacement
	FTimerHandle SlideTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	float SlideDisplacementTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	float MaxRecoilPitch;
	//Recoil pitch at the current frame
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	float RecoilPitch;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	bool bMovingSlide;

	//Max Distance for the slide on the pistol to move
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	float MaxSlideDisplacement;

	//true for automatic gun fire;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		bool bAutomatic;
	//Damage caused by a bullet
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	float Damage;
	
	//Damage when bullet hits the head
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	float HeadshotDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	float MinCrosshairSpread;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	float InAirCrosshairSpread;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	float AimingCrosshairSpread;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	float FireingCrosshairSpread;
protected:
	
	void StopFalling();
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

	void FinishMovingSlide();
	void UpdateSlideDisplacement();
public:
	AWeapon();
	void ThrowWeapon();
	//Decrement ammo when fireing weapon;
	void DecrementAmmo();
	virtual void Tick(float DeltaTime);

	FORCEINLINE int32 GetAmmo()const { return Ammo; }
	FORCEINLINE EWeaponType GetWeaponType() const{ return WeaponType; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
	FORCEINLINE int32 GetMagCapacity()const { return MagazineCapacity; }
	FORCEINLINE FName GetReloadMontageName() const { return ReloadMontageSection; }
	FORCEINLINE FName GetMagBoneName() const { return MagBoneName; }

	FORCEINLINE float GetAutoFireRate()const { return AutoFireRate; }
	FORCEINLINE UParticleSystem* GetMuzzleFlash() const { return MuzzleFlash; }
	FORCEINLINE USoundCue* GetFireSound() const { return FireSound; }

	FORCEINLINE bool GetAutomatic()const { return bAutomatic; }

	FORCEINLINE float GetDamage()const { return Damage; }
	FORCEINLINE float GetHeadshotDamage()const { return HeadshotDamage; }


	FORCEINLINE void SetMovingMag(bool Move) { bMovingMag = Move; }

	FORCEINLINE float GetMinSpread() const { return MinCrosshairSpread; }
	FORCEINLINE float GetInAirSpread() const { return InAirCrosshairSpread; }
	FORCEINLINE float GetAimingSpread() const { return AimingCrosshairSpread; }
	FORCEINLINE float GetFireingSpread() const { return FireingCrosshairSpread; }

	void StartSlidingTimer();
	void ReloadAmmo(int32 Amount);
	
	bool bMagisFull();
	
};

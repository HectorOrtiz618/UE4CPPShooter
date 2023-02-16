// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

AWeapon::AWeapon()
{
	ThrowWeaponTime = 0.7f;
	bIsFalling = false;

	PrimaryActorTick.bCanEverTick = true;
	Ammo = 30;
	MagazineCapacity = 30;
	AmmoType = EAmmoType::EAT_9MM;

	ReloadMontageSection = "ReloadSMG";
	MagBoneName = FName("smg_clip");
	bMovingMag = false;

	SlideDisplacement = 0.f;
	SlideDisplacementTime = 0.1f;

	bMovingSlide = false;
	MaxSlideDisplacement = 4.0f;
	MaxRecoilPitch = 20.f;
	bAutomatic = true;
}
void AWeapon::DecrementAmmo()
{
	if (Ammo - 1 <= 0)
	{
		Ammo = 0;
	}
	else
	{
		Ammo -= 1;
	}
}
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//Keep Weapon upright
	if (GetItemState() == EItemState::EIS_Falling && bIsFalling)
	{
		const FRotator MeshRotation{ 0.f,GetItemMesh()->GetComponentRotation().Yaw,0.f };
		GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
	UpdateSlideDisplacement();
}
void AWeapon::StartSlidingTimer()
{
	bMovingSlide = true;
	GetWorldTimerManager().SetTimer(SlideTimer, this, &AWeapon::FinishMovingSlide, SlideDisplacementTime);
}
void AWeapon::ReloadAmmo(int32 Amount)
{
	checkf(Ammo + Amount <= MagazineCapacity,TEXT("Ammo added to Magazine exceeds capacity"))
	Ammo += Amount;
}
bool AWeapon::bMagisFull()
{
	return Ammo >= MagazineCapacity;
}
void AWeapon::ThrowWeapon()
{
	FRotator MeshRotation{ 0.f,GetItemMesh()->GetComponentRotation().Yaw,0.f };
	GetItemMesh()->SetWorldRotation(MeshRotation,false,nullptr,ETeleportType::TeleportPhysics);
	
	const FVector MeshFowardVector = GetItemMesh()->GetForwardVector();
	const FVector MeshRight{ GetItemMesh()->GetRightVector() };
	
	//Direction we throw the weapon
	FVector ImpulseDirection = MeshRight.RotateAngleAxis(-20.f, MeshFowardVector);
	float RandomRotation = 30.f;
	ImpulseDirection = ImpulseDirection.RotateAngleAxis(RandomRotation, { 0.f,0.f,1.f });
	ImpulseDirection *= 2'000.f;

	GetItemMesh()->AddImpulse(ImpulseDirection);

	bIsFalling = true;
	GetWorldTimerManager().SetTimer(ThrowWeaponTimer, this, &AWeapon::StopFalling, ThrowWeaponTime);
	UE_LOG(LogTemp, Warning, TEXT("Weapon has Started Falling"));
	EnableGlowMaterial();
}


void AWeapon::StopFalling()
{
	bIsFalling = false;
	SetItemState(EItemState::EIS_Pickup);
	StartPulseTimer();
}

void AWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	const FString WeaponTablePath = TEXT("DataTable'/Game/_Game/DataTable/WeaponDataTable.WeaponDataTable'");

	UDataTable* WeaponTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *WeaponTablePath));

	if (WeaponTableObject)
	{
		FWeaponDataTable* WeaponDataRow = nullptr;
		switch (WeaponType)
		{
		case(EWeaponType::EWT_SubmachineGun):
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("SMG"), "");
			break;
		case(EWeaponType::EWT_AssaultRifle):
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("Rifle"), "");
			break;
		case(EWeaponType::EWT_Pistol):
		{
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("Pistol"), "");
			break;
		}
		}
		if (WeaponDataRow)
		{
			AmmoType = WeaponDataRow->AmmoType;
			Ammo = WeaponDataRow->WeaponAmmo;
			MagazineCapacity = WeaponDataRow->MagazineCapacity;
			SetEquipSound(WeaponDataRow->EquipSound);
			SetPickupSound(WeaponDataRow->PickupSound);

			GetItemMesh()->SetSkeletalMesh(WeaponDataRow->ItemMesh);

			SetItemName(WeaponDataRow->ItemName);
			SetMaterialInstance(WeaponDataRow->MaterialInstance);

			PreviousMaterialIndex = GetMaterialIndex();
			GetItemMesh()->SetMaterial(PreviousMaterialIndex, nullptr);
			SetMaterialIndex(WeaponDataRow->MaterialIndex);

			SetIconItem(WeaponDataRow->InventoryIcon);
			SetAmmoIcon(WeaponDataRow->AmmoIcon);

			MagBoneName = WeaponDataRow->MagBoneName;
			ReloadMontageSection = WeaponDataRow->ReloadMontageName;

			GetItemMesh()->SetAnimInstanceClass(WeaponDataRow->AnimBP);

			CrosshairMiddle = WeaponDataRow->CrosshairMiddle;
			CrosshairLeft = WeaponDataRow->CrosshairLeft;
			CrosshairRight = WeaponDataRow->CrosshairRight;
			CrosshairTop = WeaponDataRow->CrosshairTop;
			CrosshairBottom = WeaponDataRow->CrosshairBottom;

			AutoFireRate = WeaponDataRow->AutoFireRate;
			MuzzleFlash = WeaponDataRow->MuzzleFlash;
			FireSound = WeaponDataRow->FireSound;
			BoneToHide = WeaponDataRow->BoneToHide;
			bAutomatic = WeaponDataRow->bAutomatic;
			Damage = WeaponDataRow->Damage;
			HeadshotDamage = WeaponDataRow->HeadshotDamage;

			MinCrosshairSpread = WeaponDataRow->MinCrosshairSpread;
			InAirCrosshairSpread = WeaponDataRow->InAirCrosshairSpread;
			AimingCrosshairSpread = WeaponDataRow->AimingCrosshairSpread;
			FireingCrosshairSpread = WeaponDataRow->FireingCrosshairSpread;
			if (GetMaterialInstance())
			{
				SetDynamicMaterialInstance(UMaterialInstanceDynamic::Create(GetMaterialInstance(), this));
				GetDynamicMaterialInstance()->SetVectorParameterValue(TEXT("FresnelColor"), GetGlowColor());
				GetItemMesh()->SetMaterial(GetMaterialIndex(), GetDynamicMaterialInstance());

				EnableGlowMaterial();
			}
		}
	}
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	if (BoneToHide != FName(""))
	{
		GetItemMesh()->HideBoneByName(BoneToHide, EPhysBodyOp::PBO_None);
	}
}

void AWeapon::FinishMovingSlide()
{
	bMovingSlide = false;
}

void AWeapon::UpdateSlideDisplacement()
{
	if (SlideDisplacementCurve && bMovingSlide)
	{
		const float ElaspedTime = GetWorldTimerManager().GetTimerElapsed(SlideTimer);
		const float CurveValue = SlideDisplacementCurve->GetFloatValue(ElaspedTime);
		SlideDisplacement = CurveValue * MaxSlideDisplacement;
		RecoilPitch = MaxRecoilPitch * CurveValue;
	}
}

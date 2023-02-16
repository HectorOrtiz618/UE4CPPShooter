// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Item.h"
#include "Shooter.h"
#include "Enemy.h"
#include "Components/WidgetComponent.h"
#include "Weapon.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Ammo.h"
#include "BulletHitInterface.h"
#include "EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultCameraFOV = 90.f; // place holder value, will take the value iwn BeginPlay()
	ZoomedCameraFOV = 35.f;
	ZoomInterpSpeed = 0.5f;

	BaseTurnRate = 45.f;
	BaseLookupRate = 45.f;

	HipTurnRate = 90.f;
	HipLookupRate = 90.f;

	AimTurnRate = 20.f;
	AimLookupRate = 20.f;

	//Mouse Sensitivity
	MouseHipTurnRate = 1.f;
	MouseHipLookupRate = 1.f;
	MouseAimTurnRate = 0.2f;
	MouseAimLookupRate = 0.2f;

	//Crosshair spread Factors
	CrosshairSpreadMultiplier = 0.f;
	CrosshairAimFactor = 0.f;
	CrosshairShootingFactor = 0.f;
	CrosshairInAirFactor = 0.f;
	CrosshairVelocityFactor = 0.f;

	StandingCapsuleHalfHeight = 88.f;
	CrouchingCapsuleHalfHeight = 44.f;

	//Bullet Fire Timer Variable
	ShootTimeDuration = 0.5f;
	bFireingBullet = false;


	bFireButtonPressed = false;
	bShouldFire = true;
	bAimButtonPressed = false;



	//CameraBoom will follow the pawn, with a desired Arm length of 300cm, and rotates with the pawn
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength= 180.f;
	CameraBoom->bUsePawnControlRotation = true; // Use pawn Rotation to rotatoe Spring Arm Component
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 70.f);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	//Create HandSceneComponent 
	HandSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HandSceneComp"));

	//Dont rotate player when controller rotates
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;
	
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	CrosshairSpreadMultiplier = 1.f;//Defaultvalue, will change on Tick()
	bAiming = false;
	bCrouching = false;

	BaseMovementRate = 650.f;
	CrouchingMovementRate= 300.f;

	bShouldTraceForItems = false;
	TraceHitItemLastFrame = nullptr;

	CameraInterpDistance = 250.f;
	CameraInterpElevation = 65.f;

	Starting9mmAmmo = 90;
	StartingARAmmo = 60;

	InitilizeAmmoMap();

	//Create interpolation components
	WeaponInterpComp = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon Interp Comp"));
	WeaponInterpComp->SetupAttachment(GetFollowCamera());

	InterpComp1 = CreateDefaultSubobject<USceneComponent>(TEXT("Interp Comp 1"));
	InterpComp1->SetupAttachment(GetFollowCamera());

	InterpComp2 = CreateDefaultSubobject<USceneComponent>(TEXT("Interp Comp 2"));
	InterpComp2->SetupAttachment(GetFollowCamera());

	InterpComp3 = CreateDefaultSubobject<USceneComponent>(TEXT("Interp Comp 3"));
	InterpComp3->SetupAttachment(GetFollowCamera());

	InterpComp4 = CreateDefaultSubobject<USceneComponent>(TEXT("Interp Comp 4"));
	InterpComp4->SetupAttachment(GetFollowCamera());

	InterpComp5 = CreateDefaultSubobject<USceneComponent>(TEXT("Interp Comp 5"));
	InterpComp5->SetupAttachment(GetFollowCamera());

	InterpComp6 = CreateDefaultSubobject<USceneComponent>(TEXT("Interp Comp 6"));
	InterpComp6->SetupAttachment(GetFollowCamera());

	bShouldPlayPickupSound = true;
	bShouldPlayEquipSound = true;

	ResetPickupSoundTime = 0.2f;
	ResetEquipSoundTime = 0.2f;

	HighlightedSlot = -1;

	void ResetPickupTimer();
	void ResetEquipTimer();

	Health = 100.f;
	MaxHealth = 100.f;

	StunChance = .25f;

	bDead = false;

}

float AShooterCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.0f)
	{
		
		Health = 0.f;
		UE_LOG(LogTemp, Warning, TEXT("Health is at %f"), Health);
		Die();
		auto EnemyController = Cast<AEnemyAIController>(EventInstigator);
		if (EnemyController)
		{
			EnemyController->GetBlackBoardComponent()->SetValueAsBool(FName("PlayerDead"), true);
		}
	}
	else
	{
		Health -= DamageAmount;
	}
	return DamageAmount;
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	//UE_LOG(LogTemp, Warning, TEXT("BeginPlay() has been called"));
	//int myInt{42};
	//UE_LOG(LogTemp, Warning, TEXT("int myInt = %d"), myInt);

	//float myFloat{ 3.14159f };
	//UE_LOG(LogTemp, Warning, TEXT("float myFloat = %f"), myFloat);

	//double myDouble = { 0.000756 };
//UE_LOG(LogTemp, Warning, TEXT("double myDouble = %lf"), myDouble);
//char myChar{ 'c'};
//UE_LOG(LogTemp, Warning, TEXT("char myChar = %c"), myChar);
//wchar_t wideChar{ L'c' };
//UE_LOG(LogTemp, Warning, TEXT("wchar_t wideChar = %lc"), wideChar);

//bool myBool{ true };
//UE_LOG(LogTemp, Warning, TEXT("bool myBool = %d"), myBool);
//FString myFString{ TEXT("STRING!!!!!!!!!!") };
//UE_LOG(LogTemp, Warning, TEXT("FString myFString = %s"), *myFString);// the * is needed to refrence the FSTRING
//UE_LOG(LogTemp, Warning, TEXT("FString myFString = %s"), *GetName());// We are getting the name of the instance of a ShooterCharacter ingame

	if (FollowCamera)
	{
		DefaultCameraFOV = FollowCamera->FieldOfView;
		CurrentCameraFOV = DefaultCameraFOV;
	}
	//Spawn default weapon and equipped it.
	EquipWeapon(SpawnDefaultWeapon());
	Inventory.Add(EquippedWeapon);
	EquippedWeapon->SetSlotIndex(0);
	EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
	EquippedWeapon->SetCharacter(this);
	EquippedWeapon->DisableCustomDepth();
	EquippedWeapon->DisableGlowMaterial();

	InitilizeAmmoMap();
	InitializeInterpLocations();

	GetCharacterMovement()->MaxWalkSpeed = BaseMovementRate;
	GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchingMovementRate;
}

void AShooterCharacter::Jump()
{
	if (bCrouching)
	{
		bCrouching = false;
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementRate;
	}
	else
	{
		ACharacter::Jump();
	}
}

void AShooterCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && Value == 0.f)//Check
	{

	}
	else
	{
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0,Rotation.Yaw,0 };//pull only yaw from FRotator
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && Value == 0.f)//Check
	{

	}
	else
	{
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0,Rotation.Yaw,0 };//pull only yaw from FRotator
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::InterpCapsuleHalfHeght(float DeltaTime)
{
	float TargetCapsuleHalfHeight;
	if (bCrouching)
	{
		TargetCapsuleHalfHeight = CrouchingCapsuleHalfHeight;
	}
	else
	{
		TargetCapsuleHalfHeight = StandingCapsuleHalfHeight;
	}
	float InterpCapsuleHalfHeight = FMath::FInterpTo(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), TargetCapsuleHalfHeight, DeltaTime, 3.f);
	//Negative is crouching, positive value if standing
	float DeltaCapsuleHalfHeight = InterpCapsuleHalfHeight - GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const FVector MeshOffset = FVector(0.f, 0.f, -DeltaCapsuleHalfHeight);
	GetMesh()->AddLocalOffset(MeshOffset);
	GetCapsuleComponent()->SetCapsuleHalfHeight(InterpCapsuleHalfHeight);
}

void AShooterCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookupRate * GetWorld()->GetDeltaSeconds());
}
// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetAimFOV(DeltaTime);
	SetLookRates();
	CalculateCrosshairSpread(DeltaTime);

	InterpCapsuleHalfHeght(DeltaTime);
	TraceForItems();

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnAtCamera", this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpCamera", this, &AShooterCharacter::LookUpAtRate);

	PlayerInputComponent->BindAxis("Turn", this, &AShooterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AShooterCharacter::Lookup);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AShooterCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AShooterCharacter::FireButtonReleased);

	PlayerInputComponent->BindAction("Aiming", IE_Pressed, this, &AShooterCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("Aiming", IE_Released, this, &AShooterCharacter::AimingButtonReleased);

	PlayerInputComponent->BindAction("Use", IE_Pressed, this, &AShooterCharacter::SelectButtonPressed);
	PlayerInputComponent->BindAction("Use", IE_Released, this, &AShooterCharacter::SelectButtonReleased);

	PlayerInputComponent->BindAction("ReloadButton", IE_Pressed, this, &AShooterCharacter::ReloadButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AShooterCharacter::CrouchButtonPressed);

	PlayerInputComponent->BindAction("FKey", IE_Pressed, this, &AShooterCharacter::FKeyPressed);
	PlayerInputComponent->BindAction("1Key", IE_Pressed, this, &AShooterCharacter::Key1Pressed);
	PlayerInputComponent->BindAction("2Key", IE_Pressed, this, &AShooterCharacter::Key2Pressed);
	PlayerInputComponent->BindAction("3Key", IE_Pressed, this, &AShooterCharacter::Key3Pressed);
	PlayerInputComponent->BindAction("4Key", IE_Pressed, this, &AShooterCharacter::Key4Pressed);
	PlayerInputComponent->BindAction("5Key", IE_Pressed, this, &AShooterCharacter::Key5Pressed);
}
void AShooterCharacter::FinishEquipping()
{
	if (CombatState == ECombatState::ECS_Stunned) return;
	CombatState = ECombatState::ECS_Idle;
	if (bAimButtonPressed)
	{
		Aim();
	}
}
void AShooterCharacter::FinishReloading()
{
	if (CombatState == ECombatState::ECS_Stunned) return;
	CombatState = ECombatState::ECS_Idle;
	if (bAimButtonPressed) Aim();
	if (EquippedWeapon == nullptr) return;
	if (AmmoMap.Contains(EquippedWeapon->GetAmmoType()))
	{
		int32 CarriedAmmo = AmmoMap[EquippedWeapon->GetAmmoType()];
		int32 EmptySpace = EquippedWeapon->GetMagCapacity() - EquippedWeapon->GetAmmo();
		//If we dont have enough to fill the mag, fill up to what we're carrying
		if (EmptySpace > CarriedAmmo)
		{
			EquippedWeapon->ReloadAmmo(AmmoMap[EquippedWeapon->GetAmmoType()]);
			CarriedAmmo = 0;
			AmmoMap[EquippedWeapon->GetAmmoType()] = CarriedAmmo;
		}
		else
		{
			EquippedWeapon->ReloadAmmo(EmptySpace);
			CarriedAmmo -= EmptySpace;
			AmmoMap[EquippedWeapon->GetAmmoType()] = CarriedAmmo;
		}
	}
}
void AShooterCharacter::TraceForItems()
{
	if (bShouldTraceForItems)
	{
		FHitResult ItemTraceResult;
		FVector HitLocation;
		TraceUnderCrosshair(ItemTraceResult, HitLocation);
		if (ItemTraceResult.bBlockingHit)
		{
			TraceHitItem = Cast<AItem>(ItemTraceResult.Actor);
			const auto TraceHitWeapon = Cast<AWeapon>(TraceHitItem);
			if (TraceHitWeapon)
			{
				if (HighlightedSlot == -1)
				{
					//Not highlighting a slot, so we highlight one
					HighlightInventorySlot();
				}
			}
			else
			{
				//is a slot being highlighted?
				if (HighlightedSlot != -1)
				{
					UnHighlightInventorySlot();
				}
			}
			if (TraceHitItem && TraceHitItem->GetItemState() == EItemState::EIS_EquipInterping)
			{
				TraceHitItem = nullptr;
			}
			if (TraceHitItem && TraceHitItem->GetWidgetComponent())
			{
				//SHow pickup widget
				TraceHitItem->GetWidgetComponent()->SetVisibility(true);
				TraceHitItem->EnableCustomDepth();

				if (Inventory.Num() >= InventoryCapacity)
				{
					//Inv is full
					TraceHitItem->SetCharacterInventoryFull(true);
				}
				else
				{
					//still room
					TraceHitItem->SetCharacterInventoryFull(false);
				}
			}
			//We hit an AItem last frame
			if (TraceHitItemLastFrame)
			{
				if (TraceHitItem != TraceHitItemLastFrame)
				{
					//We are hitting a diffrent AItem this frame from last frame
					//Or AItem is NULL
					TraceHitItemLastFrame->GetWidgetComponent()->SetVisibility(false);
					TraceHitItemLastFrame->DisableCustomDepth();
				}
			}
			TraceHitItemLastFrame = TraceHitItem; //Will hit nullptr if not an item, but will if we do hit ain item
		}
	}
	else if (TraceHitItemLastFrame)
	{
		//No longer overlapping, disable widget visibility
		TraceHitItemLastFrame->GetWidgetComponent()->SetVisibility(false);
		TraceHitItemLastFrame->DisableCustomDepth();
	}

}
void AShooterCharacter::ResetPickupTimer()
{
	bShouldPlayPickupSound = true;
}
void AShooterCharacter::ResetEquipTimer()
{
	bShouldPlayEquipSound = true;
}
void AShooterCharacter::IncrementOverlappedItemCount(int8 Amount)
{
	if (OverlappedItemCount + Amount <= 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;
	}
	else
	{
		OverlappedItemCount += Amount;
		bShouldTraceForItems = true;
	}
}
void AShooterCharacter::Stun()
{
	if (CombatState == ECombatState::ECS_Dead) return;
	CombatState = ECombatState::ECS_Stunned;

	UAnimInstance* AnimInstance= GetMesh()->GetAnimInstance();
	if (AnimInstance && StunnedMontage)
	{
		AnimInstance->Montage_Play(StunnedMontage);
	}
}
void AShooterCharacter::StartPickupSoundTimer()
{
	bShouldPlayPickupSound = false;
	GetWorldTimerManager().SetTimer(PickupSoundTimer, this, &AShooterCharacter::ResetPickupTimer, ResetPickupSoundTime);
}
void AShooterCharacter::StartEquipSoundTimer()
{
	bShouldPlayEquipSound = false;
	GetWorldTimerManager().SetTimer(EquipSoundTimer, this, &AShooterCharacter::ResetEquipTimer, ResetEquipSoundTime);
}
FInterpLocation AShooterCharacter::GetInterpLocation(int32 index)
{
	if (index <= InterpLocations.Num())
	{
		return InterpLocations[index];
	}
	return FInterpLocation();
}
float AShooterCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}
//FVector AShooterCharacter::GetCameraInterpLocation()
//{
//	const FVector CameraWorldLocation{ FollowCamera->GetComponentLocation() };
//	const FVector CameraFoward{ FollowCamera->GetForwardVector() };
//	
//	
//	return CameraWorldLocation + CameraFoward * CameraInterpDistance +FVector(0.f,0.f,CameraInterpElevation);
//}
void AShooterCharacter::GetPickupItem(AItem* Item)
{
	if (Item->GetEquipSound())
	{
		Item->PLayEquipSound();
	}
	auto Weapon = Cast<AWeapon>(Item);
	if (Weapon)
	{
		if (Inventory.Num() < InventoryCapacity)
		{
			UE_LOG(LogTemp, Warning, TEXT("Picking up weapon and putting it into inventory"));
			Weapon->SetSlotIndex(Inventory.Num());
			Inventory.Add(Weapon);
			Weapon->SetItemState(EItemState::EIS_PickedUp);
		}
		else//Inventory is full, swap current weapon
		{
			UE_LOG(LogTemp, Warning, TEXT("Swapping"));
			SwapWeapon(Weapon);
		}
		return;
	}
	auto Ammo = Cast<AAmmo>(Item);
	if (Ammo)
	{
		UE_LOG(LogTemp, Warning, TEXT("Picking up Ammo"));
		PickupAmmo(Ammo);
	}

}
void AShooterCharacter::FireWeapon()
{
	if(EquippedWeapon == nullptr)
	{	
		return;
	}
	if (CombatState != ECombatState::ECS_Idle)
	{
		return;
	}
	if (WeaponHasAmmo() == false)
	{
		return;
	}
	//play fire sound
	PlayFireSound();
	//get line trace
	SendBullet();
	//play anim montage
	PlayGunFireMontage();
	//Spread Crosshairs while shooting
	StartCrosshairBulletFire();
	//Decrement Ammo
	EquippedWeapon->DecrementAmmo();

	StartFireTimer();
	if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol)
	{
		EquippedWeapon->StartSlidingTimer();
	}
}

void AShooterCharacter::Turn(float Value)
{
	float TurnScaleFactor;
	if (bAiming)
	{
		TurnScaleFactor = MouseAimTurnRate;
	}
	else
	{
		TurnScaleFactor = MouseHipTurnRate;
	}
	AddControllerYawInput(Value * TurnScaleFactor);
}

void AShooterCharacter::Lookup(float Value)
{
	float LookupScaleFactor;
	if (bAiming)
	{
		LookupScaleFactor = MouseAimLookupRate;
	}
	else
	{
		LookupScaleFactor = MouseHipLookupRate;
	}
	AddControllerPitchInput(Value * LookupScaleFactor);
}

void AShooterCharacter::AimingButtonPressed()
{
	bAimButtonPressed = true;
	if (CombatState != ECombatState::ECS_Reloading&& CombatState != ECombatState::ECS_Equipping && CombatState != ECombatState::ECS_Stunned )
	{
		Aim();
	}
}

void AShooterCharacter::AimingButtonReleased()
{
	bAimButtonPressed = false;
	StopAiming();
}

void AShooterCharacter::SetAimFOV(float DeltaTime)
{

	if (bAiming)
	{
		CurrentCameraFOV = FMath::FInterpTo(CurrentCameraFOV, ZoomedCameraFOV, DeltaTime, ZoomInterpSpeed);
	}
	else
	{
		CurrentCameraFOV = FMath::FInterpTo(CurrentCameraFOV, DefaultCameraFOV, DeltaTime, ZoomInterpSpeed);
	}
	GetFollowCamera()->SetFieldOfView(CurrentCameraFOV);
}

void AShooterCharacter::SetLookRates()
{
	if (bAiming)
	{
		BaseTurnRate = AimTurnRate;
		BaseLookupRate = AimLookupRate;
	}
	else
	{
		BaseTurnRate = HipTurnRate;
		BaseTurnRate = HipLookupRate;
	}
}

void AShooterCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	if (EquippedWeapon == nullptr) return;


	FVector2D WalkSpeedRange(0.f, 600.f);
	FVector2D VeliocityMultiplierRange(0.f, 1.f);
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;

	//Calculate InAirFactor
	if (GetMovementComponent()->IsFalling())
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, EquippedWeapon->GetInAirSpread(), DeltaTime, 2.25f);
	}
	else
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, EquippedWeapon->GetMinSpread(), DeltaTime, 30.f);
	}
	//Calculate AimFactor
	if (bAiming)
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, EquippedWeapon->GetAimingSpread(), DeltaTime, 30.f);
	}
	else
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, EquippedWeapon->GetMinSpread(), DeltaTime, 30.f);
	}
	//True 0.05 Secs after fireing
	if (bFireingBullet)
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, EquippedWeapon->GetFireingSpread(), DeltaTime, 60.f);
	}
	else
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, EquippedWeapon->GetMinSpread(), DeltaTime, 60.f);
	}
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange,VeliocityMultiplierRange,Velocity.Size());
	CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor + CrosshairInAirFactor - CrosshairAimFactor + CrosshairShootingFactor;
}

void AShooterCharacter::StartCrosshairBulletFire()
{
	bFireingBullet = true;
	GetWorldTimerManager().SetTimer(CrosshairShootTimer,this,&AShooterCharacter::FinishCrosshairBulletFire,ShootTimeDuration);
}

AWeapon* AShooterCharacter::SpawnDefaultWeapon()
{
	if (DefaultWeaponClass)
	{
		return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
	}
	return nullptr;
}

void AShooterCharacter::EquipWeapon(AWeapon* Weapon,bool bSwapping)
{
	if (Weapon)
	{
		//Dont want area spohere and collision box to trtigger when equipped

		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("WeaponSocket"));
		if (HandSocket)
		{
			//Attach weapon to hand
			HandSocket->AttachActor(Weapon, GetMesh());
		}
		if (EquippedWeapon==nullptr)
		{
			//-1 == no equipped weapon yet, no need to reverse icon animation
			ItemDelegate.Broadcast(-1,Weapon->GetSlotIndex());
		}
		else if(!bSwapping)
		{
			ItemDelegate.Broadcast(EquippedWeapon->GetSlotIndex(), Weapon->GetSlotIndex());
		}
		EquippedWeapon = Weapon;
		EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
	}
}

void AShooterCharacter::FinishCrosshairBulletFire()
{
	bFireingBullet = false;
}

bool AShooterCharacter::GetBeamEndLocation(const FVector& MuzzleEndLocation, FHitResult& OutHitResult)
{
	//Check for Crosshair trace hit
	FHitResult CrosshairHitResult;
	FVector OutBeamLocation;
	bool bCrosshairHit = TraceUnderCrosshair(CrosshairHitResult,OutBeamLocation,true);
	if (bCrosshairHit)
	{
		OutBeamLocation = CrosshairHitResult.Location;
	}
	else
	{
		//Out Beam Location already has a location thanks to Trace Under Crosshairs
	}

	//FHitResult WeaponTraceHit;
	const FVector WeaponTraceStart{ MuzzleEndLocation };
	const FVector StartToEnd = OutBeamLocation- MuzzleEndLocation;
	const FVector WeaponTraceEnd{ MuzzleEndLocation  + StartToEnd * 1.25f };
	GetWorld()->LineTraceSingleByChannel(OutHitResult, WeaponTraceStart, WeaponTraceEnd, ECollisionChannel::ECC_Visibility);
	if (!OutHitResult.bBlockingHit)
	{
		OutHitResult.Location = OutBeamLocation;
		return false;
	}
	return true;

}

bool AShooterCharacter::TraceUnderCrosshair(FHitResult& OutHit, FVector& OutHitLocation,bool bFireingGun)
{
	
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		//UE_LOG(LogTemp, Warning, TEXT("GettingVIewport SIze!"));
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	CrosshairLocation.Y -= 50.f;

	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);
	if (bScreenToWorld)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Project Screen to World"));
		const FVector Start = CrosshairWorldPosition;
		FVector End;
		if (bFireingGun)
		{
			const float CrosshairSpreadMultiplierInRadians = FMath::DegreesToRadians(CrosshairSpreadMultiplier / 2);
			FVector BulletImpactWorldPosition = FMath::VRandCone(CrosshairWorldDirection, CrosshairSpreadMultiplierInRadians);
			End = CrosshairWorldPosition + BulletImpactWorldPosition * 50'000.f;
		}
		else
		{
			End = Start + CrosshairWorldDirection * 50'000.f;
		}
		
		OutHitLocation = End;

		GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECollisionChannel::ECC_Visibility);
		if (OutHit.bBlockingHit)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Enable Widget!"));
			OutHitLocation = OutHit.Location;
			return true;
		}

	}
	//UE_LOG(LogTemp, Warning, TEXT("Trace failed!"));
	return false;
}

void AShooterCharacter::FireButtonPressed()
{
	bFireButtonPressed = true;
	FireWeapon();
}

void AShooterCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AShooterCharacter::StartFireTimer()
{
	if (EquippedWeapon)
	{
		CombatState = ECombatState::ECS_Shooting;
		GetWorldTimerManager().SetTimer(AutoFireTimer, this, &AShooterCharacter::FireTimerReset, EquippedWeapon->GetAutoFireRate());
	}
}

void AShooterCharacter::FireTimerReset()
{
	if (CombatState == ECombatState::ECS_Stunned) return;
	CombatState = ECombatState::ECS_Idle;
	if (!EquippedWeapon) return;
	if (WeaponHasAmmo())
	{
		if (bFireButtonPressed&& EquippedWeapon->GetAutomatic())
		{
			FireWeapon();
		}
	}
	else
	{
		ReloadWeapon();
	}
}

void AShooterCharacter::DropWeapon()
{
	if (EquippedWeapon)
	{
		FDetachmentTransformRules DetachmentTransformRule(EDetachmentRule::KeepWorld,true);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTransformRule);

		EquippedWeapon->SetItemState(EItemState::EIS_Falling);
		EquippedWeapon->ThrowWeapon();
	}
}

void AShooterCharacter::SelectButtonPressed()
{
	if (CombatState != ECombatState::ECS_Idle) return;
	if (TraceHitItem)
	{
		TraceHitItem->StartItemCurve(this,true);
		TraceHitItem = nullptr;//prevents restarting Item curve
	}
}

void AShooterCharacter::SelectButtonReleased()
{

}

void AShooterCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
	if (Inventory.Num()-1 >= EquippedWeapon->GetSlotIndex())
	{
		Inventory[EquippedWeapon->GetSlotIndex()] = WeaponToSwap;
		WeaponToSwap->SetSlotIndex(EquippedWeapon->GetSlotIndex());
	}
	DropWeapon();
	EquipWeapon(WeaponToSwap,true);

	TraceHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;
}

void AShooterCharacter::PickupAmmo(AAmmo* PickupAmmo)
{
	//check to see if ammomap contains ammotype
	if (AmmoMap.Find(PickupAmmo->GetAmmoType()))
	{
		int32 AmmoCount= AmmoMap[PickupAmmo->GetAmmoType()];
		AmmoCount += PickupAmmo->GetItemCount();
		AmmoMap[PickupAmmo->GetAmmoType()] = AmmoCount;

	}
	if (EquippedWeapon->GetAmmoType() == PickupAmmo->GetAmmoType())
	{
		//if gun is empty
		if (EquippedWeapon->GetAmmo() == 0)
		{
			ReloadWeapon();
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Destroying Ammo"));
	PickupAmmo->Destroy();

}

void AShooterCharacter::InitilizeAmmoMap()
{
	AmmoMap.Add(EAmmoType::EAT_9MM, Starting9mmAmmo);
	AmmoMap.Add(EAmmoType::EAT_AR, StartingARAmmo);
}

bool AShooterCharacter::WeaponHasAmmo()
{
	if (EquippedWeapon)
	{
		return EquippedWeapon->GetAmmo() > 0;
	}
	return false;
}

void AShooterCharacter::PlayFireSound()
{
	if (EquippedWeapon->GetFireSound())
	{
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->GetFireSound());
	}
}

void AShooterCharacter::SendBullet()
{
	const USkeletalMeshSocket* BarrelSocket = EquippedWeapon->GetItemMesh()->GetSocketByName("BarrelSocket");
	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(EquippedWeapon->GetItemMesh());
		if (EquippedWeapon->GetMuzzleFlash())
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EquippedWeapon->GetMuzzleFlash(), SocketTransform);
		}
		FHitResult BeamHitResult;
		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamHitResult);
		if (bBeamEnd)
		{
			if (BeamHitResult.Actor.IsValid())
			{
				IBulletHitInterface* BulletHitInterface = Cast<IBulletHitInterface>(BeamHitResult.Actor.Get());
				if (BulletHitInterface)
				{
					BulletHitInterface->BulletHit_Implementation(BeamHitResult,this,GetController());
				}
				AEnemy* HitEnemy = Cast<AEnemy>(BeamHitResult.Actor.Get());
				if (HitEnemy)
				{
					bool bHeadShot;
					int32 Damage = 0;
					if (BeamHitResult.BoneName.ToString() == HitEnemy->GetHeadBone())
					{
						//BOOM headshot
						Damage = EquippedWeapon->GetHeadshotDamage();
						bHeadShot = true;
						//BOOM headshot
						UGameplayStatics::ApplyDamage(BeamHitResult.Actor.Get(), EquippedWeapon->GetHeadshotDamage(), GetController(), this, UDamageType::StaticClass());
						
					}
					else
					{
						//bodyshopt
						Damage = EquippedWeapon->GetDamage();
						bHeadShot = false;
						UGameplayStatics::ApplyDamage(BeamHitResult.Actor.Get(), EquippedWeapon->GetDamage(), GetController(), this, UDamageType::StaticClass());
						
					}
					HitEnemy->ShowHitNumber(Damage, BeamHitResult.Location,bHeadShot);
				}
			}
			else
			{
				//No valid actors hit, spawn defaault particles 
				if (ImpactParticles)
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, BeamHitResult.Location);
				}
			}
			if (BeamParticles)
			{
				UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, SocketTransform);
				if (Beam)
				{
					Beam->SetVectorParameter(FName("Target"), BeamHitResult.Location);
				}
			}
		}
	}
}

void AShooterCharacter::PlayGunFireMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}

}

void AShooterCharacter::ReloadButtonPressed()
{
	ReloadWeapon();
}

void AShooterCharacter::ReloadWeapon()
{
	if (CombatState != ECombatState::ECS_Idle) return;
	if (EquippedWeapon == nullptr) return;
	//Do We have ammo for our current weapon? TODO:Create function that checks this called bool CarryingAmmo
	if (CarryingAmmo() && !EquippedWeapon->bMagisFull())
	{
		if (bAiming) StopAiming();
		CombatState = ECombatState::ECS_Reloading;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && ReloadMontage)
		{
			AnimInstance->Montage_Play(ReloadMontage);
			//TODO: create enum that allows use to determine what weapon we have and create switch statement
			AnimInstance->Montage_JumpToSection(EquippedWeapon->GetReloadMontageName());
		}
	}
}

bool AShooterCharacter::CarryingAmmo()
{
	if(EquippedWeapon == nullptr)
	{
		return false;
	}
	auto AmmoType = EquippedWeapon->GetAmmoType();
	if (AmmoMap.Contains(AmmoType))
	{
		return AmmoMap[AmmoType] > 0;
	}
	return false;
}

void AShooterCharacter::CrouchButtonPressed()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		bCrouching = !bCrouching;
	}
	if (bCrouching)
	{
		
		GetCharacterMovement()->MaxWalkSpeed = CrouchingMovementRate;
		
	}
	else
	{
		//Nothing is above the character, we can stand up
		if (!UnCrouchBlocked())
		{
			GetCharacterMovement()->MaxWalkSpeed = BaseMovementRate;
		}
		//blocked, stay crouched
		else
		{
			bCrouching = true;
			GetCharacterMovement()->MaxWalkSpeed = CrouchingMovementRate;
		}
	}
}

void AShooterCharacter::GrabMag()
{
	if (EquippedWeapon == nullptr || HandSceneComponent == nullptr) return;
	int32 MagBoneIndex{ EquippedWeapon->GetItemMesh()->GetBoneIndex(EquippedWeapon->GetMagBoneName()) };
	MagTransform = EquippedWeapon->GetItemMesh()->GetBoneTransform(MagBoneIndex);

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative,true);
	HandSceneComponent->AttachToComponent(GetMesh(), AttachmentRules,FName(TEXT("Hand_L")));
	HandSceneComponent->SetWorldTransform(MagTransform);
	EquippedWeapon->SetMovingMag(true);
}

void AShooterCharacter::StopGrabMag()
{
	EquippedWeapon->SetMovingMag(false);
}

void AShooterCharacter::Aim()
{
	bAiming = true;
	GetCharacterMovement()->MaxWalkSpeed = CrouchingMovementRate;
}

void AShooterCharacter::StopAiming()
{
	bAiming = false;
	GetCharacterMovement()->MaxWalkSpeed = BaseMovementRate;
}

void AShooterCharacter::InitializeInterpLocations()
{
	FInterpLocation WeaponLocation;
	WeaponLocation.SceneComponent = WeaponInterpComp;
	WeaponLocation.ItemCount = 0;
	InterpLocations.Add(WeaponLocation);

	FInterpLocation InterpLocation1;
	InterpLocation1.SceneComponent = InterpComp1;
	InterpLocation1.ItemCount = 0;
	InterpLocations.Add(InterpLocation1);

	FInterpLocation InterpLocation2;
	InterpLocation2.SceneComponent = InterpComp2;
	InterpLocation2.ItemCount = 0;
	InterpLocations.Add(InterpLocation2);

	FInterpLocation InterpLocation3;
	InterpLocation3.SceneComponent = InterpComp2;
	InterpLocation3.ItemCount = 0;
	InterpLocations.Add(InterpLocation3);

	FInterpLocation InterpLocation4;
	InterpLocation4.SceneComponent = InterpComp2;
	InterpLocation4.ItemCount = 0;
	InterpLocations.Add(InterpLocation4);

	FInterpLocation InterpLocation5;
	InterpLocation5.SceneComponent = InterpComp2;
	InterpLocation5.ItemCount = 0;
	InterpLocations.Add(InterpLocation5);

	FInterpLocation InterpLocation6;
	InterpLocation6.SceneComponent = InterpComp2;
	InterpLocation6.ItemCount = 0;
	InterpLocations.Add(InterpLocation6);

}

void AShooterCharacter::FKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 0) return;
	ExtangeInventoryItems(EquippedWeapon->GetSlotIndex(), 0);
}

void AShooterCharacter::Key1Pressed()
{
	if (EquippedWeapon->GetSlotIndex() == 1) return;
	ExtangeInventoryItems(EquippedWeapon->GetSlotIndex(), 1);
}

void AShooterCharacter::Key2Pressed()
{
	if (EquippedWeapon->GetSlotIndex() == 2) return;
	ExtangeInventoryItems(EquippedWeapon->GetSlotIndex(), 2);
}

void AShooterCharacter::Key3Pressed()
{
	if (EquippedWeapon->GetSlotIndex() == 3) return;
	ExtangeInventoryItems(EquippedWeapon->GetSlotIndex(), 3);
}

void AShooterCharacter::Key4Pressed()
{
	if (EquippedWeapon->GetSlotIndex() == 4) return;
	ExtangeInventoryItems(EquippedWeapon->GetSlotIndex(), 4);
}

void AShooterCharacter::Key5Pressed()
{
	if (EquippedWeapon->GetSlotIndex() == 5) return;
	ExtangeInventoryItems(EquippedWeapon->GetSlotIndex(), 5);
}

void AShooterCharacter::ExtangeInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex)
{
	if (CurrentItemIndex != NewItemIndex && NewItemIndex < Inventory.Num() && (CombatState == ECombatState::ECS_Idle || CombatState == ECombatState::ECS_Equipping))
	{
		if (bAiming)
		{
			StopAiming();
		}

		auto OldEquippedWeapon = EquippedWeapon;
		auto NewEquippedWeapon = Cast<AWeapon>(Inventory[NewItemIndex]);
		EquipWeapon(NewEquippedWeapon);

		OldEquippedWeapon->SetItemState(EItemState::EIS_PickedUp);
		//NewEquippedWeapon->SetItemState(EItemState::EIS_Equipped);
		EquipWeapon(NewEquippedWeapon);

		CombatState = ECombatState::ECS_Equipping;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && EquipMontage)
		{
			AnimInstance->Montage_Play(EquipMontage, 1.f);
			AnimInstance->Montage_JumpToSection(FName("Equip"));
		}
		NewEquippedWeapon->PLayEquipSound(true);
	}
}

int32 AShooterCharacter::GetEmptyInventorySlot()
{
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i] == nullptr)
		{
			return i;
		}
	}
	if (Inventory.Num() < InventoryCapacity)
	{
		return Inventory.Num();
	}
	return -1;
}

void AShooterCharacter::HighlightInventorySlot()
{
	const int32 EmptySlot = GetEmptyInventorySlot();
	HighlightItemDelegate.Broadcast(EmptySlot, true);
	HighlightedSlot = EmptySlot;
}

EPhysicalSurface AShooterCharacter::GetSurfaceType()
{
	//Use line trace to determine the surface we're on
	FHitResult HitResult;
	//Allows us to get Physical Material
	FCollisionQueryParams QueryParams;
	QueryParams.bReturnPhysicalMaterial = true;

	GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(), GetActorLocation() + FVector(0, 0, -400.f), ECollisionChannel::ECC_Visibility,QueryParams);

	return UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());

}

void AShooterCharacter::EndStun()
{
	CombatState = ECombatState::ECS_Idle;
	if (bAimButtonPressed)
	{
		AimingButtonPressed();
	}
}

void AShooterCharacter::Die()
{
	if (bDead) return;
	UE_LOG(LogTemp, Warning, TEXT("Calling Die()"));
	CombatState = ECombatState::ECS_Dead;
	bDead = true;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Play Death Montage!"));
		AnimInstance->Montage_Play(DeathMontage);
	}
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		DisableInput(PC);
	}
}

void AShooterCharacter::FinishDeath()
{
	CombatState = ECombatState::ECS_Dead;
	GetMesh()->bPauseAnims = true;
}

bool AShooterCharacter::UnCrouchBlocked()
{

	FVector Start = GetActorLocation();
	FVector End = FVector(Start.X, Start.Y, Start.Z + StandingCapsuleHalfHeight+CrouchingCapsuleHalfHeight);

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);
	DrawDebugLine(GetWorld(), HitResult.TraceStart, HitResult.TraceEnd, FColor::Emerald,false,5.f);

	return HitResult.bBlockingHit;
}

void AShooterCharacter::UnHighlightInventorySlot()
{
	HighlightItemDelegate.Broadcast(HighlightedSlot, false);
	HighlightedSlot = -1;
}

int32 AShooterCharacter::GetInterpLocationIndex()
{
	int32 LowestIndex = 1;
	int32 LowestCount = INT_MAX;
	for (int32 i = 1; i < InterpLocations.Num(); i++)
	{
		if (InterpLocations[i].ItemCount < LowestCount)
		{
			LowestIndex = i;
			LowestCount = InterpLocations[i].ItemCount;
		}
	}
	return LowestIndex;
}

void AShooterCharacter::InterpLocationItemCount(int32 Index, int32 Amount)
{
	if (Amount > 1 || Amount < -1)
	{
		return;
	}
	if (InterpLocations.Num() >= Index)
	{
		InterpLocations[Index].ItemCount += Amount;
	}
}



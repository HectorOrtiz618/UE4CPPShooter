// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AmmoTypes.h"
#include "ShooterCharacter.generated.h"

class UInputComponent;
class USpringArmComponent;
class UInputComponent;
class UCameraComponent;
class USoundCue;
class UParticleSystem;
class UAnimMontage;
class AItem;
class AWeapon;
class AAmmo;
class AController;

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Idle UMETA(DisplayName = "Idle"),
	ECS_Shooting UMETA(DisplayName = "Shooting"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),
	ECS_Stunned UMETA(DisplayName = "Stunned"),
	ECS_Equipping UMETA(DisplayName = "Equipping"),

	ECS_Dead UMETA(DisplayName = "Ded"),
	ECS_MAX UMETA(DisplayName = "DefaultMAX"),
};
USTRUCT(BlueprintType)
struct FInterpLocation
{
	GENERATED_BODY();
	//Scene Component to use for its location for interping
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	USceneComponent* SceneComponent;

	//Number of Items interping to/at a particular SceneComponent
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemCount;
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, CurrentSlotIndex, int32, NewSlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHighlightItemDelegate, int32, SlotIndex, bool, bStartAnimation);
UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Jump() override;

	//Movement Functions
	void MoveForward(float Value);
	void MoveRight(float Value);

	UFUNCTION()
	void InterpCapsuleHalfHeght(float DeltaTime);

	//Controller look function
	void TurnAtRate(float Rate);//Left to Right Rotation,Rate is normalizes, that is, 1.0 will be 100% of the turn rate
	void LookUpAtRate(float Rate);//Up and Down Camera Rotation
	void FireWeapon(); //Called when Fire button is pressed

	//Mouse look functions
	void Turn(float Value);
	void Lookup(float Value);

	void AimingButtonPressed();
	void AimingButtonReleased();

	void SetAimFOV(float DeltaTime);
	void SetLookRates();

	void CalculateCrosshairSpread(float DeltaTime);
	void StartCrosshairBulletFire();

	AWeapon* SpawnDefaultWeapon();

	void EquipWeapon(AWeapon* Weapon, bool bSwapping = false);
	
	UFUNCTION()
	void FinishCrosshairBulletFire();
	
	bool GetBeamEndLocation(const FVector& MuzzleEndLocation, FHitResult& OutHitResult);


	//LineTrace to see if we enable Item Widget visibility
	bool TraceUnderCrosshair(FHitResult& OutHit, FVector& OutHitLocation, bool bFireingGun = false);

	void FireButtonPressed();
	void FireButtonReleased();

	void StartFireTimer();
	void FireTimerReset();
	void DropWeapon();
	void SelectButtonPressed();
	void SelectButtonReleased();

	//Drop current weapon and replace with TraceHitWeapon
	void SwapWeapon(AWeapon* WeaponWeaponToSwap);
	void PickupAmmo(AAmmo* PickupAmmo);

	void InitilizeAmmoMap();
	bool WeaponHasAmmo();

	void PlayFireSound();
	void SendBullet();
	void PlayGunFireMontage();

	void ReloadButtonPressed();
	void ReloadWeapon();

	bool CarryingAmmo();
	void CrouchButtonPressed();

	UFUNCTION(BlueprintCallable)
	void GrabMag();
	UFUNCTION(BlueprintCallable)
	void StopGrabMag();

	void Aim();
	void StopAiming();

	void InitializeInterpLocations();

	void FKeyPressed();
	void Key1Pressed();
	void Key2Pressed();
	void Key3Pressed();
	void Key4Pressed();
	void Key5Pressed();

	void ExtangeInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex);
	int32 GetEmptyInventorySlot();
	void  HighlightInventorySlot();

	UFUNCTION(BlueprintCallable)
	EPhysicalSurface GetSurfaceType();

	UFUNCTION(BlueprintCallable)
	void EndStun();

	void Die();
	UFUNCTION(BlueprintCallable)
	void FinishDeath();

	//Function that determines if a crouching character can be uncrouched
	bool UnCrouchBlocked();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
private:
	//Base Turn Rates and Looks Rates in Degrees per seconds other factors will affect the final turn rate
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", META = (AllowPrivateAccess = "true"))
	float BaseTurnRate;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", META = (AllowPrivateAccess = "true"))
	float BaseLookupRate;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", META = (AllowPrivateAccess = "true"))
	float HipTurnRate;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", META = (AllowPrivateAccess = "true"))
	float HipLookupRate;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", META = (AllowPrivateAccess = "true"))
	float AimTurnRate;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", META = (AllowPrivateAccess = "true"))
	float AimLookupRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", META = (AllowPrivateAccess = "true"))
	bool bCrouching;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", META = (AllowPrivateAccess = "true"))
	float BaseMovementRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", META = (AllowPrivateAccess = "true"))
	float CrouchingMovementRate;

	bool bAimButtonPressed;

	//Current HalfHeight of Capsule
	float CurrentCapsuleHalfHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", META = (AllowPrivateAccess = "true"))
	float StandingCapsuleHalfHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", META = (AllowPrivateAccess = "true"))
	float CrouchingCapsuleHalfHeight;
	
	//Mouse Turn and LookUp Rates
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", META = (AllowPrivateAccess = "true"), META = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipTurnRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", META = (AllowPrivateAccess = "true"), META = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipLookupRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", META = (AllowPrivateAccess = "true"), META = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimTurnRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", META = (AllowPrivateAccess = "true"), META = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimLookupRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp6;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	USceneComponent* WeaponInterpComp;

	//Array of Interp Locations Structs
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	TArray<FInterpLocation> InterpLocations;

	//Camera Boom
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", META = (AllowPrivateAccess = "true"))//Marks it for Garbage collection, exposes it to Blueprints, and allows the private character to be accessed  
	USpringArmComponent* CameraBoom;
	//Camera that follows the camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", META = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", META = (AllowPrivateAccess = "true"))
	UAnimMontage* HipFireMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", META = (AllowPrivateAccess = "true"))
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", META = (AllowPrivateAccess = "true"))
	UAnimMontage* EquipMontage;

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();
	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", META = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;

	//Smoke Trails for Bullets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", META = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", META = (AllowPrivateAccess = "true"))
	bool bAiming;
	
	float DefaultCameraFOV;
	float ZoomedCameraFOV;
	float CurrentCameraFOV;

	FHitResult ItemHitResult;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", META = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;
	
	//Crosshair Properties
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category ="Crosshair", META = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshair", META = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshair", META = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshair", META = (AllowPrivateAccess = "true"))
	float CrosshairAimFactor;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshair", META = (AllowPrivateAccess = "true"))
	float CrosshairShootingFactor;

	float ShootTimeDuration;
	bool bFireingBullet;
	FTimerHandle CrosshairShootTimer;

	//Automatic Fire values
	bool bFireButtonPressed;
	bool bShouldFire;//True when we can fire, false when waiting for fire


	FTimerHandle AutoFireTimer;

	//Should we trace for items?
	bool bShouldTraceForItems;
	//How many overlapped items are there?
	int8 OverlappedItemCount;

	//AItem we hit last frame
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Items", META = (AllowPrivateAccess = "true"))
	AItem* TraceHitItemLastFrame;
	void TraceForItems();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", META =(AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon;

	//Gives a default weapon on spawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", META = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Combat", META = (AllowPrivateAccess = "true"))
	AItem* TraceHitItem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", META = (AllowPrivateAccess = "true"))
	float CameraInterpDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", META = (AllowPrivateAccess = "true"))
	float CameraInterpElevation;

	//Map to keep track of diffrent ammo types;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items", META = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", META = (AllowPrivateAccess = "true"))
	int32 Starting9mmAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", META = (AllowPrivateAccess = "true"))
	int32 StartingARAmmo;
	
	//Combat State can only reload when not fireing or not ded
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items", META = (AllowPrivateAccess = "true"))
	ECombatState CombatState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", META = (AllowPrivateAccess = "true"))
	FTransform MagTransform;

	//Scene Component that attaches to characters hand when reloading
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", META = (AllowPrivateAccess = "true"))
	USceneComponent* HandSceneComponent;

	FTimerHandle PickupSoundTimer;
	FTimerHandle EquipSoundTimer;

	bool bShouldPlayPickupSound;
	bool bShouldPlayEquipSound;
	
	void ResetPickupTimer();
	void ResetEquipTimer();


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timers", META = (AllowPrivateAccess = "true"))
	float ResetPickupSoundTime;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timers", META = (AllowPrivateAccess = "true"))
	float ResetEquipSoundTime;
	//Array of AItems for inventory
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Inventory", META = (AllowPrivateAccess = "true"))
	TArray<AItem*> Inventory;

	const int32 InventoryCapacity = 6;

	//Delegate for sending slopt info to Inventory bar when equiping
	UPROPERTY(BlueprintAssignable,Category = "Delegates", META = (AllowPrivateAccess = "true"))
	FEquipItemDelegate ItemDelegate;
	//Delegate for playing icon animation
	UPROPERTY(BlueprintAssignable, Category = "Delegates", META = (AllowPrivateAccess = "true"))
	FHighlightItemDelegate HighlightItemDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory", META = (AllowPrivateAccess = "true"))
	int32 HighlightedSlot;

	//Character Health
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Combat", META = (AllowPrivateAccess = "true"))
	float Health;
	
	//Max Character Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", META = (AllowPrivateAccess = "true"))
	float MaxHealth;

	//Sound made by character when hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", META = (AllowPrivateAccess = "true"))
	USoundCue* MeleeImpactSound;

	//Blood particles when hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", META = (AllowPrivateAccess = "true"))
	UParticleSystem* BloodParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", META = (AllowPrivateAccess = "true"))
	UAnimMontage* StunnedMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", META = (AllowPrivateAccess = "true"))
	float StunChance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", META = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", META = (AllowPrivateAccess = "true"))
	bool bDead;

public:
	//returns Camera Boom
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom;}
	//returns FollowCamera
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE int8 GetOverLappedItemCount() const { return OverlappedItemCount; }
	void IncrementOverlappedItemCount(int8 Amount);

	void UnHighlightInventorySlot();

	FORCEINLINE bool GetAiming() const { return bAiming; }
	FORCEINLINE ECombatState GetCombatState()const { return CombatState; }
	FORCEINLINE bool GetCrouching() const{ return bCrouching; }
	
	FORCEINLINE bool GetShouldPlayPickupSound() const{ return bShouldPlayPickupSound; }
	FORCEINLINE bool GetShouldPlayEquipSound() const { return bShouldPlayEquipSound; }

	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }

	FORCEINLINE USoundCue* GetMeleeImpactSound() const { return MeleeImpactSound; }
	FORCEINLINE UParticleSystem* GetBloodParticles()const { return BloodParticle; };

	FORCEINLINE float GetStunChance() const { return StunChance; }

	FORCEINLINE bool GetDead() const { return bDead; }

	void Stun();

	void StartPickupSoundTimer();
	void StartEquipSoundTimer();
	
	FInterpLocation GetInterpLocation(int32 index);

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;

	//FVector GetCameraInterpLocation();

	int32 GetInterpLocationIndex();
	void InterpLocationItemCount(int32 Index, int32 Amount);

	void GetPickupItem(AItem* Item);

};

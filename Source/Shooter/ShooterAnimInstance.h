// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WeaponType.h"
#include "ShooterAnimInstance.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EOffsetState : uint8
{
	EOS_Hip UMETA(DisplayName = "Hip Aiming"),
	EOS_Aiming UMETA(DisplayName = "Iron Sights"),
	EOS_Reloading UMETA(DisplayName = "Reloading"),
	EOS_InAir UMETA(DisplayName = "In Air"),

	EOS_MAX UMETA(DisplayName = "DefaultMAX")

};
UCLASS()
class SHOOTER_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UShooterAnimInstance();
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

	virtual void NativeInitializeAnimation() override;
protected:
	void TurnInPlace();//Handle turning in place variables
	void Lean(float DeltaTime); //Handles Leaning Calculations while running
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", META = (AllowPrivateAccess = "true"))
	class AShooterCharacter* ShooterCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, META = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, META = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", META = (AllowPrivateAccess = "true"))
	bool bIsAiming;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, META = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement", META = (AllowPrivateAccess = "true"))
	bool bCrouching;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement", META = (AllowPrivateAccess = "true"))
	bool bEquipping;

	//Movement variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", META = (AllowPrivateAccess = "true"))
	float MovementOffsetYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", META = (AllowPrivateAccess = "true"))
	float LastMovementOffsetYaw;

	//Yaw of character this frame for turning in place (only updated when standing still and not in place)
	float TIPCharacterYaw;
	//YawofCharacter last frame for turning in place (only updated when standing still and not in place)
	float TIPCharacterYawLastFrame;

	//Yaw of character this frame
	FRotator CharacterRotation;
	//YawofCharacter last frame
	FRotator CharacterRotationLastFrame;
	
	//the diffrence in the character's Yaw when running
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category ="Lean", meta = (AllowPrivateAccess = "true"))
	float DeltaYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", META = (AllowPrivateAccess = "true"))
	float AimingPitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly , Category = "Turn In Place", META = (AllowPrivateAccess = "true"))
	float RootYawOffset;
	//Rotation curve value this frame
	float RotationCurve;
	float RotationCurveLastFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", META = (AllowPrivateAccess = "true"))
	bool bReloading;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", META = (AllowPrivateAccess = "true"))
	EOffsetState OffsetState;

	//Change weight based on actions
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", META = (AllowPrivateAccess = "true"))
	float RecoilWeight;
	
	//True when turning in place
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", META = (AllowPrivateAccess = "true"))
	bool bTurningInPlace;

	//Weapon Type the player currently has in their hands
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", META = (AllowPrivateAccess = "true"))
	EWeaponType EquippedWeaponType;

	//True when not reloading or equipping
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", META = (AllowPrivateAccess = "true"))
	bool bShouldUseFABRIK;
};

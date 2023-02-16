// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UShooterAnimInstance::NativeInitializeAnimation()
{
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}
void UShooterAnimInstance::TurnInPlace()
{
	if (ShooterCharacter == nullptr)
	{
		return;
	}
	//Setpitch
	AimingPitch = ShooterCharacter->GetBaseAimRotation().Pitch;
	if (ShooterCharacter->GetCombatState() == ECombatState::ECS_Reloading)
	{
		bReloading = true;
	}
	else
	{
		bReloading = false;
	}

	if (Speed > 0 || bIsInAir)
	{
		//Dont Do anything
		RootYawOffset = 0;
		TIPCharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		TIPCharacterYawLastFrame = TIPCharacterYaw;

		RotationCurveLastFrame = 0.f;
		RotationCurve = 0.f;

	}
	else
	{
		TIPCharacterYawLastFrame = TIPCharacterYaw;
		TIPCharacterYaw = ShooterCharacter->GetActorRotation().Yaw;

		const float YawDelta = TIPCharacterYaw - TIPCharacterYawLastFrame;

		//Root yaw offset updates and clamped to -180 to 180 inclusive
		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset-YawDelta);

		//1.f if turning else 0.f
		const float Turning = GetCurveValue(TEXT("Turning"));
		if (Turning > 0)
		{
			bTurningInPlace = true;
			RotationCurveLastFrame = RotationCurve;
			RotationCurve = GetCurveValue(TEXT("Rotation"));
			const float DeltaRotation = RotationCurve - RotationCurveLastFrame;

			//If RootYawOffset is positive, We're turning left
			//else were turning right if its less than zero
			RootYawOffset > 0 ? RootYawOffset -= DeltaRotation : RootYawOffset += DeltaRotation;
			//Check if we rotated too much
			const float ABSRootYawOffset = FMath::Abs(RootYawOffset);
			if (ABSRootYawOffset > 90.f)
			{
				const float ABSRootYawOffsetExcess = ABSRootYawOffset - 90.f;
				RootYawOffset > 0 ? RootYawOffset-= ABSRootYawOffsetExcess : RootYawOffset += ABSRootYawOffsetExcess;
			}
		}
		else
		{
			bTurningInPlace = false;
		}
	}
	if (bTurningInPlace && !bReloading)
	{
		RecoilWeight = 0.f;
	}
	else
	{
		if (bCrouching)
		{
			if (bReloading || bEquipping)
			{
				RecoilWeight = 1.f;
			}
			else
			{
				RecoilWeight = 0.f;
			}
		}
		else //NOT turning in place
		{
			if (bCrouching)
			{
				if (bReloading || bEquipping)
				{
					RecoilWeight = 1.f;
				}
				else
				{
					RecoilWeight = 0.1f;
				}
			}
			else
			{
				if (bIsAiming || bReloading || bEquipping)
				{
					RecoilWeight = 1.f;
				}
				else
				{
					RecoilWeight = .5f;
				}
			}
		}
	}
}
void UShooterAnimInstance::Lean(float DeltaTime)
{
	if (ShooterCharacter == nullptr) return;
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = ShooterCharacter->GetActorRotation();

	FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = (Delta.Yaw) / DeltaTime;
	const float Interp = FMath::FInterpTo(Delta.Yaw, Target, DeltaTime,6.f);
	DeltaYaw = FMath::Clamp(Interp, -90.f, 90.f);
	
}
UShooterAnimInstance::UShooterAnimInstance()
{
	TIPCharacterYaw = 0.f;
	TIPCharacterYawLastFrame = 0.f;
	RootYawOffset = 0.f;
	DeltaYaw = 0.f;
	
	
	bIsAiming= false;
	bIsAccelerating = false;
	bIsInAir = false;
	bReloading = false;
	
	Speed = 0.f;
	MovementOffsetYaw = 0.f;
	LastMovementOffsetYaw = 0.f;
	AimingPitch = 0.f;

	CharacterRotation = FRotator(0);
	CharacterRotationLastFrame = FRotator(0);

	OffsetState = EOffsetState::EOS_Hip;
	RecoilWeight = 1.f;

	EquippedWeaponType = EWeaponType::EWT_MAX;
	bShouldUseFABRIK = false;
}
void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}
	if (ShooterCharacter)
	{
		FVector Velocity = ShooterCharacter->GetVelocity();
		Velocity.Z = 0;//Ignore Up/Down Velocity
		Speed = Velocity.Size();

		bCrouching = ShooterCharacter->GetCrouching();
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();
		bEquipping = ShooterCharacter->GetCombatState() == ECombatState::ECS_Equipping;
		bShouldUseFABRIK = ShooterCharacter->GetCombatState() == ECombatState::ECS_Idle|| ShooterCharacter->GetCombatState() == ECombatState::ECS_Shooting;
		if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}
		bIsAiming = ShooterCharacter->GetAiming();
		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
		FRotator MoveRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MoveRotation, AimRotation).Yaw;
		if (ShooterCharacter->GetVelocity().Size() > 0.f)
		{
			LastMovementOffsetYaw = MovementOffsetYaw;
		}
		if (bReloading)
		{
			OffsetState = EOffsetState::EOS_Reloading;
		}
		else if (bIsInAir)
		{
			OffsetState = EOffsetState::EOS_InAir;
		}
		else if (ShooterCharacter->GetAiming())
		{
			OffsetState = EOffsetState::EOS_Aiming;
		}
		else
		{
			OffsetState = EOffsetState::EOS_Hip;
		}
		//Chech if ShooterCharacter has a valid equipped weapon
		if (ShooterCharacter->GetEquippedWeapon())
		{
			EquippedWeaponType = ShooterCharacter->GetEquippedWeapon() -> GetWeaponType();
		}
	}
	TurnInPlace();
	Lean(DeltaTime);
}

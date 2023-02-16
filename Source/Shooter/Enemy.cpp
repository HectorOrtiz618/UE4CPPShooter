// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Blueprint/UserWidget.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "EnemyAIController.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "ShooterCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Health = 100.f;
	MaxHealth = 100.f;


	HealthBarDisplayTime = 4.f;

	bCanHitReact = true;

	HitReactTimeMin = 0.5;
	HitReactTimeMax = 3.f;

	HitNumberLifeTime = .75;

	AggroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AggroSphere"));
	AggroSphere->SetupAttachment(GetRootComponent());

	CombatRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatRangeSphere->SetupAttachment(GetRootComponent());

	LeftWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftWeaponHitBox"));
	LeftWeaponCollision->SetupAttachment(GetMesh(),FName("LeftWeaponSocket"));

	RightWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightWeaponHitBox"));
	RightWeaponCollision->SetupAttachment(GetMesh(), FName("RightWeaponSocket"));

	bStunned = false;
	StunChance = .3f;

	LAAttack = TEXT("LA_Attack");
	RAAttack = TEXT("RA_Attack");
	LAAttackFast = TEXT("LA_Fast_Attack");
	RAAttackFast = TEXT("RA_Fast_Attack");

	BaseDamage = 20.f;

	LeftWeaponSocket = TEXT("LeftWeaponSocket");
	RightWeaponSocket = TEXT("RightWeaponSocket");

	AttackWaitTime = 2.5f;
	bCanAttack = true;

	bDying = false;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	AggroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AggroSphereOverlapped);
	
	CombatRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatRangeOverlaped);
	CombatRangeSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatRangeEndOverlap);

	LeftWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnLeftWeaponOverlap);
	RightWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnRightWeaponOverlap);


	//Disable WeaponCollision by default, we will use notifies to enable them
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);

	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);

	EnemyController = Cast<AEnemyAIController>(GetController());

	if (EnemyController)
	{
		EnemyController->GetBlackBoardComponent();
		EnemyController->GetBlackBoardComponent()->SetValueAsBool(FName("CanAttack"), true);
		EnemyController->GetBlackBoardComponent()->SetValueAsBool(FName("Dead"), false);
		EnemyController->GetBlackBoardComponent()->SetValueAsBool(FName("HasTaunted"), false);
	}
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility,ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECR_Ignore);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECR_Ignore);

	const FVector WorldPatrolPoint = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint);
	//DrawDebugSphere(GetWorld(), WorldPatrolPoint, 25.f, 12, FColor::Red, true);

	const FVector WorldPatrolPoint2 = UKismetMathLibrary::TransformLocation(GetActorTransform(), SecondPatrolPoint);
	//DrawDebugSphere(GetWorld(), WorldPatrolPoint2, 25.f, 12, FColor::Red, true);

	if (EnemyController && EnemyController->GetBlackBoardComponent())
	{
		EnemyController->GetBlackBoardComponent()->SetValueAsVector(TEXT("PatrolPoint"), WorldPatrolPoint);
		EnemyController->GetBlackBoardComponent()->SetValueAsVector(TEXT("SecondPatrolPoint"), WorldPatrolPoint2);
		EnemyController->RunBehaviorTree(BehaviorTree);


	}
}

void AEnemy::ShowHealthBar_Implementation()
{
	GetWorldTimerManager().ClearTimer(HealthBarDisplayTimer);
	GetWorldTimerManager().SetTimer(HealthBarDisplayTimer, this, &AEnemy::HideHealthBar, HealthBarDisplayTime);
}

void AEnemy::PlayHitMontage(FName Section, float PlayRate)
{
	if (bCanHitReact)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && HitMontage)
		{
			AnimInstance->Montage_Play(HitMontage, PlayRate);
			AnimInstance->Montage_JumpToSection(Section, HitMontage);
		}
		bCanHitReact = false;
		const float HitReactTime = FMath::FRandRange(HitReactTimeMin, HitReactTimeMax);
		GetWorldTimerManager().SetTimer(HitReactTimer, this, &AEnemy::ResetHitReactTimer, HitReactTime);
	}
}

void AEnemy::PlayAttackMontage(FName Section, float PlayRate)
{
	if (bInAttackRange)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(CombatMontage,PlayRate);
			AnimInstance->Montage_JumpToSection(Section, CombatMontage);
		}
		bCanAttack = false;
		if (EnemyController)
		{
			EnemyController->GetBlackBoardComponent()->SetValueAsBool(FName("CanAttack"), false);
		}
		GetWorldTimerManager().SetTimer(AttackWaitTimer, this, &AEnemy::ResetAttack, AttackWaitTime);
	}
}

FName AEnemy::GetAttackSectionName()
{
	const int32 Chance = FMath::RandRange(1, 4);
	switch (Chance)
	{
		case 1:
			return LAAttack;
			break;
		case 2:
			return LAAttackFast;
			break;
		case 3:
			return RAAttack;
			break;
		case 4:
			return RAAttackFast;
			break;
		default:
			break;
	}
	return FName();
}

void AEnemy::Die()
{
	if (bDying) return;
	bDying = true;
	HideHealthBar();
	//PLay DeathAnims and Destroy() 
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitMontage)
	{
		AnimInstance->Montage_Play(HitMontage);
		AnimInstance->Montage_JumpToSection(FName("Death_Back"), HitMontage);
	}
	if (EnemyController)
	{
		EnemyController->GetBlackBoardComponent()->SetValueAsBool(FName("Dead"), true);
	}
}

void AEnemy::ResetHitReactTimer()
{
	bCanHitReact = true;
}

void AEnemy::StoreHitNumber(UUserWidget* HitNumber, FVector Location)
{
	HitNumberMap.Add(HitNumber, Location);

	FTimerHandle HitNumberTimer;
	FTimerDelegate HitNumberDelegate;
	HitNumberDelegate.BindUFunction(this, FName("DestroyHitNumber"), HitNumber);

	GetWorld()->GetTimerManager().SetTimer(HitNumberTimer, HitNumberDelegate, HitNumberLifeTime, false);
}

void AEnemy::DestroyHitNumber(UUserWidget* HitNumber)
{
	HitNumberMap.Remove(HitNumber);
	HitNumber->RemoveFromParent();
}

void AEnemy::UpdateHitNumbers()
{
	//For every TPair in HitNumberMap, update the screen postion every frame
	for (auto& HitPair : HitNumberMap)
	{
		UUserWidget* HitNum = HitPair.Key;
		FVector HitLoc = HitPair.Value;


		//Update screen location every frame
		FVector2D ScreenPosition;
		UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), HitLoc, ScreenPosition);
		HitNum->SetPositionInViewport(ScreenPosition);
	}
}

void AEnemy::SetStunned(bool Stunned)
{
	bStunned = Stunned;
	if (EnemyController)
	{
		EnemyController->GetBlackBoardComponent()->SetValueAsBool(TEXT("Stunned"), Stunned);
	}
}

void AEnemy::AggroSphereOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AShooterCharacter* Shooter = Cast<AShooterCharacter>(OtherActor);
		if(Shooter)
		{
			if (EnemyController)
			{
				if (EnemyController->GetBlackBoardComponent())
				{
					EnemyController->GetBlackBoardComponent()->SetValueAsObject(TEXT("Target"), Shooter);
				}
			}
		}
	}
}

void AEnemy::CombatRangeOverlaped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;
	auto ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
	if (ShooterCharacter)
	{
		bInAttackRange = true;
		if (EnemyController)
		{
			EnemyController->GetBlackBoardComponent()->SetValueAsBool(TEXT("InAttackRange"), bInAttackRange);
		}
	}
}

void AEnemy::CombatRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == nullptr) return;
	auto ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
	if (ShooterCharacter)
	{
		bInAttackRange = false;
		if (EnemyController)
		{
			EnemyController->GetBlackBoardComponent()->SetValueAsBool(TEXT("InAttackRange"), bInAttackRange);
		}
	}
	
}

void AEnemy::OnLeftWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;
	auto Shooter = Cast<AShooterCharacter>(OtherActor);
	if (Shooter)
	{
		DoDamage(Shooter);
		SpawnBlood(Shooter, LeftWeaponSocket);
	}

}

void AEnemy::OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor== nullptr) return;
	auto Shooter = Cast<AShooterCharacter>(OtherActor);
	if (Shooter)
	{
		DoDamage(Shooter);
		SpawnBlood(Shooter, RightWeaponSocket);
	}

}

void AEnemy::ActivateLeftWeapon()
{
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::ActivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateLeftWeapon()
{
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::DeactivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::DoDamage(AShooterCharacter* Victim)
{
	if (Victim == nullptr) return;
	auto Shooter = Cast<AShooterCharacter>(Victim);
	if(Shooter)
	{
		UGameplayStatics::ApplyDamage(Shooter, BaseDamage, GetController(), this, UDamageType::StaticClass());
		if (Shooter->GetMeleeImpactSound())
		{
			UGameplayStatics::PlaySoundAtLocation(this, Shooter->GetMeleeImpactSound(), Shooter->GetActorLocation());
		}
		StunCharacter(Shooter);
	}
}

void AEnemy::SpawnBlood(AShooterCharacter* Shooter, FName SocketName)
{
	const USkeletalMeshSocket* TipSocket = GetMesh()->GetSocketByName(SocketName);
	if (TipSocket)
	{
		const FTransform SocketTransform = TipSocket->GetSocketTransform(GetMesh());
		if (Shooter->GetBloodParticles())
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Shooter->GetBloodParticles(), SocketTransform);
		}
	}
}

void AEnemy::StunCharacter(AShooterCharacter* Shooter)
{
	const float StunRoll = FMath::FRandRange(0.f, 1.f);
	if (StunRoll <= Shooter->GetStunChance())
	{
		Shooter->Stun();
	}
}

void AEnemy::ResetAttack()
{
	bCanAttack = true;
	if (EnemyController)
	{
		EnemyController->GetBlackBoardComponent()->SetValueAsBool(FName("CanAttack"), true);
	}
}

void AEnemy::FinishDeath()
{
	GetMesh()->bPauseAnims = true;
	//GetMesh()->SetSimulatePhysics(true);
	//GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetWorldTimerManager().SetTimer(DeathTimer,this,&AEnemy::DestroyEnemy,DeathTime);
}

void AEnemy::DestroyEnemy()
{
	Destroy();
}
void AEnemy::FinishTaunt()
{
	if (EnemyController && EnemyController->GetBlackBoardComponent())
	{
		EnemyController->GetBlackBoardComponent()->SetValueAsBool(FName("HasTaunted"), true);
		EnemyController->GetBlackBoardComponent()->ClearValue(FName("Target"));
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateHitNumbers();

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* ShooterController)
{
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, HitResult.Location, FRotator(0),true);
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (EnemyController)
	{
		EnemyController->GetBlackBoardComponent()->SetValueAsObject(FName("Target"), DamageCauser);
	}
	if (Health - DamageAmount <= 0.f)
	{
		Health = 0;
		Die();
	}
	else
	{
		Health -= DamageAmount;
	}
	ShowHealthBar();
	if (bDying) return DamageAmount;
	//Determine wheither enemy is stunned
	const float Stunned = FMath::FRandRange(0.f, 1.f);
	if (Stunned <= StunChance)
	{
		//Stun enemy
		SetStunned(true);
		PlayHitMontage(FName("Hit_Front"));
	}
	return DamageAmount;
}


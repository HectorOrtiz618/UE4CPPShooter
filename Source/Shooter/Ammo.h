// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "AmmoTypes.h"
#include "Ammo.generated.h"

/**
 * 
 */
class UStaticMeshComponent;
class USphereComponent;
UCLASS()
class SHOOTER_API AAmmo : public AItem
{
	GENERATED_BODY()
	
private:
	//Mesh for Ammo Pickup
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category ="Ammo", meta =(AllowPrivateAccess = "true"))
	UStaticMeshComponent* AmmoMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	EAmmoType AmmoType;
	//Texture for AmmoIcon
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	//UTexture2D* AmmoIcon;

	//Allows pickup when walking over Ammo
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	USphereComponent* AmmoCollisionSphere;

	UFUNCTION()
	void AmmoSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
protected:
	virtual void BeginPlay() override;
	virtual void SetItemProperties() override;

	void SphereCollisionOverlap();

	virtual void InitializeCustomDepth() override;
public:
	virtual void Tick(float DeltaTime) override;
	FORCEINLINE UStaticMeshComponent* GetAmmoMesh() { return AmmoMesh; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }

	virtual void EnableCustomDepth() override;
	virtual void DisableCustomDepth() override;
	AAmmo();
};

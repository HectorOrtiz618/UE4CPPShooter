// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"

/**
 * 
 */
class UUserWidget;
UCLASS()
class SHOOTER_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AShooterPlayerController();
protected:
	virtual void BeginPlay() override;
private:
	//Refrence to overall hud overlay BP class
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Widgets", META = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> HUDOverlayClass;

	//Variable to hold HUD overlay widget after creating it
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets", META = (AllowPrivateAccess = "true"))
	UUserWidget* Overlay;
};

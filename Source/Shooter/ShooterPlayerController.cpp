// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerController.h"
#include "Blueprint/UserWidget.h"

AShooterPlayerController::AShooterPlayerController()
{

}

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	//Check for HUD overlay class TSubclassOf variable
	if (HUDOverlayClass)
	{
		Overlay = CreateWidget<UUserWidget>(this,HUDOverlayClass);
		if (Overlay)
		{
			Overlay->AddToViewport();
			Overlay->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Zero_PlayerCameraManager.h"

void AZero_PlayerCameraManager::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController)
	{
		// Get the Pawn controlled by the PlayerController
		APawn* ControlledPawn = PlayerController->GetPawn();

		// Cast the pawn to your custom character class
		BaseCharacter = Cast<ABaseCharacter>(ControlledPawn);
	}
}





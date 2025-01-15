// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "BaseCharacter.h"
#include "Zero_PlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class ZERO_API AZero_PlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	UPROPERTY()
	ABaseCharacter* BaseCharacter;
};

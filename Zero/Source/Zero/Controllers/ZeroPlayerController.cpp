// Fill out your copyright notice in the Description page of Project Settings.


#include "ZeroPlayerController.h"
#include "Zero_PlayerCameraManager.h"

AZeroPlayerController::AZeroPlayerController()
{
	PlayerCameraManagerClass = AZero_PlayerCameraManager::StaticClass();
}

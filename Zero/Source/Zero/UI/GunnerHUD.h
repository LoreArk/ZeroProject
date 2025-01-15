// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyHUD.h"
#include "GunnerHUD.generated.h"

/**
 * 
 */
UCLASS()
class ZERO_API UGunnerHUD : public UEnemyHUD
{
	GENERATED_BODY()
public:


	virtual void SetDamagableComponent(UComponent_Damagable* DamagableComp) override;

	UFUNCTION(BlueprintNativeEvent)
	void UpdateAmmo(int32 Shots);
	void UpdateAmmo_Implementation(int32 Shots);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxShots;
};

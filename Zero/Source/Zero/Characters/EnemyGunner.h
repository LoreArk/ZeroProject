// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyCharacter.h"
#include "EnemyGunner.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTargetAimed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFPShoot, int32, NumOfShots);

UCLASS()
class ZERO_API AEnemyGunner : public AEnemyCharacter
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void Aim(const FInputActionValue& Value) override;
	virtual void AimCompleted(const FInputActionValue& Value) override;

	FTimerHandle AimTimerHandle;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* ShootMontage;

	virtual void AttackTraceTick_Implementation() override;

	virtual void Attack1Started() override;

	UPROPERTY(EditDefaultsOnly)
	float AimToShootDelay;

	UPROPERTY()
	FOnTargetAimed OnTargetAimed;

	UPROPERTY()
	FOnFPShoot OnFPShoot;

	UPROPERTY()
	FVector TargetShootLocation;

	virtual void Attack_Implementation() override;

	UFUNCTION()
	void AimTraceForAttackTarget();

	UFUNCTION()
	void Shoot();

	UFUNCTION()
	void StartShootTimer();

	UPROPERTY()
	float AttackTimer;

	UPROPERTY()
	bool AttackStarted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Shots;


protected:

	float EvasionTraceLenght;
};

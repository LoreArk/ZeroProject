// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PatrolRoute.h"
#include "DamageStructs.h"
#include "Interface_Enemy.generated.h"

class AAIAreaOfAction;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInterface_Enemy : public UInterface
{
	GENERATED_BODY()
};

class ZERO_API IInterface_Enemy
{
	GENERATED_BODY()

public:


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	AEnemyCharacter* GetOwnerCharacter();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	APatrolRoute* GetPatrolRoute();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	AAIAreaOfAction* GetAreaOfAction();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void PassiveToInvestigation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void LookAround();


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void ToggleEvasionFlag(bool bIsEnabled);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void SetIsTarget(bool bIsTarget, bool bCheckIsPossessable, bool bUsingPerception);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	bool CanBePossessed();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	bool CanReachTargetStraight();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	bool CanSeeTarget();


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void ReviveAfterDeath();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void SetPerceivedMaterial(bool bEnable);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void PlayWayPointAnimation(UAnimMontage* MontageToPlay);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void StopWayPointAnimation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	bool IsAttackTargetRunningClose();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	bool IsFacingAttackTargetBack();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void TryGrabAttack();


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void PunishAttackTraceStart();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void PunishAttackTraceTick();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void PunishAttackTraceEnd();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void DoGrabAttackDamage(FDamageSettings DamageSettings);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void StopGrabAttack();
};

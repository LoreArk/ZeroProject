// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIC_Base.h"
#include "PlayerCharacter.h"
#include "AIC_Ally.generated.h"

/**
 * 
 */
UCLASS()
class ZERO_API AAIC_Ally : public AAIC_Base
{
	GENERATED_BODY()

public:

	UPROPERTY()
	class APlayerAllyCharacter* CharacterRef;

	UPROPERTY(BlueprintReadWrite)
	class AActor* PlayerRef;

	UFUNCTION()
	void TryExecuteMoveToTask(FVector Location);

	UFUNCTION()
	void SetAttackTarget(AActor* Actor);

	UFUNCTION()
	void EndMoveToTask();

	UFUNCTION()
	void HandleAttackTargetDeath(AActor* Actor);

	virtual void OnDeath() override;

	virtual void SetStateAsPassive_Implementation() override;

	UFUNCTION()
	void HandlePlayerDamaged(AActor* Dealer);

	UFUNCTION(BlueprintCallable)
	void SetPlayerRef();

	UFUNCTION(BlueprintCallable)
	void RemovePlayerRef();

protected:

	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void UpdateDistanceState();

	UFUNCTION()
	virtual EDistanceState GetDistanceState();

	virtual void OnAttackMoveCompleted(FAIRequestID, const FPathFollowingResult& Result) override;
};

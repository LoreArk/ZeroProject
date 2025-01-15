// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "DummyPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class ZERO_API ADummyPlayerCharacter : public APlayerCharacter
{
	GENERATED_BODY()

public:

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void SetEvasionEnabled(bool bEnabled, float TimeRemaining) override;

	virtual void ToggleEvasionFlag_Implementation(bool bEnabled) override;

	UFUNCTION(BlueprintNativeEvent)
	void SetInUse(bool InUse);
	void SetInUse_Implementation(bool InUse);

	UFUNCTION()
	void DisableActor();

	UFUNCTION()
	void EnableActor(FTransform SpawnTransform);

	UFUNCTION()
	void CastDummyActionCompleted(const FInputActionInstance& Instance);

	UFUNCTION()
	void ReturnToPlayer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInUse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle AliveTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeToLive;

	
};

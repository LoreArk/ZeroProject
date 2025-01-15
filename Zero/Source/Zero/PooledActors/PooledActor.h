// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PooledActor.generated.h"

UCLASS()
class ZERO_API APooledActor : public AActor
{
	GENERATED_BODY()
	
public:	

	APooledActor();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pooling Settings")
	bool bInUse;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pooling Settings")
	float TimeToLive;

	UPROPERTY()
	FTimerHandle AliveTimerHandle;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetInUse(bool InUse);
	virtual void SetInUse_Implementation(bool InUse);

	UFUNCTION(BlueprintCallable)
	void ReturnToPool();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

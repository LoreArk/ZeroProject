// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PooledActor.h"
#include "Component_ObjectPool.generated.h"

USTRUCT(BlueprintType)
struct ZERO_API FPoolObject
{
	GENERATED_BODY()

public:


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX to pool settings")
	TSubclassOf<APooledActor> Actor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX to pool settings")
	int32 PoolSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<APooledActor*> ActorPool;

	FPoolObject()
		: Actor(TSubclassOf<APooledActor>()), PoolSize(0), ActorPool(TArray<APooledActor*>())
	{
	}
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZERO_API UComponent_ObjectPool : public UActorComponent
{
	GENERATED_BODY()

public:	

	UComponent_ObjectPool();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Actors To Pool")
	TArray<FPoolObject> ActorsToPool;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX to pool settings")
	TSubclassOf<APooledActor> AttackHitActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX to pool settings")
	int32 AttackHitPoolSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<APooledActor*> AttackHitPool;

	UFUNCTION(BlueprintCallable)
	APooledActor* SpawnActorFromPool(FTransform SpawnTransform, TArray<APooledActor*> Pool);

	UFUNCTION(BlueprintCallable)
	APooledActor* SpawnActorFromObjectPool(FTransform SpawnTransform, int32 ActorToPoolIndex);

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void InitializePools();

	UFUNCTION()
	APooledActor* FindFirstAvailableActorInPool(TArray<APooledActor*> ActorPool);


	UFUNCTION()
	APooledActor* FindAvailableActorInPools(int32 PoolIndex);


	UFUNCTION()
	virtual void InitializeActorToPool(FPoolObject ActorToPool);

};

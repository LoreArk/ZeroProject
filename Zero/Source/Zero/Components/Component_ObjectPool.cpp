// Fill out your copyright notice in the Description page of Project Settings.


#include "Component_ObjectPool.h"

UComponent_ObjectPool::UComponent_ObjectPool()
{
	PrimaryComponentTick.bCanEverTick = false;

}


void UComponent_ObjectPool::BeginPlay()
{
	Super::BeginPlay();

	InitializePools();
}

void UComponent_ObjectPool::InitializePools()
{
	for (int i = 0; i < AttackHitPoolSize; i++)
	{
		FActorSpawnParameters SpawnParams;
		FVector Location;
		FRotator Rotation;
		APooledActor* SpawnedActor = GetOwner()->GetWorld()->SpawnActor<APooledActor>(AttackHitActor, Location, Rotation, SpawnParams);
		AttackHitPool.AddUnique(SpawnedActor);
		SpawnedActor->SetInUse(false);
	}

	for (int i = 0; i < ActorsToPool.Num(); i++)
	{
		InitializeActorToPool(ActorsToPool[i]);
	}

}

APooledActor* UComponent_ObjectPool::FindFirstAvailableActorInPool(TArray<APooledActor*> ActorPool)
{
	for (int i = 0; i < ActorPool.Num(); i++)
	{
		if (ActorPool[i]->bInUse == false)
		{
			return ActorPool[i];
		}
	}
	return nullptr;
}

APooledActor* UComponent_ObjectPool::FindAvailableActorInPools(int32 PoolIndex)
{
	TArray<APooledActor*> PoolToCheck = ActorsToPool[PoolIndex].ActorPool;

	for (int i = 0; i < ActorsToPool[PoolIndex].ActorPool.Num(); i++)
	{

		if (ActorsToPool[PoolIndex].ActorPool[i]->bInUse == false)
		{
			return ActorsToPool[PoolIndex].ActorPool[i];
		}
	}
	return nullptr;
}


void UComponent_ObjectPool::InitializeActorToPool(FPoolObject ActorToPool)
{
	for (int i = 0; i < ActorToPool.PoolSize; i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("INITIALIZE POOL ACTOR FROM STRUCT"));
		FActorSpawnParameters SpawnParams;
		FVector Location;
		FRotator Rotation;
		APooledActor* SpawnedActor = GetOwner()->GetWorld()->SpawnActor<APooledActor>(ActorToPool.Actor, Location, Rotation, SpawnParams);
		ActorsToPool[i].ActorPool.AddUnique(SpawnedActor);
		SpawnedActor->SetInUse(false);
	}
}


APooledActor* UComponent_ObjectPool::SpawnActorFromPool(FTransform SpawnTransform, TArray<APooledActor*> Pool)
{
	APooledActor* ActorFromPool = FindFirstAvailableActorInPool(Pool);

	if (ActorFromPool)
	{
		ActorFromPool->SetActorLocation(SpawnTransform.GetLocation());
		ActorFromPool->SetActorRotation(SpawnTransform.GetRotation());
		ActorFromPool->SetInUse(true);
		return ActorFromPool;
	}
	else 
	{
		
	}

	return nullptr;
}

APooledActor* UComponent_ObjectPool::SpawnActorFromObjectPool(FTransform SpawnTransform, int32 ActorToPoolIndex)
{
	APooledActor* ActorFromPool = FindAvailableActorInPools(ActorToPoolIndex);

	if (ActorFromPool)
	{
		ActorFromPool->SetActorLocation(SpawnTransform.GetLocation());
		ActorFromPool->SetActorRotation(SpawnTransform.GetRotation());
		ActorFromPool->SetInUse(true);
		return ActorFromPool;
	}
	return nullptr;
}



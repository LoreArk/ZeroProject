// Fill out your copyright notice in the Description page of Project Settings.


#include "Component_PlayerObjectPool.h"

void UComponent_PlayerObjectPool::BeginPlay()
{
	Super::BeginPlay();
}

void UComponent_PlayerObjectPool::InitializePools()
{
	Super::InitializePools();

	

	for (int i = 0; i < FireballActorPoolSize; i++)
	{
		FActorSpawnParameters SpawnParams;
		FVector Location;
		FRotator Rotation;
		APooledActor* SpawnedActor = GetOwner()->GetWorld()->SpawnActor<APooledActor>(FireballActor, Location, Rotation, SpawnParams);
		FireballActorPool.AddUnique(SpawnedActor);
		SpawnedActor->SetInUse(false);
	}

	for (int i = 0; i < BlockerActorPoolSize; i++)
	{
		FActorSpawnParameters SpawnParams;
		FVector Location;
		FRotator Rotation;
		APooledActor* SpawnedActor = GetOwner()->GetWorld()->SpawnActor<APooledActor>(BlockerActor, Location, Rotation, SpawnParams);
		BlockerActorPool.AddUnique(SpawnedActor);
		SpawnedActor->SetInUse(false);
	}

	for (int i = 0; i < CatStepsActorPoolSize; i++)
	{
		FActorSpawnParameters SpawnParams;
		FVector Location;
		FRotator Rotation;
		APooledActor* SpawnedActor = GetOwner()->GetWorld()->SpawnActor<APooledActor>(CatStepsActor, Location, Rotation, SpawnParams);
		CatStepsActorPool.AddUnique(SpawnedActor);
		SpawnedActor->SetInUse(false);
	}

	for (int i = 0; i < CatStepsPermanentPoolSize; i++)
	{
		FActorSpawnParameters SpawnParams;
		FVector Location;
		FRotator Rotation;
		APooledActor* SpawnedActor = GetOwner()->GetWorld()->SpawnActor<APooledActor>(CatStepsPermanentActor, Location, Rotation, SpawnParams);
		CatStepsPermanentPool.AddUnique(SpawnedActor);
		SpawnedActor->SetInUse(false);
	}

	for (int i = 0; i < CatFootstepActorPoolSize; i++)
	{
		FActorSpawnParameters SpawnParams;
		FVector Location;
		FRotator Rotation;
		APooledActor* SpawnedActor = GetOwner()->GetWorld()->SpawnActor<APooledActor>(CatFootstepActor, Location, Rotation, SpawnParams);
		CatFootstepActorPool.AddUnique(SpawnedActor);
		SpawnedActor->SetInUse(false);
	}

	for (int i = 0; i < ExplosiveActorPoolSize; i++)
	{
		FActorSpawnParameters SpawnParams;
		FVector Location;
		FRotator Rotation;
		APooledActor* SpawnedActor = GetOwner()->GetWorld()->SpawnActor<APooledActor>(ExplosiveActor, Location, Rotation, SpawnParams);
		ExplosiveActorPool.AddUnique(SpawnedActor);
		SpawnedActor->SetInUse(false);
	}

	for(int i = 0; i < SoundActorPoolSize; i++)
	{
		FActorSpawnParameters SpawnParams;
		FVector Location;
		FRotator Rotation;
		APooledActor* SpawnedActor = GetOwner()->GetWorld()->SpawnActor<APooledActor>(SoundActor, Location, Rotation, SpawnParams);
		SoundActorPool.AddUnique(SpawnedActor);
		SpawnedActor->SetInUse(false);
	}
}

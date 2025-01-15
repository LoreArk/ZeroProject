// Fill out your copyright notice in the Description page of Project Settings.


#include "PooledActor.h"

APooledActor::APooledActor()
{
	PrimaryActorTick.bCanEverTick = true;

}

void APooledActor::SetInUse_Implementation(bool InUse)
{
	bInUse = InUse;
	SetActorEnableCollision(bInUse);
	SetActorHiddenInGame(!bInUse);
	SetActorTickEnabled(bInUse);

	if (GetWorldTimerManager().IsTimerActive(AliveTimerHandle))
		GetWorldTimerManager().ClearTimer(AliveTimerHandle);

	AliveTimerHandle.Invalidate();

	if (bInUse)
	{
		GetWorldTimerManager().SetTimer(AliveTimerHandle, this, &APooledActor::ReturnToPool, TimeToLive);
		/*GetWorld()->GetTimerManager().SetTimer(AliveTimerHandle, [this]()
			{
				ReturnToPool();
			}, TimeToLive, false);*/
	}
	
}

void APooledActor::ReturnToPool()
{
	if (this)
	{
		SetInUse(false);
	}
}

// Called when the game starts or when spawned
void APooledActor::BeginPlay()
{
	Super::BeginPlay();
	
	SetInUse(false);
}

void APooledActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	AliveTimerHandle.Invalidate();
}


void APooledActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


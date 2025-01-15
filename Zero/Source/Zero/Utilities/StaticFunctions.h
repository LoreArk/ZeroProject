// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageStructs.h"

class ZERO_API StaticFunctions
{
public:
	static FHitResult SphereCollisionTraceChannel(UObject* WorldContextObject, FVector Start, FVector End, ETraceTypeQuery TraceChannel, TArray<AActor*> ActorsToIgnore, float Radius);

	static FHitResult SphereCollisionTraceObject(UObject* WorldContextObject, FVector Start, FVector End, TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes, TArray<AActor*> ActorsToIgnore, float Radius);
	static FVector GetLocationAtDistanceXFromActorB(AActor* ActorA, AActor* ActorB, float Distance);

	static EDamageDirection GetDirectionOfHitFromDealer(AActor* DamageReceiver, AActor* DamageDealer);
};

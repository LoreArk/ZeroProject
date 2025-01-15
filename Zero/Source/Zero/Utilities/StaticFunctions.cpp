// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticFunctions.h"
#include "Kismet/KismetSystemLibrary.h"

FHitResult StaticFunctions::SphereCollisionTraceChannel(UObject* WorldContextObject, FVector Start, FVector End, ETraceTypeQuery TraceChannel, TArray<AActor*> ActorsToIgnore, float Radius)
{
	FHitResult HitResult;


	bool bHasHit =
		UKismetSystemLibrary::SphereTraceSingle
		(
			WorldContextObject,
			Start,
			End,
			Radius,
			TraceChannel,
			true,
			ActorsToIgnore,
			EDrawDebugTrace::None,
			HitResult,
			true,
			FLinearColor::Red, FLinearColor::Green, 0.2f
		);

	return HitResult;
}

FHitResult StaticFunctions::SphereCollisionTraceObject(UObject* WorldContextObject, FVector Start, FVector End, TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes, TArray<AActor*> ActorsToIgnore, float Radius)
{
	FHitResult HitResult;

	bool bHasHit =
		UKismetSystemLibrary::SphereTraceSingleForObjects
		(
			WorldContextObject,
			Start,
			End,
			Radius,
			ObjectTypes,
			true,
			ActorsToIgnore,
			EDrawDebugTrace::None,
			HitResult,
			true,
			FLinearColor::Red, FLinearColor::Green, 0.1f
		);
	return HitResult;
}

FVector StaticFunctions::GetLocationAtDistanceXFromActorB(AActor* ActorA, AActor* ActorB, float Distance)
{
	if (!ActorA || !ActorB)
	{
		return FVector::ZeroVector;
	}

	// Get the locations of Actor A and Actor B
	FVector LocationA = ActorA->GetActorLocation();
	FVector LocationB = ActorB->GetActorLocation();

	// Calculate the direction from Actor B to Actor A
	FVector Direction = LocationA - LocationB;

	// Normalize the direction to get the unit vector
	FVector NormalizedDirection = Direction.GetSafeNormal();

	// Scale the normalized direction by the desired distance
	FVector TargetLocation = LocationB + NormalizedDirection * Distance;

	return TargetLocation;
}

EDamageDirection StaticFunctions::GetDirectionOfHitFromDealer(AActor* DamageReceiver, AActor* DamageDealer)
{
	if (!DamageReceiver || !DamageDealer)
	{
		return EDamageDirection::None; 
	}

	FVector ReceiverForward = DamageReceiver->GetActorForwardVector();

	FVector ToDealer = DamageDealer->GetActorLocation() - DamageReceiver->GetActorLocation();

	ToDealer.Normalize();

	float DotProduct = FVector::DotProduct(ReceiverForward, ToDealer);

	//UE_LOG(LogTemp, Error, TEXT("GET HIT DIRECTION Dot Product Result: %f"), DotProduct);
	if (DotProduct > 0)
	{
		return EDamageDirection::Front;
	}
	else
	{
		return EDamageDirection::Back;
	}
}

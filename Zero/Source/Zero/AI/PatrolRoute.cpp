// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolRoute.h"
#include "Containers/Array.h" 
#include "Math/Vector.h" 
#include "WayPoint.h"

// Sets default values
APatrolRoute::APatrolRoute()
{
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void APatrolRoute::BeginPlay()
{
	Super::BeginPlay();
	
}

void APatrolRoute::IncrementPatrolRoute()
{
	if (WayPoints.Num() == 0 || WayPoints.Num() == 1) return;

	if (bIsCircular)
	{
		PatrolIndex++;

		if (PatrolIndex >= WayPoints.Num() && bLoop)
		{
			PatrolIndex = 0;
			UE_LOG(LogTemp, Warning, TEXT("PatrolIndex reset to 0."));
		}
	}
	else
	{
		if (PatrolIndex >= WayPoints.Num() - 1 && bLoop) // At the end
		{
			Direction = -1; // Reverse
		}
		else if (PatrolIndex <= 0) // At the start
		{
			Direction = 1; // Forward
		}
		PatrolIndex += Direction;
	}
	/*
	if (bIsCircular)
	{
		if (PatrolIndex == WayPoints.Num())
		{
			PatrolIndex = 0;
			Direction = 1;
		}
		else
		{
			if (PatrolIndex == 0)
			{
				Direction = 1;
			}
		}
	}
	else if (PatrolIndex == WayPoints.Num() || WayPoints.IsValidIndex(PatrolIndex) == false)
	{
		Direction = -1;
	}
	else
	{
		if (PatrolIndex == 0)
		{
			Direction = 1;
		}
	}
	PatrolIndex = PatrolIndex + Direction;
	*/
}

FVector APatrolRoute::GetWayPointLocation()
{
	if (WayPoints.Num() > 0)
	{
		if (WayPoints.IsValidIndex(PatrolIndex))
		{
			AWayPoint* WayPoint = WayPoints[PatrolIndex];
			if (WayPoint)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green,
					//FString::Printf(TEXT("Retrieved WayPoint at index %d: %s"),
					//	PatrolIndex, *WayPoint->GetName()));
				return WayPoint->GetActorLocation();
			}
			else
			{
				//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,
				//	FString::Printf(TEXT("WayPoint at index %d is NULL!"), PatrolIndex));
			}
		}
		else
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,
				//FString::Printf(TEXT("Invalid PatrolIndex: %d. WayPoints.Num() = %d"),
				//	PatrolIndex, WayPoints.Num()));
		}
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "WAYPOINT array is EMPTY!");
	}

	return FVector::ZeroVector;
}


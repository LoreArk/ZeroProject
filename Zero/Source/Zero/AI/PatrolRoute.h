// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WayPoint.h"
#include "PatrolRoute.generated.h"

UCLASS()
class ZERO_API APatrolRoute : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APatrolRoute();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UPROPERTY(VisibleAnywhere)
	int32 PatrolIndex;

	UPROPERTY(VisibleAnywhere)
	int32 Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AWayPoint*> WayPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsCircular;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLoop = true;

	UFUNCTION()
	void IncrementPatrolRoute();

	UFUNCTION()
	FVector GetWayPointLocation();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WayPoint.generated.h"

UCLASS()
class ZERO_API AWayPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	AWayPoint();

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WaitTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* AnimToPlay;
};

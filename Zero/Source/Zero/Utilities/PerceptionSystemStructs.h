// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PerceptionSystemStructs.generated.h"

USTRUCT(BlueprintType)
struct ZERO_API FNoiseStimulus
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
	float Loudness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	FVector Location;


	FNoiseStimulus()
		: Loudness(0), Location(FVector::ZeroVector)
	{}
};

USTRUCT(BlueprintType)
struct ZERO_API FSightSettings
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight Settings")
	float Range;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight Settings")
	float PeripheralAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight Settings")
	FName OriginSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight Settings")
	float LooseSightRange;


	FSightSettings()
		: Range(0), PeripheralAngle(0), OriginSocketName(NAME_None), LooseSightRange(0)
	{}
};


class ZERO_API PerceptionSystemStructs
{
public:
	PerceptionSystemStructs();
	~PerceptionSystemStructs();
};

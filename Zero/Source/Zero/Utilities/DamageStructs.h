// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayEnums.h"
#include "CoreMinimal.h"
#include "DamageStructs.generated.h"

USTRUCT(BlueprintType)
struct ZERO_API FDamageSettings
{
    GENERATED_BODY()

public:
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    int32 DamageAmount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    EDamageType DamageType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    EDamageResponse DamageResponse;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    bool bDamageInvincible;

    FDamageSettings()
        : DamageAmount(0), DamageType(EDamageType::Physical), DamageResponse(EDamageResponse::None), bDamageInvincible(false)
    {}
};

USTRUCT(BlueprintType)
struct ZERO_API FHitReactionSettings
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    EDamageType DamageType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    EDamageResponse DamageResponse;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    EDamageDirection Direction;

    FHitReactionSettings()
        :  DamageType(EDamageType::Physical), DamageResponse(EDamageResponse::None), Direction(EDamageDirection::None)
    {}
};

class ZERO_API DamageStructs
{
public:
	DamageStructs();
	~DamageStructs();
};

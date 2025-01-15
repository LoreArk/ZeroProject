// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EDamageType : uint8
{
    Physical  UMETA(DisplayName = "Physical"),
    Fire      UMETA(DisplayName = "Fire"),
    Bullet    UMETA(DisplayName = "Bullet"),
    Fall      UMETA(DisplayName = "Fall"),
    Explosion UMETA(DisplayName = "Explosion")
};

UENUM(BlueprintType)
enum class EDamageResponse : uint8
{
    None            UMETA(DisplayName = "None"),
    HitReaction     UMETA(DisplayName = "HitReaction"),
    Knockback       UMETA(DisplayName = "Knockback"),
    Fall            UMETA(DisplayName = "Fall"),
};


UENUM(BlueprintType)
enum class EDamageDirection : uint8
{
    None            UMETA(DisplayName = "None"),
    Front           UMETA(DisplayName = "Front"),
    Back            UMETA(DisplayName = "Back")
};
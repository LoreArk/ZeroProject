// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageStructs.h"
#include "Components/Image.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "Engine/DataTable.h"
#include "InventoryStructs.generated.h"

/**
 * 
 */


UENUM(BlueprintType)
enum class ESigilAction : uint8
{
   // None            UMETA(DisplayName = "None"),
    Fireball        UMETA(DisplayName = "Fireball"),
    Blocker         UMETA(DisplayName = "Blocker"),
    CatSteps        UMETA(DisplayName = "CatSteps"),
    SoundDeceiving  UMETA(DisplayName = "SoundDeceiving"),
    Smoke           UMETA(DisplayName = "Smoke"),
    Explosive       UMETA(DisplayName = "Explosive"),
    None            UMETA(DisplayName = "None")
};

UENUM(BlueprintType)
enum class ESigilActionType : uint8
{
    None        UMETA(DisplayName = "None"),
    Self        UMETA(DisplayName = "Self"),
    Mine        UMETA(DisplayName = "Mine"),
    Launch      UMETA(DisplayName = "Launch"),
    Activation  UMETA(DisplayName = "Activation")
};

UENUM(BlueprintType)
enum class EMaterialType : uint8
{
    Carp        UMETA(DisplayName = "Carp"),
    Resin       UMETA(DisplayName = "Resin"),
    Flower      UMETA(DisplayName = "Flower")
};

UENUM(BlueprintType)
enum class ESigilIconState : uint8
{
    Available       UMETA(DisplayName = "Crafted"),
    Craftable       UMETA(DisplayName = "Craftable"),
    NoMaterials     UMETA(DisplayName = "NoMaterials"),
    Unknown         UMETA(DisplayName = "Unknown")
};

USTRUCT(BlueprintType)
struct ZERO_API FSigilIconSettings
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sigil Settings")
    FName SigilName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sigil Settings")
    ESigilAction SigilType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sigil Settings")
    EMaterialType MaterialType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sigil Settings")
    int32 MaterialRequired;


    FSigilIconSettings()
        : SigilName("None"), SigilType(ESigilAction::None), MaterialType(EMaterialType::Carp), MaterialRequired(1)
    {}
};

USTRUCT(BlueprintType)
struct ZERO_API FSigilSettings
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sigil Settings")
    FName SigilName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    ESigilActionType Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    UAnimMontage* Montage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    ESigilAction Effect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    FDamageSettings DamageSettings;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float Duration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    bool bGrabCounter;

    FSigilSettings()
        : SigilName("None"), Type(ESigilActionType::None), Montage(nullptr), Effect(ESigilAction::None), DamageSettings(), Duration(0.0f), bGrabCounter(false)
    {}
};

USTRUCT(BlueprintType)
struct ZERO_API FCraftableSigil
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sigil")
    FSigilIconSettings SigilIconSettings;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sigil")
    int32 CraftableUnits;

    FCraftableSigil()
        : SigilIconSettings(), CraftableUnits(0)
    {}
};

USTRUCT(BlueprintType)
struct ZERO_API FCraftedSigil
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sigil")
    FSigilSettings SigilSettings;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sigil")
    int32 CraftedUnits;

    FCraftedSigil()
        : SigilSettings(), CraftedUnits(0)
    {}
};


class ZERO_API InventoryStructs
{
public:
	InventoryStructs();
	~InventoryStructs();
};

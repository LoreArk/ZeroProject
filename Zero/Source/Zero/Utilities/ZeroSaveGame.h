// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIC_Base.h"
#include "InventoryStructs.h"
#include "KeyItem.h"
#include "GameFramework/SaveGame.h"
#include "ZeroSaveGame.generated.h"

class APlayerCharacter;
class AEnemyCharacter;

USTRUCT(BlueprintType)
struct FEnemySaveData
{
    GENERATED_BODY()

    UPROPERTY()
    FTransform Transform = FTransform::Identity;

    UPROPERTY()
    float Health = 100.0f;

    UPROPERTY()
    EAIStates AIState = EAIStates::Passive;

    UPROPERTY()
    int32 CurrentPatrolIndex = -1;

    UPROPERTY()
    bool bIsDeathTimerActive = false;

    UPROPERTY()
    float TimerRemainingTime = 0.0f;

    UPROPERTY()
    FString ID;

    FEnemySaveData() {}
};

USTRUCT(BlueprintType)
struct FLevelInteractableSaveData
{
    GENERATED_BODY()

    UPROPERTY()
    bool bEnabled = true;

    UPROPERTY()
    FString ID;

    FLevelInteractableSaveData() {}
};

USTRUCT(BlueprintType)
struct FPlayerInventorySaveData
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<FKeySettings> KeyItems;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FSigilSettings EquippedSigil;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FSigilSettings> SigilPool;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FCraftedSigil> CreatedSigils;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FSigilIconSettings> SigilsTableSettings;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<EMaterialType, int32> CraftingMaterials;

    FPlayerInventorySaveData() {}
};

USTRUCT(BlueprintType)
struct FPlayerSaveData
{
    GENERATED_BODY()

    UPROPERTY()
    FTransform Transform = FTransform::Identity;

    UPROPERTY()
    float Health = 100.0f;

    UPROPERTY()
    float Mana = 100.0f;

    UPROPERTY()
    bool bEvasionDisabled = false;

    UPROPERTY()
    float EvasionDisabledRemainingCooldown = 0.0f;

    UPROPERTY()
    FPlayerInventorySaveData InventoryData;

    FPlayerSaveData() {}
};

USTRUCT(BlueprintType)
struct FShrineSaveData
{
    GENERATED_BODY()

    UPROPERTY()
    int32 SoulsCount = 0;

    UPROPERTY()
    int32 SoulsForManaUnit = 0;

    UPROPERTY()
    int32 AvailableMana = 0;

    UPROPERTY()
    int32 AvailableManaCap = 0;

    UPROPERTY()
    TArray<AActor*> Enemies; // Use unique identifiers instead of raw actor pointers.

    UPROPERTY()
    FString ID;

    FShrineSaveData() {}
};

/**
 *
 */
UCLASS()
class ZERO_API UZeroSaveGame : public USaveGame
{
    GENERATED_BODY()

public:

    UPROPERTY()
    FPlayerSaveData PlayerData;

    UPROPERTY()
    TArray<FEnemySaveData> EnemiesData;

    UPROPERTY()
    TArray<FLevelInteractableSaveData> InteractablesData;

    UPROPERTY()
    TArray<FShrineSaveData> ShrinesData;
};
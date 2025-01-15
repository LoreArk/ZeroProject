// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Level.h"
#include "F_LevelData.generated.h"

USTRUCT(BlueprintType)
struct FLevelUIData
{
    GENERATED_BODY();


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Setting")
    FText LevelName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Setting")
    FText LevelDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Setting")
    FText LevelObjective;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Setting")
    UTexture2D* LevelImage;

    FLevelUIData()
        : LevelName(FText::GetEmpty()),
        LevelDescription(FText::GetEmpty()),
        LevelObjective(FText::GetEmpty()),
        LevelImage(nullptr)
    {
    }
};

USTRUCT(BlueprintType)
struct FLevelLoadData
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading Setting")
    TSoftObjectPtr<UWorld> LevelAsset;

    FLevelLoadData()
        : LevelAsset(nullptr)
    {
    }
};

USTRUCT(BlueprintType)
struct FLevelData
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading Setting")
    FLevelLoadData LevelLoadData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Setting")
    FLevelUIData LevelUIData;

    FLevelData()
    {
    }
};

USTRUCT(BlueprintType)
struct FLevelDataTable : public FTableRowBase
{
    GENERATED_BODY();

    FLevelDataTable()
    {
    }

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level Data")
    TArray<FLevelData> LevelsData;
};
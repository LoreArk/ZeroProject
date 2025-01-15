// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "F_LevelData.h"
#include "ZeroGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ZERO_API UZeroGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	virtual void Init() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsLoadingLevel;

	UFUNCTION(BlueprintNativeEvent)
	void BeginLoadingScreen(const FString& MapName);
	virtual void BeginLoadingScreen_Implementation(const FString& MapName);

	UFUNCTION(BlueprintNativeEvent)
	void EndLoadingScreen(UWorld* InLoadedWorld);
	virtual void EndLoadingScreen_Implementation(UWorld* InLoadedWorld);

	UFUNCTION(BlueprintCallable)
	void LevelTransition(FLevelLoadData LevelLoadData, FName PlayerStartTag);

	UFUNCTION()
	void StartLoadLevelTimer(TSoftObjectPtr<UWorld> LevelToLoad);

	UFUNCTION()
	void OnLoadLevel(TSoftObjectPtr<UWorld> LevelToLoad);

	UPROPERTY()
	FTimerHandle LoadLevelTimerHandle;

	FName SelectedPlayerStartTag;

	UFUNCTION(BlueprintCallable)
	void SetSelectedPlayerStart(FName PlayerStartTag);

	FName GetPreferredPlayerStart() const;
};

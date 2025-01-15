// Fill out your copyright notice in the Description page of Project Settings.


#include "ZeroGameInstance.h"
#include "Runtime/RenderCore/Public/RenderingThread.h"
#include "Kismet/GameplayStatics.h"

void UZeroGameInstance::Init()
{
	Super::Init();

	FlushRenderingCommands();
	
	//FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UZeroGameInstance::BeginLoadingScreen);

	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UZeroGameInstance::EndLoadingScreen);
}

void UZeroGameInstance::BeginLoadingScreen_Implementation(const FString& MapName)
{


	UE_LOG(LogTemp, Error, TEXT("BEGIN LOADING SCREEN"));
	bIsLoadingLevel = true;
}

void UZeroGameInstance::EndLoadingScreen_Implementation(UWorld* InLoadedWorld)
{
	FlushAsyncLoading();

	FlushRenderingCommands();
	bIsLoadingLevel = false;
	UE_LOG(LogTemp, Error, TEXT("END LOADING SCREEN"));
}

void UZeroGameInstance::LevelTransition(FLevelLoadData LevelLoadData, FName PlayerStartTag)
{
	if (LevelLoadData.LevelAsset.IsNull() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("GAME INSTANCE OPEN LEVEL"));
		SetSelectedPlayerStart(PlayerStartTag);
		StartLoadLevelTimer(LevelLoadData.LevelAsset);
	}
}

void UZeroGameInstance::StartLoadLevelTimer(TSoftObjectPtr<UWorld> LevelToLoad)
{
	float Delay = 1.0f;

	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, FName("OnLoadLevel"), LevelToLoad);

	BeginLoadingScreen(LevelToLoad.GetAssetName());
	GetWorld()->GetTimerManager().SetTimer(LoadLevelTimerHandle, Delegate, Delay, false);
}

void UZeroGameInstance::OnLoadLevel(TSoftObjectPtr<UWorld> LevelToLoad)
{
	GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Orange, " TIMER LOAD LEVEL");

	UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), LevelToLoad);

}

void UZeroGameInstance::SetSelectedPlayerStart(FName PlayerStartTag)
{
	SelectedPlayerStartTag = PlayerStartTag;

}

FName UZeroGameInstance::GetPreferredPlayerStart() const
{
	return SelectedPlayerStartTag;
}

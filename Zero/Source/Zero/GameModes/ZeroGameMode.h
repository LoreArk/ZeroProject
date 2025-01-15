// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CombatManager.h"
#include "ZeroSaveGame.h"
#include "Item.h"
#include "Interface_GameMode.h"
#include "ZeroGameMode.generated.h"

class AShrine;
/**
 * 
 */

UCLASS()
class ZERO_API AZeroGameMode : public AGameModeBase, public IInterface_GameMode
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<AItem*> ManaItems;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	APlayerCharacter* PlayerCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	ACombatManager* CombatManager;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	int32 MaxAttackingActors = 1;

	UPROPERTY()
	FTimerHandle PlayerDeathTimerHandle;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlayerDeathEvent(AActor* Actor);
	void PlayerDeathEvent_Implementation(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	APlayerCharacter* GetPlayerCharacter();

	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> MemorizedCharacters;

	UFUNCTION()
	void AddMemorizedCharacter(AActor* ActorToAdd);

	UFUNCTION()
	void SetHighlightMemorizedCharacters(bool bHighlightEnabled);

	virtual AZeroGameMode* GetZeroGameMode_Implementation() override;

	virtual int32 GetPlayerCurrentMana_Implementation() override;

	virtual void ConsumePlayerMana_Implementation(int32 Amount) override;

	UFUNCTION(BlueprintNativeEvent)
	void PauseGame();
	void PauseGame_Implementation();

	UFUNCTION()
	void GameOver();

	UFUNCTION()
	void RestartCurrentLevel();

	//Save system
	UPROPERTY(BlueprintReadWrite)
	bool GameSaved;

	void SavePlayerData(UZeroSaveGame* SaveGameInstance, class APlayerCharacter* Player);

	void LoadPlayerData(UZeroSaveGame* SaveGameInstance, class APlayerCharacter* Player);

	void SaveEnemyData(UZeroSaveGame* SaveGameInstance, const TArray<class AEnemyCharacter*>& Enemies);

	void LoadEnemyData(UZeroSaveGame* SaveGameInstance, TArray<class AEnemyCharacter*>& Enemies);

	void SaveInteractablesActorData(UZeroSaveGame* SaveGameInstance);
	void LoadInteractablesActorData(UZeroSaveGame* SaveGameInstance);

	void SaveShrineData(UZeroSaveGame* SaveGameInstance);
	void LoadShrineData(UZeroSaveGame* SaveGameInstance);

	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void SaveGameToDisk();

	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void LoadGameFromDisk();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void LoadedGameEvent();
	void LoadedGameEvent_Implementation();


	//Enemies IDs system
	FString GenerateUniqueEnemyID();
	void RegisterEnemyID(const FString& ID, AEnemyCharacter* Enemy);
	void UnregisterEnemyID(const FString& ID);

	//Generic Interactable IDs System
	FString GenerateUniqueActorID();
	void RegisterActorID(const FString& ID, AInteractableStateActor* Actor);

	//Shrine IDs System
	FString GenerateUniqueShrineID();
	void RegisterShrineID(const FString& ID, AShrine* Actor);

protected:

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	FString SaveSlotName = TEXT("Checkpoint");

	UFUNCTION()
	AEnemyCharacter* FindEnemyByUniqueID(const FString& ID);

	UPROPERTY()
	TMap<FString, AEnemyCharacter*> EnemiesMap;

	UFUNCTION()
	AInteractableStateActor* FindActorByUniqueID(const FString& ID);

	UPROPERTY()
	TMap<FString, AInteractableStateActor*> InteractableActorsMap;


	UFUNCTION()
	AShrine* FindShrineByUniqueID(const FString& ID);

	UPROPERTY()
	TMap<FString, AShrine*> ShrineActorsMap;
};

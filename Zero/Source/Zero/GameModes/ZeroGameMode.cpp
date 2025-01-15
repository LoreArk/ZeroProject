// Fill out your copyright notice in the Description page of Project Settings.


#include "ZeroGameMode.h"
#include "PlayerCharacter.h"
#include "EnemyCharacter.h"
#include "Shrine.h"
#include "Interface_Damagable.h"
#include "ZeroGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "Interface_Enemy.h"

void AZeroGameMode::BeginPlay()
{
	Super::BeginPlay();

	CombatManager = GetWorld()->SpawnActor<ACombatManager>(ACombatManager::StaticClass());
	CombatManager->MaxAttackingActors = MaxAttackingActors;

	PlayerCharacter = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());


	if (PlayerCharacter == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ZeroGameMode BeginPlay - PlayerCharacter not fount! Check DefaultPawnClass"))
	}
	else
	{
		IInterface_Damagable::Execute_GetDamagableComponent(PlayerCharacter)->OnDeath.AddDynamic(this, &AZeroGameMode::PlayerDeathEvent);
		PlayerCharacter->OnGamePaused.AddDynamic(this, &AZeroGameMode::PauseGame);
	}
}

void AZeroGameMode::PlayerDeathEvent_Implementation(AActor* Actor)
{
	GetWorldTimerManager().SetTimer(PlayerDeathTimerHandle, this, &AZeroGameMode::GameOver, 4.f);
}

APlayerCharacter* AZeroGameMode::GetPlayerCharacter()
{
	if (PlayerCharacter == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ZeroGameMode GetPlayerCharacter - PlayerCharacter not fount! Check DefaultPawnClass"));
	}

	return PlayerCharacter;
}

void AZeroGameMode::AddMemorizedCharacter(AActor* ActorToAdd)
{
	MemorizedCharacters.AddUnique(ActorToAdd);
}

void AZeroGameMode::SetHighlightMemorizedCharacters(bool bHighlightEnabled)
{
	if (MemorizedCharacters.Num() <= 0)
		return;

	for (AActor* actor : MemorizedCharacters)
	{
		if (actor != nullptr)
		{
			IInterface_Enemy::Execute_SetPerceivedMaterial(actor, bHighlightEnabled);
		}
	}
}

AZeroGameMode* AZeroGameMode::GetZeroGameMode_Implementation()
{
	return this;
}

int32 AZeroGameMode::GetPlayerCurrentMana_Implementation()
{
	if (PlayerCharacter)
	{
		return PlayerCharacter->DamagableComponent->Mana;
	}

	UE_LOG(LogTemp, Error, TEXT("ZeroGameMode GetPlayerCurrentMana_Implementation - PlayerCharacter not fount! Check DefaultPawnClass"))

	return 0;
}

void AZeroGameMode::ConsumePlayerMana_Implementation(int32 Amount)
{
	if (PlayerCharacter)
	{
		IInterface_Player::Execute_ConsumeMana(PlayerCharacter, Amount);
	}

	UE_LOG(LogTemp, Error, TEXT("ZeroGameMode GetPlayerCurrentMana_Implementation - PlayerCharacter not fount! Check DefaultPawnClass"))

}

void AZeroGameMode::PauseGame_Implementation()
{
}

void AZeroGameMode::GameOver()
{
	if (GameSaved)
	{
		LoadGameFromDisk();
	}
	else
	{
		RestartCurrentLevel();
	}
}

void AZeroGameMode::RestartCurrentLevel()
{
	UWorld* World = GetWorld();
	if (World)
	{
		UZeroGameInstance* ZeroGI = Cast<UZeroGameInstance>(UGameplayStatics::GetGameInstance(World));
		if (!ZeroGI)
		{
			UE_LOG(LogTemp, Error, TEXT("GameInstance not found. Cannot restart level."));
			return;
		}

		FString FullMapName = World->GetMapName();

		int32 LIndex = FullMapName.Find(TEXT("L_"));
		FString CurrentLevelName = (LIndex != INDEX_NONE) ? FullMapName.Mid(LIndex) : FullMapName;

		UE_LOG(LogTemp, Error, TEXT("Restarting current level: %s"), *CurrentLevelName);

		ZeroGI->BeginLoadingScreen(CurrentLevelName);

		UGameplayStatics::OpenLevel(this, FName(CurrentLevelName));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("World not found. Cannot restart level."));
	}
}

void AZeroGameMode::SavePlayerData(UZeroSaveGame* SaveGameInstance, APlayerCharacter* Player)
{
	if (!SaveGameInstance || !Player) return;

	SaveGameInstance->PlayerData.Transform = Player->GetActorTransform();
	SaveGameInstance->PlayerData.Health = Player->DamagableComponent->Health;
	SaveGameInstance->PlayerData.Mana = Player->DamagableComponent->Mana;
	SaveGameInstance->PlayerData.InventoryData.CraftingMaterials = Player->InventoryComponent->CraftingMaterials;
	SaveGameInstance->PlayerData.InventoryData.CreatedSigils = Player->InventoryComponent->CreatedSigils;
	SaveGameInstance->PlayerData.InventoryData.EquippedSigil = Player->InventoryComponent->EquippedSigil;
	SaveGameInstance->PlayerData.InventoryData.KeyItems = Player->InventoryComponent->KeyItems;
	SaveGameInstance->PlayerData.InventoryData.SigilPool = Player->InventoryComponent->SigilPool;
	SaveGameInstance->PlayerData.InventoryData.SigilsTableSettings = Player->InventoryComponent->SigilsTableSettings;
	SaveGameInstance->PlayerData.bEvasionDisabled = Player->bEvasionDisabled;
	if (Player->bEvasionDisabled)
	{
		SaveGameInstance->PlayerData.EvasionDisabledRemainingCooldown = Player->GetWorldTimerManager().GetTimerRemaining(Player->EvasionCoolDownTimerHandle);
	}
	else
	{
		SaveGameInstance->PlayerData.EvasionDisabledRemainingCooldown = 0.0f;
	}
}

void AZeroGameMode::LoadPlayerData(UZeroSaveGame* SaveGameInstance, APlayerCharacter* Player)
{
	if (!SaveGameInstance || !Player) return;

	IInterface_BaseCharacter::Execute_ForcedSetTransform(Player, SaveGameInstance->PlayerData.Transform);
	Player->DamagableComponent->Health = SaveGameInstance->PlayerData.Health;
	Player->DamagableComponent->Mana = SaveGameInstance->PlayerData.Mana;
	Player->InventoryComponent->CraftingMaterials = SaveGameInstance->PlayerData.InventoryData.CraftingMaterials;
	Player->InventoryComponent->CreatedSigils = SaveGameInstance->PlayerData.InventoryData.CreatedSigils;
	Player->InventoryComponent->EquippedSigil = SaveGameInstance->PlayerData.InventoryData.EquippedSigil;
	Player->InventoryComponent->KeyItems = SaveGameInstance->PlayerData.InventoryData.KeyItems;
	Player->InventoryComponent->SigilPool = SaveGameInstance->PlayerData.InventoryData.SigilPool;
	Player->InventoryComponent->SigilsTableSettings = SaveGameInstance->PlayerData.InventoryData.SigilsTableSettings;
	Player->bEvasionDisabled = SaveGameInstance->PlayerData.bEvasionDisabled;
	Player->EvasionCooldownRemainingTime = SaveGameInstance->PlayerData.EvasionDisabledRemainingCooldown;

	Player->OnGameLoaded.Broadcast();
}

void AZeroGameMode::SaveEnemyData(UZeroSaveGame* SaveGameInstance, const TArray<class AEnemyCharacter*>& Enemies)
{
	if (!SaveGameInstance) return;

	for (const TPair<FString, AEnemyCharacter*>& EnemyPair : EnemiesMap)
	{
		AEnemyCharacter* Enemy = EnemyPair.Value;
		if (Enemy)
		{
			FEnemySaveData EnemyData;

			EnemyData.ID = EnemyPair.Key; 
			EnemyData.Transform = Enemy->GetActorTransform();
			EnemyData.Health = Enemy->DamagableComponent->Health;
			EnemyData.AIState = Enemy->AIC->AiState;

			EnemyData.bIsDeathTimerActive = Enemy->GetWorldTimerManager().IsTimerActive(Enemy->DeathTimerHandle);
			if (EnemyData.bIsDeathTimerActive)
			{
				EnemyData.TimerRemainingTime = Enemy->GetWorldTimerManager().GetTimerRemaining(Enemy->DeathTimerHandle);
			}
			else
			{
				EnemyData.TimerRemainingTime = 0.f;
			}

			SaveGameInstance->EnemiesData.Add(EnemyData);
		}
	}
}

void AZeroGameMode::LoadEnemyData(UZeroSaveGame* SaveGameInstance, TArray<class AEnemyCharacter*>& Enemies)
{
	if (!SaveGameInstance) return;

	for (const FEnemySaveData& EnemyData : SaveGameInstance->EnemiesData)
	{
		AEnemyCharacter* Enemy = FindEnemyByUniqueID(EnemyData.ID);
		if (Enemy)
		{
			Enemy->SetActorTransform(EnemyData.Transform);
			Enemy->DamagableComponent->Health = EnemyData.Health;
			Enemy->AIC->AiState = EnemyData.AIState;

			if (EnemyData.bIsDeathTimerActive && EnemyData.TimerRemainingTime > 0.f)
			{
				Enemy->GetWorldTimerManager().SetTimer(
					Enemy->DeathTimerHandle,
					Enemy,
					&AEnemyCharacter::Revive,
					EnemyData.TimerRemainingTime,
					false
				);
			}
			else
			{
				Enemy->GetWorldTimerManager().ClearTimer(Enemy->DeathTimerHandle);
			}

			Enemy->OnGameLoaded.Broadcast();
		}
	}
}

void AZeroGameMode::SaveInteractablesActorData(UZeroSaveGame* SaveGameInstance)
{
	if (!SaveGameInstance) return;

	for (const TPair<FString, AInteractableStateActor*>& InteractablePair : InteractableActorsMap)
	{
		AInteractableStateActor* Interactable = InteractablePair.Value;
		if(Interactable)
		{
			FLevelInteractableSaveData InteractableData;

			InteractableData.ID = InteractablePair.Key;
			InteractableData.bEnabled = Interactable->GetState();
			
			SaveGameInstance->InteractablesData.Add(InteractableData);

			InteractablePair.Value->OnGameSaved.Broadcast();
		}
	}
}

void AZeroGameMode::LoadInteractablesActorData(UZeroSaveGame* SaveGameInstance)
{
	if (!SaveGameInstance) return;

	for (const FLevelInteractableSaveData& InteractableData : SaveGameInstance->InteractablesData)
	{
		AInteractableStateActor* InteractableActor = FindActorByUniqueID(InteractableData.ID);
		if (InteractableActor)
		{
			InteractableActor->SetState(InteractableData.bEnabled);

			InteractableActor->OnGameLoaded.Broadcast();
		}
	}

}

void AZeroGameMode::SaveShrineData(UZeroSaveGame* SaveGameInstance)
{
	if (!SaveGameInstance) return;

	for (const TPair<FString, AShrine*>& ShrinePair : ShrineActorsMap)
	{
		AShrine* Shrine = ShrinePair.Value;
		if (Shrine)
		{
			FShrineSaveData ShrineData; 
			
			ShrineData.ID = ShrinePair.Key;
			ShrineData.AvailableMana = Shrine->AvailableMana;
			ShrineData.AvailableManaCap = Shrine->AvailableManaCap;
			ShrineData.Enemies = Shrine->Enemies;
			ShrineData.SoulsCount = Shrine->SoulsCount;
			ShrineData.SoulsForManaUnit = Shrine->SoulsForManaUnit;

			SaveGameInstance->ShrinesData.Add(ShrineData);
		}
	}
}

void AZeroGameMode::LoadShrineData(UZeroSaveGame* SaveGameInstance)
{
	if (!SaveGameInstance) return;

	for (const FShrineSaveData& ShrineData : SaveGameInstance->ShrinesData)
	{
		AShrine* Shrine = FindShrineByUniqueID(ShrineData.ID);
		if (Shrine)
		{
			Shrine->AvailableMana = ShrineData.AvailableMana;
			Shrine->AvailableManaCap = ShrineData.AvailableManaCap;
			Shrine->Enemies = ShrineData.Enemies;
			Shrine->SoulsCount = ShrineData.SoulsCount;
			Shrine->SoulsForManaUnit = ShrineData.SoulsForManaUnit;

			Shrine->InitManaVFX();
			Shrine->UpdateManaVFX();
		}
	}
}

void AZeroGameMode::SaveGameToDisk()
{
	UZeroSaveGame* SaveGameInstance = Cast<UZeroSaveGame>(UGameplayStatics::CreateSaveGameObject(UZeroSaveGame::StaticClass()));
	if (!SaveGameInstance) return;

	GameSaved = true;

	TArray<AEnemyCharacter*> Enemies;

	SavePlayerData(SaveGameInstance, GetPlayerCharacter());
	SaveEnemyData(SaveGameInstance, Enemies);
	SaveInteractablesActorData(SaveGameInstance);
	SaveShrineData(SaveGameInstance);

	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveSlotName, 0);

	UE_LOG(LogTemp, Log, TEXT("Game Saved!"));
}

void AZeroGameMode::LoadGameFromDisk()
{
	UZeroSaveGame* SaveGameInstance = Cast<UZeroSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	if (!SaveGameInstance) return;

	TArray<AEnemyCharacter*> Enemies;

	LoadedGameEvent();

	LoadPlayerData(SaveGameInstance, GetPlayerCharacter());
	LoadEnemyData(SaveGameInstance, Enemies);
	LoadInteractablesActorData(SaveGameInstance);
	LoadShrineData(SaveGameInstance);

	UE_LOG(LogTemp, Log, TEXT("Game Loaded!"));
}

void AZeroGameMode::LoadedGameEvent_Implementation()
{
}


FString AZeroGameMode::GenerateUniqueEnemyID()
{
	FString NewID;
	do
	{
		NewID = FGuid::NewGuid().ToString();
	} while (EnemiesMap.Contains(NewID));

	return NewID;
}

void AZeroGameMode::RegisterEnemyID(const FString& ID, AEnemyCharacter* Enemy)
{
	if (Enemy && !EnemiesMap.Contains(ID))
	{
		EnemiesMap.Add(ID, Enemy);
	}
}

void AZeroGameMode::UnregisterEnemyID(const FString& ID)
{
	if (EnemiesMap.Contains(ID))
	{
		EnemiesMap.Remove(ID);
	}
}

AEnemyCharacter* AZeroGameMode::FindEnemyByUniqueID(const FString& ID)
{
	AEnemyCharacter** FoundEnemy = EnemiesMap.Find(ID);

	if (FoundEnemy)
	{
		return *FoundEnemy;
	}

	return nullptr;
}

AInteractableStateActor* AZeroGameMode::FindActorByUniqueID(const FString& ID)
{
	if (InteractableActorsMap.Contains(ID))
	{
		return InteractableActorsMap[ID];
	}
	return nullptr;
}

AShrine* AZeroGameMode::FindShrineByUniqueID(const FString& ID)
{
	if (ShrineActorsMap.Contains(ID))
	{
		return ShrineActorsMap[ID];
	}
	return nullptr;
}

FString AZeroGameMode::GenerateUniqueActorID()
{
	FString NewID;
	do
	{
		NewID = FGuid::NewGuid().ToString();
	} while (InteractableActorsMap.Contains(NewID));

	return NewID;
}

void AZeroGameMode::RegisterActorID(const FString& ID, AInteractableStateActor* Actor)
{
	if (Actor && !InteractableActorsMap.Contains(ID))
	{
		InteractableActorsMap.Add(ID, Actor);
	}
}

FString AZeroGameMode::GenerateUniqueShrineID()
{
	FString NewID;
	do
	{
		NewID = FGuid::NewGuid().ToString();
	} while (ShrineActorsMap.Contains(NewID));

	return NewID;
}

void AZeroGameMode::RegisterShrineID(const FString& ID, AShrine* Actor)
{
	if (Actor && !ShrineActorsMap.Contains(ID))
	{
		ShrineActorsMap.Add(ID, Actor);
	}
}

AActor* AZeroGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	UZeroGameInstance* GameInstance = Cast<UZeroGameInstance>(GetGameInstance());
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance not found. Falling back to default player start selection."));
		return Super::ChoosePlayerStart_Implementation(Player);
	}

	FName PreferredTag = GameInstance->GetPreferredPlayerStart();
	if (PreferredTag.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("Preferred PlayerStart tag is not set. Falling back to default player start selection."));
		return Super::ChoosePlayerStart_Implementation(Player);
	}

	UE_LOG(LogTemp, Log, TEXT("Looking for PlayerStart with tag: %s"), *PreferredTag.ToString());

	for (APlayerStart* PlayerStart : TActorRange<APlayerStart>(GetWorld()))
	{
		if (PlayerStart)
		{
			UE_LOG(LogTemp, Log, TEXT("Found PlayerStart with tag: %s"), *PlayerStart->PlayerStartTag.ToString());
			if (PlayerStart->PlayerStartTag == PreferredTag)
			{
				UE_LOG(LogTemp, Log, TEXT("Selected PlayerStart with tag: %s"), *PlayerStart->PlayerStartTag.ToString());
				return PlayerStart;
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("No PlayerStart found with the preferred tag: %s. Falling back to default selection."), *PreferredTag.ToString());
	return Super::ChoosePlayerStart_Implementation(Player);
}






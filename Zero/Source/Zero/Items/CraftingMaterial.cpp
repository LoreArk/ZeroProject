// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftingMaterial.h"
#include "Interface_Player.h"
#include "Kismet/GameplayStatics.h"
#include "ZeroGameMode.h"
#include "PlayerCharacter.h"

void ACraftingMaterial::Interact_Implementation(AActor* InteractingActor)
{
	Super::Interact_Implementation(InteractingActor);

	AZeroGameMode* gm = Cast<AZeroGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (gm)
	{
		gm->GetPlayerCharacter()->InventoryComponent->AddCraftingMaterial(MaterialType, Amount);
	}
}


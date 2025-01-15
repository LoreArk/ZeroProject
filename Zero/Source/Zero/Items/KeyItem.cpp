// Fill out your copyright notice in the Description page of Project Settings.


#include "KeyItem.h"
#include "Kismet/GameplayStatics.h"
#include "ZeroGameMode.h"
#include "PlayerCharacter.h"

void AKeyItem::Interact_Implementation(AActor* InteractingActor)
{
	Super::Interact_Implementation(InteractingActor);

	AZeroGameMode* gm = Cast<AZeroGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (gm)
	{
		gm->GetPlayerCharacter()->InventoryComponent->AddKeyItem(Settings);
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableStateActor.h"
#include "ZeroGameMode.h"

AInteractableStateActor::AInteractableStateActor()
{
    PrimaryActorTick.bCanEverTick = true;
	bEnabled = true;
}

void AInteractableStateActor::BeginPlay()
{
    Super::BeginPlay();

    if (UniqueID.IsEmpty())
    {
        AZeroGameMode* GameMode = Cast<AZeroGameMode>(GetWorld()->GetAuthGameMode());
        if (GameMode)
        {
            UniqueID = GameMode->GenerateUniqueActorID();
            GameMode->RegisterActorID(UniqueID, this);
        }
    }
}

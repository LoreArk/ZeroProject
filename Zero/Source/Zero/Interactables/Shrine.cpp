// Fill out your copyright notice in the Description page of Project Settings.


#include "Shrine.h"
#include "Interface_Player.h"
#include "Kismet/GameplayStatics.h"


AShrine::AShrine()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AShrine::BeginPlay()
{
	Super::BeginPlay();

    if (UniqueID.IsEmpty())
    {
        AZeroGameMode* GameMode = Cast<AZeroGameMode>(GetWorld()->GetAuthGameMode());
        if (GameMode)
        {
            UniqueID = GameMode->GenerateUniqueShrineID();
            GameMode->RegisterShrineID(UniqueID, this);
        }
    }

    TArray<AActor*> TempEnemies;

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyCharacter::StaticClass(), TempEnemies);

    for (AActor* Actor : TempEnemies)
    {
        AEnemyCharacter* EnemyCharacter = IInterface_Enemy::Execute_GetOwnerCharacter(Actor);
        if (EnemyCharacter)
        {
            Enemies.Add(EnemyCharacter);

            EnemyCharacter->DamagableComponent->OnDeath.AddDynamic(this, &AShrine::AddSouls);
        }
    }

    InitManaVFX();
    UpdateManaVFX();
}

void AShrine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShrine::AddSouls(AActor* DeadActor)
{
    if (Enemies.Contains(DeadActor) == false)
        return;

    SoulsCount++;

    AEnemyCharacter* EnemyCharacter = IInterface_Enemy::Execute_GetOwnerCharacter(DeadActor);
    //EnemyCharacter->DamagableComponent->OnDeath.RemoveDynamic(this, &AShrine::AddSouls);

    Enemies.Remove(DeadActor);
    SetAvailableMana();

    EnemyDeath(DeadActor);
    //Update HUD
}

void AShrine::SetAvailableMana()
{
    int32 Souls = SoulsCount / SoulsForManaUnit;

    AvailableMana = FMath::Clamp(Souls, 0, AvailableManaCap);

    UpdateManaVFX();

    if (AvailableMana >= 1)
    {
        //Set text color active
    }
    else
    {
        //set text color disabled
    }

}

void AShrine::UpdateManaVFX_Implementation()
{
}

void AShrine::DisplayHUD()
{
}

void AShrine::InitManaVFX_Implementation()
{
}

void AShrine::UpdateSoulsCount(int32 ManaToSubtract)
{
    int32 SoulsToSubtract = ManaToSubtract * SoulsForManaUnit;

    SoulsCount -= SoulsToSubtract;

    //UpdateSoulsCounter HUD
    SetAvailableMana();
}

void AShrine::EnemyDeath_Implementation(AActor* EnemyActor)
{
}

void AShrine::Interact_Implementation(AActor* InteractingActor)
{
    UE_LOG(LogTemp, Warning, TEXT("SHRINE INTERACTION"));
    if (InteractingActor->Implements<UInterface_Player>() == false)
        return;


    UE_LOG(LogTemp, Warning, TEXT("SHRINE INTERACTION - Interactor is player"));
    if (AvailableMana <= 0)
    {
        AZeroGameMode* GameMode = Cast<AZeroGameMode>(GetWorld()->GetAuthGameMode());
        GameMode->SaveGameToDisk();

        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("SHRINE INTERACTION - Interactor is player, shrine has mana"));

    UComponent_Damagable* damagable = IInterface_Damagable::Execute_GetDamagableComponent(InteractingActor);

    MaxManaRequested = damagable->MaxMana - damagable->Mana;

    if (MaxManaRequested < 1)
    {
        return;
    }
    if (MaxManaRequested >= AvailableMana)
    {
        IInterface_Player::Execute_ShrineInteraction(InteractingActor, AvailableMana);
        UpdateSoulsCount(AvailableMana);
        UpdateManaVFX();
    }
    else
    {
        int32 ManaToGive = AvailableMana - MaxManaRequested;
        IInterface_Player::Execute_ShrineInteraction(InteractingActor, ManaToGive);
        UpdateSoulsCount(ManaToGive);
        UpdateManaVFX();

    }
}

void AShrine::EnterInteractionTargeting_Implementation()
{
}

void AShrine::ExitInteractionTargeting_Implementation()
{
}


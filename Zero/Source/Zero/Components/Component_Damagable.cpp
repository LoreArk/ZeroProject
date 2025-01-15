// Fill out your copyright notice in the Description page of Project Settings.


#include "Component_Damagable.h"
#include "Interface_Damagable.h"
#include "StaticFunctions.h"

UComponent_Damagable::UComponent_Damagable()
{
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UComponent_Damagable::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void UComponent_Damagable::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool UComponent_Damagable::TakeDamage(FDamageSettings DamageSettings, AActor* DamageDealer)
{
    if (Health <= 0)
       return false;

    if (bIsIntangible)
        return false;

    if (bIsInvincible && DamageSettings.bDamageInvincible == false)
        return false;

    Health -= DamageSettings.DamageAmount;

    if (Health <= 0)
    {
        Health = 0;
        OnDeath.Broadcast(GetOwner());
        return true;
    }

    LastDamageSettings = DamageSettings;

    if (DamageSettings.DamageAmount > 0)
        OnDamage.Broadcast(DamageDealer);

    DamageResponse(DamageSettings.DamageResponse, DamageDealer);

    return true;
}

void UComponent_Damagable::DamageResponse(EDamageResponse DamageResponse, AActor* DamageDealer)
{
    UAnimMontage* TargetMontage = nullptr;

    AActor* HitActor = GetOwner();  // Or the actual actor reference you're working with
    if (HitActor)
    {
       // UE_LOG(LogTemp, Warning, TEXT("DamageResponse called on actor: %s"), *HitActor->GetName());
    }

   // UE_LOG(LogTemp, Warning, TEXT("Searching for DamageResponse: %s"), *UEnum::GetValueAsString(DamageResponse));
    
    EDamageDirection Direction = StaticFunctions::GetDirectionOfHitFromDealer(GetOwner(), DamageDealer);

    for (const auto& Elem : HitReactions)
    {
       // UE_LOG(LogTemp, Warning, TEXT("Checking HitReaction with DamageResponse: %s"),
         //   *UEnum::GetValueAsString(Direction));

        if (DamageResponse == EDamageResponse::Knockback)
        {
            if (Elem.Value.DamageResponse == DamageResponse && Elem.Value.Direction == Direction)
            {
                TargetMontage = Elem.Key;
                break;
            }
        }
        else 
        {
            if (Elem.Value.DamageResponse == DamageResponse)
            {
                TargetMontage = Elem.Key;
                break;
            }
        }
    }

    if (TargetMontage == nullptr)
    {
       // UE_LOG(LogTemp, Error, TEXT("UComponenet_Damagable::DamageResponse - TargetMontage is null!"))
            return;
    }
    //UE_LOG(LogTemp, Warning, TEXT("DAMAGABLE PLAY HIT REACTION"));

    IInterface_Damagable::Execute_PlayHitReaction(GetOwner(), TargetMontage);
}

void UComponent_Damagable::FallDamage(float StartHeigth)
{
	float FallHeight = StartHeigth - GetOwner()->GetActorLocation().Z;

    if (FallHeight > MinFallDamageHeightThreshold) //if fall height surpass a minimum heigh treshold for damage, apply one 1 unit damage for each times the ExtraHeight surpass UnitFallDamageThreshold 
    {
        float ExtraHeight = FallHeight - MinFallDamageHeightThreshold;
        FString HeightMessage = FString::Printf(TEXT("ExtraHeight: %f"), ExtraHeight); 
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, HeightMessage);

        float FloatDamage = ExtraHeight / UnitFallDamageThreshold;

        int32 DamageToApply = (int32)FloatDamage;

        DamageToApply += 1;

        FString DamageMessage = FString::Printf(TEXT("Fall Damage Applied: %d"), DamageToApply);
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, DamageMessage);

        Health -= DamageToApply;

        if (Health <= 0)
        {
            Health = 0;
            OnDeath.Broadcast(GetOwner());
            return;
        }

        DamageResponse(EDamageResponse::Fall, nullptr);

    }
    else if (FallHeight > MinFallHeightForLanding && SoftLandMontage != nullptr)
    {
        IInterface_Damagable::Execute_PlayHitReaction(GetOwner(), SoftLandMontage);
    }
}



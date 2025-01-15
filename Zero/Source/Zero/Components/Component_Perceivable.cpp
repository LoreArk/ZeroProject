// Fill out your copyright notice in the Description page of Project Settings.


#include "Component_Perceivable.h"
#include "Component_Perception.h"
#include "BaseCharacter.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UComponent_Perceivable::UComponent_Perceivable()
{
	PrimaryComponentTick.bCanEverTick = false;

	SightTargetSocket = "sight_target";
}


// Called when the game starts
void UComponent_Perceivable::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Perception component - BeginPlay called"));

	OwnerCharacter = Cast<ABaseCharacter>(GetOwner());
	if (OwnerCharacter == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Perception component - BeginPlay: Character is null"));
	}
}

bool UComponent_Perceivable::IsSameTeam(ETeam ActorTeam)
{
	return ActorTeam == Team;
}

void UComponent_Perceivable::MakeNoise(FNoiseStimulus Noise)
{
	NoiseSettings = Noise;

	OnMakeNoise.Broadcast(GetOwner());
}

ETeam UComponent_Perceivable::GetTeam()
{
	return Team;
}

FVector UComponent_Perceivable::GetSightTarget()
{
	if (OwnerCharacter == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Perception component - GetSightTarget: Character is null"));
		return FVector::ZeroVector;
	}

	return OwnerCharacter->GetMesh()->GetSocketLocation(SightTargetSocket);
}



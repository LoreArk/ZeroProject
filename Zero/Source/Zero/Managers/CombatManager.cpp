// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatManager.h"
#include "Kismet/GameplayStatics.h"
#include "Interface_Damagable.h"
#include "Component_Damagable.h"

// Sets default values
ACombatManager::ACombatManager()
{
	PrimaryActorTick.bCanEverTick = true;

	MaxAttackingActors = 1;
}

void ACombatManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACombatManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ACombatManager::CanAttack(AActor* Actor)
{
	if (AttackingActors.Num() < MaxAttackingActors || AttackingActors.Contains(Actor))
	{
		return true;
	}

	return false;
}

void ACombatManager::AddAttackingActor(AActor* Actor)
{
	if (!AttackingActors.Contains(Actor))
	{
		AttackingActors.AddUnique(Actor);
		//UComponent_Damagable* Damagable = IInterface_Damagable::Execute_GetDamagableComponent(Actor);
		//Damagable->OnDeath.AddDynamic(this, &ACombatManager::EnemyDeathHandler);
	}
}

void ACombatManager::EnemyDeathHandler(AActor* Actor)
{
	RemoveAttackingActor(Actor);

	if (!DeadEnemies.Contains(Actor))
	{
		DeadEnemies.Add(Actor);
	}
}

void ACombatManager::RemoveAttackingActor(AActor* Actor)
{
	if (AttackingActors.Contains(Actor))
	{
		AttackingActors.Remove(Actor);
	}
}


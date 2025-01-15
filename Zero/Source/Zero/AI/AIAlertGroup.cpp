

#include "AIAlertGroup.h"
#include "EnemyCharacter.h"
#include "Interface_Enemy.h"

AAIAlertGroup::AAIAlertGroup()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AAIAlertGroup::BeginPlay()
{
	Super::BeginPlay();
	
	for (AActor* actor : AlertGroup)
	{
		AEnemyCharacter* enemy = IInterface_Enemy::Execute_GetOwnerCharacter(actor);
		AAIC_EnemyBase* AIC = Cast<AAIC_EnemyBase>(enemy->GetController());
		AIC->OnCombatStart.AddDynamic(this, &AAIAlertGroup::OnMemberAlerted);
	}
}

void AAIAlertGroup::OnMemberAlerted(FVector ActorLocation)
{
	if (bGroupAlerted)
		return;

	bGroupAlerted = true;

	for (AActor* actor : AlertGroup)
	{
		IInterface_Enemy::Execute_GetOwnerCharacter(actor)->OnAlertStart.Broadcast(ActorLocation);
	}
}

void AAIAlertGroup::EndAlert()
{
	for (AActor* actor : AlertGroup)
	{
		IInterface_Enemy::Execute_GetOwnerCharacter(actor)->OnAlertEnd.Broadcast();
	}
}


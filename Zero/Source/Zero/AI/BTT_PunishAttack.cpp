// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_PunishAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interface_Enemy.h"
#include "AIC_EnemyBase.h"
#include "EnemyCharacter.h"

UBTT_PunishAttack::UBTT_PunishAttack(FObjectInitializer const& ObjectInitializer)
{
	NodeName = "Punish Attack";
}

EBTNodeResult::Type UBTT_PunishAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{
	return EBTNodeResult::Succeeded;
}

void UBTT_PunishAttack::OnAttackFinished(AAIC_EnemyBase* AIC)
{
	
}

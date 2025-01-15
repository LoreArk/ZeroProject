// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_SetMovementSpeed.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIC_EnemyBase.h"
#include "Interface_Enemy.h"

UBTT_SetMovementSpeed::UBTT_SetMovementSpeed(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("Set Movement Speed");
}

EBTNodeResult::Type UBTT_SetMovementSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{
	if (IInterface_AIAgent* const interface = Cast<IInterface_AIAgent>(OwnerComponent.GetAIOwner()))
	{
		IInterface_AIAgent::Execute_SetMovementSpeed(OwnerComponent.GetAIOwner(), MovementSpeed);
	}

	return EBTNodeResult::Type();
}

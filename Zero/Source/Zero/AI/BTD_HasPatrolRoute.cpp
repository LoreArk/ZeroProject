// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_HasPatrolRoute.h"
#include "AIC_EnemyBase.h"
#include "EnemyCharacter.h"
#include "PatrolRoute.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTD_HasPatrolRoute::UBTD_HasPatrolRoute(const FObjectInitializer& ObjectInitializer)
{
	NodeName = "Has Patrol Route";
}

bool UBTD_HasPatrolRoute::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (auto* const cont = Cast<AAIC_EnemyBase>(OwnerComp.GetAIOwner()))
	{
		
		if (APatrolRoute* PatrolRoute = IInterface_Enemy::Execute_GetPatrolRoute(OwnerComp.GetAIOwner()))
		{
			return true;
		}
	}

	return false;
}

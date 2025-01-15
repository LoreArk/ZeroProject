// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_CanReachTarget.h"
#include "AIC_EnemyBase.h"


UBTD_CanReachTarget::UBTD_CanReachTarget(const FObjectInitializer& ObjectInitializer)
{
	NodeName = "Perform Obstacle Check";
}

bool UBTD_CanReachTarget::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (OwnerComp.GetAIOwner())
	{
		if (OwnerComp.GetAIOwner()->GetClass()->ImplementsInterface(UInterface_Enemy::StaticClass()))
		{
			return IInterface_Enemy::Execute_CanReachTargetStraight(OwnerComp.GetAIOwner());
		}
	}
	

	return false;
}

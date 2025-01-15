// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_CanSeeTarget.h"
#include "AIC_EnemyBase.h"

UBTD_CanSeeTarget::UBTD_CanSeeTarget(const FObjectInitializer& ObjectInitializer)
{
	NodeName = "Can See Target";
}

bool UBTD_CanSeeTarget::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (OwnerComp.GetAIOwner())
	{
		if (OwnerComp.GetAIOwner()->GetClass()->ImplementsInterface(UInterface_Enemy::StaticClass()))
		{
			return IInterface_Enemy::Execute_CanSeeTarget(OwnerComp.GetAIOwner());
		}
	}


	return false;
}

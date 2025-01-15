// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTD_HasPatrolRoute.generated.h"

/**
 * 
 */
UCLASS()
class ZERO_API UBTD_HasPatrolRoute : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTD_HasPatrolRoute(const FObjectInitializer& ObjectInitializer);

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

};

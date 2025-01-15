// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PatrolRoute.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_WaitAtWayPoint.generated.h"

/**
 * 
 */
UCLASS()
class ZERO_API UBTT_WaitAtWayPoint : public UBTTask_BlackboardBase
{
	GENERATED_BODY()


public:
	explicit UBTT_WaitAtWayPoint(FObjectInitializer const& ObjectInitializer);

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory) override;

	float WaitTime;
	float RemainingTime;
	bool bTaskCompleted = false;


	APatrolRoute* PatrolRoute;
};

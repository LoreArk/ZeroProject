// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_EnterInvestigation.generated.h"

/**
 * 
 */
UCLASS()
class ZERO_API UBTT_EnterInvestigation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	explicit UBTT_EnterInvestigation(FObjectInitializer const& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory) override;
};

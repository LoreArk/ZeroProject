// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_LookAround.generated.h"

/**
 * 
 */
UCLASS()
class ZERO_API UBTT_LookAround : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	explicit UBTT_LookAround(FObjectInitializer const& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory) override;
};
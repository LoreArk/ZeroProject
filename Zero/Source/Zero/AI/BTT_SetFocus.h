// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_SetFocus.generated.h"

/**
 * 
 */
UCLASS()
class ZERO_API UBTT_SetFocus : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	explicit UBTT_SetFocus(FObjectInitializer const& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory) override;

};

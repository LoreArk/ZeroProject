// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "AIC_EnemyBase.h"
#include "BTT_PunishAttack.generated.h"

/**
 * 
 */
UCLASS()
class ZERO_API UBTT_PunishAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	explicit UBTT_PunishAttack(FObjectInitializer const& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory) override;

	UFUNCTION()
	void OnAttackFinished(AAIC_EnemyBase* AIC);

	UPROPERTY()
	UBehaviorTreeComponent* CachedOwnerComponent;
};

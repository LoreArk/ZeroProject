
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTD_IsTargetInsideArea.generated.h"

/**
 * 
 */
UCLASS()
class ZERO_API UBTD_IsTargetInsideArea : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTD_IsTargetInsideArea(const FObjectInitializer& ObjectInitializer);

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector BlackboardKeyTargetActor;
};

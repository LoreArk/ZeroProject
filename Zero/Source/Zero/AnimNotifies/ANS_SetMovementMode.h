// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ANS_SetMovementMode.generated.h"

/**
 * 
 */
UCLASS()
class ZERO_API UANS_SetMovementMode : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:


	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference);
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference);
};

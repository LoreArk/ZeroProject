// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "DamageStructs.h"
#include "AN_DoPairedAnimationDamage.generated.h"

/**
 * 
 */
UCLASS()
class ZERO_API UAN_DoPairedAnimationDamage : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDamageSettings DamageSettings;
};

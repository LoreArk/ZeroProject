// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PooledActor.h"
#include "Interface_Sigil.h"
#include "PA_Sigil.generated.h"

/**
 * 
 */
UCLASS()
class ZERO_API APA_Sigil : public APooledActor, public IInterface_Sigil
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDamageSettings DamageSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* DamageTarget;

	virtual void SetDamageSettings_Implementation(FDamageSettings Settings, AActor* TargetActor) override;

	virtual APA_Sigil* GetSigilPooledActor_Implementation() override;

};

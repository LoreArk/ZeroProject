// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIAlertGroup.generated.h"

UCLASS()
class ZERO_API AAIAlertGroup : public AActor
{
	GENERATED_BODY()
	
public:	
	AAIAlertGroup();

protected:
	virtual void BeginPlay() override;

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> AlertGroup;

	UPROPERTY(VisibleAnywhere)
	bool bGroupAlerted;

	UFUNCTION()
	void OnMemberAlerted(FVector ActorLocation);

	UFUNCTION(BlueprintCallable)
	void EndAlert();
};

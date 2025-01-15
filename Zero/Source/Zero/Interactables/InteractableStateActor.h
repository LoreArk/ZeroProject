// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableStateActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActorDataSaved);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActorDataLoaded);

UCLASS()
class ZERO_API AInteractableStateActor : public AActor
{
	GENERATED_BODY()
	
public:	

	AInteractableStateActor();

	virtual void BeginPlay() override;

	UPROPERTY()
	FString UniqueID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
    bool bEnabled;

    bool GetState() const { return bEnabled; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetState(bool NewState);
    virtual void SetState_Implementation(bool NewState) { bEnabled = NewState; }


	UPROPERTY(BlueprintAssignable)
	FOnActorDataSaved OnGameSaved;

	UPROPERTY(BlueprintAssignable)
	FOnActorDataLoaded OnGameLoaded;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component_Perceivable.h"
#include "Components/ActorComponent.h"
#include "PerceptionSystemStructs.h"
#include "Component_Perception.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHearEvent, FNoiseStimulus, NoiseSettings);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLostSight, AActor*, LostActor);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, AActor*, DeadActor);



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZERO_API UComponent_Perception : public UActorComponent
{
	GENERATED_BODY()

public:	
	UComponent_Perception();

protected:
	virtual void BeginPlay() override;

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSightSettings CurrentSightSettings;

	//UPROPERTY(BlueprintCallable)
	//FOnDeath OnDeath;

	UPROPERTY(BlueprintCallable)
	FOnLostSight OnLostSight;

	UPROPERTY(BlueprintCallable)
	FOnHearEvent OnHearEvent;

	UPROPERTY()
	UComponent_Perceivable* SelfPerceivable;

	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> TargetsInsideHearRange;

	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> PreviousTargetsInsideHearRange;

	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> SeenActors;

	AActor* OwnerActor;

	FVector SightOrigin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HearRange;

	UPROPERTY()
	bool bHeardTarget;

	UPROPERTY()
	bool bDead;

	UPROPERTY()
	FRotator SightRotation;

	UPROPERTY()
	FTimerHandle PerceptionTimerHandle;

	UFUNCTION()
	void StartPerceptionTimerLoop();

	UFUNCTION()
	void PausePerceptionTimerLoop();

	UFUNCTION()
	void SensePerception();

	UFUNCTION()
	void HearEvent(AActor* NoiseMaker);

	UFUNCTION()
	bool TraceSound(FVector SoundPosition, AActor* Actor);

	UFUNCTION()
	bool IsInsideVisionAngle(AActor* TargetActor);

	UFUNCTION()
	bool CanSeeTargetActor(AActor* TargetActor);

	UFUNCTION()
	void RemoveSeenActor(AActor* ActorToRemove);

	UFUNCTION()
	void BindRemoveActorOnDeath(UComponent_Damagable* DamagableComponent);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Perception");
	bool bShowHearSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Perception");
	bool bShowInOutHearRangeSpheres;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Perception");
	bool bShowHearTraceDebug;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Perception");
	bool bShowSightCone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Perception");
	bool bShowInOutSightSpheres;
};

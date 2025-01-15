// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component_ObjectPool.h"
#include "Component_PlayerObjectPool.generated.h"

/**
 * 
 */
UCLASS()
class ZERO_API UComponent_PlayerObjectPool : public UComponent_ObjectPool
{
	GENERATED_BODY()
	
public:


	//FIREBALL SIGIL

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sigil Fireball")
	TSubclassOf<APooledActor> FireballActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sigil Fireball ")
	int32 FireballActorPoolSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<APooledActor*> FireballActorPool;

	//BLOCKER SIGIL

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sigil Blocker")
	TSubclassOf<APooledActor> BlockerActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sigil Blocker")
	int32 BlockerActorPoolSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<APooledActor*> BlockerActorPool;

	//CAT STEPS SIGIL

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sigil CatSteps")
	TSubclassOf<APooledActor> CatStepsActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sigil CatSteps")
	int32 CatStepsActorPoolSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<APooledActor*> CatStepsActorPool;

	//CAT STEPS PERMANENT

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sigil CatSteps")
	TSubclassOf<APooledActor> CatStepsPermanentActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sigil CatSteps")
	int32 CatStepsPermanentPoolSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<APooledActor*> CatStepsPermanentPool;

	//CAT FOOTSTEPS

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " Sigil CatSteps")
	TSubclassOf<APooledActor> CatFootstepActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " Sigil CatSteps")
	int32 CatFootstepActorPoolSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<APooledActor*> CatFootstepActorPool;

	//EXPLOSIVE SIGIL
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sigil Explosive")
	TSubclassOf<APooledActor> ExplosiveActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sigil Explosive")
	int32 ExplosiveActorPoolSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<APooledActor*> ExplosiveActorPool;
	
	//SOUND DECEPTION SIGIL
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sigil Sound")
	TSubclassOf<APooledActor> SoundActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sigil Sound")
	int32 SoundActorPoolSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<APooledActor*> SoundActorPool;


protected:
	virtual void BeginPlay() override;

	void InitializePools() override;
};

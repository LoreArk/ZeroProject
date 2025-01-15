// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface_Interactable.h"
#include "InteractableStateActor.h"
#include "Components/SphereComponent.h"
#include "Item.generated.h"



UCLASS()
class ZERO_API AItem : public AInteractableStateActor, public IInterface_Interactable
{
	GENERATED_BODY()
	
public:	

	AItem();


protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	

	virtual void SetState_Implementation(bool NewState) override;

	virtual void Interact_Implementation(AActor* InteractingActor) override;

	UFUNCTION(BlueprintNativeEvent)
	void OnPlayerOverlap(AActor* Actor);
	void OnPlayerOverlap_Implementation(AActor* Actor);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneComponent;
};

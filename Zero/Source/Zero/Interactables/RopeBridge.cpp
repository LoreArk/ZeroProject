// Fill out your copyright notice in the Description page of Project Settings.


#include "RopeBridge.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"


// Sets default values
ARopeBridge::ARopeBridge()
{
	PrimaryActorTick.bCanEverTick = true;
   
    SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));
    RootComponent = SceneComponent;

    PlankStep = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlankStep"));
    PlankStep->SetupAttachment(this->RootComponent);
    
    for (int32 i = 0; i < 5; i++) {

        FName name = *FString::Printf(TEXT("PlankMesh %i"), i);

        UPROPERTY(EditAnywhere)
        UStaticMeshComponent* CreatedMesh;

        CreatedMesh = CreateDefaultSubobject<UStaticMeshComponent>(name);
        CreatedMesh->SetupAttachment(this->RootComponent);
        CreatedMesh->SetStaticMesh(PlankStep->GetStaticMesh());

        CreatedPlanks.Add(CreatedMesh);
    }
}

void ARopeBridge::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARopeBridge::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


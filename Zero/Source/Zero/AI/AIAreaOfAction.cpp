// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAreaOfAction.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AAIAreaOfAction::AAIAreaOfAction()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BoxArea = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Area Of Action"));

}

// Called when the game starts or when spawned
void AAIAreaOfAction::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAIAreaOfAction::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AAIAreaOfAction::IsLocationInsideBox(FVector Location)
{
	
	return UKismetMathLibrary::IsPointInBox(Location, BoxArea->GetComponentLocation(), BoxArea->GetScaledBoxExtent());
}


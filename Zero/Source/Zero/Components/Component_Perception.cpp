// Fill out your copyright notice in the Description page of Project Settings.


#include "Component_Perception.h"
#include "BaseCharacter.h"
#include "Interface_Perception.h"
#include "Interface_Damagable.h"
#include "Component_Damagable.h"
#include "Engine/EngineTypes.h"
#include "Component_Perceivable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

UComponent_Perception::UComponent_Perception()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UComponent_Perception::BeginPlay()
{
	Super::BeginPlay();
}


void UComponent_Perception::StartPerceptionTimerLoop()
{
	GetWorld()->GetTimerManager().SetTimer(PerceptionTimerHandle, this, &UComponent_Perception::SensePerception, 0.2f, true, 1.f);
}

void UComponent_Perception::PausePerceptionTimerLoop()
{
	GetWorld()->GetTimerManager().PauseTimer(PerceptionTimerHandle);
	
	if (PreviousTargetsInsideHearRange.Num() > 0)
	{
		for (AActor* PreviousActor : PreviousTargetsInsideHearRange)
		{
			UComponent_Perceivable* Perceivable = IInterface_Perception::Execute_GetPerceivableComponent(PreviousActor);
			if (Perceivable)
			{
				Perceivable->OnMakeNoise.RemoveDynamic(this, &UComponent_Perception::HearEvent);
			}
		}

		PreviousTargetsInsideHearRange.Empty();
	}
}

void UComponent_Perception::SensePerception()
{
	FTransform OriginTransform = IInterface_Perception::Execute_GetSightOriginTransform(OwnerActor);
	SightOrigin = OriginTransform.GetLocation();
	SightRotation = OriginTransform.GetRotation().Rotator();

	FVector SpherePos = SightOrigin;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwnerActor);

	TArray<AActor*> OverlappingActors;

	float MaxRange = FMath::Max(1500.f, HearRange);
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(), SightOrigin, MaxRange,
		{ UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn) },
		ABaseCharacter::StaticClass(), ActorsToIgnore, OverlappingActors
	);

	if (bShowSightCone)
	{
		float ConeRadius = CurrentSightSettings.Range; // Sight range radius
		float FOVAngle = CurrentSightSettings.PeripheralAngle; // Field of view angle
		FVector ForwardVector = SightRotation.Vector(); // Forward direction for sight

		DrawDebugCone(
			GetWorld(),
			SightOrigin,
			ForwardVector,
			ConeRadius,
			FMath::DegreesToRadians(FOVAngle),
			FMath::DegreesToRadians(FOVAngle),
			12,
			FColor::Green,
			false,
			0.1f,
			0,
			2.f
		);
	}
	
	if (bShowHearSphere)
	{
		DrawDebugSphere(GetWorld(), SpherePos, HearRange, 12, FColor::Blue, false, 0.1f, 0, 0.5f);
	}

	TargetsInsideHearRange.Empty();

	for (AActor* CurrentActor : OverlappingActors)
	{
		if (CurrentActor == nullptr)
			continue;

		UComponent_Perceivable* Perceivable = IInterface_Perception::Execute_GetPerceivableComponent(CurrentActor);

		if (!Perceivable)
		{
			UE_LOG(LogTemp, Error, TEXT("PerceptionComponent, SensePerception: Target Perceivable component is null!"));

			continue;
		}

		// Sight Logic: Is actor within sight range and angle?
		if (SelfPerceivable == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("PerceptionComponent, SensePerception: SelfPerceivable is null!"));
			continue;
		}
		if (!Perceivable->IsSameTeam(SelfPerceivable->Team) && Perceivable->GetTeam() != ETeam::None)
		{
			// Calculate distance to actor
			float DistanceToActor = FVector::Dist(SightOrigin, CurrentActor->GetActorLocation());
			bool bInsideRange = DistanceToActor <= CurrentSightSettings.Range;
			// Check if the actor is within the vision cone
			
			if (bInsideRange && IsInsideVisionAngle(CurrentActor) && CanSeeTargetActor(CurrentActor))
			{
				if (!SeenActors.Contains(CurrentActor))
				{
					GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("%s Entered Sight Range"), *CurrentActor->GetName()));
					SeenActors.AddUnique(CurrentActor);
					IInterface_Perception::Execute_SeenNewActor(GetOwner(), CurrentActor);
					if (bShowInOutSightSpheres) 
					{
						DrawDebugSphere(GetWorld(), CurrentActor->GetActorLocation(), 50.0f, 12, FColor::Red, false, 2.f, 0, 0.5f);
					}

					// Bind death event for actors in sight
					UComponent_Damagable* Damagable = IInterface_Damagable::Execute_GetDamagableComponent(CurrentActor);
					if (Damagable)
					{
						BindRemoveActorOnDeath(Damagable);
					}
				}
			}
			else if(SeenActors.Contains(CurrentActor))
			{
				if (bShowInOutSightSpheres)
				{
					DrawDebugSphere(GetWorld(), CurrentActor->GetActorLocation(), 50.0f, 12, FColor::Green, false, 2.f, 0, 0.5f);
				}

				if(CanSeeTargetActor(CurrentActor) == false || bInsideRange == false)
					RemoveSeenActor(CurrentActor);
			}

			// Hearing Logic: Is actor within hearing range?
			if (DistanceToActor <= HearRange)
			{
				if (!TargetsInsideHearRange.Contains(CurrentActor))
				{
					TargetsInsideHearRange.AddUnique(CurrentActor);
					//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, FString::Printf(TEXT("%s Entered Hearing Range"), *CurrentActor->GetName()));

					// Bind noise event
					if (!PreviousTargetsInsideHearRange.Contains(CurrentActor))
					{
						Perceivable->OnMakeNoise.AddDynamic(this, &UComponent_Perception::HearEvent);
						if (bShowInOutHearRangeSpheres) 
						{
							DrawDebugSphere(GetWorld(), CurrentActor->GetActorLocation(), 10.0f, 12, FColor::Blue, false, 2.f, 0, 0.5f);
						}
					}
				}
			}
		}
	}

	// Handle actors that left the hearing range
	for (AActor* PreviousActor : PreviousTargetsInsideHearRange)
	{
		if (!TargetsInsideHearRange.Contains(PreviousActor))
		{
			UComponent_Perceivable* Perceivable = IInterface_Perception::Execute_GetPerceivableComponent(PreviousActor);
			if (Perceivable)
			{
				Perceivable->OnMakeNoise.RemoveDynamic(this, &UComponent_Perception::HearEvent);
				//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, FString::Printf(TEXT("%s Left Hearing Range"), *PreviousActor->GetName()));
				if (bShowInOutHearRangeSpheres)
				{
					DrawDebugSphere(GetWorld(), PreviousActor->GetActorLocation(), 20.0f, 12, FColor::Orange, false, 2.f, 0, 0.5f);
				}
			}
		}
	}

	// Update the list of previously detected hearing targets
	PreviousTargetsInsideHearRange = TargetsInsideHearRange;
}


void UComponent_Perception::HearEvent(AActor* NoiseMaker)
{
	UComponent_Perceivable* Perceivable = IInterface_Perception::Execute_GetPerceivableComponent(NoiseMaker);

	FNoiseStimulus NoiseSettings = Perceivable->NoiseSettings;
	FVector NoiseLocation = NoiseSettings.Location;

	float ActorZLocation = OwnerActor->GetActorLocation().Z;
	float NoiseZLocation = NoiseLocation.Z;
	float ZDiff = ActorZLocation - NoiseZLocation;

	float Distance = FVector::Distance(OwnerActor->GetActorLocation(), NoiseSettings.Location);

	if (NoiseSettings.Loudness == 0.0f && Distance > HearRange / 2)
		return;

	if (TargetsInsideHearRange.Contains(NoiseMaker) && ZDiff * 1 < 200.f)
	{
		if (NoiseSettings.Loudness < 50.f)
		{
			FHitResult HitResult;
			FVector StartLocation = SightOrigin;
			FVector EndLocation = NoiseMaker->GetActorLocation();

			FCollisionQueryParams TraceParams;
			TraceParams.AddIgnoredActor(OwnerActor);

			bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, TraceParams);

			if (bShowHearTraceDebug)
			{
				if (bHit)
				{
					DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 1.0f, 0, 1.0f);
				}
				else
				{
					DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, false, 1.0f, 0, 1.0f);
				}
			}

			if (bHit && HitResult.GetActor() != NoiseMaker)
			{
				return;
			}
		}

		//TraceSound(NoiseLocation, NoiseMaker);
		OnHearEvent.Broadcast(NoiseSettings);
	}
}

bool UComponent_Perception::TraceSound(FVector SoundPosition, AActor* Actor)
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwnerActor);

	FHitResult HitResult;
	UKismetSystemLibrary::LineTraceSingleByProfile(
							GetWorld(), 
						SightOrigin, SoundPosition, "Visibility", 
					false, ActorsToIgnore, 
				EDrawDebugTrace::ForDuration, 
			HitResult, 
		true, FLinearColor::Red, FLinearColor::Green, 2.f
	);

	return HitResult.bBlockingHit;

}

bool UComponent_Perception::IsInsideVisionAngle(AActor* TargetActor)
{
	FVector TargetDirection = IInterface_Perception::Execute_GetSightTarget(TargetActor) - SightOrigin;
	TargetDirection.Normalize(0.0001f);

	FVector RightVector = UKismetMathLibrary::GetForwardVector(SightRotation);
	RightVector.Normalize(0.0001f);

	float DotProd = FVector::DotProduct(TargetDirection, RightVector);

	float Angle = FMath::RadiansToDegrees(FMath::Acos(DotProd));

	//UE_LOG(LogTemp, Log, TEXT("Angle: %f"), Angle);

	return Angle < CurrentSightSettings.PeripheralAngle;
}

bool UComponent_Perception::CanSeeTargetActor(AActor* TargetActor)
{
	//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Orange, FString::Printf(TEXT("Sense sight: %s Entered"), *TargetActor->GetName()));

	float Distance = FVector::Distance(OwnerActor->GetActorLocation(), TargetActor->GetActorLocation());
	//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Orange, FString::Printf(TEXT("Distance: %f"), Distance));

	if (Distance < 130.f)
	{
		return true;
	}

	FVector TraceStart = SightOrigin;
	FVector TraceEnd = IInterface_Perception::Execute_GetSightTarget(TargetActor);
	//FVector TraceEnd = TargetActor->GetActorLocation();

	FVector SpherePos = SightOrigin;
	TArray<AActor*> ActorsToIgnore = TArray<AActor*>();

	FHitResult HitResult;


	bool bHit = UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		TraceStart,
		TraceEnd,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		HitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		2.0f
	);

	return !bHit;
}

void UComponent_Perception::RemoveSeenActor(AActor* ActorToRemove)
{	
	SeenActors.Remove(ActorToRemove);
	OnLostSight.Broadcast(ActorToRemove);
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, " REMOVE SEEN ACTOR");

	UComponent_Damagable* Damagable = IInterface_Damagable::Execute_GetDamagableComponent(ActorToRemove);
	
	Damagable->OnDeath.RemoveDynamic(this, &UComponent_Perception::RemoveSeenActor);
}

void UComponent_Perception::BindRemoveActorOnDeath(UComponent_Damagable* DamagableComponent)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "SEEN: BIND ON DEATH EVENT");

	DamagableComponent->OnDeath.AddDynamic(this, &UComponent_Perception::RemoveSeenActor);
}



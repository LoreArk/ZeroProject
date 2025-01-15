// Fill out your copyright notice in the Description page of Project Settings.


#include "AIC_EnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyCharacter.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlayerCharacter.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "NavMesh/NavMeshPath.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

/*
AAIC_EnemyBase::AAIC_EnemyBase(FObjectInitializer const& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{

}*/

void AAIC_EnemyBase::BeginPlay()
{
	Super::BeginPlay();

	if (CharacterRef)
	{
		StartLocation = CharacterRef->GetActorLocation();
		if (UBlackboardComponent* BlackBoardComponent = GetBlackboardComponent())
		{
			BlackBoardComponent->SetValueAsVector("StartLocation", StartLocation);
		}
		CharacterRef->OnPlayerPossessed.AddDynamic(this, &AAIC_EnemyBase::PauseAI);
		CharacterRef->OnAIPossessed.AddDynamic(this, &AAIC_EnemyBase::ResumeAI);
		CharacterRef->PerceptionComponent->OnLostSight.AddDynamic(this, &AAIC_EnemyBase::HandleLostSight);
		CharacterRef->OnBlockedEnter.AddDynamic(this, &AAIC_EnemyBase::SetBlockedState);
		CharacterRef->OnBlockedExit.AddDynamic(this, &AAIC_EnemyBase::SetStateAsPreviousState);
		CharacterRef->DamagableComponent->OnDamage.AddDynamic(this, &AAIC_EnemyBase::HandleSensedDamage);
		if (CharacterRef->PerceptionComponent)
		{
			CharacterRef->PerceptionComponent->StartPerceptionTimerLoop();
		}
	}

	OnTargetSeen.AddDynamic(this, &AAIC_EnemyBase::HandleSensedSight);
}

void AAIC_EnemyBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	PlayerRef = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass());
	//CombatManager = Cast<ACombatManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACombatManager::StaticClass()));
	/*
	UCrowdFollowingComponent* CrowdFollowingComponent = FindComponentByClass<UCrowdFollowingComponent>();
	
	if (CrowdFollowingComponent)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Black, ("CROWD COMPONENT FOUND"));
		CrowdFollowingComponent->SetCrowdSeparation(true);
		CrowdFollowingComponent->SetCrowdSeparationWeight(50.f);
		CrowdFollowingComponent->SetCrowdAvoidanceRangeMultiplier(1.1f);
		CrowdFollowingComponent->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::High, true);
	}*/

	if (AEnemyCharacter* const enemy = Cast<AEnemyCharacter>(InPawn))
	{
		CharacterRef = enemy;
		if (UBehaviorTree* tree = enemy->GetBehviorTree())
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Black, ("INIT BEHAVIOR TREE"));
			UBlackboardComponent* b;
			UseBlackboard(tree->BlackboardAsset, b);
			Blackboard = b;
			RunBehaviorTree(tree);
		}
	}

}

void AAIC_EnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GetBlackboardComponent()->SetValueAsBool("IsFrozen", CharacterRef->CurrentHitReaction != nullptr);
	GetBlackboardComponent()->SetValueAsBool("Alert", bAlertMode);

	if (AiState == EAIStates::Combat)
	{
		UpdateDistanceState();

		GetBlackboardComponent()->SetValueAsBool("CanAttack", CharacterRef->bCanAttack);
		GetBlackboardComponent()->SetValueAsBool("CanPunishAttack", CharacterRef->CombatManager->CanAttack(GetOwner()));
		GetBlackboardComponent()->SetValueAsBool("IsAttacking", CharacterRef->bIsAttacking);
	}
}


APatrolRoute* AAIC_EnemyBase::GetPatrolRoute_Implementation()
{
	return IInterface_Enemy::Execute_GetPatrolRoute(GetPawn());
}

AAIAreaOfAction* AAIC_EnemyBase::GetAreaOfAction_Implementation()
{
	return IInterface_Enemy::Execute_GetAreaOfAction(GetPawn());
}


void AAIC_EnemyBase::LookAround_Implementation()
{
	IInterface_Enemy::Execute_LookAround(GetPawn());
}

bool AAIC_EnemyBase::CanReachTargetStraight_Implementation()
{
	FVector TraceStart = CharacterRef->GetActorLocation();
	TraceStart.Z -= 20.f;
	FVector TraceEnd = AttackTarget->GetActorLocation();
	TraceEnd.Z = TraceStart.Z;
	//FVector TraceEnd = TargetActor->GetActorLocation();

	TArray<AActor*> ActorsToIgnore = TArray<AActor*>();
	ActorsToIgnore.Add(CharacterRef);


	FHitResult HitResult;

	bool bHit = UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		TraceStart,
		TraceEnd,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		HitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		2.0f
	);

	return !bHit;
}

bool AAIC_EnemyBase::CanSeeTarget_Implementation()
{
	FVector TraceStart = CharacterRef->PerceptionComponent->SightOrigin;
	FVector TraceEnd = AttackTarget->GetActorLocation();

	TArray<AActor*> ActorsToIgnore = TArray<AActor*>();
	ActorsToIgnore.Add(CharacterRef);

	FHitResult HitResult;

	bool bHit = UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		TraceStart,
		TraceEnd,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		HitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		2.0f
	);

	return !bHit;
}

void AAIC_EnemyBase::PlayWayPointAnimation_Implementation(UAnimMontage* MontageToPlay)
{
	IInterface_Enemy::Execute_PlayWayPointAnimation(CharacterRef, MontageToPlay);
}

void AAIC_EnemyBase::StopWayPointAnimation_Implementation()
{
	IInterface_Enemy::Execute_StopWayPointAnimation(CharacterRef);
}

void AAIC_EnemyBase::HandleSensedSight(AActor* ActorSensed)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("%s - SENSED SIGHT"), *ActorSensed->GetName()));

	if (AiState == EAIStates::Dead || AiState == EAIStates::Frozen)
		return;

	AActor* EvaluetedTarget = GetBestAttackTarget();

	if (EvaluetedTarget == nullptr)
	{
		EvaluetedTarget = ActorSensed;
	}
	/*
	if (ActorSensed != AttackTarget && AttackTarget != nullptr)
	{
		//if (IInterface_Perception::Execute_GetPerceivableComponent(AttackTarget)->GetTeam() == ETeam::PlayerDummy)
			//return;
		if (IInterface_Perception::Execute_GetPerceivableComponent(AttackTarget)->GetTeam() != ETeam::PlayerDummy)
		{
			return;
		}
		//else if (IInterface_Perception::Execute_GetPerceivableComponent(AttackTarget)->GetTeam() == ETeam::Player)
		//	return;
	}
	if (IInterface_Perception::Execute_GetPerceivableComponent(ActorSensed)->GetTeam() != ETeam::PlayerDummy)
	{
		InterruptAttackMove();
	}*/
	if (EvaluetedTarget)
	{
		SetStateAsCombat(false, EvaluetedTarget);
	}
}

bool AAIC_EnemyBase::IsLocationReachable(FVector TargetLocation)
{
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSys || !CharacterRef)
		return false;

	// Get the AI character's current location
	FVector SelfLocation = CharacterRef->GetActorLocation();

	UNavigationPath* NavPath = NavSys->FindPathToLocationSynchronously(GetWorld(), SelfLocation, TargetLocation, CharacterRef);

	return NavPath && NavPath->IsValid() && NavPath->PathPoints.Num() > 1;
}

void AAIC_EnemyBase::HandleLostSight(AActor* LostActor)
{
	if (CharacterRef->bIsAttacking)
	{
		InterruptAttackMove();
	}

	if (LostActor == AttackTarget)
	{
		SetStateAsSeeking(LostActor->GetActorLocation());
	}
}

void AAIC_EnemyBase::HandleHearEvent(FNoiseStimulus NoiseSettings)
{
	UE_LOG(LogTemp, Warning, TEXT("AI - HEAR EVENT"));

	DrawDebugSphere(GetWorld(), NoiseSettings.Location, 15.0f, 12, FColor::Blue, false, 2.f, 0, 0.5f);
	switch (AiState)
	{
	case EAIStates::Passive:
		SetStateAsInvestigating(NoiseSettings.Location);
		//PassiveToInvestigationTransition(NoiseSettings.Location);
			break;
	case EAIStates::Investigating:
		SetStateAsInvestigating(NoiseSettings.Location);
			break;
	case EAIStates::Seeking:
		SetStateAsSeeking(NoiseSettings.Location);
		break;
	default:
			break;
	}
}

void AAIC_EnemyBase::HandleSensedDamage(AActor* DamageDealer)
{
	switch (AiState)
	{
	case EAIStates::Passive:
		SetStateAsInvestigating(DamageDealer->GetActorLocation());
		//PassiveToInvestigationTransition(NoiseSettings.Location);
		break;
	case EAIStates::Investigating:
		SetStateAsInvestigating(DamageDealer->GetActorLocation());
		break;
	case EAIStates::Seeking:
		SetStateAsSeeking(DamageDealer->GetActorLocation());
		break;
	default:
		break;
	}

	if (IInterface_Perception::Execute_GetPerceivableComponent(DamageDealer)->GetTeam() != CharacterRef->PerceivableComponent->GetTeam())
	{
		ActorsDamagedBy.AddUnique(DamageDealer);
	}
}

void AAIC_EnemyBase::HandleAttackTargetDeath(AActor* DeadActor)
{
	CharacterRef->EndAttack(nullptr, true);
	CharacterRef->PerceptionComponent->SeenActors.Remove(DeadActor);

	AttackTarget = nullptr;
	GetBlackboardComponent()->SetValueAsObject("AttackTarget", nullptr);

	AActor* Target = GetBestAttackTarget();
	if (Target)
	{
		UE_LOG(LogTemp, Warning, TEXT("TARGET DEAD - Set new target"));

		SetStateAsCombat(false, Target);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("TARGET DEAD - Set investigate"));

		SetStateAsInvestigating(DeadActor->GetActorLocation());
	}
}

AActor* AAIC_EnemyBase::GetBestAttackTarget()
{
	AActor* BestTarget = nullptr;
	int32 BestPriority = 999;
	int32 CurrentTargetPriority = (AttackTarget != nullptr) ? GetPriorityForTarget(IInterface_Perception::Execute_GetPerceivableComponent(AttackTarget)->GetTeam(), AttackTarget) : 999;

	for (AActor* PerceivedActor : CharacterRef->PerceptionComponent->SeenActors)
	{
		if (!PerceivedActor)
			continue;

		if (PerceivedActor->Implements<UInterface_Perception>() == false)
			continue;


		ETeam Team = IInterface_Perception::Execute_GetPerceivableComponent(PerceivedActor)->GetTeam();

		int32 Priority = GetPriorityForTarget(Team, PerceivedActor);

		UE_LOG(LogTemp, Warning, TEXT("Evaluated Target: %s with Priority: %d"), *PerceivedActor->GetName(), Priority);

		if (AttackTarget != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Current Attack Target: %s with Priority: %d"), *AttackTarget->GetName(), CurrentTargetPriority);
		}

		if (Priority < BestPriority)
		{
			BestPriority = Priority;
			BestTarget = PerceivedActor;
		}
	}

	if (AttackTarget != nullptr && CurrentTargetPriority <= BestPriority)
	{
		BestTarget = AttackTarget;
		BestPriority = CurrentTargetPriority;
	}

	if (!BestTarget && CharacterRef->PerceptionComponent->SeenActors.Num() > 0)
	{
		BestTarget = CharacterRef->PerceptionComponent->SeenActors[0];
		BestPriority = 999;
	}

	if (BestTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Best Attack Target: %s with Priority: %d"), *BestTarget->GetName(), BestPriority);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Best Attack Target found"));
	}
	return BestTarget;
}

int32 AAIC_EnemyBase::GetPriorityForTarget(ETeam Team, AActor* TargetActor)
{
	if (Team == ETeam::PlayerDummy)
	{
		return 1;
	}
	if (Team == ETeam::Player)
	{
		return 2;
	}
	if (Team == ETeam::AgainstAll)
	{
		if (ActorsDamagedBy.Contains(TargetActor))
		{
			return 1;
		}
		return 3;
	}
	if (Team == ETeam::Enemies)
	{
		return 4;
	}

	return 999;
}

void AAIC_EnemyBase::HandleSensedAlert(FVector AlertLocation)
{
	if (CharacterRef->AlertBehavior == EAlertBehavior::Patrol)
	{
		if (AiState == EAIStates::Passive)
		{

		}
	}
	else
	{
		if (AiState == EAIStates::Passive || AiState == EAIStates::Seeking || AiState == EAIStates::Investigating)
		{
			SetStateAsSeeking(AlertLocation);
		}
	}
	
	bAlertMode = true;
}

void AAIC_EnemyBase::HandleAlertEnd()
{
	bAlertMode = false;
}

void AAIC_EnemyBase::PassiveToInvestigation_Implementation()
{
	IInterface_Enemy::Execute_PassiveToInvestigation(CharacterRef);
	GetBlackboardComponent()->SetValueAsBool("InvestigationExecuteOnce", false);
}

void AAIC_EnemyBase::SetStateAsPassive_Implementation()
{
	PreviousAiState = AiState;
	AiState = EAIStates::Passive;

	CharacterRef->SetSightSettings(AiState);
	GetBlackboardComponent()->SetValueAsEnum("AiStates", static_cast<uint8>(AiState));
}

void AAIC_EnemyBase::PassiveToInvestigationTransition(FVector Location)
{
	//IInterface_Enemy::Execute_PassiveToInvestigation(CharacterRef);

	GetBlackboardComponent()->SetValueAsBool("HasInvestigationPoint", true);
	GetBlackboardComponent()->SetValueAsVector("PointOfInterest", Location);

	SetFocalPoint(Location);

	InvestigationLocation = Location;

	GetWorld()->GetTimerManager().SetTimer(InvestigationTimerHandle, this, &AAIC_EnemyBase::EndInvestigationTransition, 2.f, false);

}

void AAIC_EnemyBase::EndInvestigationTransition()
{
	SetStateAsInvestigating(InvestigationLocation);
}

void AAIC_EnemyBase::SetStateAsCombat(bool bUseLastKnownAttackTarget, AActor* Target)
{
	if (AiState == EAIStates::Dead)
		return;

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("%s - SET STATE AS COMBAT"), *Target->GetName()));
	if (bUseLastKnownAttackTarget && AttackTarget)
	{
		NewAttackTarget = AttackTarget;
	}
	else
	{
		NewAttackTarget = Target;
	}

	UBlackboardComponent* BBComp = GetBlackboardComponent();

	if (!BBComp)
	{
		UE_LOG(LogTemp, Error, TEXT("BlackboardComponent is not valid!"));
		return;
	}

	PreviousAiState = AiState;
	AiState = EAIStates::Combat;

	CharacterRef->SetSightSettings(AiState);

	BBComp->SetValueAsObject("AttackTarget", NewAttackTarget);
	BBComp->SetValueAsEnum("AiStates", static_cast<uint8>(AiState));

	if (AttackTarget != NewAttackTarget)
	{
		if (AttackTarget != nullptr)
		{
			UComponent_Damagable* OldTargetDamagable = IInterface_Damagable::Execute_GetDamagableComponent(AttackTarget);
			if (OldTargetDamagable)
			{
				OldTargetDamagable->OnDeath.RemoveDynamic(this, &AAIC_EnemyBase::HandleAttackTargetDeath);
			}
		}

		if (NewAttackTarget != nullptr)
		{
			UComponent_Damagable* NewTargetDamagable = IInterface_Damagable::Execute_GetDamagableComponent(NewAttackTarget);
			if (NewTargetDamagable)
			{
				// Check if delegate is already bound
				if (!NewTargetDamagable->OnDeath.IsAlreadyBound(this, &AAIC_EnemyBase::HandleAttackTargetDeath))
				{
					NewTargetDamagable->OnDeath.AddDynamic(this, &AAIC_EnemyBase::HandleAttackTargetDeath);
				}
			}
		}
	}

	AttackTarget = NewAttackTarget;

	OnCombatStart.Broadcast(AttackTarget->GetActorLocation());
}

void AAIC_EnemyBase::SetStateAsInvestigating(FVector Location)
{
	if (AiState == EAIStates::Dead)
		return;
	UE_LOG(LogTemp, Warning, TEXT("SET STATE AS INVESTIGATING"));

	if (PreviousAiState != EAIStates::Investigating)
	{
		GetBlackboardComponent()->SetValueAsBool("InvestigationExecuteOnce", true);
	}

	PreviousAiState = AiState;
	AiState = EAIStates::Investigating;

	CharacterRef->SetSightSettings(AiState);

	GetBlackboardComponent()->SetValueAsEnum("AiStates", static_cast<uint8>(AiState));
	GetBlackboardComponent()->SetValueAsVector("PointOfInterest", Location);
}

void AAIC_EnemyBase::SetStateAsSeeking(FVector Location)
{
	if (AiState == EAIStates::Dead)
		return;

	UE_LOG(LogTemp, Warning, TEXT("SET STATE AS SEEKING"));

	PreviousAiState = AiState;
	AiState = EAIStates::Seeking;

	CharacterRef->SetSightSettings(AiState);

	GetBlackboardComponent()->SetValueAsEnum("AiStates", static_cast<uint8>(AiState));
	GetBlackboardComponent()->SetValueAsVector("PointOfInterest", Location);
}

void AAIC_EnemyBase::SetStateAsDead()
{
	if (AiState == EAIStates::Frozen)
	{
		CharacterRef->SetVulnerabilityWidgetEnabled(false);
	}

	PreviousAiState = AiState;
	AiState = EAIStates::Dead;

	GetBlackboardComponent()->SetValueAsEnum("AiStates", static_cast<uint8>(AiState));
}

void AAIC_EnemyBase::SetBlockedState()
{
	UE_LOG(LogTemp, Warning, TEXT("SET BLOCK STATE"));

	PreviousAiState = AiState;
	AiState = EAIStates::Frozen;
	CharacterRef->EndAttack(nullptr, true);
	CharacterRef->GetMesh()->GetAnimInstance()->StopAllMontages(0.0f);

	CharacterRef->PerceptionComponent->PausePerceptionTimerLoop();

	InterruptAttackMove();
	StopMovement();
	CharacterRef->bIsBlocked = true;
	CharacterRef->VulnerabilityFlag->SetVisibility(true);
	GetBlackboardComponent()->SetValueAsEnum("AiStates", static_cast<uint8>(AiState));
}

void AAIC_EnemyBase::SetStateAsPreviousState()
{
	CharacterRef->PerceptionComponent->StartPerceptionTimerLoop();

	AiState = PreviousAiState;
	CharacterRef->bIsBlocked = false;
	CharacterRef->SetVulnerabilityWidgetEnabled(false);

	GetBlackboardComponent()->SetValueAsEnum("AiStates", static_cast<uint8>(AiState));
}

void AAIC_EnemyBase::OnAttackMoveCompleted(FAIRequestID, const FPathFollowingResult& Result)
{
	if (Result.IsInterrupted())
	{
		UE_LOG(LogTemp, Warning, TEXT("MOVEMENT ATTACK: MOVEMENT INTERRUPTED"));

		if (BrainComponent->IsActive() == false)
		{
			return;
		}

		// Log the reason for the interruption
		switch (Result.Code)
		{
		case EPathFollowingResult::Aborted:
			//CharacterRef->PlaySelectedAttackMontage();
			CharacterRef->EndAttack(nullptr, false);
			UE_LOG(LogTemp, Warning, TEXT("Reason: Path following aborted."));
			break;
		case EPathFollowingResult::Blocked:
			UE_LOG(LogTemp, Warning, TEXT("Reason: Path was blocked."));
			break;
		case EPathFollowingResult::OffPath:
			UE_LOG(LogTemp, Warning, TEXT("Reason: Character went off the path."));
			break;
		case EPathFollowingResult::Invalid:
			UE_LOG(LogTemp, Warning, TEXT("Reason: Invalid."));
		case EPathFollowingResult::Success:
			UE_LOG(LogTemp, Warning, TEXT("Reason: Success."));

		default:
			UE_LOG(LogTemp, Warning, TEXT("Reason: Unknown interruption."));
			break;
		}
	}
	else if (Result.IsFailure())
	{
		UE_LOG(LogTemp, Warning, TEXT("MOVEMENT ATTACK: FAILED"));

		if (BrainComponent->IsActive() == false)
		{
			return;
		}

		switch (Result.Code)
		{
		case EPathFollowingResult::Aborted:
			UE_LOG(LogTemp, Warning, TEXT("Reason: Path following aborted."));
			CharacterRef->EndAttack(nullptr, false);
			break;
		case EPathFollowingResult::Blocked:
			UE_LOG(LogTemp, Warning, TEXT("Reason: Path was blocked."));
			break;
		case EPathFollowingResult::OffPath:
			UE_LOG(LogTemp, Warning, TEXT("Reason: Character went off the path."));
			break;
		case EPathFollowingResult::Invalid:
			UE_LOG(LogTemp, Warning, TEXT("Reason: Invalid."));
		case EPathFollowingResult::Success:
			UE_LOG(LogTemp, Warning, TEXT("Reason: Success."));
			CharacterRef->PlaySelectedAttackMontage();
		default:
			UE_LOG(LogTemp, Warning, TEXT("Reason: Unknown interruption."));
			CharacterRef->EndAttack(nullptr, false);
			break;
		}
	}
	else if (Result.IsSuccess())
	{
		UE_LOG(LogTemp, Warning, TEXT("MOVEMENT ATTACK: SUCCESS"));

		if (BrainComponent->IsActive() == false)
		{
			return;
		}

		CharacterRef->PlaySelectedAttackMontage();
	}

	//CharacterRef->EndAttack(nullptr);
	GetPathFollowingComponent()->OnRequestFinished.RemoveAll(this);
}

void AAIC_EnemyBase::InterruptAttackMove()
{
	if (GetPathFollowingComponent())
	{
		GetPathFollowingComponent()->OnRequestFinished.RemoveAll(this);
		GetPathFollowingComponent()->AbortMove(*this, FPathFollowingResultFlags::NewRequest);

		CharacterRef->EndAttack(nullptr, true);
	}
}

void AAIC_EnemyBase::PauseAI()
{
	CharacterRef->PerceptionComponent->PausePerceptionTimerLoop();

	if (GetBrainComponent()->IsActive())
	{
		GetBrainComponent()->PauseLogic(TEXT("Pausing"));
	}
}

void AAIC_EnemyBase::ResumeAI()
{
	CharacterRef->PerceptionComponent->StartPerceptionTimerLoop();

	if (GetBrainComponent()->IsPaused())
	{
		GetBrainComponent()->ResumeLogic("Resuming");
	}
}

void AAIC_EnemyBase::OnDeath()
{
	//PauseAI();
	CharacterRef->PerceptionComponent->PausePerceptionTimerLoop();

	SetStateAsDead();
	StopMovement();
	//PauseAI();
	//UnPossess();
}

void AAIC_EnemyBase::OnRevive()
{
	IInterface_AIAgent::Execute_SetStateAsPassive(this);
	CharacterRef->PerceptionComponent->StartPerceptionTimerLoop();
}

void AAIC_EnemyBase::OnGameLoadedStateSetup()
{
	UE_LOG(LogTemp, Warning, TEXT("AIC_EnemyBase::OnGameLoadedStateSetup()"));


	GetBlackboardComponent()->SetValueAsEnum("AiStates", static_cast<uint8>(AiState));
}

void AAIC_EnemyBase::UpdateDistanceState()
{
	CurrentDistanceState = GetDistanceState();

	if (AiState == EAIStates::Combat)
	{
		GetBlackboardComponent()->SetValueAsEnum("DistanceState", static_cast<uint8>(CurrentDistanceState));
	}
}

EDistanceState AAIC_EnemyBase::GetDistanceState()
{
	if (AttackTarget == nullptr)
		return EDistanceState::Far;

	DistanceFromTarget = FVector::Distance(AttackTarget->GetActorLocation(), CharacterRef->GetActorLocation());

	if (DistanceFromTarget > 600.f)
	{
		return EDistanceState::Far;
	}
	else if (DistanceFromTarget > 250.f)
	{
		return EDistanceState::Mid;
	}

	return EDistanceState::Close;
}



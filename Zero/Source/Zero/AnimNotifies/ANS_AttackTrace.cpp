// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_AttackTrace.h"
#include "Interface_AIAgent.h"

void UANS_AttackTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (MeshComp->GetOwner() == nullptr)
		return;

	if (bool Char = MeshComp->GetOwner()->Implements<UInterface_AIAgent>())
	{
		// Call your function to start the action
		IInterface_AIAgent::Execute_AttackTraceStart(MeshComp->GetOwner(), DamageSettings);
	}
}

void UANS_AttackTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (MeshComp->GetOwner() == nullptr)
		return;

	if (bool Char = MeshComp->GetOwner()->Implements<UInterface_AIAgent>())
	{
		// Call your function to start the action
		IInterface_AIAgent::Execute_AttackTraceTick(MeshComp->GetOwner());
	}
}

void UANS_AttackTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp->GetOwner() == nullptr)
		return;

	if (bool Char = MeshComp->GetOwner()->Implements<UInterface_AIAgent>())
	{
		// Call your function to start the action
		IInterface_AIAgent::Execute_AttackTraceEnd(MeshComp->GetOwner());
	}
}

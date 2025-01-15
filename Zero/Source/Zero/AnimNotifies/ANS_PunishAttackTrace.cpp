// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_PunishAttackTrace.h"
#include "Interface_Enemy.h"

void UANS_PunishAttackTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{

	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

}

void UANS_PunishAttackTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (MeshComp->GetOwner() == nullptr)
		return;

	if (bool Char = MeshComp->GetOwner()->Implements<UInterface_Enemy>())
	{
		// Call your function to start the action
		IInterface_Enemy::Execute_PunishAttackTraceTick(MeshComp->GetOwner());
	}
}

void UANS_PunishAttackTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp->GetOwner() == nullptr)
		return;

	if (bool Char = MeshComp->GetOwner()->Implements<UInterface_Enemy>())
	{
		// Call your function to start the action
		IInterface_Enemy::Execute_PunishAttackTraceEnd(MeshComp->GetOwner());
	}
}

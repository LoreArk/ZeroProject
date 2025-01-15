// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_DoPairedAnimationDamage.h"
#include "Interface_Enemy.h"

void UAN_DoPairedAnimationDamage::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	AActor* Owner = MeshComp->GetOwner();

	if (Owner)
	{
		if (Owner->GetClass()->ImplementsInterface(UInterface_Enemy::StaticClass()))
		{
			IInterface_Enemy::Execute_DoGrabAttackDamage(Owner, DamageSettings);
		}
	}
}

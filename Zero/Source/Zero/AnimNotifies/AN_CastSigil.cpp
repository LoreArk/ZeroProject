// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_CastSigil.h"
#include "Interface_Player.h"

void UAN_CastSigil::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	AActor* Owner = MeshComp->GetOwner();

	if (Owner)
	{
		if (Owner->GetClass()->ImplementsInterface(UInterface_Player::StaticClass()))
		{
			IInterface_Player::Execute_CastSigilOfType(Owner);
		}
	}

}

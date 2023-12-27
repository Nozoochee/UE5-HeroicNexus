// Fill out your copyright notice in the Description page of Project Settings.


#include "HNPlayerCharacter.h"

#include "Card.h"
#include "PlayField.h"


// Sets default values
AHNPlayerCharacter::AHNPlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AHNPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	int32 monster = StaticCast<int32>(ECardTypes::Monster | ECardTypes::Charge | ECardTypes::Hero); //add
	int32 deez = static_cast<int32>(ECardTypes::Monster);
	deez |= (int32)ECardTypes::Charge;
	deez ^= (int32)ECardTypes::Charge; //flip flop
	monster &= ~StaticCast<int32>(ECardTypes::Monster | ECardTypes::Hero); //remove multiple
	UE_LOG(LogTemp, Warning, TEXT("mon: %i | monch: %i"), monster, deez);
}

void AHNPlayerCharacter::PlayPhase(const EPhases Phase)
{
	Super::PlayPhase(Phase);
	PlayPhaseBP(Phase); //BP version is used for UI stuff, otherwise cpp
}

void AHNPlayerCharacter::SelectCards(const TArray<ACard*> CardPool, int32 Min, int32 Max)
{
	Super::SelectCards(CardPool, Min, Max);
	SelectCardsBP(CardPool, Min, Max);
}

void AHNPlayerCharacter::PlayPhaseBP_Implementation(const EPhases Phase)
{
	if(Phase == EPhases::Main)
	{
		for (const auto card : Hand)
		{
			if(card->CurrentState.Id==1)
			{
				
				break;
			}
		}
	}
	else if (Phase == EPhases::Battle)
	{
		
	}
	OnPhaseFinishedDelegate.Broadcast();
}

void AHNPlayerCharacter::SelectCardsBP_Implementation(const TArray<ACard*>& CardPool, int32 Min, int32 Max)
{
	TArray<ACard*> result;
	UE_LOG(LogTemp, Warning, TEXT("Selecting cards: Pool %i | Min %i | Max %i"), CardPool.Num(), Min, Max);
	const int32 choices = FMath::RandRange(Min,Max);
	result.Reserve(choices);
	const int32 arrLen = CardPool.Num()-1;
	while(result.Num() < choices)
	{
		result.AddUnique(CardPool[FMath::RandRange(0,arrLen)]);
	}
	OnPlayerCardsSelected.Broadcast(result);
}

// Called every frame
void AHNPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


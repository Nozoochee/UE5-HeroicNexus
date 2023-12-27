// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacter.h"


// Sets default values
AAICharacter::AAICharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAICharacter::PlayPhase(const EPhases Phase)
{
	Super::PlayPhase(Phase);
	OnPhaseFinishedDelegate.Broadcast();
}

void AAICharacter::SelectCards(const TArray<ACard*> CardPool, int32 Min, int32 Max)
{
	Super::SelectCards(CardPool, Min, Max);
	UE_LOG(LogTemp, Warning, TEXT("Selecting cards: Pool %i | Min %i | Max %i"), CardPool.Num(), Min, Max);
	TArray<ACard*> result;
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
void AAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


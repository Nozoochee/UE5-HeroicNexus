// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

#include "Card.h"
#include "PlayField.h"

template<typename Type>
static void ShuffleArray(FRandomStream& Stream, TArray<Type>& Array) {
	const int32 LastIndex = Array.Num() - 1;

	for (int32 i = 0; i <= LastIndex; i += 1) {
		const int32 Index = Stream.RandRange(i, LastIndex);
		if (i == Index) {
			continue;
		}

		Array.Swap(i, Index);
	}
}


// Sets default values
ABaseCharacter::ABaseCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

TArray<ACard*> ABaseCharacter::LoadDeck_Implementation(APlayField* Field)
{
	TArray<ACard*> result;
	for (auto card : Deck)
	{
		const FTransform SpawnLocRot;
		ACard* spawnedCard = GetWorld()->SpawnActorDeferred<ACard>(card, SpawnLocRot, this, this, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		spawnedCard->Field = Field;
		spawnedCard->FinishSpawning(SpawnLocRot);
		result.Add(spawnedCard);
	}

	FRandomStream Rand(FMath::Rand());
	ShuffleArray(Rand, result);
	return result;
}

bool ABaseCharacter::RequestDuel_Implementation(ABaseCharacter* Target)
{
	return !bIsDueling;
}

void ABaseCharacter::SetIsDueling(const bool IsDueling)
{
	bIsDueling = IsDueling;
}

void ABaseCharacter::PlayPhase(const EPhases Phase)
{
	
}

void ABaseCharacter::AddCard(ACard* Card, ELocations Location)
{
	switch (Location)
	{
	case ELocations::None:
		break;
	case ELocations::Deck:
		break;
	case ELocations::Hand:
		{
			Hand.Add(Card);
		}
		break;
	case ELocations::MZone:
		break;
	case ELocations::CCZone:
		break;
	case ELocations::Grave:
		break;
	case ELocations::All:
		break;
	default: ;
	}
}

void ABaseCharacter::SelectCards(const TArray<ACard*> CardPool, int32 Min, int32 Max)
{
	
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


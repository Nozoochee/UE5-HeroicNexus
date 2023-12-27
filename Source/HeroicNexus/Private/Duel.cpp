// Fill out your copyright notice in the Description page of Project Settings.


#include "Duel.h"

#include "BaseCharacter.h"
#include "FieldVisualizer.h"
#include "HCGameMode.h"
#include "PlayField.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ADuel::ADuel()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ADuel::BeginPlay()
{
	Super::BeginPlay();
	if(Players.Num()<2)
	{
		UE_LOG(LogTemp, Warning, TEXT("Duel did not have enough players to start!"));
		Destroy();
	}
}

void ADuel::StartDuel()
{
	bool bIsHumanPlayerDuel = false;
	for (const auto player : Players)
	{
		if(player == UGameplayStatics::GetPlayerCharacter(this, 0))
		{
			bIsHumanPlayerDuel = true;
		}
		player->SetIsDueling(true);
		player->OnDuelStateChangedDelegate.Broadcast(EDuelState::Dueling);
	}
	
	MainField = GetWorld()->SpawnActorDeferred<APlayField>(
		APlayField::StaticClass(),
		GetTransform(),
		GetOwner(),
		GetOwner()->GetInstigatorController()->GetPawn(),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	MainField->Players = Players;
	MainField->Rules = Rules;
	MainField->FinishSpawning(GetTransform());

	const FVector MiddlePointBetweenPlayers = (Players[0]->GetActorLocation() + Players[1]->GetActorLocation())/2;
	const FRotator VisualRotator = (Players[1]->GetActorLocation() - Players[0]->GetActorLocation()).Rotation();
	FTransform VisualizeTransform = GetTransform();
	VisualizeTransform.SetLocation(MiddlePointBetweenPlayers);
	VisualizeTransform.SetRotation(VisualRotator.Quaternion());
	const auto GM = Cast<AHCGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	
	Visualizer = GetWorld()->SpawnActorDeferred<AFieldVisualizer>(
		GM->VisualizerClass,
		VisualizeTransform,
		GetOwner(),
		GetOwner()->GetInstigatorController()->GetPawn(),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	Visualizer->Field = MainField;
	Visualizer->Rules = Rules;
	Visualizer->FinishSpawning(VisualizeTransform);

	MainField->Visualizer = Visualizer;
	if(bIsHumanPlayerDuel)
	{
		Visualizer->OnCatchupCompleteDelegate.BindLambda([this](){MainField->StartDuel();});
		Visualizer->StartHumanPlayerDuelUI();
	}
	else
	{
		MainField->StartDuel();
	}
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "FieldVisualizer.h"

#include "BaseCharacter.h"
#include "HCGameMode.h"
#include "PlayerVisualizer.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AFieldVisualizer::AFieldVisualizer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

//Quick function so I can keep using my lambda delegate bind
void AFieldVisualizer::FinishedCatchingUp()
{
	OnCatchupCompleteDelegate.Execute();
}

void AFieldVisualizer::StartVisualization_Implementation()
{
	//for now BP only
}

void AFieldVisualizer::StartHumanPlayerDuelUI_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("CPP"));
	OnCatchupCompleteDelegate.Execute();
}

// Called when the game starts or when spawned
void AFieldVisualizer::BeginPlay()
{
	Super::BeginPlay();

	auto GM = Cast<AHCGameMode>(UGameplayStatics::GetGameMode(this));
	
	for (const auto player : Field->Players)
	{
		auto playViz = GetWorld()->SpawnActorDeferred<APlayerVisualizer>(GM->PlayerVisualizerClass, player->GetTransform(), this, GetOwner()->GetInstigatorController()->GetPawn(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		PlayerVisualizers.Add(player, playViz);
		playViz->Rules=Rules;
		playViz->PlayerRef=player;
		playViz->FinishSpawning(player->GetTransform());
	}
}

// Called every frame
void AFieldVisualizer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


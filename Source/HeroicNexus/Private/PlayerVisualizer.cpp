// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerVisualizer.h"

#include "HCGameMode.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
APlayerVisualizer::APlayerVisualizer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void APlayerVisualizer::BeginPlay()
{
	Super::BeginPlay();
	GameMode = Cast<AHCGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

}



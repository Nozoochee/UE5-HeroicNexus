// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CardSlot.generated.h"

UCLASS()
class HEROICNEXUS_API ACardSlot : public AActor
{
	GENERATED_BODY()

protected:
	//The mesh that gets displayed to show where the card slots are on the field
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="CardSlot")
	UStaticMeshComponent* SlotMesh; 
	
public:
	// Sets default values for this actor's properties
	ACardSlot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

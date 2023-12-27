// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Duel.h"
#include "Engine/DataTable.h"
#include "GameFramework/Character.h"
#include "HeroicNexus/Common.h"
#include "BaseCharacter.generated.h"

class APlayField;
enum class EPhases : uint8;
class ACard;

USTRUCT(Blueprintable)
struct FNPCDecks : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NpcId = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSoftClassPtr<ACard>> Deck;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPhaseFinishedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerCardsSelectedSignature, const TArray<ACard*>&, SelectedCards);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDuelStateChangedSignature, const EDuelState, NewState);


UCLASS()
class HEROICNEXUS_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NPC")
	int32 CharacterID;
	UPROPERTY()
	APlayField* MainField;
	UPROPERTY(BlueprintReadWrite, Category="Duel")
	TArray<ACard*> Hand;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Duel")
	FOnPhaseFinishedSignature OnPhaseFinishedDelegate;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Duel")
	FOnPlayerCardsSelectedSignature OnPlayerCardsSelected;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Duel")
	FOnDuelStateChangedSignature OnDuelStateChangedDelegate;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Deck")
	TArray<TSubclassOf<ACard>> Deck;

private:
	bool bIsDueling = false;
	
public:
	// Sets default values for this character's properties
	ABaseCharacter();

	UFUNCTION(BlueprintNativeEvent)
	TArray<ACard*> LoadDeck(APlayField* Field);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool RequestDuel(ABaseCharacter* Opponent);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetIsDueling() const {return bIsDueling;}
	UFUNCTION(BlueprintCallable)
	void SetIsDueling(const bool IsDueling);

	//Called by the PlayField
	UFUNCTION(BlueprintCallable)
	virtual void PlayPhase(const EPhases Phase);

	void AddCard(ACard* Card, ELocations Location);
	
	UFUNCTION()
	virtual void SelectCards(const TArray<ACard*> CardPool, int32 Min, int32 Max);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};

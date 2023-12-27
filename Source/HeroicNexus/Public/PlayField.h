// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Card.h"
#include "CardEffect.h"
#include "GameFramework/Actor.h"
#include "HeroicNexus/Common.h"
#include "PlayField.generated.h"

class AFieldVisualizer;
class ACard;
class UCardEffect;
class ABaseCharacter;

//Defines anything substantial that can happen during the course of a duel
UENUM(BlueprintType)
enum class EProcess : uint8
{
	Turn,
	Draw,
	SwitchAtkDef,
	SelectCard,
	Summon,
	EndDuel,
};

UENUM(BlueprintType)
enum class EPhases : uint8
{
	PreDraw,
	Draw,
	PreMain,
	Main,
	PreBattle,
	Battle,
	End
};

UENUM(BlueprintType)
enum class EChainStatus : uint8
{
	Active,
	Negated,
};

UENUM()
enum class EDuelEventType : uint8
{
	Target,
	Response,
};

USTRUCT(BlueprintType)
struct FDuelInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ABaseCharacter* TurnPlayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TurnID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPhases Phase;

	FDuelInfo()
	{
		TurnPlayer = nullptr;
		TurnID = 0;
		Phase = EPhases::PreDraw;
	}
};

USTRUCT(BlueprintType)
struct FProcessUnit
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EProcess Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Step;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCardEffect* Effect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> Targets;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EReasons Reason;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Arg1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Arg2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Arg3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ArgString;

	FProcessUnit()
	{
		Type = EProcess::Draw;
		Step = 0;
		Effect = nullptr;
		Targets = TArray<AActor*>();
		Reason = EReasons::Rule;
		Arg1 = 0;
		Arg2 = 0;
		Arg3 = 0;
		ArgString = "";
	}
};

USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Mana;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ACard*> MonsterZone;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ACard*> CCZone;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ACard*> Deck;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ACard*> Hand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ACard*> Grave;

	FPlayerInfo()
	{
		LP = 1;
		Mana = 0;
		MonsterZone = TArray<ACard*>();
		CCZone = TArray<ACard*>();
		Deck = TArray<ACard*>();
		Hand = TArray<ACard*>();
		Grave = TArray<ACard*>();
	}
};

USTRUCT(BlueprintType)
struct FDuelEvent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACard* TriggerCard;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEffectTrigger Trigger;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EReasons Reason;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCardEffect* ReasonEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ABaseCharacter* ReasonPlayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ABaseCharacter* EventPlayer;

	FDuelEvent()
	{
		TriggerCard = nullptr;
		Trigger = EEffectTrigger::Auto;
		Reason = EReasons::Rule;
		ReasonEffect = nullptr;
		ReasonPlayer = nullptr;
		EventPlayer = nullptr;
	}
};

/**
 * A struct to describe an entire operation (such as summoning a monster), consisting of the process and all related events. Used for visualization.
 */
USTRUCT(BlueprintType)
struct FDuelOperation
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)	//e.g. 1P Draw 5
	FProcessUnit Process;	
	UPROPERTY(BlueprintReadWrite)	//this will consist of 5 cards that got there via the draw event
	TArray<FDuelEvent> Events;

	FDuelOperation()
	{
		Events.Empty();
	}
};

USTRUCT(BlueprintType)
struct FChain
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCardEffect* TriggeringEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ABaseCharacter* TriggeringPlayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACard* TriggeringCard;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ACard*> TargetCards;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ABaseCharacter* TargetPlayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EChainStatus Status;

	FChain()
	{
		TriggeringEffect = nullptr;
		TriggeringPlayer = nullptr;
		TriggeringCard = nullptr;
		TargetCards = TArray<ACard*>();
		TargetPlayer = nullptr;
		Status = EChainStatus::Active;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChangedSignature, EPhases, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCardsSelectedSignature, const TArray<ACard*>&, SelectedCards);
DECLARE_DELEGATE(FOnProcessAddedSignature);
DECLARE_DELEGATE(FOnEventExecutedSignature);

UCLASS()
class HEROICNEXUS_API APlayField : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Field")
	TArray<ABaseCharacter*> Players;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Field")
	FDuelRules Rules;
	UPROPERTY(BlueprintReadOnly, Category="Field")
	AFieldVisualizer* Visualizer = nullptr;
	UPROPERTY(BlueprintReadOnly, Category="Field")
	FDuelOperation Operation;	
	UPROPERTY(BlueprintAssignable, Category="Field")
	FOnPhaseChangedSignature OnPhaseChangedDelegate;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Field")
	FOnCardsSelectedSignature OnCardsSelectedDelegate;
	FOnProcessAddedSignature OnProcessAddedDelegate;
	FOnEventExecutedSignature OnEventExecutedDelegate;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Field")
	TArray<FProcessUnit> Processes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Field")
	TArray<FDuelEvent> TargetingEvents;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Field")
	TArray<FDuelEvent> ResponseEvents;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Field")
	TArray<FChain> Chains;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Field")
	FDuelInfo DuelInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Field")
	TMap<ABaseCharacter*, FPlayerInfo> PlayerInfos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Field")
	FProcessUnit CurrentProcess;

private:
	UPROPERTY(EditAnywhere, Category="Field")
	bool bIsProcessingEvents = false;
	UPROPERTY(EditAnywhere, Category="Field")
	bool bIsProcessingChains = false;
	UPROPERTY()
	bool bIsWaitingForPlayerInput = false;
	UPROPERTY()
	bool bIsWaitingForVisuals = false;
	
public:
	// Sets default values for this actor's properties
	APlayField();
	
	UFUNCTION(BlueprintCallable)
	void AddProcess(const FProcessUnit& Process);
	/**
	 * @brief Only used for events that target a specific card.
	 * @param Event 
	 */
	UFUNCTION(BlueprintCallable)
	void RaiseTargetingEvent(const FDuelEvent& Event);
	/**
	 * @brief Used after all TargetingEvents have been raised to find effects that can respond to it
	 * @param Event 
	 */
	UFUNCTION(BlueprintCallable)
	void RaiseResponseEvent(const FDuelEvent& Event);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FPlayerInfo GetInfo(ABaseCharacter* Player) const {return *PlayerInfos.Find(Player);}
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FDuelInfo GetTurnInfo() const {return DuelInfo;}
	void StartDuel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnPhaseChanged(EPhases NewPhase);
	void OnProcessAdded();
	void Process();
	void Draw();
	void Turn();
	void SelectCards();
	void SwitchAtkDef();
	void PreDrawPhase();
	void DrawPhase();
	void PreMainPhase();
	void MainPhase();
	void PreBattlePhase();
	void BattlePhase();
	void EndPhase();
	void AdvancePhase(EPhases Phase);
	UFUNCTION()
	void OnPhaseFinished();
	void ProcessEvents(const EDuelEventType Type);
	void OnEventExecuted();
	void CreateChainLink(UCardEffect* Effect);
	void ProcessChain();
	void OnChainExecuted();
	UFUNCTION()
	void ProcessCardSelection(const TArray<ACard*>& ChosenCards);

	void UseCard(ACard* Card, ECardActions Action);
	void SummonCard(ACard* Card, ELocations Location);
	void MoveToField(ACard* Card, ELocations Location, EReasons Reason);
	void MoveCard(ACard* Card, ABaseCharacter* Player, ELocations Location);
	void RemoveCard(ACard* Card, ELocations Location);
	void AddCard(ACard* Card, ABaseCharacter* Player, ELocations Location);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool DoesCardOfTypeExist(UPARAM(meta=(Bitmask, BitmaskEnum="/Script/HeroicNexus.ECardTypes")) int32 Type, ELocations Location);
	UFUNCTION(BlueprintCallable)
	TMap<ACard*, FCardActions> GetAllUsableCards(ABaseCharacter* Player);
	UFUNCTION(BlueprintCallable)
	TArray<ACard*> GetTargetsForEffect(ABaseCharacter* Player, UCardEffect* Effect, TArray<ELocations> Locations);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	ABaseCharacter* GetOpponent(ABaseCharacter* Player) const;
private:
	TArray<ACard*> GetAllCardsOnField();
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Field")
	int32 GetNumEmptyMonsterZones(ABaseCharacter* Player) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Field")
	int32 GetNumEmptyCCZones(ABaseCharacter* Player) const;
	
};

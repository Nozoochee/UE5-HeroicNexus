// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayField.h"
#include "FieldVisualizer.h"
#include "BaseCharacter.h"
#include "Card.h"

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
APlayField::APlayField()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void APlayField::AddProcess(const FProcessUnit& Process)
{
	Processes.Add(Process);
	OnProcessAddedDelegate.Execute();
}

void APlayField::RaiseTargetingEvent(const FDuelEvent& Event)
{
	TargetingEvents.Add(Event);
}

void APlayField::RaiseResponseEvent(const FDuelEvent& Event)
{
	ResponseEvents.Add(Event);
}

// Called when the game starts or when spawned
void APlayField::BeginPlay()
{
	Super::BeginPlay();
	OnPhaseChangedDelegate.AddUniqueDynamic(this, &APlayField::OnPhaseChanged);
	OnProcessAddedDelegate.BindUObject(this, &APlayField::OnProcessAdded);
}

void APlayField::OnPhaseChanged(const EPhases NewPhase)
{
	const FString PhaseStr = UEnum::GetValueAsString(NewPhase);
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.f, FColor::Yellow, FString::Printf(TEXT("New Phase: %s"), *PhaseStr));
}

void APlayField::OnProcessAdded()
{
	UE_LOG(LogTemp, Warning, TEXT("New Process Added: %s"), *UEnum::GetValueAsString(Processes.Last().Type));
	Process();
}

void APlayField::StartDuel()
{
	UE_LOG(LogTemp, Warning, TEXT("Duel started!"));	
	FRandomStream Rand(FMath::Rand());
	//ShuffleArray(Rand, Players); TODO unshuffle
	DuelInfo.TurnPlayer = Players[0];
	for (const auto Player : Players)
	{
		auto deck = Player->LoadDeck(this);
		FPlayerInfo info;
		info.LP = Rules.StartLP;
		info.Mana = Rules.StartMana;
		info.Deck = deck;
		info.MonsterZone.Reserve(Rules.MonsterZones);
		info.CCZone.Reserve(Rules.CCZones);
		info.Hand.Reserve(Rules.HandMaxCardCount);
		Player->Hand = info.Hand;
		info.Grave.Reserve(Rules.MaxDeckSize/2);

		PlayerInfos.Add(Player, info);

		FProcessUnit DrawProcess;
		TArray<AActor*> PlayerArr;
		PlayerArr.Add(Player);
		DrawProcess.Type = EProcess::Draw;
		DrawProcess.Targets = PlayerArr;
		DrawProcess.Reason = EReasons::Rule;
		DrawProcess.Arg1 = Rules.StartHandCount;
		Processes.Add(DrawProcess); //AddProcess() immediately runs it afterwards (usually that's good), but this is a special one-off situation where I need to add multiple to get the duel started
		UE_LOG(LogTemp, Warning, TEXT("New Process Added: %s"), *UEnum::GetValueAsString(Processes.Last().Type));
	}
	FProcessUnit TurnProcess;
	TurnProcess.Type = EProcess::Turn;
	AddProcess(TurnProcess);
}

void APlayField::Process()
{
	if(!bIsWaitingForVisuals && Visualizer->GetNumVisualizeOperations() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Switching to visualizing"));
		bIsWaitingForVisuals = true;
		Visualizer->OnCatchupCompleteDelegate.BindLambda([this]()
		{
			bIsWaitingForVisuals = false;
			Process();
		});
		Visualizer->StartVisualization();
		return;
	}
	if(bIsWaitingForPlayerInput) return;
	
	while (!Processes.IsEmpty() && !bIsWaitingForPlayerInput && !bIsWaitingForVisuals)
	{
		Operation = FDuelOperation();
		CurrentProcess = Processes[0];
		Processes.RemoveAt(0);
		Operation.Process = CurrentProcess;
		
		switch (CurrentProcess.Type)
		{
		case EProcess::Draw:
			Draw();
			break;
		case EProcess::Turn:
			Turn();
			break;
		case EProcess::EndDuel:
			break;
		case EProcess::SelectCard:
			SelectCards();
			break;
		case EProcess::SwitchAtkDef:
			break;
		case EProcess::Summon:
			SummonCard(StaticCast<ACard*>(CurrentProcess.Targets[0]), StaticCast<ACard*>(CurrentProcess.Targets[0])->ToFieldParams.Location);
			break;
		default: ;
		}

		Visualizer->AddVisualizeOperation(Operation);
	}
}

/**
 * @brief Draw cards from the deck to the hand. Raises 'Draw' 'Move' 'ToHand' events for each card.
 * If not enough cards can be drawn, the player loses from deck-out.
 */
void APlayField::Draw()
{
	if(ABaseCharacter* player = Cast<ABaseCharacter>(CurrentProcess.Targets[0]))
	{
		FPlayerInfo* info = PlayerInfos.Find(player);
		for (int i = 0; i < CurrentProcess.Arg1; ++i)
		{
			ACard* card = info->Deck.Pop();
			if(!card)
			{
				//TODO Proper deck-out implementation
				GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.f, FColor::Red, "Can't Draw cause deck empty!");
				Destroy();
			}

			info->Hand.Add(card);
			player->AddCard(card, ELocations::Hand);
			
			card->CurrentState.Location = ELocations::Hand;
			FDuelEvent event;
			event.TriggerCard = card;
			event.Trigger = EEffectTrigger::Draw;
			event.Reason = CurrentProcess.Reason;
			event.ReasonEffect = CurrentProcess.Effect;
			event.ReasonPlayer = player;
			event.EventPlayer = player;
			RaiseTargetingEvent(event);
			Operation.Events.Add(event);

			event.Trigger = EEffectTrigger::Move;
			RaiseTargetingEvent(event);

			event.Trigger = EEffectTrigger::ToHand;
			RaiseTargetingEvent(event);
		}

		ProcessEvents(EDuelEventType::Target);
		
		FDuelEvent event;
		event.Trigger = EEffectTrigger::Draw;
		event.Reason = CurrentProcess.Reason;
		event.ReasonEffect = CurrentProcess.Effect;
		event.ReasonPlayer = player;
		event.EventPlayer = player;
		RaiseResponseEvent(event);

		event.Trigger = EEffectTrigger::Move;
		RaiseResponseEvent(event);

		event.Trigger = EEffectTrigger::ToHand;
		RaiseResponseEvent(event);

		ProcessEvents(EDuelEventType::Response);
		
		UE_LOG(LogTemp, Warning, TEXT("Draw finished. Hand size: %d"), info->Hand.Num());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Draw target was not a player!"));
	}
}

void APlayField::Turn()
{
	UE_LOG(LogTemp, Warning, TEXT("Starting phase %s"), *UEnum::GetValueAsString(DuelInfo.Phase));
	OnPhaseChangedDelegate.Broadcast(DuelInfo.Phase);
	switch (DuelInfo.Phase)
	{
	case EPhases::PreDraw:
		PreDrawPhase();
		break;
	case EPhases::Draw:
		DrawPhase();
		break;
	case EPhases::PreMain:
		PreMainPhase();
		break;
	case EPhases::Main:
		MainPhase();
		break;
	case EPhases::PreBattle:
		PreBattlePhase();
		break;
	case EPhases::Battle:
		BattlePhase();
		break;
	case EPhases::End:
		EndPhase();
		break;
	}
}

void APlayField::SelectCards()
{
	bIsWaitingForPlayerInput = true;
	//different if simulating/AI
	ABaseCharacter* player = CurrentProcess.Effect->TargetPlayer;
	player->OnPlayerCardsSelected.AddUniqueDynamic(this, &APlayField::ProcessCardSelection);
	
	player->SelectCards(CurrentProcess.Effect->Targets, CurrentProcess.Arg1, CurrentProcess.Arg2);
}

void APlayField::SwitchAtkDef()
{
	
	for (const auto target : CurrentProcess.Targets)
	{
		if(ACard* card = Cast<ACard>(target))
		{
			card->PreviousState.Atk = card->CurrentState.Atk;
			card->PreviousState.Def = card->CurrentState.Def;
			card->CurrentState.Atk = card->PreviousState.Def;
			card->CurrentState.Def = card->PreviousState.Atk;

			FDuelEvent noticeEvent;
			noticeEvent.TriggerCard = card;
			noticeEvent.Trigger = EEffectTrigger::ChangeAtk;
			noticeEvent.Reason = CurrentProcess.Reason;
			noticeEvent.ReasonEffect = CurrentProcess.Effect;
			noticeEvent.ReasonPlayer = Cast<ABaseCharacter>(CurrentProcess.Effect->OwnerCard->Owner);
			noticeEvent.EventPlayer = Cast<ABaseCharacter>(CurrentProcess.Effect->OwnerCard->Owner);
			RaiseTargetingEvent(noticeEvent);
			noticeEvent.Trigger = EEffectTrigger::ChangeDef;
			RaiseTargetingEvent(noticeEvent);
		}
	}
	FDuelEvent responseEvent;
	responseEvent.Trigger = EEffectTrigger::ChangeAtk;
	responseEvent.Reason = CurrentProcess.Reason;
	responseEvent.ReasonEffect = CurrentProcess.Effect;
	responseEvent.ReasonPlayer = Cast<ABaseCharacter>(CurrentProcess.Effect->OwnerCard->Owner);
	responseEvent.EventPlayer = Cast<ABaseCharacter>(CurrentProcess.Effect->OwnerCard->Owner);
	RaiseResponseEvent(responseEvent);
	responseEvent.Trigger = EEffectTrigger::ChangeDef;
	RaiseResponseEvent(responseEvent);

	ProcessEvents(EDuelEventType::Target);
	ProcessEvents(EDuelEventType::Response);
}

//Clears all cards from their turn specific attributes and add mana
void APlayField::PreDrawPhase()
{
	for (const auto player : Players)
	{
		const FPlayerInfo* info = PlayerInfos.Find(player);
		TArray<ACard*> fieldCards;
		fieldCards.Append(info->MonsterZone);
		fieldCards.Append(info->CCZone);
		
		for (const auto card : fieldCards)
		{
			if(card)
			{
				card->ResetTurnAttributes();
			}
		}
	}
	PlayerInfos.Find(DuelInfo.TurnPlayer)->Mana++; //TODO Make something proper for adding mana
	AdvancePhase(EPhases::Draw);
}

void APlayField::DrawPhase()
{
	TArray<AActor*> arr;
	arr.Add(DuelInfo.TurnPlayer);
	FProcessUnit drawProcess;
	drawProcess.Type = EProcess::Draw;
	drawProcess.Targets = arr;
	drawProcess.Reason = EReasons::Rule;
	drawProcess.Arg1 = 1;
	AddProcess(drawProcess);

	AdvancePhase(EPhases::PreMain);
}

//Raise event to trigger all cards that should trigger at the beginning of the main phase
void APlayField::PreMainPhase()
{
	FDuelEvent event;
	event.Trigger = EEffectTrigger::MainPhaseStart;
	event.EventPlayer = DuelInfo.TurnPlayer;
	RaiseResponseEvent(event);
	ProcessEvents(EDuelEventType::Response);
	AdvancePhase(EPhases::Main);
}

void APlayField::MainPhase()
{
	DuelInfo.TurnPlayer->OnPhaseFinishedDelegate.AddDynamic(this, &APlayField::OnPhaseFinished);
	DuelInfo.TurnPlayer->PlayPhase(EPhases::Main);
	//GetAllUsableCards(DuelInfo.TurnPlayer);
}

//Raise event to trigger all cards that should trigger at the beginning of the battle phase
void APlayField::PreBattlePhase()
{
	FDuelEvent event;
	event.Trigger = EEffectTrigger::BattlePhaseStart;
	event.EventPlayer = DuelInfo.TurnPlayer;
	RaiseResponseEvent(event);
	ProcessEvents(EDuelEventType::Response);
	AdvancePhase(EPhases::Battle);
}

void APlayField::BattlePhase()
{
	//No attacking on the first turn of the duel
	if(DuelInfo.TurnID == 0)
	{
		AdvancePhase(EPhases::End);
	}
	else
	{
		DuelInfo.TurnPlayer->OnPhaseFinishedDelegate.AddDynamic(this, &APlayField::OnPhaseFinished);
		DuelInfo.TurnPlayer->PlayPhase(EPhases::Battle);
	}
}

//Adds charge to turn player's cards and sets up for next turn
void APlayField::EndPhase()
{
	const FPlayerInfo* info = PlayerInfos.Find(DuelInfo.TurnPlayer);
	TArray<ACard*> fieldCards;
	fieldCards.Append(info->MonsterZone);
	fieldCards.Append(info->CCZone);
	for (const auto card : fieldCards)
	{
		if(card)
		{
			card->CurrentState.Charge++; //TODO Make something proper for adding card charge
		}
	}
	int32 playerIndex = Players.Find(DuelInfo.TurnPlayer);
	playerIndex = (playerIndex + 1)%Players.Num();
	DuelInfo.TurnPlayer = Players[playerIndex];
	DuelInfo.TurnID++;

	AdvancePhase(EPhases::PreDraw);
}

void APlayField::AdvancePhase(const EPhases Phase)
{
	UE_LOG(LogTemp, Warning, TEXT("Finished phase: %s"), *UEnum::GetValueAsString(DuelInfo.Phase));
	DuelInfo.Phase = Phase;
	TArray<AActor*> arr;
	arr.Add(DuelInfo.TurnPlayer);
	FProcessUnit process;
	process.Type = EProcess::Turn;
	process.Targets = arr;
	process.Reason = EReasons::Rule;
	AddProcess(process);
}

void APlayField::OnPhaseFinished() //For Player input
{
	UE_LOG(LogTemp, Warning, TEXT("Finished phase: %s"), *UEnum::GetValueAsString(DuelInfo.Phase));
	DuelInfo.TurnPlayer->OnPhaseFinishedDelegate.Clear();
	uint8 phase = static_cast<uint8>(DuelInfo.Phase);
	++phase;
	AdvancePhase(static_cast<EPhases>(phase));
}

/**
 * @brief Handles triggering response effects for events
 */
void APlayField::ProcessEvents(const EDuelEventType Type)
{
	if(TargetingEvents.IsEmpty() && ResponseEvents.IsEmpty())
	{
		bIsProcessingEvents = false;
		return;
	}
	
	bIsProcessingEvents = true;
	OnEventExecutedDelegate.BindUObject(this, &APlayField::OnEventExecuted);

	FDuelEvent event;
	switch (Type)
	{
	case EDuelEventType::Target:
		event = TargetingEvents[0];
		if(ACard* currentCard = event.TriggerCard)
		{
			TArray<UCardEffect*> effects = currentCard->GetTriggerableEffects(event.Trigger);
			if(!effects.IsEmpty())
			{
				for (const auto Effect : effects)
				{
					if(Effect->IsActivatable())
						currentCard->IncreaseUseCount();
						CreateChainLink(Effect);
				}
			}
		}
		return;
		
	case EDuelEventType::Response:
		event = ResponseEvents[0];
		//Iterate through all cards on the field for possible responses
		const TArray<ACard*> cards = GetAllCardsOnField();
		for (const auto Card : cards)
		{
			TArray<UCardEffect*> effects = Card->GetTriggerableEffects(event.Trigger);
			if(!effects.IsEmpty())
			{
				for (const auto Effect : effects)
				{
					//Currently just adds everything that can possibly be activated
					if(Effect->IsActivatable())
						Card->IncreaseUseCount();
						CreateChainLink(Effect);
				}
			}
		}
		//Once all response events have been found, just try the chain?
		//Currently everything that can respond WILL be added
		//Also I'm not using the delegate
		ProcessChain();
	}
}

void APlayField::OnEventExecuted()
{
	OnEventExecutedDelegate.Unbind();
	TargetingEvents.RemoveAt(0);
	ProcessEvents(EDuelEventType::Target);
}

void APlayField::CreateChainLink(UCardEffect* Effect)
{
	FChain chain;
	chain.TriggeringEffect = Effect;
	chain.TriggeringPlayer = Cast<ABaseCharacter>(Effect->OwnerCard->CurrentState.Controller);
	chain.TriggeringCard = Effect->OwnerCard;
	chain.TargetCards = Effect->Targets;
	chain.TargetPlayer = Effect->TargetPlayer;

	Chains.Add(chain);
}

void APlayField::ProcessChain()
{
	if(Chains.IsEmpty())
	{
		bIsProcessingChains = false;
		return;
	}

	bIsProcessingChains = true;
	FChain chain = Chains.Pop();
	chain.TriggeringEffect->OnExecuteFinishedDelegate.BindUObject(this, &APlayField::OnChainExecuted);
	chain.TriggeringEffect->Execute();
}

void APlayField::OnChainExecuted()
{
	Chains.Pop();
	ProcessChain();
}

void APlayField::ProcessCardSelection(const TArray<ACard*>& ChosenCards)
{
	bIsWaitingForPlayerInput = false;
	OnCardsSelectedDelegate.Broadcast(ChosenCards);
	OnCardsSelectedDelegate.RemoveAll(this);
	Process();
}

void APlayField::UseCard(ACard* Card, ECardActions Action)
{
	UE_LOG(LogTemp, Warning, TEXT("Using Card: %s | With action:  %s"),*Card->OriginalData.Name.ToString(), *UEnum::GetValueAsString(Action));
	switch (Action)
	{
	case ECardActions::Inspect:
		break;
	case ECardActions::Summon:
		{
			if((Card->CurrentState.Type & StaticCast<int32>(ECardTypes::Monster)) > 0)
			{
				FToFieldParams params;
				params.MovePlayer = DuelInfo.TurnPlayer;
				params.Player = DuelInfo.TurnPlayer;
				params.Location = ELocations::MZone;
				params.Position = ECardPosition::FaceupAttack;
				Card->ToFieldParams = params;
				FProcessUnit process;
				process.Targets.Add(Card);
				process.Type = EProcess::Summon;
				AddProcess(process);
			}
		}
		break;
	case ECardActions::Set:
		{
			if((Card->CurrentState.Type & StaticCast<int32>(ECardTypes::Monster | ECardTypes::Charge)) > 0)
			{
				FToFieldParams params;
				params.MovePlayer = DuelInfo.TurnPlayer;
				params.Player = DuelInfo.TurnPlayer;
				if((Card->CurrentState.Type & StaticCast<int32>(ECardTypes::Monster)) > 0)
				{
					params.Location = ELocations::MZone;
				}
				else
				{
					params.Location = ELocations::CCZone;
				}
				params.Position = ECardPosition::Set;
				Card->ToFieldParams = params;
				FProcessUnit process;
				process.Targets.Add(Card);
				process.Type = EProcess::Summon;
				AddProcess(process);
			}
		}
		break;
	case ECardActions::Activate: 
		{
			if(Card->IsType(ECardTypes::Charge) && Card->CurrentState.Location == ELocations::Hand)
			{
				FToFieldParams params;
				params.MovePlayer = DuelInfo.TurnPlayer;
				params.Player = DuelInfo.TurnPlayer;
				params.Location = ELocations::CCZone;
				params.Position = ECardPosition::FaceupAttack;
				Card->ToFieldParams = params;
				SummonCard(Card, Card->ToFieldParams.Location);
			}
			
			FDuelEvent Ev;
			Ev.TriggerCard = Card;
			Ev.Trigger = EEffectTrigger::ActivateEffect;
			Ev.EventPlayer = Cast<ABaseCharacter>(Card->Owner);
			RaiseTargetingEvent(Ev);
			ProcessEvents(EDuelEventType::Target);

			RaiseResponseEvent(Ev);
			ProcessEvents(EDuelEventType::Response);
		}
		break;
	case ECardActions::Flip:
		{
			Card->PreviousState.Position = Card->CurrentState.Position;
			Card->CurrentState.Position = ECardPosition::FaceupAttack;
			Card->Statuses |= StaticCast<int32>(ECardStatuses::FlipSummonTurn);

			FDuelEvent flipEvent;
			flipEvent.TriggerCard = Card;
			flipEvent.Trigger = EEffectTrigger::Flip;
			flipEvent.Reason = EReasons::Summon;
			flipEvent.EventPlayer = Cast<ABaseCharacter>(Card->Owner);
			RaiseTargetingEvent(flipEvent);
			flipEvent.Trigger = EEffectTrigger::FlipSummonSuccess;
			RaiseTargetingEvent(flipEvent);
			flipEvent.Trigger = EEffectTrigger::ChangePosition;
			RaiseTargetingEvent(flipEvent);

			ProcessEvents(EDuelEventType::Target);
			
			flipEvent.TriggerCard = nullptr;
			flipEvent.Trigger = EEffectTrigger::Flip;
			RaiseResponseEvent(flipEvent);
			flipEvent.Trigger = EEffectTrigger::FlipSummonSuccess;
			RaiseResponseEvent(flipEvent);
			flipEvent.Trigger = EEffectTrigger::ChangePosition;
			RaiseResponseEvent(flipEvent);

			ProcessEvents(EDuelEventType::Response);
		}
		break;
	case ECardActions::ChangePosition:
		{
			Card->PreviousState.Position = Card->CurrentState.Position;
			Card->CurrentState.Position = Card->CurrentState.Position == ECardPosition::FaceupAttack ? ECardPosition::FaceupDefense : ECardPosition::FaceupAttack;
			Card->Statuses |= StaticCast<int32>(ECardStatuses::PosChanged);
			FDuelEvent posEv;
			posEv.TriggerCard = Card;
			posEv.Trigger = EEffectTrigger::ChangePosition;
			posEv.EventPlayer = Cast<ABaseCharacter>(Card->Owner);
			RaiseTargetingEvent(posEv);
			ProcessEvents(EDuelEventType::Target);

			RaiseResponseEvent(posEv);
			ProcessEvents(EDuelEventType::Response);
		}
		break;
	case ECardActions::Attack: //TODO finish all other action cases
		break;
	case ECardActions::Tribute:
		break;
	default: ;
	}
}

void APlayField::SummonCard(ACard* Card, ELocations Location)
{
	UE_LOG(LogTemp, Warning, TEXT("Summoning: %s to: %s"),*Card->OriginalData.Name.ToString(), *UEnum::GetValueAsString(Location));
	Operation.Events.Empty();
	MoveToField(Card, Location, EReasons::Summon);
	Card->Statuses |= StaticCast<int32>(ECardStatuses::SummonTurn);
	if(Card->ToFieldParams.Position != ECardPosition::Set)
	{
		FDuelEvent summon;
		summon.TriggerCard = Card;
		summon.Trigger = EEffectTrigger::SummonSuccess;
		summon.Reason = EReasons::Summon;
		summon.ReasonPlayer = Card->ToFieldParams.Player;
		summon.EventPlayer = Card->ToFieldParams.Player;
		RaiseTargetingEvent(summon);
		Operation.Events.Add(summon);
		Visualizer->AddVisualizeOperation(Operation);
		summon.TriggerCard = nullptr;
		RaiseResponseEvent(summon);
		ProcessEvents(EDuelEventType::Target);
		ProcessEvents(EDuelEventType::Response);
	}
}

void APlayField::MoveToField(ACard* Card, ELocations Location, EReasons Reason)
{
	UE_LOG(LogTemp, Warning, TEXT("Moving: %s to Field: %s"),*Card->OriginalData.Name.ToString(), *UEnum::GetValueAsString(Location));
	MoveCard(Card, Card->ToFieldParams.Player, Location);
	Card->CurrentState.Reason = Reason;
	Card->CurrentState.ReasonPlayer = Card->ToFieldParams.Player;
	FDuelEvent moveEvent;
	moveEvent.TriggerCard = Card;
	moveEvent.Trigger = EEffectTrigger::Move;
	moveEvent.Reason = Reason;
	moveEvent.ReasonPlayer = Card->ToFieldParams.Player;
	moveEvent.EventPlayer = Card->ToFieldParams.Player;
	RaiseTargetingEvent(moveEvent);
	Operation.Events.Add(moveEvent);
	ProcessEvents(EDuelEventType::Target);
	moveEvent.TriggerCard = nullptr;
	RaiseResponseEvent(moveEvent);
	ProcessEvents(EDuelEventType::Response);
}

void APlayField::MoveCard(ACard* Card, ABaseCharacter* Player, ELocations Location)
{
	UE_LOG(LogTemp, Warning, TEXT("Moving: %s to: %s"),*Card->OriginalData.Name.ToString(), *UEnum::GetValueAsString(Location));
	Card->PreviousState.Controller = Card->CurrentState.Controller;
	Card->PreviousState.Location = Card->CurrentState.Location;
	Card->PreviousState.Position = Card->CurrentState.Position;

	Card->CurrentState.Controller = Player;
	Card->CurrentState.Location = Location;

	RemoveCard(Card, Card->CurrentState.Location);
	AddCard(Card, Player, Location);
}

void APlayField::RemoveCard(ACard* Card, ELocations Location)
{
	const auto info = PlayerInfos.Find(Cast<ABaseCharacter>(Card->GetOwner()));
	switch (Location)
	{
	case ELocations::Deck:
		info->Deck.Remove(Card);
		break;
	case ELocations::Hand:
		info->Hand.Remove(Card);
		break;
	case ELocations::MZone:
		info->MonsterZone.Remove(Card);
		break;
	case ELocations::CCZone:
		info->CCZone.Remove(Card);
		break;
	case ELocations::Grave:
		info->Grave.Remove(Card);
		break;
	default: ;
	}
}

void APlayField::AddCard(ACard* Card, ABaseCharacter* Player, ELocations Location)
{
	const auto info = PlayerInfos.Find(Cast<ABaseCharacter>(Player));
	switch (Location)
	{
	case ELocations::Deck:
		info->Deck.Add(Card);
		break;
	case ELocations::Hand:
		info->Hand.Add(Card);
		break;
	case ELocations::MZone:
		{
			info->MonsterZone.Add(Card);
			Card->CurrentState.Position = Card->ToFieldParams.Position;
		}
		break;
	case ELocations::CCZone:
		{
			info->CCZone.Add(Card);
			Card->CurrentState.Position = Card->ToFieldParams.Position;
		}
		break;
	case ELocations::Grave:
		info->Grave.Add(Card);
		break;
	default: ;
	}
}


bool APlayField::DoesCardOfTypeExist(int32 Type, ELocations Location)
{
	switch (Location)
	{
	case ELocations::All:
		{
			TArray<ACard*> fieldCards = GetAllCardsOnField();
			for (const auto card : fieldCards)
			{
				if((card->CurrentState.Type & Type) == Type) //Make sure EVERY bit is the same. If just > 0, would mean at least 1
				{
					return true;
				}
			}
		}
		break;
	case ELocations::Deck: //TODO finish rest of the switch cases
		break;
	case ELocations::Grave:
		break;
	case ELocations::Hand:
		break;
	case ELocations::MZone:
		break;
	case ELocations::CCZone:
		break;
	case ELocations::None:
		return false;
	default:
		return false;
	}
	return false;
}

TMap<ACard*, FCardActions> APlayField::GetAllUsableCards(ABaseCharacter* Player)
{
	TMap<ACard*, FCardActions> result;
	TArray<ACard*> cards;
	//auto opponent = GetOpponent(Player);
	cards.Append(GetInfo(Player).Hand);
	cards.Append(GetInfo(Player).Grave);
	cards.Append(GetInfo(Player).MonsterZone);
	cards.Append(GetInfo(Player).CCZone);

	for (auto card : cards)
	{
		TArray<ECardActions> actions = card->GetAllUsableActions(Player);
		FCardActions struc;
		struc.Actions = actions;
		if(actions.Num()>1)
		{
			//more than just inspect
			result.Add(card, struc);
			FString str;
			for (auto action : actions)
			{
				str += " | ";
				str += UEnum::GetValueAsString(action);
			}
			UE_LOG(LogTemp, Warning, TEXT("%s %s"), *card->OriginalData.Name.ToString(), *str)
		}
	}
	
	return result;
}

TArray<ACard*> APlayField::GetTargetsForEffect(ABaseCharacter* Player, UCardEffect* Effect,
                                               TArray<ELocations> Locations)
{
	//Consider using the effect's Range param
	//Otherwise the Locations arr [0] = self, [1] = opp
	TArray<ACard*> res;
	TArray<ACard*> cardsToSearchThrough;

	ABaseCharacter* player = DuelInfo.TurnPlayer;
	
	for (const auto location : Locations)
	{
		switch (location) {
		case ELocations::Deck:
			cardsToSearchThrough.Append(PlayerInfos.Find(player)->Deck);
			break;
		case ELocations::Hand:
			cardsToSearchThrough.Append(PlayerInfos.Find(player)->Hand);
			break;
		case ELocations::MZone:
			cardsToSearchThrough.Append(PlayerInfos.Find(player)->MonsterZone);
			break;
		case ELocations::CCZone:
			cardsToSearchThrough.Append(PlayerInfos.Find(player)->CCZone);
			break;
		case ELocations::Grave:
			cardsToSearchThrough.Append(PlayerInfos.Find(player)->Grave);
			break;
		case ELocations::All:
			//TODO All case
			break;
		default: ;
		}
		player = GetOpponent(player);
	}
	for (auto const card : cardsToSearchThrough)
	{
		if(Effect->FilterTarget(card))
		{
			res.Add(card);
		}
	}

	return res;
}

ABaseCharacter* APlayField::GetOpponent(ABaseCharacter* Player) const
{
	for (auto p : PlayerInfos)
	{
		if(p.Get<0>()!=Player)
		{
			return p.Get<0>();
		}
	}
	return nullptr; //should be impossible
}

TArray<ACard*> APlayField::GetAllCardsOnField()
{
	TArray<ACard*> res;
	for (auto PlayerInfo : PlayerInfos)
	{
		res.Append(PlayerInfo.Get<1>().MonsterZone);
		res.Append(PlayerInfo.Get<1>().CCZone);
	}
	return res;
}

// Called every frame
void APlayField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

int32 APlayField::GetNumEmptyMonsterZones(ABaseCharacter* Player) const
{
	return Rules.MonsterZones - GetInfo(Player).MonsterZone.Num();
}

int32 APlayField::GetNumEmptyCCZones(ABaseCharacter* Player) const
{
	return Rules.CCZones - GetInfo(Player).CCZone.Num();
}


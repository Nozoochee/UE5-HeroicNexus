// Fill out your copyright notice in the Description page of Project Settings.


#include "Card.h"

#include "BaseCharacter.h"
#include "PlayField.h"


// Sets default values
ACard::ACard()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ACard::ResetTurnAttributes()
{
	//TODO implement
	AttackCount = 1;
	EffectActivationCount = 1;
	ChangePositionCount = 1;
	AppliedAttributes &= ~StaticCast<int32>(ECardStatuses::SummonTurn | ECardStatuses::PosChanged | ECardStatuses::FlipSummonTurn | ECardStatuses::SetTurn);
}

TArray<UCardEffect*> ACard::GetTriggerableEffects(const EEffectTrigger Trigger)
{
	TArray<UCardEffect*> res;
	if(Statuses & static_cast<int32>(ECardStatuses::EffectDisabled) || EffectActivationCount > 0)
	{
		return res;
	}
	for (auto Effect : AvailableEffects)
	{
		if(Effect->Trigger == Trigger)
		{
			res.Add(Effect);
		}
	}
	return res;
}

TArray<ECardActions> ACard::GetAllUsableActions(ABaseCharacter* Player)
{
	TArray<ECardActions> result;
	const bool isOwner = CurrentState.Controller == Player;
	const bool isFaceUp = CurrentState.Position != ECardPosition::Set;
	/*
	Inspect 
	Summon	
	Set
	Activate
	Flip
	ChangePosition	
	Attack
	Tribute
	 */
	switch (CurrentState.Location)
	{
	case ELocations::Deck: //probably irrelevant case, I don't think I'll let cards do things from deck
		break;
		
	case ELocations::Hand:
		result.Add(ECardActions::Inspect);
		if(CanBeSummoned())
			result.Add(ECardActions::Summon);
		if(CanBeSet())
			result.Add(ECardActions::Set);
		if(CanActivate())
			result.Add(ECardActions::Activate);
		break;
		
	case ELocations::MZone:
		if(isOwner || isFaceUp)
			result.Add(ECardActions::Inspect);
		if(CanAttack())
			result.Add(ECardActions::Attack);
		if(CanBeTributed())
			result.Add(ECardActions::Tribute);
		if(CanFlip())
			result.Add(ECardActions::Flip);
		if(CanChangePosition())
			result.Add(ECardActions::ChangePosition);
		if(CanActivate())
			result.Add(ECardActions::Activate);
		break;
		
	case ELocations::CCZone:
		if(isOwner || isFaceUp)
			result.Add(ECardActions::Inspect);
		if(CanActivate())
			result.Add(ECardActions::Activate);
		break;
		
	case ELocations::Grave:
		result.Add(ECardActions::Inspect);
		if(CanActivate())
			result.Add(ECardActions::Activate);
		break;
		
	default: ;
	}
	return result;
}

void ACard::IncreaseUseCount(const int32 Count)
{
	EffectActivationCount += Count;
}

// Called when the game starts or when spawned
void ACard::BeginPlay()
{
	Super::BeginPlay();
	CurrentState.Controller = GetOwner();
}

void ACard::RegisterEffect(UCardEffect* Effect)
{
	for (auto effect : AvailableEffects)
	{
		if(effect->Type == EEffectType::SelfEffect)
		{
			//TODO applies things like piercing, summon restrictions, etc
			effect->Execute();
		}
	}
}

// Called every frame
void ACard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ACard::CanAttack() const
{
	return IsType(ECardTypes::Monster) && AttackCount && !HasAttribute(ECardAttributes::CannotAttack) && Field->GetTurnInfo().Phase == EPhases::Battle;
}

bool ACard::CanBeSummoned() const
{
	ABaseCharacter* owner = Cast<ABaseCharacter>(CurrentState.Controller);
	const int32 mana = Field->GetInfo(owner).Mana;
	return IsType(ECardTypes::Monster) && !HasAttribute(ECardAttributes::CannotSummon) && CurrentState.Cost <= mana && Field->GetNumEmptyMonsterZones(owner) && Field->GetTurnInfo().Phase == EPhases::Main;
}

bool ACard::CanBeSet() const
{
	ABaseCharacter* owner = Cast<ABaseCharacter>(CurrentState.Controller);
	const int32 mana = Field->GetInfo(owner).Mana;
	if(IsType(ECardTypes::Monster))
	{
		return CanBeSummoned();
	}
	if(IsType(ECardTypes::Charge))
	{
		return Field->GetNumEmptyCCZones(owner) && CurrentState.Cost <= mana && Field->GetTurnInfo().Phase == EPhases::Main;
	}
	return false;
}

bool ACard::CanActivate() const
{//Monster effects, CC slow effects, CC instant effects
	bool activate = false;
	for (const auto effect : AvailableEffects)
	{
		if(effect->IsActivatable())
		{
			activate = true;
			break;
		}
	}
	if(!activate) return false;
	
	if(IsType(ECardTypes::Monster) && IsType(ECardTypes::Effect))
	{
		return EffectActivationCount && !HasAttribute(ECardAttributes::CannotActivate);
	}
	if(IsType(ECardTypes::Charge))
	{
		ABaseCharacter* owner = Cast<ABaseCharacter>(CurrentState.Controller);
		const int32 mana = Field->GetInfo(owner).Mana;
		if(IsType(ECardTypes::Instant))
		{
			if(CurrentState.Location == ELocations::CCZone) //on field = mana cost already paid
				return !HasAttribute(ECardAttributes::CannotActivate);
			if(CurrentState.Location == ELocations::Hand) //in hand = cost + space
				return Field->GetNumEmptyCCZones(owner) && mana >= CurrentState.Cost;
		}
		//Slow charge
		return CurrentState.Location == ELocations::CCZone && CurrentState.Charge >= CurrentState.Cost && !HasAttribute(ECardAttributes::CannotActivate) && EffectActivationCount;
	}
	return false;
}

bool ACard::CanFlip() const
{
	const bool setMonster = IsType(ECardTypes::Monster) && CurrentState.Location == ELocations::MZone && CurrentState.Position == ECardPosition::Set;
	return setMonster && !HasStatus(ECardStatuses::FlipSummonTurn) && Field->GetTurnInfo().Phase == EPhases::Main;
}

bool ACard::CanChangePosition() const
{
	const bool faceupMonster = IsType(ECardTypes::Monster) && CurrentState.Location == ELocations::MZone && CurrentState.Position != ECardPosition::Set;
	return faceupMonster && ChangePositionCount && !HasStatus(ECardStatuses::SummonTurn) && Field->GetTurnInfo().Phase == EPhases::Main;
}

bool ACard::CanBeTributed() const
{
	const bool faceupMonster = IsType(ECardTypes::Monster) && CurrentState.Location == ELocations::MZone && CurrentState.Position != ECardPosition::Set;
	return faceupMonster && !HasAttribute(ECardAttributes::CannotTribute) && Field->GetTurnInfo().Phase == EPhases::Main;
}

bool ACard::HasAttribute(ECardAttributes Attribute) const
{
	return AppliedAttributes & StaticCast<int32>(Attribute);
}

bool ACard::HasStatus(ECardStatuses Status) const
{
	return Statuses & StaticCast<int32>(Status);
}

bool ACard::IsType(ECardTypes Type) const
{
	return CurrentState.Type & StaticCast<int32>(Type);
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "CardEffect.h"

bool UCardEffect::IsActivatable_Implementation()
{
	return Condition() && Cost() && Target();
}

bool UCardEffect::Condition_Implementation()
{
	return true;
}

bool UCardEffect::Target_Implementation()
{
	return true;
}

bool UCardEffect::Cost_Implementation()
{
	return true;
}

bool UCardEffect::FilterTarget_Implementation(ACard* Target)
{
	return true;
}

/**
 * Executes the effect. Implement custom logic before the Super call. Calls OnExecuteFinishedDelegate and unbinds itself afterwards.
 * @return Returns Finished unless specified otherwise
 */
EExecuteResult UCardEffect::Execute_Implementation()
{
	OnExecuteFinishedDelegate.Execute();
	OnExecuteFinishedDelegate.Unbind();
	return EExecuteResult::Finished;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/GE/Enemy_Invulnerable.h"

UEnemy_Invulnerable::UEnemy_Invulnerable()
{
	FInheritedTagContainer InheritedTags;
	InheritedTags.Added.AddTag(GASTAG::State_Invulnerable);
	//InheritedTags.Granted.AddTag(GASTAG::State_Invulnerable.GetTag());
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "BoidManager.h"

// Sets default values
ABoidManager::ABoidManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABoidManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABoidManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ABoidManager::GetMinSpeed()
{
	return MinSpeed;
}

float ABoidManager::GetMaxSpeed()
{
	return MaxSpeed;
}

float ABoidManager::GetLocalFlockRadius()
{
	return LocalFlockRadius;
}

float ABoidManager::GetSeparationRadius()
{
	return SeparationRadius;
}

float ABoidManager::GetSeparationStrength()
{
	return SeparationStrength;
}

float ABoidManager::GetAlignRadius()
{
	return AlignRadius;
}

float ABoidManager::GetAlignStrength()
{
	return AlignStrength;
}

float ABoidManager::GetCohesionRadius()
{
	return CohesionRadius;
}

float ABoidManager::GetCohesionStrength()
{
	return CohesionStrength;
}


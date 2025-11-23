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


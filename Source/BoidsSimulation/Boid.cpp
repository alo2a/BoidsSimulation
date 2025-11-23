// Fill out your copyright notice in the Description page of Project Settings.


#include "Boid.h"

// Sets default values
ABoid::ABoid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABoid::BeginPlay()
{
	Super::BeginPlay();

	Velocity = GetActorForwardVector();
	Velocity = Velocity.GetSafeNormal();
	Velocity *= 500;
	
}

// Called every frame
void ABoid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Steer(DeltaTime);
	StayInBoundries();
}

void ABoid::Steer(float DeltaTime)
{
	FVector Acceleration = FVector::ZeroVector;
	Velocity += Acceleration * DeltaTime;
	SetActorLocation(GetActorLocation()+(Velocity * DeltaTime));
	
}

void ABoid::StayInBoundries()
{
	FVector currentLocation = GetActorLocation();
	if (currentLocation.X < -4000)
	{
		currentLocation.X = 4000;
	}
	else if (currentLocation.X > 4000)
	{
		currentLocation.X = -4000;
	}
	if (currentLocation.Y < -4000)
	{
		currentLocation.Y = 4000;
	}
	else if (currentLocation.Y > 4000)
	{
		currentLocation.Y = -4000;
	}
	if (currentLocation.Z < -4000)
	{
		currentLocation.Z = 4000;
	}
	else if (currentLocation.Z > 4000)
	{
		currentLocation.Z = -4000;
	}

	SetActorLocation(currentLocation);
}


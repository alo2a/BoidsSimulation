// Fill out your copyright notice in the Description page of Project Settings.


#include "Boid.h"

#include "Kismet/GameplayStatics.h"

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

	BoidManager = Cast<ABoidManager>(UGameplayStatics::GetActorOfClass(GetWorld(),ABoidManager::StaticClass()));

	LocalFlockArea = GetComponentByClass<USphereComponent>();

	if (LocalFlockArea != nullptr)
	{
		LocalFlockArea -> SetSphereRadius(BoidManager -> GetLocalFlockRadius());
	}
	
	Velocity = GetActorForwardVector();
	Velocity = Velocity.GetSafeNormal();
	Velocity *= FMath::RandRange(BoidManager->GetMinSpeed(),BoidManager->GetMaxSpeed());
	
}

// Called every frame
void ABoid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Steer(DeltaTime);
	StayInBoundaries();
}

void ABoid::Steer(float DeltaTime)
{
	FVector Acceleration = FVector::ZeroVector;

	TArray<AActor*> LocalFlock;
	LocalFlockArea->GetOverlappingActors(LocalFlock,ABoid::StaticClass());

	Acceleration += AvoidObstacles();
	Acceleration += Separate(LocalFlock);
	Acceleration += Align(LocalFlock);
	Acceleration += Cohesion(LocalFlock);
	
	Velocity += Acceleration * DeltaTime;
	Velocity = Velocity.GetClampedToSize(BoidManager -> GetMinSpeed(),BoidManager -> GetMaxSpeed());
	SetActorLocation(GetActorLocation()+(Velocity * DeltaTime));
	SetActorRotation(Velocity.ToOrientationQuat());
}

void ABoid::StayInBoundaries()
{
	FVector currentLocation = GetActorLocation();
	if (currentLocation.X < -1000)
	{
		currentLocation.X = 1000;
	}
	else if (currentLocation.X > 1000)
	{
		currentLocation.X = -1000;
	}
	if (currentLocation.Y < -1000)
	{
		currentLocation.Y = 1000;
	}
	else if (currentLocation.Y > 1000)
	{
		currentLocation.Y = -1000;
	}
	if (currentLocation.Z < -400)
	{
		currentLocation.Z = 400;
	}
	else if (currentLocation.Z > 400)
	{
		currentLocation.Z = -400;
	}

	SetActorLocation(currentLocation);
}

FVector ABoid::Separate(TArray<AActor*> LocalFlock)
{
	FVector Steering = FVector::ZeroVector;
	FVector SeparationDirection= FVector::ZeroVector;
	
	int FlockCount = 0;

	for (AActor * OtherBoid : LocalFlock)
	{
		if (OtherBoid != nullptr && OtherBoid != this)
		{
			float DistanceToOtherBoid = FVector::Dist(GetActorLocation(), OtherBoid->GetActorLocation());

			if (DistanceToOtherBoid > BoidManager -> GetSeparationRadius())
			{
				continue;
			}

			SeparationDirection = GetActorLocation() - OtherBoid->GetActorLocation();
			SeparationDirection = SeparationDirection.GetSafeNormal();

			Steering += SeparationDirection;
			FlockCount++;
		}

		if (FlockCount > 0)
		{
			Steering /= FlockCount;
			Steering = Steering.GetSafeNormal();
			Steering -= Velocity.GetSafeNormal();
			Steering *= BoidManager -> GetSeparationStrength();

			return Steering;
		}
	}
	
	return  FVector();
}

FVector ABoid::Align(TArray<AActor*> LocalFlock)
{
	FVector Steering = FVector::ZeroVector;
	
	int FlockCount = 0;

	for (AActor * OtherActor : LocalFlock)
	{
		if (OtherActor != nullptr && OtherActor != this)
		{

			ABoid * OtherBoid = Cast<ABoid>(OtherActor);
			float DistanceToOtherBoid = FVector::Dist(GetActorLocation(), OtherActor->GetActorLocation());

			if (DistanceToOtherBoid > BoidManager -> GetAlignRadius())
			{
				continue;
			}

			Steering += OtherBoid->Velocity.GetSafeNormal();
			Steering = Steering.GetSafeNormal();
			
			FlockCount++;
		}

		if (FlockCount > 0)
		{
			Steering /= FlockCount;
			Steering = Steering.GetSafeNormal();
			Steering -= Velocity.GetSafeNormal();
			Steering *= BoidManager -> GetAlignStrength();

			return Steering;
		}
	}
	
	return  FVector();
}

FVector ABoid::Cohesion(TArray<AActor*> LocalFlock)
{
	FVector Steering = FVector::ZeroVector;
	FVector AveragePosition;
	int FlockCount = 0;

	for (AActor * OtherActor : LocalFlock)
	{
		if (OtherActor != nullptr && OtherActor != this)
		{

			float DistanceToOtherBoid = FVector::Distance(GetActorLocation(), OtherActor->GetActorLocation());

			if (DistanceToOtherBoid > BoidManager -> GetCohesionRadius())
			{
				continue;
			}
			
			AveragePosition += OtherActor->GetActorLocation();
			
			FlockCount++;
		}

		if (FlockCount > 0)
		{
			AveragePosition/=FlockCount;
			
			Steering = AveragePosition-GetActorLocation();
			Steering = Steering.GetSafeNormal();
			Steering -= Velocity.GetSafeNormal();
			Steering *= BoidManager -> GetCohesionStrength();

			return Steering;
		}
	}
	
	return  FVector();
}

FVector ABoid::AvoidObstacles()
{
	FVector Steering = FVector::ZeroVector;
	FVector Forward = Velocity.GetSafeNormal();
	FVector CurrentLocation = GetActorLocation();
	float AvoidanceRadius = BoidManager->GetObstacleAvoidanceRadius();
    
	FVector Right = FVector::CrossProduct(FVector::UpVector, Forward).GetSafeNormal();
	FVector Up = FVector::CrossProduct(Forward, Right).GetSafeNormal();
    
	TArray<FVector> RayDirections;
	RayDirections.Add(Forward);
	RayDirections.Add((Forward + Right * 0.5f).GetSafeNormal());
	RayDirections.Add((Forward - Right * 0.5f).GetSafeNormal());
	RayDirections.Add((Forward + Up * 0.5f).GetSafeNormal());
	RayDirections.Add((Forward - Up * 0.5f).GetSafeNormal());
    
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
    
	FVector TotalAvoidance = FVector::ZeroVector;
	int ObstacleCount = 0;
    
	for (const FVector& Direction : RayDirections)
	{
		FHitResult HitResult;
		FVector RayEnd = CurrentLocation + (Direction * AvoidanceRadius);
        
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, CurrentLocation, RayEnd, ECC_Visibility, QueryParams);
        
		if (bHit && Cast<ABoid>(HitResult.GetActor()) == nullptr)
		{
			FVector AwayFromObstacle = (CurrentLocation - HitResult.ImpactPoint).GetSafeNormal();
			float Weight = 1.0f - (HitResult.Distance / AvoidanceRadius);
            
			TotalAvoidance += AwayFromObstacle * Weight;
			ObstacleCount++;
		}
	}
    
	if (ObstacleCount > 0)
	{
		TotalAvoidance /= ObstacleCount;
		Steering = TotalAvoidance.GetSafeNormal() - Velocity.GetSafeNormal();
		Steering *= BoidManager->GetObstacleAvoidanceStrength();
	}
    
	return Steering;
}
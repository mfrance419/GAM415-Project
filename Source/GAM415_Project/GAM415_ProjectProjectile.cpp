// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAM415_ProjectProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "perlinProcTerrain.h"

AGAM415_ProjectProjectile::AGAM415_ProjectProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AGAM415_ProjectProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	ballMesh = CreateDefaultSubobject<UStaticMeshComponent>("Ball Mesh");

	// Set as root component
	RootComponent = CollisionComp;
	//Attach Ball Mesh to Root Component
	ballMesh->SetupAttachment(CollisionComp);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}


void AGAM415_ProjectProjectile::BeginPlay()
{
	Super::BeginPlay();
	//Set Random Color for the projectile
	randColor = FLinearColor(UKismetMathLibrary::RandomFloatInRange(0.f, 1.f), UKismetMathLibrary::RandomFloatInRange(0.f, 1.f), UKismetMathLibrary::RandomFloatInRange(0.f, 1.f), 1.f);
	
	dmiMat = UMaterialInstanceDynamic::Create(projMat, this);
	//Set the material of the ball mesh to the dynamic material instance
	ballMesh->SetMaterial(0, dmiMat);

	dmiMat->SetVectorParameterValue("ProjColor", randColor);
}

void AGAM415_ProjectProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		Destroy();
	}

	if(OtherActor != nullptr)
	{
		//Checks if the Color Particle is valid
		if(colorP)
		{
			//Spawn Particle at the location of the hit with a random color matching the color to the projectile
			UNiagaraComponent* particleComp = UNiagaraFunctionLibrary::SpawnSystemAttached(colorP, HitComp, NAME_None, FVector(-20.f, 0.f, 0.f), FRotator(0.f, 0.f, 0.f), EAttachLocation::KeepRelativeOffset, true);
			particleComp->SetNiagaraVariableLinearColor("RandomColor", randColor);
			ballMesh->DestroyComponent();
			CollisionComp->BodyInstance.SetCollisionProfileName("NoCollision");
		}


		//Generate random numbers for the frame of the decal
		float frameNum = UKismetMathLibrary::RandomFloatInRange(0.f, 3.f);
		//Spawn a decal at the location of the hit with a random size and rotation
		auto Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), baseMat, FVector(UKismetMathLibrary::RandomFloatInRange(20.f, 40.f)), Hit.Location, Hit.Normal.Rotation(), 0.f);
		auto MatInstance = Decal->CreateDynamicMaterialInstance();
		
		MatInstance->SetVectorParameterValue("Color", randColor);
		MatInstance->SetScalarParameterValue("Frame", frameNum);

		APerlinProcTerrain* procTerrain = Cast<APerlinProcTerrain>(OtherActor);

		if (procTerrain)
		{
			procTerrain->AlterMesh(Hit.ImpactPoint);
		}
	}
}


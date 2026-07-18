// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeDMIMod.h"
#include "GAM415_ProjectCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
ACubeDMIMod::ACubeDMIMod()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	boxComp = CreateDefaultSubobject<UBoxComponent>("Box Component");
	cubeMesh = CreateDefaultSubobject<UStaticMeshComponent>("Cube Mesh");

	RootComponent = boxComp;
	cubeMesh->SetupAttachment(boxComp);

}

// Called when the game starts or when spawned
void ACubeDMIMod::BeginPlay()
{
	Super::BeginPlay();
	boxComp->OnComponentBeginOverlap.AddDynamic(this, &ACubeDMIMod::OnOverLapBegin);

	if(baseMat)
	{
		dmiMat = UMaterialInstanceDynamic::Create(baseMat, this);
	}

	if(cubeMesh)
	{
		cubeMesh->SetMaterial(0, dmiMat);
	}
}

// Called every frame
void ACubeDMIMod::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACubeDMIMod::OnOverLapBegin(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	AGAM415_ProjectCharacter* overlappedActor  = Cast<AGAM415_ProjectCharacter>(OtherActor);
	//Checks if the overlapped actor is valid
	if (overlappedActor)
	{
		//Randomly generates a color for the cube
		float ranNumX = UKismetMathLibrary::RandomFloatInRange(0.0f, 1.f);
		float ranNumY = UKismetMathLibrary::RandomFloatInRange(0.0f, 1.f);
		float ranNumZ = UKismetMathLibrary::RandomFloatInRange(0.0f, 1.f);

		FLinearColor randColor = FLinearColor(ranNumX, ranNumY, ranNumZ, 1.f);

		if (colorP)
		{
			//Spawn particle where overlapped actor is with color matching the cube.
			UNiagaraComponent* particleComp = UNiagaraFunctionLibrary::SpawnSystemAttached(colorP, OtherComp, NAME_None, FVector(0.f), FRotator(0.f), EAttachLocation::KeepRelativeOffset, true);
			particleComp->SetNiagaraVariableLinearColor("RandColor", randColor);
		}

		if (dmiMat)
		{
			dmiMat->SetVectorParameterValue("Color", randColor);
			dmiMat->SetScalarParameterValue("Darkness", ranNumX);
			
		}
	}
}

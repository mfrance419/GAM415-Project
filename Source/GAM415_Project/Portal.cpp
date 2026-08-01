// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"
#include "GAM415_ProjectCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APortal::APortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//Creating subobjects for the portal actor
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	BoxComp = CreateDefaultSubobject<UBoxComponent>("Box Comp");
	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>("Capture");
	ArrowMesh = CreateDefaultSubobject<UArrowComponent>("Arrow");

	RootComponent = BoxComp;
	//Attach Subobjects
	Mesh->SetupAttachment(BoxComp);
	SceneCapture->SetupAttachment(Mesh);
	ArrowMesh->SetupAttachment(RootComponent);

	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
}

// Called when the game starts or when spawned
void APortal::BeginPlay()
{
	Super::BeginPlay();
	//Bind the overlap event to the OnOverlapBegin function and hide the mesh
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &APortal::OnOverlapBegin);
	Mesh->SetHiddenInSceneCapture(true);

	//Verify the material is valid and set it to the mesh
	if (mat)
	{
		Mesh->SetMaterial(0, mat);
	}
	
}

// Called every frame
void APortal::Tick(float DeltaTime)
{
	//Update portals every frame
	Super::Tick(DeltaTime);
	UpdatePortals();
}

void APortal::OnOverlapBegin(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	AGAM415_ProjectCharacter* playerChar = Cast<AGAM415_ProjectCharacter>(OtherActor);
	//verify the player is valid and the other portal is valid
	if (playerChar)
	{
		if (OtherPortal)
		{
			//Check if the player is already teleporting to avoid infinite loop
			if (!playerChar->isTeleporting)
			{
				//Grab the other portals location and set the player to that location
				playerChar->isTeleporting = true;
				FVector loc = OtherPortal->ArrowMesh->GetComponentLocation();
				playerChar->SetActorLocation(loc);

				FTimerHandle TimerHandle;
				FTimerDelegate TimerDelegate;
				TimerDelegate.BindUFunction(this, "SetBool", playerChar);
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 1, false);
			}
		}
	}
}

void APortal::SetBool(AGAM415_ProjectCharacter *playerChar)
{
	if (playerChar)
	{
		playerChar->isTeleporting = false;
	}
}

void APortal::UpdatePortals()
{
	FVector Location = this->GetActorLocation() - OtherPortal->GetActorLocation();
	FVector camLocation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetTransformComponent()->GetComponentLocation();
	FRotator camRotation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetTransformComponent()->GetComponentRotation();
	FVector CombinedLocation = camLocation + Location;

	SceneCapture->SetWorldLocationAndRotation(CombinedLocation, camRotation);

}

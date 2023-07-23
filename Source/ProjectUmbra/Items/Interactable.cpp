// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/Items/Interactable.h"
#include "ProjectUmbra/Characters/PlayerCharacter.h"
#include "Interactable.h"
#include "Components/SphereComponent.h"

// Sets default values
AInteractable::AInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mc_USphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	mc_USphere->SetupAttachment(GetRootComponent());
	mc_USphere->OnComponentBeginOverlap.AddDynamic(this, &AInteractable::OnBeginOverlap);
	mc_USphere->OnComponentEndOverlap.AddDynamic(this, &AInteractable::OnEndOverlap);
}

// Called when the game starts or when spawned
void AInteractable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractable::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// APlayerCharacter* pPlayer = Cast<APlayerCharacter>(OtherActor);
	// if (pPlayer)
	// {
	// 	pPlayer->RegisterInteractable(this);
	// }
}

void AInteractable::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// APlayerCharacter* pPlayer = Cast<APlayerCharacter>(OtherActor);
	// if (pPlayer)
	// {
	// 	pPlayer->UnregisterInteractable(this);
	// }
}

void AInteractable::Interact(ACharacter* pCharacter)
{
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/Items/Deathzone.h"
#include "ProjectUmbra/System/UmbraGameMode.h"
#include "Components/BoxComponent.h"
#include "ProjectUmbra/Components/HealthComponetCpp.h"
#include "ProjectUmbra/Characters/PlayerCharacter.h"

// Sets default values
ADeathzone::ADeathzone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mc_UBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	mc_UBox->SetupAttachment(GetRootComponent());
	mc_UBox->OnComponentBeginOverlap.AddDynamic(this, &ADeathzone::OnBeginOverlap);
}

// Called when the game starts or when spawned
void ADeathzone::BeginPlay()
{
	Super::BeginPlay();
	AUmbraGameMode* gamemode = Cast<AUmbraGameMode>(GetWorld()->GetAuthGameMode());
	if (gamemode)
	{
		m_pGM = gamemode;
	}
	else {
		#ifdef WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Error in Deathzone: Could not cast to Umbra gamemode from deathzone"));
		#endif 
		UE_LOG(LogTemp, Error, TEXT("Error in deathzone: Could not cast to Umbra gamemode from death zone"));
	}
}

void ADeathzone::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	mc_UBox->SetBoxExtent(m_vDeathzoneExtent, true);
}

// Called every frame
void ADeathzone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADeathzone::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* pPlayer = Cast<APlayerCharacter>(OtherActor);
	if (pPlayer)
	{
		//pPlayer->mc_UHealth->ResetHealth();
		if (m_pGM)
		{
			m_pGM->RespawnCharacter(pPlayer,true);
		}
		return;
	}
	ABaseEnemy* aBaseEnemy = Cast<ABaseEnemy>(OtherActor);
	if (aBaseEnemy)
	{
		//pPlayer->mc_UHealth->ResetHealth();
		aBaseEnemy->m_bDeathByDeathZone = true;
		aBaseEnemy->mc_pHealthComp->TakeDamage(aBaseEnemy->mc_pHealthComp->GetHealth());
	}
}


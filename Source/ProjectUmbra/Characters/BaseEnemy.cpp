// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/Characters/BaseEnemy.h"
#include "ProjectUmbra/Components/HealthComponetCpp.h"
#include "ProjectUmbra/Items/HealthPack.h"
#include "ProjectUmbra/System/ProbabilityManager.h"
#include "ProjectUmbra/System/ProjectUmbraGameInstance.h"

void ABaseEnemy::OnDeathCallback()
{
	OnEnemyDeath();
	DropObject();
	Destroy();
}

// Sets default values
ABaseEnemy::ABaseEnemy(): m_eEnemyType(EEnemyType::Normal), m_bDeathByDeathZone(false)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mc_pHealthComp = CreateDefaultSubobject<UHealthComponentCpp>(TEXT("Health Component"));
}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	mc_pHealthComp->OnDeath.AddDynamic(this, &ABaseEnemy::OnDeathCallback);
	m_vCenterPatrolLoc = GetActorLocation();
	OnTakeAnyDamage.AddUniqueDynamic(this, &ABaseEnemy::OnAnyDamage);
}

// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseEnemy::ShootBullet() 
{
	OnEnemyShoot();
}
void ABaseEnemy::DropObject()
{
	if (!m_bDeathByDeathZone)
	{
		if (m_bDropsHealth && GetOwner()->GetGameInstance<UProjectUmbraGameInstance>()->GetProbabilityManager()->CanSpawnHealthDrop())
		{
			if (GetOwner()->GetGameInstance<UProjectUmbraGameInstance>()->GetProbabilityManager()->CanSpawnHealthDrop())
			{
				AHealthPack* ADrop = GetWorld()->SpawnActor<AHealthPack>(m_THealthPackClass, GetOwner()->GetActorLocation(), FRotator::ZeroRotator);
				GetGameInstance<UProjectUmbraGameInstance>()->GetProbabilityManager()->RegisterHealthDrop(ADrop);
			}
		}
	}
}

void ABaseEnemy::OnAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	mc_pHealthComp->TakeDamage(Damage);
	OnEnemyDamaged();
}

void ABaseEnemy::KnockBack(FVector _VKnockBackDirection, float _fKnockBackSpeed, float _fKnockBackDistance,
	float _fStunDuration)
{
}


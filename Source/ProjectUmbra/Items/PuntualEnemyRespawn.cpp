// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/Items/PuntualEnemyRespawn.h"
#include "ProjectUmbra/Components/EnemyDeathDetectorComponent.h"
#include "ProjectUmbra/Characters/BaseEnemy.h"
#include "ProjectUmbra/Characters/NormalEnemy.h"
#include "ProjectUmbra/Characters/SniperEnemy.h"
#include "ProjectUmbra/Characters/ExplosiveEnemy.h"
#include "ProjectUmbra/Characters/HaloShooterEnemy.h"
#include "ProjectUmbra/Components/HealthComponetCpp.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectUmbra/Items/Waypoint.h"

// Sets default values
APuntualEnemyRespawn::APuntualEnemyRespawn():m_TWaypoints(TArray<FWaypointData>()), m_iNumOfWaves(1), m_iAmmountOfEnemies(0), m_iActualWave(0)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mc_UDeathDetector = CreateDefaultSubobject<UEnemyDeathDetectorComponent>(TEXT("Death detector"));
	mc_UDeathDetector->OnAllEnemiesDeath.AddDynamic(this, &APuntualEnemyRespawn::OnAllEnemiesDeathRespawn);

}

// Called when the game starts or when spawned
void APuntualEnemyRespawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APuntualEnemyRespawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
//Called when enemies registered in death detector component are dead
void APuntualEnemyRespawn::OnAllEnemiesDeathRespawn()
{
	if (m_TWaypoints.Num() == 0)
	{
		#ifdef WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Error in puntual enemy respawner: YOU MUST PROVIDE WAYPOINT DATA"));
		#endif 
		UE_LOG(LogTemp, Error, TEXT("Error in puntual enemy respawner: YOU MUST PROVIDE WAYPOINT DATA"));
	}
	else {
		int iDiference = 0;
		for (int i = 0; i < m_TWaypoints.Num(); i++)
		{
			FWaypointData& Data = m_TWaypoints[i];
			for (int j = 0; j < Data.m_TEnemyTypes.Num(); j++)
			{
				if (Data.m_TEnemyRefs[j].m_TEnemyRefs.Num() != Data.m_TEnemyAmounts[j])
				{
					if (Data.m_TEnemyRefs[j].m_TEnemyRefs.Num() > Data.m_TEnemyAmounts[j])
					{
						#ifdef WITH_EDITOR
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Error in puntual enemy respawner: YOU HAVE MORE REFERENCES THAN ENEMY AMOUNT, CHANGE THE AMOUNT FOR THIS TYPE OR THE REFERENCES"));
						#endif 
						UE_LOG(LogTemp, Error, TEXT("Error in puntual enemy respawner: YOU HAVE MORE REFERENCES THAN ENEMY AMOUNT, CHANGE THE AMOUNT FOR THIS TYPE OR THE REFERENCES"));
					}
					else {
						iDiference = Data.m_TEnemyAmounts[j] - Data.m_TEnemyRefs[j].m_TEnemyRefs.Num();
						if (Data.m_pWaypoint)
						{
							for (int k = 0; k < iDiference; k++)
							{
								FActorSpawnParameters Params = FActorSpawnParameters();
								Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
								ABaseEnemy* pEnemy = GetWorld()->SpawnActor<ABaseEnemy>(GetEnemyClass(Data.m_TEnemyTypes[j]), Data.m_pWaypoint->GetActorLocation(), FRotator(0.f, 0.f, 0.f), Params);
								SetRandomStadistics(pEnemy, pEnemy->m_eEnemyType, i);
								++m_iAmmountOfEnemies;
								Data.m_TEnemyRefs[j].m_TEnemyRefs.Add(pEnemy);
								pEnemy->mc_pHealthComp->OnDeath.AddDynamic(this, &APuntualEnemyRespawn::OnEnemyDeath);
							}
						}
						else {
							#ifdef WITH_EDITOR
							GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Error in puntual enemy respawner:  YOU DID NOT PROVIDE A WAYPOINT FOR THIS WAYPOINT DATA"));
							#endif 
							UE_LOG(LogTemp, Error, TEXT("Error in puntual enemy respawner:  YOU DID NOT PROVIDE A WAYPOINT FOR THIS WAYPOINT DATA"));
							break;
						}
					}
				}
			}
		}
	}
}
void APuntualEnemyRespawn::CleanEnemyReferences()
{
	//for (int i = 0; i < m_TWaypoints.Num(); ++i)
	//{
	//	for (int j = 0; j < m_TWaypoints[i].m_TEnemyRefs.Num(); ++j)
	//	{
	//		for (int k = m_TWaypoints[i].m_TEnemyRefs[j].m_TEnemyRefs.Num()-1; k>=0; ++k)
	//		{
	//			if (!m_TWaypoints[i].m_TEnemyRefs[j].m_TEnemyRefs[k])
	//			{
	//				m_TWaypoints[i].m_TEnemyRefs[j].m_TEnemyRefs.RemoveAt(k);
	//			}
	//		}
	//	}
	//}
}
//Called for each enemy death
void APuntualEnemyRespawn::OnEnemyDeath()
{
	--m_iAmmountOfEnemies;
	UE_LOG(LogTemp, Error, TEXT("ENEMY OF PUNTUAL SPAWNER KILLED. REMAIN: %d"),m_iAmmountOfEnemies);
	if (m_iAmmountOfEnemies <= 0)
	{
		++m_iActualWave;
		if (m_iActualWave != m_iNumOfWaves) {
			m_iAmmountOfEnemies = 0;
			UE_LOG(LogTemp, Error, TEXT("NEW WAVE RESPAWNED"));
			OnAllEnemiesDeathRespawn();
		}
	}
}

UClass* APuntualEnemyRespawn::GetEnemyClass(EEnemyType _EnemyType)
{
	switch (_EnemyType)
	{
	case EEnemyType::Normal:
		return m_TNormalClass;
		break;
	case EEnemyType::Kamikaze:
		return m_TKamikazeClass;
		break;
	case EEnemyType::Circular:
		return m_THaloClass;
		break;
	case EEnemyType::Sniper:
		return m_TSniperClass;
		break;
	case EEnemyType::Shotgun:
		return m_TShotgunClass;
		break;
	case EEnemyType::Burst:
		return m_TBurstClass;
		break;
	default:
		return m_TNormalClass;
		break;
	}
}

void APuntualEnemyRespawn::SetRandomStadistics(ABaseEnemy* _Enemy, EEnemyType _EnemyType, int _WaypointDataIndex)
{
	ANormalEnemy* pNormal = nullptr;
	AExplosiveEnemy* pExplosive = nullptr;
	AHaloShooterEnemy* pHalo = nullptr;
	ASniperEnemy* pSniper = nullptr;
	switch (_EnemyType)
	{
	case EEnemyType::Normal:
		pNormal = Cast<ANormalEnemy>(_Enemy);
		if (pNormal)
		{
			pNormal->m_fFireRate = (UKismetMathLibrary::RandomFloatInRange(0.5f, 1.f));
			pNormal->m_eShootingType = EShootingType::Normal;
			pNormal->m_iBurstNum = 1;
			pNormal->m_iBullNum = 1;
			pNormal->m_fRallyProb = UKismetMathLibrary::RandomFloatInRange(0.f, 1.f);
		}
		break;
	case EEnemyType::Kamikaze:
		pExplosive = Cast<AExplosiveEnemy>(_Enemy);
		if (pExplosive)
		{
			pExplosive->m_fExplosionRange = UKismetMathLibrary::RandomFloatInRange(300.f, 500.f);
			pExplosive->m_fTimeToExplode = UKismetMathLibrary::RandomFloatInRange(1.f, 2.f);
		}
		break;
	case EEnemyType::Circular:
		pHalo = Cast<AHaloShooterEnemy>(_Enemy);
		if (pHalo)
		{
			pHalo->m_fShootDelay = UKismetMathLibrary::RandomFloatInRange(2.f, 4.f);
			pHalo->m_iBulletAmount = UKismetMathLibrary::RandomIntegerInRange(6, 10);
			pHalo->m_fIdleTime = UKismetMathLibrary::RandomFloatInRange(2.f, 4.f);
		}
		break;
	case EEnemyType::Sniper:
		pSniper = Cast<ASniperEnemy>(_Enemy);
		if (pSniper)
		{
			pSniper->m_fCooldown = UKismetMathLibrary::RandomFloatInRange(3.f, 6.f);
			pSniper->m_fChargeTime = UKismetMathLibrary::RandomFloatInRange(3.f, 6.f);
			pSniper->m_fTimeUntilTeleport = UKismetMathLibrary::RandomFloatInRange(1.f, 3.f);
			pSniper->m_iMaxTeleportCount = UKismetMathLibrary::RandomIntegerInRange(3, 6);
			pSniper->m_TWaypoints = m_TWaypoints[_WaypointDataIndex].m_pSniperWaypoints;
		}
		break;
	case EEnemyType::Shotgun:
		break;
	case EEnemyType::Burst:
		break;
	}
}



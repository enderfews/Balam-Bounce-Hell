// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/Characters/EnemyGenerator.h"

#include "FMODAudioComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectUmbra/Widgets/HealthBar.h"
#include "ProjectUmbra/Characters/PlayerCharacter.h"
#include "ProjectUmbra/Components/HealthComponetCpp.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ProjectUmbra/Characters/NormalEnemy.h"
#include "ProjectUmbra/Characters/SniperEnemy.h"
#include "ProjectUmbra/Characters/ExplosiveEnemy.h"
#include "ProjectUmbra/Characters/HaloShooterEnemy.h"
#include "Engine/World.h"
#include "ProjectUmbra/Items/Waypoint.h"
#include "ProjectUmbra/System/ProjectUmbraGameInstance.h"
#include "ProjectUmbra/System/SoundManager.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraCommon.h"
#include "NiagaraComponent.h"

void AEnemyGenerator::DestroyEnemies()
{
	for (FWaypointData W : m_TWaypointData)
	{
		for (FEnemyRef R : W.m_TEnemyRefs)
		{
			for (ABaseEnemy* pEnemy : R.m_TEnemyRefs)
			{
				pEnemy->Destroy();
			}
		}
	}
}

void AEnemyGenerator::StunEnemies()
{
	for (FWaypointData W : m_TWaypointData)
	{
		for (FEnemyRef R : W.m_TEnemyRefs)
		{
			for (ABaseEnemy* pEnemy : R.m_TEnemyRefs)
			{
				if(IsValid(pEnemy))
					pEnemy->KnockBack(FVector::ZeroVector, 0, 0, 999.f);
			}
		}
	}
}


void AEnemyGenerator::ClearRefs()
{
	for (int i = 0; i < m_TWaypointData.Num(); i++)
	{
		FWaypointData& Data = m_TWaypointData[i];
		for (int j = 0; j < Data.m_TEnemyTypes.Num(); j++)
		{
			Data.m_TEnemyRefs[j].m_TEnemyRefs.Empty();
		}
	}
}

void AEnemyGenerator::Explode()
{
	m_USndManager->PlayEventAtLocation(GetWorld(), m_UExplodeFMODEvent, GetActorLocation());

	Destroy();
}

void AEnemyGenerator::PlaySpawnSound()
{
	if(!m_bIsExploding)
		m_USndManager->PlayEventAtLocation(GetWorld(), m_USpawnEnemyFMODEvent, GetActorLocation());
}

AEnemyGenerator::AEnemyGenerator() :m_bInitialWaveRespawned(false), m_bPlayerDetected(false), m_fGeneratorLive(5.f), m_fDetectionRadius(600.f), m_bActivateEnteringArea(true), m_bTestMode(false)
{
	PrimaryActorTick.bCanEverTick = false;
	mc_USphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere"));
	mc_USphere->SetupAttachment(GetRootComponent());
	mc_UCone = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cone"));
	mc_UCone->SetupAttachment(GetRootComponent());
	mc_UBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body Mesh"));
	mc_UBodyMesh->SetupAttachment(GetRootComponent());
	mc_UDetectionArea = CreateDefaultSubobject<USphereComponent>(TEXT("Detection area"));
	mc_UDetectionArea->InitSphereRadius(m_fDetectionRadius);
	mc_UDetectionArea->SetupAttachment(GetRootComponent());
	mc_UDetectionArea->OnComponentBeginOverlap.AddDynamic(this, &AEnemyGenerator::OnEnteringArea);
	OnDestroyed.AddDynamic(this, &AEnemyGenerator::OnGeneratorDestroyed);

}

void AEnemyGenerator::BeginPlay()
{
	Super::BeginPlay();

	// mc_pHealthComp->m_fHealth = m_fGeneratorLive;

	if (!m_bTestMode)
	{
		if ((!m_bActivateEnteringArea) && (!m_bInitialWaveRespawned))
		{
			PlaySpawnSound();
			GetWorld()->GetTimerManager().SetTimer(m_FFirstSpawnTimerHandle, this, &AEnemyGenerator::RespawnInitialWave, m_fTimeOffsetForSpawnSound, false);
			m_bInitialWaveRespawned = true;
		}
	}

	m_USndManager = GetGameInstance<UProjectUmbraGameInstance>()->GetSoundManager();
	m_UIdleAudioComponent = m_USndManager->PlayEventAttached(m_UIdleFMODEvent, GetRootComponent(), FVector::ZeroVector, EAttachLocation::KeepRelativeOffset, true, false, false);
}

void AEnemyGenerator::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	mc_UDetectionArea->SetSphereRadius(m_fDetectionRadius, true);
}

void AEnemyGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyGenerator::RespawnInitialWave()
{
	if (m_TWaypointData.Num() == 0)
	{
#ifdef WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Error in enemy generator: YOU MUST PROVIDE WAYPOINT DATA"));
#endif 
		UE_LOG(LogTemp, Error, TEXT("Error in enemy generator: YOU MUST PROVIDE WAYPOINT DATA"));
	}
	else {
		int iDiference = 0;
		for (int i = 0; i < m_TWaypointData.Num(); i++)
		{
			FWaypointData& Data = m_TWaypointData[i];
			for (int j = 0; j < Data.m_TEnemyTypes.Num(); j++)
			{
				if (Data.m_TEnemyRefs[j].m_TEnemyRefs.Num() != Data.m_TEnemyAmounts[j])
				{
					if (Data.m_TEnemyRefs[j].m_TEnemyRefs.Num() > Data.m_TEnemyAmounts[j])
					{
#ifdef WITH_EDITOR
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Error in enemy generator: YOU HAVE MORE REFERENCES THAN ENEMY AMOUNT, CHANGE THE AMOUNT FOR THIS TYPE OR THE REFERENCES"));
#endif 
						UE_LOG(LogTemp, Error, TEXT("Error in enemy generator: YOU HAVE MORE REFERENCES THAN ENEMY AMOUNT, CHANGE THE AMOUNT FOR THIS TYPE OR THE REFERENCES"));
					}
					else {
						iDiference = Data.m_TEnemyAmounts[j] - Data.m_TEnemyRefs[j].m_TEnemyRefs.Num();
						if (Data.m_pWaypoint)
						{
							for (int k = 0; k < iDiference; k++)
							{
								FActorSpawnParameters Params = FActorSpawnParameters();
								Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
								//Params.OverrideParentComponent = nullptr;
								UNiagaraComponent* USpawnEffectComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),m_USpawnEnemyParticleSystem,
						GetActorLocation() + FVector::UpVector * 10.0f,FRotator::ZeroRotator,FVector::OneVector,true,true,
								ENCPoolMethod::AutoRelease, true);
								USpawnEffectComp->SetVectorParameter("End", Data.m_pWaypoint->GetActorLocation());
								ABaseEnemy* pEnemy = GetWorld()->SpawnActor<ABaseEnemy>(GetEnemyClass(Data.m_TEnemyTypes[j]), Data.m_pWaypoint->GetActorLocation(), FRotator(0.f, 0.f, 0.f), Params);
								//ABaseEnemy* pEnemy = GetWorld()->SpawnActorDeferred<ABaseEnemy>(GetEnemyClass(Data.m_TEnemyTypes[j]), FTransform(Data.m_pWaypoint->GetActorLocation()), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
								SetRandomStadistics(pEnemy, pEnemy->m_eEnemyType, i);
								Data.m_TEnemyRefs[j].m_TEnemyRefs.Add(pEnemy);
								pEnemy->mc_pHealthComp->OnDeath.AddDynamic(this, &AEnemyGenerator::OnSpawnedEnemyDeath);
							}
						}
						else {
#ifdef WITH_EDITOR
							GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Error in enemy generator:  YOU DID NOT PROVIDE A WAYPOINT FOR THIS WAYPOINT DATA"));
#endif 
							UE_LOG(LogTemp, Error, TEXT("Error in enemy generator:  YOU DID NOT PROVIDE A WAYPOINT FOR THIS WAYPOINT DATA"));
							break;
						}
					}
				}
			}
		}
	}
}

UClass* AEnemyGenerator::GetEnemyClass(EEnemyType _EnemyType)
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

void AEnemyGenerator::SetRandomStadistics(ABaseEnemy* _Enemy, EEnemyType _EnemyType, int _WaypointDataIndex)
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
			pSniper->m_TWaypoints = m_TWaypointData[_WaypointDataIndex].m_pSniperWaypoints;
		}
		break;
	case EEnemyType::Shotgun:
		break;
	case EEnemyType::Burst:
		break;
	}
}

void AEnemyGenerator::CheckValidEnemyRefs(TArray<ABaseEnemy*>& _Refs, int _WaypointIndex, int _ArrayIndex)
{
	ANormalEnemy* pNormal = nullptr;
	AExplosiveEnemy* pExplosive = nullptr;
	ASniperEnemy* pSniper = nullptr;
	AHaloShooterEnemy* pHalo = nullptr;
	ABaseEnemy* pEnemy = nullptr;
	for (int i = _Refs.Num() - 1; i >= 0; --i)
	{
		pEnemy = _Refs[i];
		if (!pEnemy)
		{
			_Refs.RemoveAt(i);
		}
		else {
			pNormal = Cast<ANormalEnemy>(pEnemy);
			pExplosive = Cast<AExplosiveEnemy>(pEnemy);
			pSniper = Cast<ASniperEnemy>(pEnemy);
			pHalo = Cast<AHaloShooterEnemy>(pEnemy);
			if (pNormal)
			{
				if (pNormal->mc_pHealthComp->GetHealth() <= 0.f)
				{
					_Refs.RemoveAt(i);
					GetWorldTimerManager().SetTimer(m_TWaypointData[_WaypointIndex].m_FSpawnSoundTimer, this, &AEnemyGenerator::PlaySpawnSound, m_TWaypointData[_WaypointIndex].m_fTimeBetweenRespawn - m_fTimeOffsetForSpawnSound, false);
					GetWorldTimerManager().SetTimer(m_TWaypointData[_WaypointIndex].m_FEnemyTimer, FTimerDelegate::CreateUObject(this, &AEnemyGenerator::CheckEnemyNumbers, _WaypointIndex), m_TWaypointData[_WaypointIndex].m_fTimeBetweenRespawn, false);
				}
			}
			else if (pExplosive)
			{
				if (pExplosive->mc_pHealthComp->GetHealth() <= 0.f)
				{
					_Refs.RemoveAt(i);
					GetWorldTimerManager().SetTimer(m_TWaypointData[_WaypointIndex].m_FSpawnSoundTimer, this, &AEnemyGenerator::PlaySpawnSound, m_TWaypointData[_WaypointIndex].m_fTimeBetweenRespawn - m_fTimeOffsetForSpawnSound, false);
					GetWorldTimerManager().SetTimer(m_TWaypointData[_WaypointIndex].m_FEnemyTimer, FTimerDelegate::CreateUObject(this, &AEnemyGenerator::CheckEnemyNumbers, _WaypointIndex), m_TWaypointData[_WaypointIndex].m_fTimeBetweenRespawn, false);
				}
			}
			else if (pSniper)
			{
				if (pSniper->mc_pHealthComp->GetHealth() <= 0.f)
				{
					_Refs.RemoveAt(i);
					GetWorldTimerManager().SetTimer(m_TWaypointData[_WaypointIndex].m_FSpawnSoundTimer, this, &AEnemyGenerator::PlaySpawnSound, m_TWaypointData[_WaypointIndex].m_fTimeBetweenRespawn - m_fTimeOffsetForSpawnSound, false);
					GetWorldTimerManager().SetTimer(m_TWaypointData[_WaypointIndex].m_FEnemyTimer, FTimerDelegate::CreateUObject(this, &AEnemyGenerator::CheckEnemyNumbers, _WaypointIndex), m_TWaypointData[_WaypointIndex].m_fTimeBetweenRespawn, false);
				}
			}
			else if (pHalo)
			{
				if (pHalo->mc_pHealthComp->GetHealth() <= 0.f)
				{
					_Refs.RemoveAt(i);
					GetWorldTimerManager().SetTimer(m_TWaypointData[_WaypointIndex].m_FSpawnSoundTimer, this, &AEnemyGenerator::PlaySpawnSound, m_TWaypointData[_WaypointIndex].m_fTimeBetweenRespawn - m_fTimeOffsetForSpawnSound, false);
					GetWorldTimerManager().SetTimer(m_TWaypointData[_WaypointIndex].m_FEnemyTimer, FTimerDelegate::CreateUObject(this, &AEnemyGenerator::CheckEnemyNumbers, _WaypointIndex), m_TWaypointData[_WaypointIndex].m_fTimeBetweenRespawn, false);
				}
			}
		}
	}
}

void AEnemyGenerator::RespawnMoreEnemies(AWaypoint* _Waypoint, EEnemyType _Type, int _AmountToSpawn, TArray<ABaseEnemy*>& _ArrayRefs, int _WaypointIndex, int _ArrayIndex)
{
	ABaseEnemy* pEnemy = nullptr;
	FActorSpawnParameters Params = FActorSpawnParameters();
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	//Params.OverrideParentComponent = nullptr;
	for (int i = 0; i < _AmountToSpawn; i++)
	{
		pEnemy = GetWorld()->SpawnActor<ABaseEnemy>(GetEnemyClass(_Type), _Waypoint->GetActorLocation(), FRotator(0.f, 0.f, 0.f), Params);
		UNiagaraComponent* USpawnEffectComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),m_USpawnEnemyParticleSystem,
			GetActorLocation() + FVector::UpVector * 10.0f,FRotator::ZeroRotator,FVector::OneVector,true,true,
			ENCPoolMethod::AutoRelease, true);
		USpawnEffectComp->SetVectorParameter("End", _Waypoint->GetActorLocation());
		//pEnemy = GetWorld()->SpawnActorDeferred<ABaseEnemy>(GetEnemyClass(_Type), FTransform(_Waypoint->GetActorLocation()), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
		SetRandomStadistics(pEnemy, _Type, _WaypointIndex);
		_ArrayRefs.Add(pEnemy);
		pEnemy->mc_pHealthComp->OnDeath.AddDynamic(this, &AEnemyGenerator::OnEnemyDeath);
	}
}

void AEnemyGenerator::OnEnteringArea(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!m_bPlayerDetected)
	{
		APlayerCharacter* pPlayer = Cast<APlayerCharacter>(OtherActor);
		if (pPlayer) {
			m_bPlayerDetected = true;
			if (!m_bInitialWaveRespawned)
			{
				PlaySpawnSound();
				GetWorld()->GetTimerManager().SetTimer(m_FFirstSpawnTimerHandle, this, &AEnemyGenerator::RespawnInitialWave, m_fTimeOffsetForSpawnSound, false);
				m_bInitialWaveRespawned = true;
			}
		}
	}
}

void AEnemyGenerator::CheckEnemyNumbers(int index)
{
	if(m_bIsExploding)
	{
		return;
	}
	
	if (m_TWaypointData.Num() == 0 || m_TWaypointData.Num() < (int32)(index + 1))
	{
#ifdef WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Error in enemy generator: YOU MUST PROVIDE WAYPOINT DATA"));
#endif 
		UE_LOG(LogTemp, Error, TEXT("Error in enemy generator: YOU MUST PROVIDE WAYPOINT DATA"));
	}
	else
	{
		FWaypointData& Data = m_TWaypointData[index];
		if ((Data.m_TEnemyTypes.Num() != Data.m_TEnemyAmounts.Num()) && (Data.m_TEnemyAmounts.Num() != Data.m_TEnemyRefs.Num())) {
#ifdef WITH_EDITOR
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Error in enemy generator: YOU MUST PROVIDE THE SAME NUMBER OF AMOUNTS, TYPES AND ARRAY REFS"));
#endif 
			UE_LOG(LogTemp, Error, TEXT("Error in enemy generator: YOU MUST PROVIDE THE SAME NUMBER OF AMOUNTS, TYPES AND ARRAY REFS"));
			return;
		}
		for (int j = 0; j < Data.m_TEnemyTypes.Num(); j++) {
			if (Data.m_TEnemyRefs[j].m_TEnemyRefs.Num() < Data.m_TEnemyAmounts[j])
			{
				int len = Data.m_TEnemyRefs[j].m_TEnemyRefs.Num();
				RespawnMoreEnemies(Data.m_pWaypoint, Data.m_TEnemyTypes[j], Data.m_TEnemyAmounts[j] - len, Data.m_TEnemyRefs[j].m_TEnemyRefs, index, j);
			}

		}
	}
}

void AEnemyGenerator::OnGeneratorDestroyed(AActor* DestroyedActor)
{
	DestroyEnemies();
}

void AEnemyGenerator::OnSpawnedEnemyDeath()
{
	for (int i = 0; i < m_TWaypointData.Num(); i++)
	{
		FWaypointData& Data = m_TWaypointData[i];
		for (int j = 0; j < Data.m_TEnemyTypes.Num(); j++)
		{
			CheckValidEnemyRefs(Data.m_TEnemyRefs[j].m_TEnemyRefs, i, 0);
		}
	}
}

void AEnemyGenerator::OnPlayerDeath()
{
	if(!m_bIsExploding)
	{
		m_bPlayerDetected = false;
		m_bInitialWaveRespawned = false;
		DestroyEnemies();
		ClearRefs();
	}
}

void AEnemyGenerator::OnDeathCallback()
{
	// Super::OnDeathCallback();
	if(!m_bIsExploding)
	{
		m_bIsExploding = true;
	
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),m_UDestroyParticleSystem,
				GetActorLocation() + FVector::UpVector * 10.0f,FRotator::ZeroRotator,FVector::OneVector,true,true,
				ENCPoolMethod::AutoRelease, true);

		m_USndManager->PlayEventAtLocation(GetWorld(), m_UChargeFMODEvent, GetActorLocation());

		GetWorldTimerManager().SetTimer(m_FExplosionTimerHandle, this, &AEnemyGenerator::Explode, m_fDelayBeforeExplosion, false);
	
		StunEnemies();

		if(IsValid(m_UIdleAudioComponent))
			m_UIdleAudioComponent->Stop();

		// m_USndManager->PlayEventAtLocation(GetWorld(), m_UExplodeFMODEvent, GetActorLocation());
	}
}

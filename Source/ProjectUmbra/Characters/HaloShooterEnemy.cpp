// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/Characters/HaloShooterEnemy.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectUmbra/Widgets/HealthBar.h"
#include "ProjectUmbra/Components/HealthComponetCpp.h"
#include "ProjectUmbra/System/ProbabilityManager.h"
#include "ProjectUmbra/System/ProjectUmbraGameInstance.h"
#include "ProjectUmbra/System/UmbraGameMode.h"
#include "ProjectUmbra/System/SoundManager.h"
#include "ProjectUmbra/Items/TagDrop.h"

void AHaloShooterEnemy::OnDeathCallback()
{
	Super::OnDeathCallback();

	m_USndManager->PlayEventAtLocation(GetWorld(), m_UDestroyedFMODEvent, GetActorLocation());
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),m_UDeathParticleSystem,
				GetActorLocation(),FRotator::ZeroRotator,FVector::OneVector,true,true,
				ENCPoolMethod::AutoRelease,true);
}

void AHaloShooterEnemy::OnDamagedCallback(float _fCurrentHealth)
{
	m_USndManager->PlayEventAtLocation(GetWorld(), m_UHitFMODEvent, GetActorLocation());
}

AHaloShooterEnemy::AHaloShooterEnemy(): m_eTag(EBulletModifier::Bouncy), m_iBulletAmount(6), m_fDetectionRange(1500.f),
                                        m_fShootDelay(2.0f), m_fIdleTime(2.0f), m_bDoNotShoot(false), m_fBulletSpeed(500.0f)
{
	m_eEnemyType = EEnemyType::Circular;
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mc_pCubeBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	mc_pCubeBody->SetupAttachment(GetRootComponent());
	mc_pMuzzle = CreateDefaultSubobject<USphereComponent>(TEXT("Muzzle"));
	mc_pMuzzle->SetupAttachment(mc_pCubeBody);

	//ConstructorHelpers::FObjectFinder<UClass> SearchProjectile(TEXT("Blueprint'/Game/Blueprints/Items/Projectile_BP.Projectile_BP_C'"));
	//m_FoundProjectile = (AProjectile*)SearchProjectile.Object;
}

void AHaloShooterEnemy::BeginPlay()
{
	Super::BeginPlay();

	mc_pHealthComp->OnDamaged.AddDynamic(this, &AHaloShooterEnemy::OnDamagedCallback);

	m_USndManager = GetGameInstance<UProjectUmbraGameInstance>()->GetSoundManager();
	
	// mc_pHealthComp->m_fHealth = m_fHealth;
}

void AHaloShooterEnemy::ShootBullet()
{
	AUmbraGameMode* pGM = Cast<AUmbraGameMode>(GetWorld()->GetAuthGameMode());
	if (pGM)
	{
		if (!m_bDoNotShoot && !m_bIsInKnockBack)
		{
			OnEnemyShoot();
			m_USndManager->PlayEventAtLocation(GetWorld(), m_UShootFMODEvent, GetActorLocation());
			if (ProjectileToSpawn)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_UShootSound, GetActorLocation(), 1, UKismetMathLibrary::RandomFloatInRange(0.85, 1.15));
				m_fAngle = 0;
				FRotator rRotator = mc_pMuzzle->GetComponentRotation();
				m_fAngle += 360 / m_iBulletAmount;

				for (int i = 0; i < m_iBulletAmount; ++i)
				{
					if (!pGM->PopUpBulletFromPool(mc_pMuzzle->GetComponentLocation(), rRotator, m_fBulletSpeed, EBulletModifier::Bouncy, this, false))
					{
						AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileToSpawn, mc_pMuzzle->GetComponentLocation(), rRotator);
						Projectile->m_fSpeed = m_fBulletSpeed;
						Projectile->AddTag(EBulletModifier::Bouncy);
						// TODO: Change back when Player and Enemies in C++
						Projectile->SetOwnerAndMaterial(false, this);
					}
					rRotator.Yaw += m_fAngle;
				}
			}
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error in Halo enemy: Could not find the UmbraGameMode to shoot projectiles from pool"));
	}
}

void AHaloShooterEnemy::DropObject()
{
	Super::DropObject();

	if(m_bDropsTag && GetGameInstance<UProjectUmbraGameInstance>()->GetProbabilityManager()->CanSpawnTagDrop())
	{
		FHitResult FHitResult;
		if(UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			GetActorLocation(),
			GetActorLocation() +
			-GetActorUpVector() * 500,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::None,
			FHitResult,
			true
			))
		{
			ATagDrop* ADrop = GetWorld()->SpawnActor<ATagDrop>(m_TTagDropClass, FHitResult.Location, FRotator::ZeroRotator);
			GetGameInstance<UProjectUmbraGameInstance>()->GetProbabilityManager()->RegisterTagDrop(ADrop);
		}
	}
}

void AHaloShooterEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(m_bIsInKnockBack)
	{
		UpdateKnockBack(DeltaTime);
	}
}

void AHaloShooterEnemy::KnockBack(FVector _VKnockBackDirection, float _fKnockBackSpeed, float _fKnockBackDistance,
                                  float _fStunDuration)
{
	Super::KnockBack(_VKnockBackDirection, _fKnockBackSpeed, _fKnockBackDistance, _fStunDuration);

	m_USndManager->PlayEventAtLocation(GetWorld(), m_UStunFMODEvent, GetActorLocation());
	
	// m_fRemainingKnockBackDistance = _fKnockBackDistance;
	m_fRemainingStunDuration = _fStunDuration;
	// m_fKnockBackSpeed = _fKnockBackSpeed;
	// m_VKnockBackDirection = _VKnockBackDirection;
	m_bIsInKnockBack = true;

	GetController<AAIController>()->GetBrainComponent()->StopLogic(TEXT(""));
}


void AHaloShooterEnemy::UpdateKnockBack(float _fDeltaTime)
{
	
	// if(m_fRemainingKnockBackDistance > 0)
	// {
	// 	TArray<AActor*> TActorsToIgnore = TArray<AActor*>();
	// 	TActorsToIgnore.Add(this);
	//
	// 	FHitResult FHitResult;
	// 	
	// 	float m_fDistanceThisFrame = m_fKnockBackSpeed * _fDeltaTime;
	// 	m_fDistanceThisFrame = m_fDistanceThisFrame > m_fRemainingKnockBackDistance ? m_fRemainingKnockBackDistance : m_fDistanceThisFrame;
	//
	// 	const FVector vCurrentLocation = GetActorLocation();
	// 	FVector vFinalLocation = GetActorLocation() + m_VKnockBackDirection * m_fDistanceThisFrame;
	//
	// 	if(UKismetSystemLibrary::LineTraceSingle(
	// 		GetWorld(),
	// 		vCurrentLocation,
	// 		vFinalLocation,
	// 		UEngineTypes::ConvertToTraceType(ECC_Visibility),
	// 		false,
	// 		TActorsToIgnore,
	// 		EDrawDebugTrace::None,
	// 		FHitResult,
	// 		true
	// 		)
	// 	)
	// 	{
	// 		vFinalLocation = FHitResult.ImpactPoint - m_VKnockBackDirection * GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	// 	}
	// 	m_fRemainingKnockBackDistance -= m_fDistanceThisFrame;
	//
	// 	SetActorLocation(vFinalLocation);
	// }
	// else
	if(m_fRemainingStunDuration > 0.0f)
	{
		m_fRemainingStunDuration -= _fDeltaTime;
	}
	else
	{
		GetController<AAIController>()->GetBrainComponent()->RestartLogic();
		m_bIsInKnockBack = false;
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/Characters/SniperEnemy.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "ProjectUmbra/Characters/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectUmbra/Items/AProjectile.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectUmbra/Widgets/HealthBar.h"
#include "ProjectUmbra/Items/AProjectile.h"
#include "ProjectUmbra/Components/HealthComponetCpp.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "ProjectUmbra/System/ProbabilityManager.h"
#include "ProjectUmbra/System/ProjectUmbraGameInstance.h"
#include "NiagaraSystem.h"
#include "ProjectUmbra/Items/TagDrop.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraCommon.h"
#include "NiagaraComponent.h"
#include "Curves/CurveLinearColor.h"
#include "ProjectUmbra/System/UmbraGameMode.h"
#include "ProjectUmbra/System/SoundManager.h"

void ASniperEnemy::OnDeathCallback()
{
	Super::OnDeathCallback();

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),m_UDeathParticleSystem,
				GetActorLocation(),FRotator::ZeroRotator,FVector::OneVector,true,true,
				ENCPoolMethod::AutoRelease,true);
	
	StopTeleportEffect();
	StopTeleportEndEffect();
	m_pTeleportNiagaraComp->ReleaseToPool();
}

ASniperEnemy::ASniperEnemy():m_eTeleportPattern(ESniperTeleportPattern::FollowWaypointsRandomly), m_bKeepsTeleportLocation(false),m_fCooldown(5.f),m_eTag(EBulletModifier::Freezing),m_fChargeTime(3.f), m_fTimeAfterAiming(0.f),m_fTimeUntilTeleport(0.f)
                             ,m_iMaxTeleportCount(3.f),m_fSpeedMultiplier(5.f),m_fDetectionRange(4000.f),m_fBulletSpeed(1000.f)
{
	PrimaryActorTick.bCanEverTick = true;
	m_eEnemyType = EEnemyType::Sniper;
	mc_pCubeBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube Body"));
	mc_pCubeBody->SetupAttachment(GetMesh());
	mc_pNextTeleportMark = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Next Teleport Mark"));
	mc_pNextTeleportMark->SetupAttachment(GetRootComponent());
	mc_pMuzzle = CreateDefaultSubobject<USphereComponent>(TEXT("Muzzle"));
	mc_pMuzzle->SetupAttachment(GetMesh());
	mc_pLaserMuzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Laser Muzzle"));
	mc_pLaserMuzzle->SetupAttachment(GetMesh());
	mc_pArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("DebugArrow"));
	mc_pArrow->SetupAttachment(GetMesh());

	static ConstructorHelpers::FObjectFinder<UCurveFloat> FindRotatingCurve(TEXT("CurveFloat'/Game/Blueprints/Curves/Normal_Enemy_Curves/RotateToFacePlayer_Curve.RotateToFacePlayer_Curve'"));
	if (FindRotatingCurve.Succeeded())
	{
		m_fRotateToFaceCurve = FindRotatingCurve.Object;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error in Sniper enemy: Could not get the rotating to face curve reference in the constructor. Something went wrong :("));
	}
	//OnTakeAnyDamage.AddDynamic(this, &ASniperEnemy::OnAnyDamage);
	static ConstructorHelpers::FObjectFinder<UCurveLinearColor> FindColorCurve(TEXT("CurveLinearColor'/Game/Blueprints/Curves/Sniper_Enemy_Curves/LaserColorCurve.LaserColorCurve'"));
	if (FindColorCurve.Succeeded())
	{
		m_FChangeLaserCurve = FindColorCurve.Object;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error in Sniper enemy: Could not get the change color laser curve reference in the constructor. Something went wrong :("));
	}
}

void ASniperEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (m_fRotateToFaceCurve)
	{
		FOnTimelineFloat TLEvent2;
		TLEvent2.BindUFunction(this, FName("RotateToFacePlayerFunc"));
		m_TLRotateToFacePlayer.AddInterpFloat(m_fRotateToFaceCurve, TLEvent2);
		m_TLRotateToFacePlayer.SetLooping(false);
	}
	if (m_FChangeLaserCurve)
	{
		FOnTimelineLinearColor TLColorEvent;
		TLColorEvent.BindUFunction(this, FName("ChangeLaserColorFunc"));
		m_TLChangeLaserColor.AddInterpLinearColor(m_FChangeLaserCurve, TLColorEvent);
		m_TLChangeLaserColor.SetPlayRate(1/m_fChargeTime);
		FOnTimelineEvent TLColorEnd;
		TLColorEnd.BindUFunction(this, FName("OnLaserColorEnded"));
		//m_TLChangeLaserColor.SetTimelineFinishedFunc(TLColorEnd);
	}
		// mc_pHealthComp->m_fHealth = m_fHealth;

	m_USndManager = GetGameInstance<UProjectUmbraGameInstance>()->GetSoundManager();
	m_UAimAudioComponent = m_USndManager->PlayEventAttached(m_UAimFMODEvent, GetRootComponent(), FVector::ZeroVector, EAttachLocation::KeepRelativeOffset, true, false, false);
	m_UTeleportAudioComponent = m_USndManager->PlayEventAttached(m_UTeleportFMODEvent, GetRootComponent(), FVector::ZeroVector, EAttachLocation::KeepRelativeOffset, true, false, false);

	m_pTeleportNiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),m_pTeleportParticleSystem,
			GetActorLocation(),FRotator::ZeroRotator,FVector::OneVector,true,false,
			ENCPoolMethod::ManualRelease, true);

	m_pTeleportEndNiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),m_pTeleportEndParticleSystem,
			GetActorLocation(),FRotator::ZeroRotator,FVector::OneVector,true,false,
			ENCPoolMethod::ManualRelease, true);

}

void ASniperEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	m_TLRotateToFacePlayer.TickTimeline(DeltaTime);
	m_TLChangeLaserColor.TickTimeline(DeltaTime);
	if(m_bIsInKnockBack)
	{
		UpdateKnockBack(DeltaTime);
	}
}

void ASniperEnemy::ShootBullet()
{

	m_USndManager->PlayEventAtLocation(GetWorld(), m_UShootFMODEvent, GetActorLocation());
	
	AUmbraGameMode* pGM = Cast<AUmbraGameMode>(GetWorld()->GetAuthGameMode());
	if (pGM)
	{
		Super::ShootBullet();
		if (!pGM->PopUpBulletFromPool(mc_pMuzzle->GetComponentLocation(), mc_pMuzzle->GetComponentRotation(), m_fBulletSpeed * m_fSpeedMultiplier, m_eTag, this, false))
		{
			AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(m_pProjectileClass, mc_pMuzzle->GetComponentLocation(), mc_pMuzzle->GetComponentRotation());
			if (Projectile)
			{
				Projectile->m_fSpeed = m_fBulletSpeed * m_fSpeedMultiplier;
				Projectile->AddTag(m_eTag);
				Projectile->SetOwnerAndMaterial(false, this);
			}
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error in Sniper enemy: Could not find the UmbraGameMode to shoot projectiles from pool"));
	}
}

void ASniperEnemy::DropObject()
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
	AbortLaser();
}

void ASniperEnemy::SmoothFacePlayer(FRotator& _CurrentRot, FRotator& _TargetRot)
{
	m_rCurrent = _CurrentRot;
	m_rTarget = _TargetRot;
	m_TLRotateToFacePlayer.PlayFromStart();
}
void ASniperEnemy::RotateToFacePlayerFunc(float alpha)
{
	SetActorRotation(UKismetMathLibrary::RLerp(m_rCurrent, m_rTarget, alpha, true));
}

void ASniperEnemy::ChangeLaserColorFunc(FLinearColor FColor)
{
	if (m_pNiagaraComp)
	{
		if (m_pPlayerRef)
		{
			m_pNiagaraComp->SetNiagaraVariableVec3(FString("End"), m_pPlayerRef->GetActorLocation());
			m_pNiagaraComp->SetNiagaraVariableLinearColor(FString("Color"), FColor);
		}
	}
}

void ASniperEnemy::OnLaserColorEnded()
{
	if (m_pNiagaraComp)
	{
		m_pNiagaraComp->ReleaseToPool();
		m_pNiagaraComp->SetVisibility(false);
	}
}

void ASniperEnemy::SetLaserAim(APlayerCharacter* pTarget)
{
	m_pPlayerRef = pTarget;
	m_pNiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), m_pLaserParticleSystem, mc_pLaserMuzzle->GetComponentLocation(), FRotator::ZeroRotator, FVector::OneVector, true, true,
		ENCPoolMethod::ManualRelease, true);
	//m_pNiagaraComp->SetNiagaraVariableFloat(FString("LifeTime"), m_fChargeTime);
	m_TLChangeLaserColor.PlayFromStart();
}

void ASniperEnemy::KnockBack(FVector _VKnockBackDirection, float _fKnockBackSpeed, float _fKnockBackDistance,
	float _fStunDuration)
{
	Super::KnockBack(_VKnockBackDirection, _fKnockBackSpeed, _fKnockBackDistance, _fStunDuration);

	m_fRemainingKnockBackDistance = _fKnockBackDistance;
	m_fRemainingStunDuration = _fStunDuration;
	m_fKnockBackSpeed = _fKnockBackSpeed;
	m_VKnockBackDirection = _VKnockBackDirection;
	m_bIsInKnockBack = true;

	StopTeleportEffect();
	StopTeleportEndEffect();
	if(GetWorld()->GetTimerManager().IsTimerActive(m_FTeleportSoundTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(m_FTeleportSoundTimerHandle);
	}
	else
	{
		m_UTeleportAudioComponent->Stop();
	}
	
	GetController<AAIController>()->GetBrainComponent()->StopLogic(TEXT(""));
}


void ASniperEnemy::UpdateKnockBack(float _fDeltaTime)
{
	
	if(m_fRemainingKnockBackDistance > 0)
	{
		TArray<AActor*> TActorsToIgnore = TArray<AActor*>();
		TActorsToIgnore.Add(this);

		FHitResult FHitResult;
		
		float m_fDistanceThisFrame = m_fKnockBackSpeed * _fDeltaTime;
		m_fDistanceThisFrame = m_fDistanceThisFrame > m_fRemainingKnockBackDistance ? m_fRemainingKnockBackDistance : m_fDistanceThisFrame;

		const FVector vCurrentLocation = GetActorLocation();
		FVector vFinalLocation = GetActorLocation() + m_VKnockBackDirection * m_fDistanceThisFrame;

		if(UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			vCurrentLocation,
			vFinalLocation,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			TActorsToIgnore,
			EDrawDebugTrace::None,
			FHitResult,
			true
			)
		)
		{
			vFinalLocation = FHitResult.ImpactPoint - m_VKnockBackDirection * GetCapsuleComponent()->GetUnscaledCapsuleRadius();
		}
		m_fRemainingKnockBackDistance -= m_fDistanceThisFrame;

		SetActorLocation(vFinalLocation);
	}
	else if(m_fRemainingStunDuration > 0.0f)
	{
		m_fRemainingStunDuration -= _fDeltaTime;
	}
	else
	{
		GetController<AAIController>()->GetBrainComponent()->RestartLogic();
		m_bIsInKnockBack = false;
	}
}

void ASniperEnemy::AbortLaser()
{
	if (m_pNiagaraComp)
	{
		m_TLChangeLaserColor.Stop();
		m_pNiagaraComp->ReleaseToPool();
		m_pNiagaraComp->SetVisibility(false);
	}
}

void ASniperEnemy::PlayAimSound()
{
	if(IsValid(m_UAimAudioComponent))
		m_UAimAudioComponent->Play();
}

void ASniperEnemy::StopAimSound()
{
	if(IsValid(m_UAimAudioComponent))
		m_UAimAudioComponent->Stop();
}

void ASniperEnemy::PlayTeleportSoundDelay(float _fTimeDelay)
{
	if(_fTimeDelay > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(m_FTeleportSoundTimerHandle, this, &ASniperEnemy::PlayTeleportSound, _fTimeDelay, false);
	}
	else
	{
		PlayTeleportSound();
	}
}

void ASniperEnemy::PlayTeleportSound()
{
	m_UTeleportAudioComponent->Play();
}


void ASniperEnemy::PlayTeleportEffect(FVector _FLocation)
{
	m_pTeleportNiagaraComp->SetWorldLocation(_FLocation);
	m_pTeleportNiagaraComp->Activate(true);
}

void ASniperEnemy::StopTeleportEffect()
{
	if(IsTeleportEffectPlaying())
		m_pTeleportNiagaraComp->DeactivateImmediate();
}

bool ASniperEnemy::IsTeleportEffectPlaying()
{
	return !m_pTeleportNiagaraComp->IsComplete();
}

void ASniperEnemy::PlayTeleportEndEffect(FVector _FLocation)
{
	m_pTeleportEndNiagaraComp->SetWorldLocation(_FLocation);
	m_pTeleportEndNiagaraComp->Activate(true);
}

void ASniperEnemy::StopTeleportEndEffect()
{
	m_pTeleportEndNiagaraComp->DeactivateImmediate();
}



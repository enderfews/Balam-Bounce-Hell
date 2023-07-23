// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/Characters/NormalEnemy.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "ProjectUmbra/Characters/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "ProjectUmbra/Items/AProjectile.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectUmbra/Items/AProjectile.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectUmbra/Widgets/HealthBar.h"
#include "ProjectUmbra/Components/HealthComponetCpp.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "ProjectUmbra/System/ProbabilityManager.h"
#include "ProjectUmbra/System/ProjectUmbraGameInstance.h"
#include "Components/DecalComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraCommon.h"
#include "NiagaraComponent.h"
#include "ProjectUmbra/System/UmbraGameMode.h"
#include "ProjectUmbra/System/SoundManager.h"
//#include "DrawDebugHelpers.h"
//#include "Kismet/KismetStringLibrary.h"
//#include "NormalEnemy.h"

// Sets default values
ANormalEnemy::ANormalEnemy():m_bIsAbortedByCheckpoint(false),m_bIsCharging(false), m_bChargeStart(false), m_bIsEnemyAtLimit(false), m_bChargeWasCancelled(false), m_fBrakingFrictionFactor(0.f), m_fBrakingDecelerationWalking(0.f), m_fGravityScale(0.f), m_fTimeToResetChargeDuration(0.f), m_fCurrentWalkSpeed(0.f), m_vEndPosition(FVector::ZeroVector), m_vEndPositionAfterAdjustment(FVector::ZeroVector), m_vVelocity(FVector::ZeroVector),
m_bTestMode(false),m_eMoveMode(EEnemyMoveMode::WanderAroundMap),m_bCanShoot(true),m_bCanUseChargeAttack(false),m_fMaxChargeDistance(1500.f), m_fChargeSpeed(3000.f), m_fRotationRate(2.f), m_fChargeDuration(0.5f), m_fChargeRestDuration(1.f), m_fChargeAimDuration(2.f), m_fPrepareChargeDuration(0.5f), m_fChargeDistanceVariation(50.f), m_fChargeDamage(1.f), m_bPredictLocToShoot(false),m_fPatrolSpeed(500.f),m_fChasingSpeed(1000.f),m_fShootingRate(0.f),m_fAimTime(0.f),m_eShootingType(EShootingType::Normal),
m_eTag(EBulletModifier::Normal),m_fShootingRange(7000.f),m_fRallyProb(0.75f),m_iBurstNum(1),
m_fFireRate(0.3f),m_iBullNum(1),m_fRallyProbDecrement(0.05f),m_fCombatRange(10000.f),m_fPursuitRange(15000.f),m_fBulletSpeed(600.f), m_fMinDistanceBetweenPlayer(900.f), m_fShootingAngle(30.f),m_fCastTime(1.f),m_fAreaCD(10.f),m_fAreaRadius(200.f),m_fAreaDmg(3.f),
m_fStunTime(0.f),m_fKnockbackTime(10.f),m_fPunchRange(100.f),m_fPunchSpeed(500.f),m_fAngleBulletIncrement(0.f),
m_fAngleBullet(0.f),m_fPunchCurve(nullptr),m_fRotateToFaceCurve(nullptr),m_RCurrent(FRotator()),m_RTarget(FRotator())
{
	m_eEnemyType = EEnemyType::Normal;
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mc_pRallyCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Rally Capsule"));
	mc_pRallyCapsule->SetupAttachment(GetRootComponent());
	mc_pRallyCapsule->OnComponentBeginOverlap.AddDynamic(this, &ANormalEnemy::OnOverlapBegin);
	mc_pCubeBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube Body"));
	mc_pCubeBody->SetupAttachment(GetMesh());
	mc_pMuzzle = CreateDefaultSubobject<USphereComponent>(TEXT("Muzzle"));
	mc_pMuzzle->SetupAttachment(GetMesh());
	mc_pArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("DebugArrow"));
	mc_pArrow->SetupAttachment(GetMesh());
	mc_pPunchAreaDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("Punch area decal"));
	mc_pPunchAreaDecal->SetupAttachment(GetMesh());
	mc_pPunchAreaDecal->SetVisibility(false);
	static ConstructorHelpers::FObjectFinder<UCurveFloat> FindPunchCurve(TEXT("CurveFloat'/Game/Blueprints/Curves/Normal_Enemy_Curves/Punch_Curve.Punch_Curve'"));
	if (FindPunchCurve.Succeeded())
	{
		m_fPunchCurve = FindPunchCurve.Object;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error in Normal enemy: Could not get the punch curve reference in the constructor. Something went wrong :("));
	}
	static ConstructorHelpers::FObjectFinder<UCurveFloat> FindRotatingCurve(TEXT("CurveFloat'/Game/Blueprints/Curves/Normal_Enemy_Curves/RotateToFacePlayer_Curve.RotateToFacePlayer_Curve'"));
	if (FindRotatingCurve.Succeeded())
	{
		m_fRotateToFaceCurve = FindRotatingCurve.Object;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error in Normal enemy: Could not get the rotating to face curve reference in the constructor. Something went wrong :("));
	}
}

void ANormalEnemy::ChangeDamageImmunityColor(FColor _FColor)
{
	m_UBodyMat->SetVectorParameterValue("BlinkColor", _FColor);
}

void ANormalEnemy::ActivateDamageImmunity(float _fActive)
{
	m_UBodyMat->SetScalarParameterValue("Active", _fActive);
}

void ANormalEnemy::OnDamagedCallback(float _fCurrentHealth)
{
	m_USndManager->PlayEventAtLocation(GetWorld(), m_UHitFMODEvent, GetActorLocation());
	ActivateDamageImmunity(1.0f);
	ChangeDamageImmunityColor(m_FDamagedRimColor);
}

void ANormalEnemy::OnEndDamagedCallback()
{
	ChangeDamageImmunityColor(m_FImmunityRimColor);
}

void ANormalEnemy::OnEndImmunityCallback()
{
	ActivateDamageImmunity(0.0f);
}

void ANormalEnemy::OnDeathCallback()
{
	Super::OnDeathCallback();
	AUmbraGameMode* pGM = Cast<AUmbraGameMode>(GetWorld()->GetAuthGameMode());
	if (pGM)
	{
		pGM->OnPlayerKilled.RemoveDynamic(this, &ANormalEnemy::OnPlayerKilled);
		pGM->OnPlayerRespawned.RemoveDynamic(this, &ANormalEnemy::OnPlayerRespawned);
	}
	m_USndManager->PlayEventAtLocation(GetWorld(),m_UDestroyedFMODEvent,GetActorLocation());
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),m_UDeathParticleSystem,
				GetActorLocation(),FRotator::ZeroRotator,FVector::OneVector,true,true,
				ENCPoolMethod::AutoRelease,true);
	
	if (stunParticles != nullptr)
	{
		stunParticles->Deactivate();
		stunParticles->ReleaseToPool();
	}
}

void ANormalEnemy::OnPlayerKilled()
{
	m_bCanShoot = false;
}

void ANormalEnemy::OnPlayerRespawned()
{
	m_bCanShoot = true;
}

//Used as last resource when enemies are not killed but destroyed
void ANormalEnemy::OnDestruction(AActor* _ADestroyedActor)
{
	if(IsValid(stunParticles))
	{
		stunParticles->Deactivate();
		stunParticles->ReleaseToPool();
	}
}

// Called when the game starts or when spawned
void ANormalEnemy::BeginPlay()
{
	Super::BeginPlay();


	if (m_fPunchCurve) {
		FOnTimelineFloat TLEvent;
		TLEvent.BindUFunction(this, FName("PunchFunc"));
		m_TLPunch.AddInterpFloat(m_fPunchCurve, TLEvent);
		m_TLPunch.SetLooping(false);
	}
	if (m_fRotateToFaceCurve)
	{
		FOnTimelineFloat TLEvent2;
		TLEvent2.BindUFunction(this, FName("RotateToFacePlayerFunc"));
		m_TLRotateToFacePlayer.AddInterpFloat(m_fRotateToFaceCurve, TLEvent2);
		m_TLRotateToFacePlayer.SetLooping(false);
		m_TLRotateToFacePlayer.SetPlayRate(m_fRotationRate);
		FOnTimelineEvent EndRotation;
		EndRotation.BindUFunction(this, FName("OnRotationFaceEnded"));
		m_TLRotateToFacePlayer.SetTimelineFinishedFunc(EndRotation);
	}
	
	GetCharacterMovement()->MaxWalkSpeed =m_fPatrolSpeed;
	// mc_pHealthComp->m_fHealth = m_fHealth;
	mc_pHealthComp->OnDamaged.AddDynamic(this, &ANormalEnemy::OnDamagedCallback);
	mc_pHealthComp->OnDamagedEnd.AddDynamic(this, &ANormalEnemy::OnEndDamagedCallback);
	mc_pHealthComp->OnImmunityEnd.AddDynamic(this, &ANormalEnemy::OnEndImmunityCallback);
	
	m_UBodyMat = mc_pCubeBody->CreateDynamicMaterialInstance(1, GetMesh()->GetMaterial(1));

	m_USndManager = GetGameInstance<UProjectUmbraGameInstance>()->GetSoundManager();
	AUmbraGameMode* pGM = Cast<AUmbraGameMode>(GetWorld()->GetAuthGameMode());
	if (pGM)
	{
		pGM->OnPlayerKilled.AddDynamic(this, &ANormalEnemy::OnPlayerKilled);
		pGM->OnPlayerRespawned.AddDynamic(this, &ANormalEnemy::OnPlayerRespawned);
	}

	OnDestroyed.AddDynamic(this, &ANormalEnemy::OnDestruction);
}

// Called every frame
void ANormalEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//FString sAIState=(GetController<AAIController>()->GetBrainComponent()->IsPaused() || !GetController<AAIController>()->GetBrainComponent()->IsRunning())? "True":"False";
	//DrawDebugString(GetWorld(), GetActorLocation(), sAIState, nullptr, FColor::Red, 0.1f);
	if(m_bIsPunchRunning)
	{
		PunchFunc(DeltaTime);
	}

	if(m_bIsInKnockBack)
	{
		UpdateKnockBack(DeltaTime);
	}
	
	m_TLRotateToFacePlayer.TickTimeline(DeltaTime);
	UpdateCharge(DeltaTime);
}

// Called to bind functionality to input
void ANormalEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ANormalEnemy::OnDelayPunchCastTimeCompleted()
{
	// m_TLPunch.PlayFromStart();
	m_fCurrentPunchDistance = 0.0f;
	m_bIsPunchRunning = true;
}

void ANormalEnemy::OnRotationFaceEnded()
{
	if (m_FOnRotationFaceEnded.IsBound())
		m_FOnRotationFaceEnded.Broadcast();
}

void ANormalEnemy::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (m_fRemainingStunDuration <= 0.f)
	{
		if (UGameplayStatics::GetPlayerPawn(GetWorld(), 0) == OtherActor->GetOwner() || m_bTestMode)
		{
			AProjectile* Projectile = Cast<AProjectile>(OtherActor);
			if (Projectile)
			{
				if (UKismetMathLibrary::RandomFloatInRange(0.f, 1.f) <= m_fRallyProb)
				{
					Projectile->m_fSpeed = Projectile->m_fSpeed * 1.25f;
					FVector Dir;
					ReturnDir(Projectile, Dir);
					Projectile->Execute_GetHit(Projectile, Dir, this, false, 2000.f, 30.f);
					m_fRallyProb -= m_fRallyProbDecrement;
				}
			}
		}
		else if (m_bIsCharging)
		{
			APlayerCharacter* pPlayer = Cast<APlayerCharacter>(OtherActor);
			FVector vKnockbackDir;
			if (pPlayer)
			{
				GetPunchKnockbackDir(pPlayer, vKnockbackDir);
				pPlayer->ReceiveKnockback(vKnockbackDir, m_fKnockbackTime, m_fPunchSpeed, m_fStunTime);
				UGameplayStatics::ApplyDamage(pPlayer, m_fChargeDamage, nullptr, this, nullptr);
			}
		}
	}
}

void ANormalEnemy::KnockBack(FVector _VKnockBackDirection, float _fKnockBackSpeed, float _fKnockBackDistance,
	float _fStunDuration)
{
	Super::KnockBack(_VKnockBackDirection, _fKnockBackSpeed, _fKnockBackDistance, _fStunDuration);

	m_USndManager->PlayEventAtLocation(GetWorld(), m_UStunFMODEvent, GetActorLocation());
	
	m_fRemainingKnockBackDistance = _fKnockBackDistance;
	m_fRemainingStunDuration = _fStunDuration;
	m_fKnockBackSpeed = _fKnockBackSpeed;
	m_VKnockBackDirection = _VKnockBackDirection;
	m_bIsInKnockBack = true;

	const AAIController* AController = GetController<AAIController>();
	if(IsValid(Controller))
	{
		UBrainComponent* Brain = AController->GetBrainComponent();
		if(IsValid(Brain))
		{
			Brain->StopLogic(TEXT(""));
		}
	}
}


void ANormalEnemy::UpdateKnockBack(float _fDeltaTime)
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
		if (m_bStartStun)
		{
			// Stun Particle System
			stunParticles = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),m_UStunParticleSystem,
				GetActorLocation(),FRotator::ZeroRotator,FVector::OneVector,true,true,
				ENCPoolMethod::ManualRelease,true);
			m_bStartStun = false;
		}
		m_fRemainingStunDuration -= _fDeltaTime;
	}
	else
	{
		GetController<AAIController>()->GetBrainComponent()->RestartLogic();
		m_bIsInKnockBack = false;
		// Stun Particles
		stunParticles->Deactivate();
		stunParticles->ReleaseToPool();
		m_bStartStun = true;
	}
}


void ANormalEnemy::ShootBullet() 
{
	
	AUmbraGameMode* pGM = Cast<AUmbraGameMode>(GetWorld()->GetAuthGameMode());
	if (pGM)
	{
		if (m_bCanShoot)
		{
			if (!IsPendingKill() && !m_bIsInKnockBack)
			{
				Super::ShootBullet();
				m_USndManager->PlayEventAtLocation(GetWorld(), m_UShootFMODEvent, GetActorLocation());

				switch (m_eShootingType)
				{
				case EShootingType::Normal:
					if (!pGM->PopUpBulletFromPool(mc_pMuzzle->GetComponentLocation(), mc_pMuzzle->GetComponentRotation(), m_fBulletSpeed, m_eTag, this, false))
					{
						SpawnProjectile(mc_pMuzzle->GetComponentLocation(), mc_pMuzzle->GetComponentRotation(), m_eTag);
					}
					break;
				case EShootingType::Shotgun:
					if (m_iBullNum == 1)
					{
						if (!pGM->PopUpBulletFromPool(mc_pMuzzle->GetComponentLocation(), mc_pMuzzle->GetComponentRotation(), m_fBulletSpeed, m_eTag, this, false))
						{
							SpawnProjectile(mc_pMuzzle->GetComponentLocation(), mc_pMuzzle->GetComponentRotation(), m_eTag);
						}
					}
					else if (m_iBullNum % 2 == 0)
					{
						m_fAngleBulletIncrement = 90.f / m_iBullNum;
						m_fAngleBullet = m_fAngleBulletIncrement;
						FRotator LeftRot, RightRot;
						for (int i = 0; i < (m_iBullNum / 2); i++)
						{
							LeftRot = mc_pMuzzle->GetComponentRotation();
							LeftRot.Yaw += m_fAngleBullet;
							if (!pGM->PopUpBulletFromPool(mc_pMuzzle->GetComponentLocation(),LeftRot, m_fBulletSpeed, m_eTag, this, false))
							{
								SpawnProjectile(mc_pMuzzle->GetComponentLocation(), LeftRot, m_eTag);
							}
							RightRot = mc_pMuzzle->GetComponentRotation();
							RightRot.Yaw -= m_fAngleBullet;
							if (!pGM->PopUpBulletFromPool(mc_pMuzzle->GetComponentLocation(), RightRot, m_fBulletSpeed, m_eTag, this, false))
							{
								SpawnProjectile(mc_pMuzzle->GetComponentLocation(), RightRot, m_eTag);
							}
							m_fAngleBullet += m_fAngleBulletIncrement;
						}
					}
					else {
						m_fAngleBulletIncrement = 90.f / m_iBullNum;
						m_fAngleBullet = m_fAngleBulletIncrement;
						FRotator LeftRot,RightRot;
						for (int j = 0; j < (m_iBullNum / 2); j++)
						{
							LeftRot = mc_pMuzzle->GetComponentRotation();
							LeftRot.Yaw += m_fAngleBullet;
							if (!pGM->PopUpBulletFromPool(mc_pMuzzle->GetComponentLocation(), LeftRot, m_fBulletSpeed, m_eTag, this, false))
							{
								SpawnProjectile(mc_pMuzzle->GetComponentLocation(), LeftRot, m_eTag);
							}
							RightRot = mc_pMuzzle->GetComponentRotation();
							RightRot.Yaw -= m_fAngleBullet;
							if (!pGM->PopUpBulletFromPool(mc_pMuzzle->GetComponentLocation(),RightRot, m_fBulletSpeed, m_eTag, this, false))
							{
								SpawnProjectile(mc_pMuzzle->GetComponentLocation(),RightRot, m_eTag);
							}
							m_fAngleBullet += m_fAngleBulletIncrement;
						}
						if (!pGM->PopUpBulletFromPool(mc_pMuzzle->GetComponentLocation(), mc_pMuzzle->GetComponentRotation(), m_fBulletSpeed, m_eTag, this, false))
						{
							SpawnProjectile(mc_pMuzzle->GetComponentLocation(), mc_pMuzzle->GetComponentRotation(), m_eTag);
						}
					}
					break;
				}
			}
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error in Normal enemy: Could not find the UmbraGameMode to shoot projectiles from pool"));
	}
}
void ANormalEnemy::DropObject()
{
	Super::DropObject();
}
void ANormalEnemy::Punch()
{
	if(!m_bIsPunching)
	{
		mc_pPunchAreaDecal->SetVisibility(true);
		m_bIsPunching = true;
		GetWorldTimerManager().SetTimer(m_Timer, this, &ANormalEnemy::OnDelayPunchCastTimeCompleted, m_fCastTime, false);
	}
}
void ANormalEnemy::ReturnDir(const AActor* _Projectile, FVector& Direction_)
{
	FVector newDir = _Projectile->GetActorLocation() - GetActorLocation();
	newDir.Z = 0.f;
	newDir.Normalize();
	Direction_ = newDir;
}

void ANormalEnemy::SpawnProjectile(const FVector& _Position, const FRotator& _Rotation, EBulletModifier& _BulletType)
{
	if (!IsPendingKill() && !m_bIsInKnockBack)
	{
		AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(m_pProjectileClass, _Position, _Rotation);
		if (Projectile)
		{
			Projectile->m_fSpeed = m_fBulletSpeed;
			Projectile->AddTag(_BulletType);
			// TODO: Change back when Player and enemies on C++
			// Projectile->SetOwnerAndMaterial(this);
			Projectile->SetOwnerAndMaterial(false, this);
		}
	}
}

void ANormalEnemy::GetPunchKnockbackDir(const ACharacter* _Player, FVector& Dir_)
{
	Dir_ = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), _Player->GetActorLocation());
	Dir_.Z = 0.f;
}

void ANormalEnemy::ChargeAttack(ACharacter* pChar)
{
	if (m_bCanUseChargeAttack && !m_bIsCharging)
	{
		m_bChargeStart = true;
		m_bIsCharging = true;
		m_bCanUseChargeAttack = false;
		mc_pHealthComp->m_bGodMode = true;
		GetCharacterMovement()->MovementMode = EMovementMode::MOVE_None;
		GetCharacterMovement()->bApplyGravityWhileJumping = false;
		m_fBrakingFrictionFactor = GetCharacterMovement()->BrakingFrictionFactor;
		m_fBrakingDecelerationWalking = GetCharacterMovement()->BrakingDecelerationWalking;
		m_fGravityScale = GetCharacterMovement()->GravityScale;
		GetCharacterMovement()->GravityScale = 0.f;
		GetCharacterMovement()->BrakingFrictionFactor = 0.f;
		GetCharacterMovement()->BrakingDecelerationWalking = 0.f;
		m_fTimeToResetChargeDuration = m_fChargeDuration;
		FHitResult OutHit(ForceInit);
		if (pChar)
		{
			m_vEndPosition = pChar->GetActorLocation();
		}
		else 
		{
			m_vEndPosition = GetActorLocation() + GetActorForwardVector() * m_fChargeSpeed * m_fChargeDuration;
		}
		TArray<AActor*> ToIgnore;
		ToIgnore.Add(this);
		ToIgnore.Add(pChar);
		if ((m_vEndPosition - GetActorLocation()).Size() > m_fMaxChargeDistance || UKismetSystemLibrary::LineTraceSingle(GetWorld(), GetActorLocation(), m_vEndPosition, ETraceTypeQuery::TraceTypeQuery1, false, ToIgnore,
			EDrawDebugTrace::None, OutHit, true))
		{
			m_fTimeToResetChargeDuration = -9999.f;
			m_bChargeWasCancelled = true;
		}
		else
		{
			GetWorld()->LineTraceSingleByChannel(OutHit, m_vEndPosition, m_vEndPosition - FVector(0.f, 0.f, 100.f), ECollisionChannel::ECC_Visibility);
			if (!OutHit.bBlockingHit)
			{
				m_vEndPositionAfterAdjustment = m_vEndPosition + GetActorForwardVector() * m_fChargeDistanceVariation;
				GetWorld()->LineTraceSingleByChannel(OutHit, m_vEndPositionAfterAdjustment, m_vEndPositionAfterAdjustment - FVector(0.f, 0.f, 100.f), ECollisionChannel::ECC_Visibility);
				if (OutHit.bBlockingHit)
				{
					m_fTimeToResetChargeDuration += m_fChargeDistanceVariation / m_fChargeSpeed;
				}
				else
				{
					m_vEndPositionAfterAdjustment = m_vEndPosition - GetActorForwardVector() * m_fChargeDistanceVariation;
					GetWorld()->LineTraceSingleByChannel(OutHit, m_vEndPositionAfterAdjustment, m_vEndPositionAfterAdjustment - FVector(0.f, 0.f, 100.f), ECollisionChannel::ECC_Visibility);
					if (OutHit.bBlockingHit)
					{
						m_fTimeToResetChargeDuration -= m_fChargeDistanceVariation / m_fChargeSpeed;
					}
				}
			}
		}
	}
}

void ANormalEnemy::UpdateCharge(float DeltaTime)
{
	if (m_bChargeStart)
	{
		m_bChargeStart = false;
		GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Walking;
		m_fCurrentWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
		GetCharacterMovement()->MaxWalkSpeed = m_fChargeSpeed;
		GetCharacterMovement()->MaxAcceleration += 100000;
		float fSpeed = (m_vEndPositionAfterAdjustment-GetActorLocation()).Size() / m_fTimeToResetChargeDuration;
		GetCharacterMovement()->Velocity.Set(GetActorForwardVector().X * fSpeed, GetActorForwardVector().Y * fSpeed, 0.f);
		m_vVelocity = GetCharacterMovement()->Velocity;
		if (!m_bChargeWasCancelled)
		{
			LaunchCharacter(m_vVelocity, true, true);
		}
	}
	if (m_fTimeToResetChargeDuration > 0.f)
	{
		m_fTimeToResetChargeDuration -= DeltaTime;

		// Correct final position so the distance traveled by the dash will never be more than the max (duration * speed)
		if (m_fTimeToResetChargeDuration < 0.f)
		{
			const TArray<AActor*> TActorsToIgnore;
			FHitResult FHitResult;
			const FVector vCurrentLocation = GetActorLocation();
			FVector vFinalLocation = vCurrentLocation + GetActorForwardVector() * GetCharacterMovement()->MaxWalkSpeed * m_fTimeToResetChargeDuration;

			if (UKismetSystemLibrary::LineTraceSingle(
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
				vFinalLocation = FHitResult.ImpactPoint - GetActorForwardVector() * GetCapsuleComponent()->GetUnscaledCapsuleRadius();
			}
			SetActorLocation(vFinalLocation);
		}
	}
	
	else if (m_bIsCharging)
	{
		m_bIsCharging = false;

		GetCharacterMovement()->Velocity.Set(0.f, 0.f, 0.f);
		GetCharacterMovement()->MaxWalkSpeed = m_fCurrentWalkSpeed;
		GetCharacterMovement()->MaxAcceleration -= 100000;
		GetCharacterMovement()->bApplyGravityWhileJumping = true;
		GetCharacterMovement()->BrakingFrictionFactor = m_fBrakingFrictionFactor;
		GetCharacterMovement()->BrakingDecelerationWalking = m_fBrakingDecelerationWalking;
		GetCharacterMovement()->GravityScale = m_fGravityScale;
		mc_pHealthComp->m_bGodMode = false;
		m_FOnChargeEnded.Broadcast();
	}

}

void ANormalEnemy::RotateToFacePlayerFunc(float alpha)
{
	SetActorRotation(UKismetMathLibrary::RLerp(m_RCurrent, m_RTarget, alpha, true));
}

void ANormalEnemy::PunchFunc(float alpha)
{
	m_USndManager->PlayEventAtLocation(GetWorld(),m_UPunchFMODEvent,GetActorLocation());
	
	m_fCurrentPunchDistance = UKismetMathLibrary::Clamp(m_fCurrentPunchDistance+m_fPunchSpeed*alpha, 0.0f, m_fPunchRange);
	FVector PunchLocation = GetActorLocation();
	TArray<AActor*> ToIgnore;
	ToIgnore.Add(this);
	TArray<FHitResult> Hits;
	UKismetSystemLibrary::SphereTraceMulti(GetWorld(), PunchLocation, PunchLocation, m_fCurrentPunchDistance, m_EPunchTraceChannel, false, ToIgnore, EDrawDebugTrace::None, Hits, true, FLinearColor::Red, FLinearColor::Green, 0.5f);
	FVector vKnockbackDir;
	for (int i = 0; i < Hits.Num(); i++)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(Hits[i].Actor);
		if (Player)
		{
			GetPunchKnockbackDir(Player, vKnockbackDir);
			Player->ReceiveKnockback(vKnockbackDir, m_fKnockbackTime, m_fPunchSpeed, m_fStunTime);
			//TODO: I need an apply damage function in character
			UGameplayStatics::ApplyDamage(Player, m_fAreaDmg, nullptr, this, nullptr);

		}
	}

	if(m_fCurrentPunchDistance == m_fPunchRange)
	{
		m_bIsPunching = false;
		m_bIsPunchRunning = false;
		mc_pPunchAreaDecal->SetVisibility(false);
	}
}
void ANormalEnemy::SmoothFacePlayer(FRotator& _Current, FRotator& _Target)
{
	m_RCurrent = _Current;
	m_RTarget = _Target;
	m_TLRotateToFacePlayer.PlayFromStart();
}
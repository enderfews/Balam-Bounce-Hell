// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosiveEnemy.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "ProjectUmbra/Components/HealthComponetCpp.h"
#include "ProjectUmbra/Items/TagDrop.h"
#include "ProjectUmbra/System/ProjectUmbraGameInstance.h"
#include "ProjectUmbra/Widgets/HealthBar.h"
#include "ProjectUmbra/System/ProbabilityManager.h"
#include "ProjectUmbra/System/SoundManager.h"

AExplosiveEnemy::AExplosiveEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	//Creates of components and setups the attachment for scene components
	mc_UBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	mc_UBody->SetupAttachment(GetRootComponent());
}

void AExplosiveEnemy::ShootBullet()
{
	Super::ShootBullet();
	//Stops the Behaviour Tree to stop the AI from doing anything
	Cast<AAIController>(GetController())->GetBrainComponent()->StopLogic(TEXT(""));
	m_bExploding = true;
}

void AExplosiveEnemy::DropObject()
{
	Super::DropObject();

	if(m_bHasDropped)
		return;

	m_bHasDropped = true;
	
	if(m_bDropsTag && GetGameInstance<UProjectUmbraGameInstance>()->GetProbabilityManager()->CanSpawnTagDrop())
	{
		FHitResult FHitResult;
		if(UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			GetActorLocation(),
			GetActorLocation() +
			-GetActorUpVector() * 100,
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

void AExplosiveEnemy::ExplosionProgress(float _fTime)
{
	//Updates the explosion countdown
	m_fCurrentTimeToExplode += _fTime;

	float fBlinkFrequency = 0.0f;

	if(m_bExploding)
	{
		fBlinkFrequency = m_fCurrentTimeToExplode/m_fTimeToExplode;
	}
	else
	{
		fBlinkFrequency = m_fCurrentTimeToExplode/m_fTimeToExplodeAfterHit;
	}

	//Updates the visuals
	mc_UBody->SetScalarParameterValueOnMaterials(FName("BlinkFrequency"),fBlinkFrequency);
}

void AExplosiveEnemy::OnCollisionHit(UPrimitiveComponent* _UHitComponent, AActor* _AOtherActor,
	UPrimitiveComponent* _UOtherComp, FVector _vNormalImpulse, const FHitResult& _FHit)
{
	//Tests if the enemy has been bat before Exploding
	if(m_bHit)
	{
		Explode();
	}
}

void AExplosiveEnemy::GetHit_Implementation(FVector _vHitDirection, AActor* _ANewOwner, bool m_bPlayerOwnership, float _fAutoAimRange, float _fAutoAimAngle)
{
	if(!m_bHit)
	{
		OnExplosiveEnemyHit();
		
		m_USndManager->PlayEventAtLocation(GetWorld(), m_UBatFMODEvent, GetActorLocation());
		
		m_bExploding = false;
		m_fCurrentTimeToExplode = 0.f;
		//Stops the Behaviour Tree and disables the Character Movement Component to stop gravity and other undesirable behaviours from happening when becoming a projectile
		Cast<AAIController>(GetController())->GetBrainComponent()->StopLogic(TEXT(""));

		GetCharacterMovement()->SetActive(false);

		//Calculate which target will be auto aimed
		const float fAutoAimCos = UKismetMathLibrary::DegCos(_fAutoAimAngle/2);
	
		_vHitDirection = GetAutoAimDirection(_vHitDirection, _fAutoAimRange, fAutoAimCos);
		m_vHitDirection = _vHitDirection;

		//Sets owner and starts hit behavior
		SetOwner(_ANewOwner);
		m_bHit = true;

		if(m_bDropsHealth || m_bDropsTag)
		{
			DropObject();
		}
	}
}

void AExplosiveEnemy::OnDeathCallback()
{
	Explode();
}

void AExplosiveEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//If in explosion behavior update the explosion time and at the end explodes
	if(m_bExploding)
	{
		if(m_fCurrentTimeToExplode < m_fTimeToExplode)
		{
			ExplosionProgress(DeltaTime);
		} else
		{
			Explode();
		}
	}

	//If in hit behavior updates the position like a linear projectile
	if(m_bHit)
	{
		if(m_fCurrentTimeToExplode < m_fTimeToExplodeAfterHit)
		{
			ExplosionProgress(DeltaTime);
			
			SetActorLocation(
			GetActorLocation() +
			m_vHitDirection * m_fBattedSpeed * DeltaTime,
			true
		);
		} else
		{
			Explode();
		}
		
	}
}

void AExplosiveEnemy::EnableChaseSound(bool _bEnable)
{
	if(_bEnable)
	{
		if(IsValid(m_UChaseAudioComponent))
			m_UChaseAudioComponent->Play();
	}
	else
	{
		if(IsValid(m_UChaseAudioComponent))
			m_UChaseAudioComponent->Stop();
	}
}

void AExplosiveEnemy::BeginPlay()
{
	Super::BeginPlay();

	//Setups HealthComponent
	// mc_pHealthComp->m_fHealth = m_fHealth;

	//Setups callback for collision and damage
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AExplosiveEnemy::OnCollisionHit);

	//Sets the movement speed using the enemy varaible
	UCharacterMovementComponent* UCharacterMovement = GetCharacterMovement();
	UCharacterMovement->MaxWalkSpeed = m_fMoveSpeed;

	m_USndManager = GetGameInstance<UProjectUmbraGameInstance>()->GetSoundManager();
	m_UChaseAudioComponent = m_USndManager->PlayEventAttached(m_UChaseFMODEvent, GetRootComponent(), FVector::ZeroVector, EAttachLocation::KeepRelativeOffset, true, false, false);
}

void AExplosiveEnemy::OnAnyDamage(AActor* _ADamagedActor, float _fDamage, const UDamageType* _UDamageType,
	AController* _AInstigatedBy, AActor* _ADamageCauser)
{
	//Calls the damage method of the HealthComponent
	if(!m_bHit && !m_bExploding)
	{
		Super::OnAnyDamage(_ADamagedActor,_fDamage,_UDamageType,_AInstigatedBy,_ADamageCauser);
	}
}

void AExplosiveEnemy::Explode()
{
	m_USndManager->PlayEventAtLocation(GetWorld(), m_UExplodeFMODEvent, GetActorLocation());

	UNiagaraComponent* m_UExplosionNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),m_UExplosionNiagaraSystem,
			GetActorLocation() + FVector::UpVector * 80.0f,FRotator::ZeroRotator,FVector::OneVector,true,true,
			ENCPoolMethod::AutoRelease, true);
	
	m_UExplosionNiagaraComponent->SetFloatParameter("Scale", m_fExplosionRange/100.0f);
	
	TArray<FHitResult> THitResults;

	TArray<AActor*> TActorsToIgnore = TArray<AActor*>();
	
	
	//Gets all valid targets in the explosion range and damages them
	if(UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		GetActorLocation(),
		GetActorLocation(),
		m_fExplosionRange,
		m_TObjectsDamaged,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::ForDuration,
		THitResults,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		2
	))
	{
		for(const FHitResult& FHitResult : THitResults)
		{
			const float fDamage = m_bExploding ?
				FHitResult.GetActor() == UGameplayStatics::GetPlayerPawn(GetWorld(), 0) ?
					m_fDamageToPlayer : m_fDamageToEnemies :
				m_bHit ? m_fDamageToEnemies : m_fDamageToPlayer;
			UGameplayStatics::ApplyDamage(
				FHitResult.GetActor(),
				fDamage,
				nullptr,
				this,
				nullptr);
		}
	}
	
	OnEnemyDeath();
	DropObject();
	Destroy();
	
	//Dies
	// if (mc_pHealthComp->OnDeath.IsBound())
	// 	mc_pHealthComp->OnDeath.Broadcast();
}

FVector AExplosiveEnemy::GetAutoAimDirection(const FVector& _vCurrentNewDirection, const float _fAutoAimRange, const float _fAutoAimCos) const
{
	//Sets the FinalDirection of auto aim to the original, in case of not finding targets the direction will remain the initial direction
	FVector vFinalNewDirection_ = _vCurrentNewDirection;

	//Gets all the valid targets in the range of auto aim
	//Filters the targets by angle
	//Gets the closest targets that passes al the filters
	TArray<FHitResult> THitResults;
	if(UKismetSystemLibrary::SphereTraceMulti(
		GetWorld(),
		GetActorLocation(),
		GetActorLocation(),
		_fAutoAimRange,
		UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel4),
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::None,
		THitResults,
		true
		))
	{
		FVector vAutoAimPosition = FVector::ZeroVector;
		for(FHitResult FHitResult : THitResults)
		{
			if(vAutoAimPosition.IsZero() &&
				GetInsideDotProductXY(
					_vCurrentNewDirection,
					FHitResult.ImpactPoint - GetActorLocation(),
					_fAutoAimCos
				)
			)
			{
				vAutoAimPosition = FHitResult.ImpactPoint;
				continue;
			}

			if((FVector::DistSquared(GetActorLocation(), FHitResult.ImpactPoint) <
				FVector::DistSquared(GetActorLocation(), vAutoAimPosition)) &&
				GetInsideDotProductXY(
					_vCurrentNewDirection,
					FHitResult.ImpactPoint - GetActorLocation(),
					_fAutoAimCos
				)
			)
			{
				vAutoAimPosition = FHitResult.ImpactPoint;
			}
		}
		
		if(!vAutoAimPosition.IsZero())
		{
			vFinalNewDirection_ = FVector(vAutoAimPosition.X,vAutoAimPosition.Y,0) - FVector(GetActorLocation().X,GetActorLocation().Y,0);
		}
	}
	
	return vFinalNewDirection_.GetSafeNormal();
}

bool AExplosiveEnemy::GetInsideDotProductXY(const FVector _vLookDirection, const FVector _vTestDirection, const float _fThreshold) const
{
	//Looks if a direction is inside an angle using the cosine of half the angle
	const FVector _FLookDirectionNormalized = _vLookDirection.GetSafeNormal();
	const FVector _FTestDirectionNormalized = _vTestDirection.GetSafeNormal();
	const FVector _FLookDirectionXY = FVector(_FLookDirectionNormalized.X,_FLookDirectionNormalized.Y,0);
	const FVector _FTestDirectionXY = FVector(_FTestDirectionNormalized.X, _FTestDirectionNormalized.Y, 0);

	return FVector::DotProduct(_FTestDirectionXY,_FLookDirectionXY) > _fThreshold;
}

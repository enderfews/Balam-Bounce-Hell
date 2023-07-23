#include "AProjectile.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectUmbra/Characters/BaseEnemy.h"
#include "ProjectUmbra/Characters/PlayerCharacter.h"
#include "ProjectUmbra/Components/HealthComponetCpp.h"
#include "ProjectUmbra/System/ProjectUmbraGameInstance.h"
#include "Engine/TriggerBox.h"
#include "ProjectUmbra/System/SoundManager.h"
#include "ProjectUmbra/System/UmbraGameMode.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	
	mc_UCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(mc_UCollision);
	
	mc_UCollisionTwo = CreateDefaultSubobject<USphereComponent>(TEXT("Collision With Player"));
	mc_UCollisionTwo->SetupAttachment(mc_UCollision);

	mc_UMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	mc_UMesh->SetupAttachment(mc_UCollision);

	m_UNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Base Niagara Component"));
	m_UNiagaraComponent->SetupAttachment(mc_UCollision);
}

void AProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//If the Actor is not being renderd it will despawn after 2.0 seconds
	if (!m_bIsInPool)
	{
		m_fCurrentLife += DeltaSeconds;
		if (m_fCurrentLife>=5.f)
		{
			if (IsValid(m_UFlyAudioComponent))
				m_UFlyAudioComponent->Stop();
			//Destroy();
			if (!m_bWasLeavingPool)
			{
				SendBackToPool();
			}
			else {
				m_bWasLeavingPool = false;
			}
			return;
		};


		//Updates the location of the bullet every tick

		if (!m_bInElectricBehaviour)
		{
			SetActorLocation(
				GetActorLocation() +
				GetActorForwardVector() *
				(DeltaSeconds * m_fSpeed),
				true
			);
		}
		else
		{
			if (m_fTimeToNextElectricTarget <= 0.0f && m_iElectricTagBounces <= 0)
			{
				if (mc_ElectricBounceComponent)
				{
					mc_ElectricBounceComponent->Deactivate();
					mc_ElectricBounceComponent->ReleaseToPool();
				}
				/*AUmbraGameMode* mGM = Cast<AUmbraGameMode>(GetWorld()->GetAuthGameMode());
				if (mGM)
				{
					mGM->OnPlayerKilled.RemoveDynamic(this, &AProjectile::RemoveBulletOnPlayerRespawned);
				}*/
				//Destroy();
				SendBackToPool();
			}
			else if (m_fTimeToNextElectricTarget <= 0.0f)
			{
				m_iElectricTagBounces--;
				m_fTimeToNextElectricTarget = m_fElectricTagBounceDurationPerTarget;

				TArray<TEnumAsByte<EObjectTypeQuery>> TObjectsToHit;
				TObjectsToHit.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

				TArray<AActor*> TActorsToIgnore;

				if (m_TEnemiesHitByElectric.Num() > 0)
				{
					AActor* AToIgnore = m_TEnemiesHitByElectric[m_TEnemiesHitByElectric.Num() - 1];
					TActorsToIgnore.Add(AToIgnore);
				}

				TArray<FHitResult> THitResults;

				if (UKismetSystemLibrary::SphereTraceMultiForObjects(
					GetWorld(),
					m_VLastTargetPosition,
					m_VLastTargetPosition,
					m_fElectricTagDetectionRadius,
					TObjectsToHit,
					false,
					TArray<AActor*>(),
					EDrawDebugTrace::None,
					THitResults,
					true,
					FLinearColor::Red, FLinearColor::Green,
					0.1f
				))
				{
					AActor* ACandidate = nullptr;
					FVector VCandidateImpactPoint = FVector::ZeroVector;
					for (const FHitResult& FResult : THitResults) {
						if (!IsValid(ACandidate))
						{
							ACandidate = FResult.GetActor();
							VCandidateImpactPoint = FResult.ImpactPoint;
						}
						else
						{
							FVector VNewCandidatePos = FResult.ImpactPoint;
							AActor* ANewCandidate = FResult.GetActor();

							if (!m_TEnemiesHitByElectric.Contains(ANewCandidate) && m_TEnemiesHitByElectric.Contains(ACandidate))
							{
								ACandidate = FResult.GetActor();
								VCandidateImpactPoint = FResult.ImpactPoint;
							}
							else if (!m_TEnemiesHitByElectric.Contains(ANewCandidate) && !m_TEnemiesHitByElectric.Contains(ACandidate)
								|| m_TEnemiesHitByElectric.Contains(ANewCandidate) && m_TEnemiesHitByElectric.Contains(ACandidate))
							{
								if (FVector::Distance(m_VLastTargetPosition, VNewCandidatePos) <
									FVector::Distance(m_VLastTargetPosition, VCandidateImpactPoint))
								{

									ACandidate = FResult.GetActor();
									VCandidateImpactPoint = FResult.ImpactPoint;
								}
							}
						}
					}

					if (mc_ElectricBounceComponent)
					{
						mc_ElectricBounceComponent->Deactivate();
						mc_ElectricBounceComponent->ReleaseToPool();
					}

					mc_ElectricBounceComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), m_UElectricBounceSystem,
						m_VLastTargetPosition, FRotator::ZeroRotator, FVector::OneVector, true, true,
						ENCPoolMethod::ManualRelease, true);

					m_VLastTargetPosition = FVector(
						ACandidate->GetActorLocation().X,
						ACandidate->GetActorLocation().Y,
						GetActorLocation().Z
					);

					mc_ElectricBounceComponent->SetVectorParameter("End", m_VLastTargetPosition);

					DoDamage(ACandidate);

					if (IsValid(ACandidate))
					{
						m_TEnemiesHitByElectric.Add(ACandidate);
						TActorsToIgnore.Empty();
						TActorsToIgnore.Add(ACandidate);
					}
				}
				else
				{
					if (mc_ElectricBounceComponent)
					{
						mc_ElectricBounceComponent->Deactivate();
						mc_ElectricBounceComponent->ReleaseToPool();
					}
					/*AUmbraGameMode* mGM = Cast<AUmbraGameMode>(GetWorld()->GetAuthGameMode());
					if (mGM)
					{
						mGM->OnPlayerKilled.RemoveDynamic(this, &AProjectile::RemoveBulletOnPlayerRespawned);
					}*/
					//Destroy();
					SendBackToPool();
				}
			}
			else
			{
				m_fTimeToNextElectricTarget -= DeltaSeconds;
			}
		}
	}
}

void AProjectile::RemoveBulletOnPlayerRespawned()
{
	/*AUmbraGameMode* mGM = Cast<AUmbraGameMode>(GetWorld()->GetAuthGameMode());
	if (mGM)
	{
		mGM->OnPlayerKilled.RemoveDynamic(this, &AProjectile::RemoveBulletOnPlayerRespawned);
	}*/
	if(IsValid(m_UFlyAudioComponent))
		m_UFlyAudioComponent->Stop();
	SendBackToPool();
	//Destroy();
}

void AProjectile::SendBackToPool()
{
	if (!m_bIsInPool)
	{
		m_fSpeed = 0.f;
		m_fCurrentLife = 0.f;
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		m_bInElectricBehaviour = false;
		m_UFlyAudioComponent->Stop();
		m_EModifiers.Empty();
		AddTag(EBulletModifier::Normal);
		SetOwnerAndMaterial(false, nullptr);
		if (mc_ElectricBounceComponent)
		{
			mc_ElectricBounceComponent->Deactivate();
			mc_ElectricBounceComponent->ReleaseToPool();
		}
		SetActorTickEnabled(false);
		//m_EModifiers.Add(EBulletModifier::Normal);
		AUmbraGameMode* pGM = Cast<AUmbraGameMode>(GetWorld()->GetAuthGameMode());
		if (pGM)
		{
			pGM->m_TBulletPool.Add(this);
			m_bIsInPool = true;
			if(pGM->m_pBulletPoolBounds)
				SetActorLocation(pGM->m_pBulletPoolBounds->GetActorLocation());
		}
		else {
			#ifdef WITH_EDITOR
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Error in projectile:Could not send back to the pool, please check the current game mode is UmbraGameMode or the pool bounds is set"));
			#endif 
			UE_LOG(LogTemp, Error, TEXT("Error in projectile:Could not send back to the pool, please check the current game mode is UmbraGameMode or the pool bounds is set"));
		}
	}
}

void AProjectile::PlayFlySound()
{
	if (IsValid(m_UFlyAudioComponent))
	{
		m_UFlyAudioComponent->Play();
	}
	else {
		m_UFlyAudioComponent = m_USndManager->PlayEventAttached(m_UFlyFMODEvent, GetRootComponent(), FVector::ZeroVector, EAttachLocation::KeepRelativeOffset, true, true, false);
	}
}


void AProjectile::AddTag(EBulletModifier _ETagToAdd)
{
	//If tag is None or already in array returns
	if(_ETagToAdd == EBulletModifier::None
		|| m_EModifiers.Contains(_ETagToAdd))
		return;

	//If there are 2 tags put the second one on the first position and the new one on the second
	if(m_EModifiers.Num() == 2)
	{
		m_EModifiers[0] = m_EModifiers[1];
		m_EModifiers[1] = _ETagToAdd;
	}

	//If there is only 1 tag add it to the second position
	if(m_EModifiers.Num() < 2)
	{
		m_EModifiers.Add(_ETagToAdd);
	}

	//Get the bullet data from the BulletLUT
	if(m_UBulletLUT != nullptr)
	{
		 m_UBulletLUT->GetBulletData(GetTagsUint(), m_FBulletData);
		if (m_bPlayerOwnership)
			if (m_FBulletData.m_UPlayerNSystem)
			{
				m_UNiagaraComponent->SetAsset(m_FBulletData.m_UPlayerNSystem);
				m_UNiagaraComponent->Activate(true);
			}
		else
			if (m_FBulletData.m_UEnemyNSystem)
			{
				m_UNiagaraComponent->SetAsset(m_FBulletData.m_UEnemyNSystem);  // TODO: Fix get from LUT
				m_UNiagaraComponent->Activate(true);
			}
		
	}	
	
}

void AProjectile::OnCollisionHit(
	UPrimitiveComponent* _UHitComponent,
	AActor* _AOtherActor,
	UPrimitiveComponent* _UOtherComp,
	FVector _vNormalImpulse,
	const FHitResult& _FHit)
{
	//UE_LOG(LogTemp, Error, TEXT("Projectile: Collision Hit"))
	//If it has bounces, bounce
	if(m_FBulletData.m_iNumBounces > 0)
	{
		--m_FBulletData.m_iNumBounces;
		m_USndManager->PlayEventAtLocation(GetWorld(), m_UBounceFMODEvent, GetActorLocation());
		BounceOnWall(_FHit);
		return;
	}

	if(IsValid(m_UFlyAudioComponent))
		m_UFlyAudioComponent->Stop();
	
	//If it has area of effect Explode
	if(m_FBulletData.m_fAreaOfEffect > 0)
	{
		DoDamage(nullptr);
	}
	/*AUmbraGameMode* mGM = Cast<AUmbraGameMode>(GetWorld()->GetAuthGameMode());
	if (mGM)
	{
		mGM->OnPlayerKilled.RemoveDynamic(this, &AProjectile::RemoveBulletOnPlayerRespawned);
	}*/
	m_USndManager->PlayEventAtLocation(GetWorld(), m_UDestroyFMODEvent, GetActorLocation());
	SendBackToPool();
	//Destroy();
}

void AProjectile::OnCollisionOverlap(
	UPrimitiveComponent* _UOverlappedComponent,
	AActor* _AOtherActor,
	UPrimitiveComponent* _UOtherComp,
	int32 _iOtherBodyIndex,
	bool _bFromSweep,
	const FHitResult& _FSweepResult)
{
	if(!m_bPlayerOwnership)
	{
		return;
	}
	
	//If actor is invalid destroy bullet
	//TODO:check if this is the wanted behavior
	if(!IsValid(_AOtherActor))
	{
		/*AUmbraGameMode* mGM = Cast<AUmbraGameMode>(GetWorld()->GetAuthGameMode());
		if (mGM)
		{
			mGM->OnPlayerKilled.RemoveDynamic(this, &AProjectile::RemoveBulletOnPlayerRespawned);
		}*/
		SendBackToPool();
		//Destroy();
		return;
	}

	//Checks if the overlapped component is the rally capsule to ignore it
	FString sName;
	_UOtherComp->GetName(sName);
	if(sName == "Rally Capsule")
	{
		return;
	}

	if(m_FBulletData.m_bCanStun)
	{
		ABaseEnemy* AHitEnemy = Cast<ABaseEnemy>(_AOtherActor);
		if(AHitEnemy){
			if(!AHitEnemy->mc_pHealthComp->GetImmune())
			{
				AHitEnemy->KnockBack(FVector::ZeroVector, 0.0f, 0.0f, m_fStunDuration);
			}
		}
	}

	m_USndManager->PlayEventAtLocation(GetWorld(), m_UDestroyFMODEvent, GetActorLocation());
	if(m_FBulletData.m_bCanTrack)
	{
		m_bInElectricBehaviour = true;
		SetActorEnableCollision(false);
		//mc_UCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//mc_UCollisionTwo->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		m_UNiagaraComponent->Deactivate();
		// mc_UMesh->SetVisibility(false);

		m_VLastTargetPosition =FVector(
			_AOtherActor->GetActorLocation().X,
			_AOtherActor->GetActorLocation().Y,
			GetActorLocation().Z);

		DoDamage(_AOtherActor);

		if(IsValid(m_UFlyAudioComponent))
			m_UFlyAudioComponent->Stop();
		
		return;
	}
	
	//If target is valid damage it
	// if(TargetIsValid(_AOtherActor))
	// {
	if(IsValid(m_UFlyAudioComponent))
		m_UFlyAudioComponent->Stop();
	
	DoDamage(_AOtherActor);
	/*AUmbraGameMode* mGM = Cast<AUmbraGameMode>(GetWorld()->GetAuthGameMode());
	if (mGM)
	{
		mGM->OnPlayerKilled.RemoveDynamic(this, &AProjectile::RemoveBulletOnPlayerRespawned);
	}*/
	SendBackToPool();
	//Destroy();
	// }
}

//TODO: Remove when player and enemies on C++
void AProjectile::OnCollisionOverlapPlayer(UPrimitiveComponent* _UOverlappedComponent, AActor* _AOtherActor,
	UPrimitiveComponent* _UOtherComp, int32 _iOtherBodyIndex, bool _bFromSweep, const FHitResult& _FSweepResult)
{
	if(m_bPlayerOwnership)
	{
		return;
	}
	
	//If actor is invalid destroy bullet
	//TODO:check if this is the wanted behavior
	if(!IsValid(_AOtherActor))
	{
		/*AUmbraGameMode* mGM = Cast<AUmbraGameMode>(GetWorld()->GetAuthGameMode());
		if (mGM)
		{
			mGM->OnPlayerKilled.RemoveDynamic(this, &AProjectile::RemoveBulletOnPlayerRespawned);
		}*/
		if(IsValid(m_UFlyAudioComponent))
			m_UFlyAudioComponent->Stop();
		//Destroy();
		SendBackToPool();
		return;
	}

	//Checks if the overlapped component is the rally capsule to ignore it
	FString sName;
	_UOtherComp->GetName(sName);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, sName);
	if(sName == "Rally Capsule")
	{
		return;
	}

	APlayerCharacter* AHitPlayer = Cast<APlayerCharacter>(_AOtherActor);

	if(!AHitPlayer || AHitPlayer->GetIsDashing())
	{
		return;
	}

	if(m_FBulletData.m_bCanStun)
	{
		if(!AHitPlayer->GetHealthComponent()->GetImmune())
		{
			AHitPlayer->ReceiveSlow(m_fSlowDuration, m_fSlowAmount);
		}
		// AHitEnemy->KnockBack(FVector::ZeroVector, 0.0f, 0.0f, m_fStunDuration);
	}

	if(IsValid(m_UFlyAudioComponent))
		m_UFlyAudioComponent->Stop();

	m_USndManager->PlayEventAtLocation(GetWorld(), m_UDestroyFMODEvent, GetActorLocation());
	DoDamage(_AOtherActor);
	/*AUmbraGameMode* mGM = Cast<AUmbraGameMode>(GetWorld()->GetAuthGameMode());
	if (mGM)
	{
		mGM->OnPlayerKilled.RemoveDynamic(this, &AProjectile::RemoveBulletOnPlayerRespawned);
	}*/
	SendBackToPool();
	//Destroy();
}

//TODO: Return to old behaviour when player and enemies on C++
// void AProjectile::SetOwnerAndMaterial(AActor* _ANewOwner)
// {
// 	//If actor is invalid ignore the owner change
// 	if(!IsValid(_ANewOwner))
// 	{
// 		return;
// 	}
// 	
// 	SetOwner(_ANewOwner);
//
// 	//Sets the material based on the new owner being the player or an enemy
// 	APawn* APlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
// 	
// 	mc_UMesh->SetMaterial(0, APlayerPawn == _ANewOwner ? m_UPlayerMaterial : m_UEnemyMaterial);
// }

void AProjectile::SetOwnerAndMaterial(bool _bPlayerOwnership, AActor* _ANewOwner)
{
	m_bPlayerOwnership = _bPlayerOwnership;
	SetOwner(_ANewOwner);
	if(_bPlayerOwnership)
	{
		mc_UCollision->SetActive(false);
		mc_UCollisionTwo->SetActive(true);
		if(m_FBulletData.m_UPlayerNSystem)
		{
			m_UNiagaraComponent->SetAsset(m_FBulletData.m_UPlayerNSystem); // TODO: Fix get from LUT
			m_UNiagaraComponent->Activate(true);
		}
	} else
	{
		mc_UCollision->SetActive(true);
		mc_UCollisionTwo->SetActive(false);
		if(m_FBulletData.m_UEnemyNSystem)
		{
			m_UNiagaraComponent->SetAsset(m_FBulletData.m_UEnemyNSystem); // TODO: Fix get from LUT
			m_UNiagaraComponent->Activate(true);
		}
	}
	
	// mc_UMesh->SetMaterial(0, _bPlayerOwnership ? m_UPlayerMaterial : m_UEnemyMaterial);
}


void AProjectile::GetHit_Implementation(FVector _vHitDirection, AActor* _NewOwner, bool _bPlayerOwnership, float _fAutoAimRange, float _AutoAimAngle)
{
	m_USndManager->PlayEventAtLocation(GetWorld(), m_UBatFMODEvent, GetActorLocation());

	if(_bPlayerOwnership)
		OnBulletHit();
	
	//Sets the owner after the hit
	SetOwnerAndMaterial(_bPlayerOwnership, _NewOwner);

	//Saves the values for the auto aim
	m_fAutoAimRange = _fAutoAimRange;
	m_fAutoAimAngle = _AutoAimAngle;
	m_fAutoAimCos = UKismetMathLibrary::DegCos(m_fAutoAimAngle/2);

	//Calculates new direction and rotates acordingly
	const FVector vDirection = GetAutoaimDirection(_vHitDirection);

	SetActorRotation(FindLookAtRotationXY(GetActorLocation(),GetActorLocation() + vDirection));
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	m_EModifiers.Empty();
	AddTag(EBulletModifier::Normal);

	//Setups callbacks
	mc_UCollision->OnComponentHit.AddDynamic(this, &AProjectile::OnCollisionHit);
	mc_UCollision->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnCollisionOverlap);

	mc_UCollisionTwo->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnCollisionOverlapPlayer);
	AUmbraGameMode* mGM = Cast<AUmbraGameMode>(GetWorld()->GetAuthGameMode());
	if (mGM)
	{
		mGM->OnPlayerKilled.AddDynamic(this, &AProjectile::RemoveBulletOnPlayerRespawned);
	}

	m_USndManager = GetGameInstance<UProjectUmbraGameInstance>()->GetSoundManager();
	m_UFlyAudioComponent = m_USndManager->PlayEventAttached(m_UFlyFMODEvent, GetRootComponent(), FVector::ZeroVector, EAttachLocation::KeepRelativeOffset, true, true, false);
}

uint8 AProjectile::GetTagsUint()
{
	uint8 uSum_ = 0;
	for(EBulletModifier BulletModifier : m_EModifiers)
	{
		uSum_ += static_cast<uint8>(BulletModifier);
	}
	return uSum_;
}

void AProjectile::BounceOnWall(const FHitResult _FHitResult)
{
	//Effect for bounces
	if(m_UBounceNiagaraSystem != nullptr)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),m_UBounceNiagaraSystem,
			_FHitResult.ImpactPoint + _FHitResult.ImpactNormal*1,
			UKismetMathLibrary::MakeRotFromZ(_FHitResult.ImpactNormal),
			FVector::OneVector,true,true,
			ENCPoolMethod::AutoRelease, true);
		
		
	}

	//Vector reflection math
	const float fDotForwardN = FVector::DotProduct(GetActorForwardVector(),_FHitResult.ImpactNormal);

	const FVector vReflectedForward = GetActorForwardVector() - (_FHitResult.ImpactNormal * (fDotForwardN*2));

	//Rotate actor on XY plane
	SetActorRotation(FindLookAtRotationXY(GetActorLocation(),GetActorLocation() + vReflectedForward));
}

void AProjectile::DoDamage(AActor* _ATargetHitted)
{
	//Damages the actor hit by the bullet and saves it to be excluded from the explosion damage
	TArray<AActor*> TAlreadyHit;
	if(_ATargetHitted != nullptr)
	{
		UGameplayStatics::ApplyDamage(
			_ATargetHitted,
			m_FBulletData.m_fDamage,
			nullptr,
			this,
			nullptr);
		TAlreadyHit.Add(_ATargetHitted);

		//Small hit effect
		if(m_UHitParticleSystem != nullptr)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),m_UHitParticleSystem,
			GetActorLocation(),FRotator::ZeroRotator,FVector::OneVector,true,true,
			ENCPoolMethod::AutoRelease, true);
		}
	}

	//If area of effect is greater than 0 trace for all enemies or the player (based on owner) and damage them
	if(m_FBulletData.m_fAreaOfEffect > 0)
	{
		m_USndManager->PlayEventAtLocation(GetWorld(), m_UExplodeFMODEvent, GetActorLocation());
		
		TArray<FHitResult> THitResults;
		if(m_UExplosionNiagaraSystem != nullptr)
		{
			UNiagaraComponent* m_UExplosionNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),m_UExplosionNiagaraSystem,
			GetActorLocation(),FRotator::ZeroRotator,FVector::OneVector,true,true,
			ENCPoolMethod::AutoRelease, true);
			
			m_UExplosionNiagaraComponent->SetFloatParameter("Scale", m_FBulletData.m_fAreaOfEffect/100.0f);
		}
		if(UKismetSystemLibrary::SphereTraceMultiForObjects(
			GetWorld(),
			GetActorLocation(),
			GetActorLocation(),
			m_FBulletData.m_fAreaOfEffect,
			GetOwner() == UGameplayStatics::GetPlayerPawn(GetWorld(),0) ? m_TObjectsDamagedByPlayerExplosion : m_TObjectsDamagedByEnemyExplosion,
			false,
			TAlreadyHit,
			EDrawDebugTrace::ForDuration,
			THitResults,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			2)
		)
		{
			for(FHitResult FHitResult : THitResults)
			{
				UGameplayStatics::ApplyDamage(
				FHitResult.GetActor(),
				m_FBulletData.m_fDamage,
				nullptr,
				this,
				nullptr);

				OnBulletExplode();
			}
		}
	}
}

bool AProjectile::TargetIsValid(const AActor* _AHittedActor) const
{
	APawn* APlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(),0);
	
	return (APlayerPawn == _AHittedActor && GetOwner() != APlayerPawn) || (APlayerPawn != _AHittedActor && GetOwner() == APlayerPawn);
}

FVector AProjectile::GetAutoaimDirection(const FVector _vCurrentNewDirection) const
{
	//If owner not player, don't calculate autoaim direction
	// if(GetOwner() != UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	// {
	// 	return _vCurrentNewDirection;
	// }
	if(!m_bPlayerOwnership)
	{
		return _vCurrentNewDirection;
	}

	//DrawDebugs(_vCurrentNewDirection);

	//Sets the final direction to the initial one in case no auto aim target is found
	FVector vFinalNewDirection_ = _vCurrentNewDirection;
	TArray<FHitResult> THitResults;
	
	//Search all auto aim targets on range and use the hit point of the trace to calculate who is the nearest to the bullet
	if(UKismetSystemLibrary::SphereTraceMulti(
		GetWorld(),
		GetActorLocation(),
		GetActorLocation(),
		m_fAutoAimRange,
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
					m_fAutoAimCos
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
					m_fAutoAimCos
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
	
	return vFinalNewDirection_;
}

FRotator AProjectile::FindLookAtRotationXY(const FVector _vFrom, const FVector _vTo) const
{
	const FVector vFromXY = FVector(_vFrom.X,_vFrom.Y,0);

	const FVector vToXY = FVector(_vTo.X,_vTo.Y,0);

	return UKismetMathLibrary::FindLookAtRotation(vFromXY,vToXY);
}

void AProjectile::DrawDebugs(FVector _vDirection) const 
{
	FVector vSegementStart = GetActorLocation();
	FVector vSegementEnd = FVector::ZeroVector;

	FLinearColor FLineColor(1,0.41,0,1);
	
	GetDebugForwardSegment(-1,_vDirection,vSegementEnd);
	UKismetSystemLibrary::DrawDebugLine(GetWorld(), vSegementStart, vSegementEnd,FLineColor,2,2);

	GetDebugForwardSegment(0,_vDirection,vSegementEnd);
	UKismetSystemLibrary::DrawDebugLine(GetWorld(), vSegementStart, vSegementEnd,FLineColor,2,2);

	GetDebugForwardSegment(1,_vDirection,vSegementEnd);
	UKismetSystemLibrary::DrawDebugLine(GetWorld(), vSegementStart, vSegementEnd,FLineColor,2,2);

	for(uint8 i = 0; i < m_fAutoAimAngle; ++i)
	{
		GetDebugArcSegment(i,_vDirection,vSegementStart,vSegementEnd);
		UKismetSystemLibrary::DrawDebugLine(GetWorld(), vSegementStart, vSegementEnd,FLineColor,2,2);
	}
}

void AProjectile::GetDebugForwardSegment(float _fSide, FVector _vDirection, FVector& _vSegmentEnd_) const
{
	const FVector vRotatedForward = _vDirection.RotateAngleAxis((m_fAutoAimAngle/2)*_fSide,FVector::UpVector);
	
	_vSegmentEnd_ = FVector(
		GetActorLocation().X+vRotatedForward.X*m_fAutoAimRange,
		GetActorLocation().Y+vRotatedForward.Y*m_fAutoAimRange,
		GetActorLocation().Z
		);
}

void AProjectile::GetDebugArcSegment(uint8 _uIndex, FVector _vDirection, FVector& _vSegmentStart_, FVector& _vSegmentEnd_) const
{
	float fAngle = (((m_fAutoAimAngle/2.f)*-1.f)+static_cast<uint8>(_uIndex));
	const FVector vRotatedForwardStart = _vDirection.RotateAngleAxis(fAngle,FVector::UpVector);

	_vSegmentStart_ = FVector(
		GetActorLocation().X+vRotatedForwardStart.X*m_fAutoAimRange,
		GetActorLocation().Y+vRotatedForwardStart.Y*m_fAutoAimRange,
		GetActorLocation().Z
		);

	fAngle = (((m_fAutoAimAngle/2.f)*-1.f)+(_uIndex+1.f));
	const FVector vRotatedForwardEnd = _vDirection.RotateAngleAxis(fAngle,FVector::UpVector);

	_vSegmentEnd_ = FVector(
		GetActorLocation().X+vRotatedForwardEnd.X*m_fAutoAimRange,
		GetActorLocation().Y+vRotatedForwardEnd.Y*m_fAutoAimRange,
		GetActorLocation().Z
		);
}

bool AProjectile::GetInsideDotProductXY(const FVector _vLookDirection, const FVector _vTestDirection, const float _fThreshold) const
{
	const FVector _vLookDirectionNormalized = _vLookDirection.GetSafeNormal();
	const FVector _vTestDirectionNormalized = _vTestDirection.GetSafeNormal();
	const FVector _vLookDirectionXY = FVector(_vLookDirectionNormalized.X,_vLookDirectionNormalized.Y,0);
	const FVector _vTestDirectionXY = FVector(_vTestDirectionNormalized.X,_vTestDirectionNormalized.Y, 0);

	return FVector::DotProduct(_vTestDirectionXY,_vLookDirectionXY) > _fThreshold;
}

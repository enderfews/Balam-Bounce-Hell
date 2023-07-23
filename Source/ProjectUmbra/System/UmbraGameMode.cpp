// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/System/UmbraGameMode.h"
#include "UmbraGameMode.h"
#include "ProjectUmbra/Characters/PlayerCharacter.h"
#include "ProjectUmbra/Components/HealthComponetCpp.h"
#include "TimerManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/TriggerBox.h"
#include "ProjectUmbra/Items/Checkpoint.h"
#include "ProjectUmbra/Items/MovilePlatform.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Camera/CameraActor.h"
#include "NavigationSystem.h"
#include "Components/SceneComponent.h"
#include "ProjectUmbra/AI/SmartObjects/SplineMovementCurve.h"
#include "ProjectUmbra/Items/AProjectile.h"
#include "ProjectUmbra/System/SoundManager.h"
//#include "Camera/CameraComponent.h"

AUmbraGameMode::AUmbraGameMode():m_TCheckpointRegister(TArray<ACheckpoint*>()),m_pCheckpoint(nullptr),m_vDefaultLoc(FVector::ZeroVector),
m_fDefaultRespawnTime(2.f), m_bInstaRespawnWhenFalling(false), m_fRemoveControlsAfterPlayerFallsIntoAbyss(0.5f),m_fRemoveControlsAfterRespawnDuration(4.f),m_TBulletPool(TArray<AProjectile*>()),m_iInitialBulletAmount(40), m_pBulletPoolBounds(nullptr), m_fCurrentRespawnTime(0.f), m_bCurrentDeathByDeathZone(false), m_bRespawnWasCalled(false), m_bCurrentScreenFade(false),
m_pCurrentChar(nullptr), m_vCameraLoc(FVector::ZeroVector)
{
	PrimaryActorTick.bCanEverTick = false;
}

void AUmbraGameMode::RespawnCharacter(ACharacter* _Char, bool bDeathByDeathZone)
{
	UE_LOG(LogTemp, Error, TEXT("RespawnCharacter function called from Gamemode: %p"),_Char);
	if (!m_bRespawnWasCalled)
	{
		m_pCurrentChar = _Char;
		m_bCurrentDeathByDeathZone = bDeathByDeathZone;
		m_bRespawnWasCalled = true;
		APlayerCharacter* pPlayer = Cast<APlayerCharacter>(m_pCurrentChar);
		if (m_pCurrentChar)
		{
			if (pPlayer && m_bInstaRespawnWhenFalling && bDeathByDeathZone && m_pCheckpoint && m_pCheckpoint->PlayerIsFarFromCheckpoint())
			{
				if (pPlayer->mc_UHealth->m_fHealth > 1.f)
				{
					pPlayer->mc_UHealth->TakeDamage(1.f,true);
					pPlayer->SetActorLocation(m_vDefaultLoc, false, nullptr, ETeleportType::ResetPhysics);
					pPlayer->RemoveControls(m_fRemoveControlsAfterPlayerFallsIntoAbyss);
				}
				else {
					if(bDeathByDeathZone)
						pPlayer->mc_UHealth->TakeDamage(1.f,true);
					
					if (OnPlayerKilled.IsBound())
						OnPlayerKilled.Broadcast();
					UE_LOG(LogTemp, Error, TEXT("RespawnCharacter function called from Gamemode: Player died by deathzone"));
					StopEnemiesLogic();
					if (m_bCurrentScreenFade)
					{
						UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(0.f, 1.f, m_fCurrentRespawnTime / 2.f, FLinearColor::Black, true, true);
					}
					pPlayer->RemoveControls(m_fRemoveControlsAfterRespawnDuration);
					m_pCurrentChar->GetCharacterMovement()->MovementMode = EMovementMode::MOVE_None;
					GetWorldTimerManager().SetTimer(m_FRespawnTimer, this, &AUmbraGameMode::SpawnCharacter, m_fCurrentRespawnTime, false);
				}
				m_bRespawnWasCalled = false;
			}
			else {
				if (pPlayer)
				{
					if (pPlayer->mc_UHealth->m_fHealth > 1.f )
					{
						UE_LOG(LogTemp, Error, TEXT("RespawnCharacter function called from Gamemode: Player lose 1 HP"));
						pPlayer->mc_UHealth->TakeDamage(1.f);
						pPlayer->SetActorLocation(m_vDefaultLoc, false, nullptr, ETeleportType::ResetPhysics);
						pPlayer->RemoveControls(m_fRemoveControlsAfterPlayerFallsIntoAbyss);
					}else
					{
						UE_LOG(LogTemp, Error, TEXT("RespawnCharacter function called from Gamemode: Player died not by deathzone"));
						if(bDeathByDeathZone)
							pPlayer->mc_UHealth->TakeDamage(1.f);
						
						if (OnPlayerKilled.IsBound())
							OnPlayerKilled.Broadcast();
						StopEnemiesLogic();
						if (m_bCurrentScreenFade)
						{
							UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(0.f, 1.f, m_fCurrentRespawnTime / 2.f, FLinearColor::Black, true, true);
						}
						pPlayer->RemoveControls(m_fRemoveControlsAfterRespawnDuration);
						m_pCurrentChar->GetCharacterMovement()->MovementMode = EMovementMode::MOVE_None;
						GetWorldTimerManager().SetTimer(m_FRespawnTimer, this, &AUmbraGameMode::SpawnCharacter, m_fCurrentRespawnTime, false);
					}
					m_bRespawnWasCalled = false;
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Error in UmbraGameMode: Something went wrong during respawn, player was nullptr"));
		}
	}
}

void AUmbraGameMode::SpawnCharacter()
{
	APlayerCharacter* pPlayer = Cast<APlayerCharacter>(m_pCurrentChar);
	if (pPlayer)
	{
		if (m_pCheckpoint)
		{
			if (m_pCheckpoint->PlayerIsFarFromCheckpoint() && m_bCurrentDeathByDeathZone)
			{
				if (pPlayer->mc_UHealth->m_fHealth > 1.f)
				{
					pPlayer->mc_UHealth->TakeDamage(1.f,true);
					pPlayer->SetActorLocation(m_vDefaultLoc, false, nullptr, ETeleportType::ResetPhysics);
					pPlayer->m_ACamera->SetActorLocation(m_vCameraLoc,false,nullptr,ETeleportType::ResetPhysics);
					pPlayer->RemoveControls(m_fRemoveControlsAfterPlayerFallsIntoAbyss);
				}
				else {
					UE_LOG(LogTemp, Error, TEXT("SpawnCharacter function called from Gamemode: Player died by deathzone"));
					m_vDefaultLoc = m_pCheckpoint->mc_URespawnPoint->GetComponentLocation();
					pPlayer->mc_UHealth->SetHealth(m_pCheckpoint->m_fSavedHealth);
					pPlayer->SetActorLocation(m_pCheckpoint->mc_URespawnPoint->GetComponentLocation(), false, nullptr, ETeleportType::ResetPhysics);
					pPlayer->m_ACamera->SetActorLocation(m_vCameraLoc, false, nullptr, ETeleportType::ResetPhysics);
					pPlayer->RemoveControls(1.f);
					// pPlayer->mc_UHealth->MakeInmmune();
				}

			}
			else {
				UE_LOG(LogTemp, Error, TEXT("SpawnCharacter function called from Gamemode: Player died NOT by deathzone"));
				m_vDefaultLoc = m_pCheckpoint->mc_URespawnPoint->GetComponentLocation();
				pPlayer->mc_UHealth->SetHealth(m_pCheckpoint->m_fSavedHealth);
				pPlayer->SetActorLocation(m_pCheckpoint->mc_URespawnPoint->GetComponentLocation(), false, nullptr, ETeleportType::ResetPhysics);
				pPlayer->m_ACamera->SetActorLocation(m_vCameraLoc, false, nullptr, ETeleportType::ResetPhysics);
				pPlayer->RemoveControls(1.f);
				// pPlayer->mc_UHealth->MakeInmmune();
			}
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("SpawnCharacter function called from Gamemode: Player died without checkpoint"));
			pPlayer->mc_UHealth->SetHealth(pPlayer->mc_UHealth->m_fMaxHealth);
			pPlayer->SetActorLocation(m_vDefaultLoc, false, nullptr, ETeleportType::ResetPhysics);
			pPlayer->RemoveControls(1.f);
			// pPlayer->mc_UHealth->MakeInmmune();
		}
		GetWorldTimerManager().SetTimer(m_FResetTimer, this, &AUmbraGameMode::ResetAllCheckpoints,1.f, false);
		m_pCurrentChar->GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Walking;
		m_bRespawnWasCalled = false;
		if (m_bCurrentScreenFade)
		{
			UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(1.f, 0.f, m_fCurrentRespawnTime , FLinearColor::Black, true, true);
		}
	}
}

void AUmbraGameMode::CreateInitialBullets()
{
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AProjectile* pProjectile = nullptr;
	for (int i = 0; i < m_iInitialBulletAmount; ++i)
	{
		pProjectile=GetWorld()->SpawnActor<AProjectile>(m_TBulletClass, m_pBulletPoolBounds->GetActorLocation(),m_pBulletPoolBounds->GetActorRotation(), Params);
		if (pProjectile)
		{
			pProjectile->m_fSpeed=0.f;
			pProjectile->SetActorHiddenInGame(true);
			pProjectile->SetActorEnableCollision(false);
			pProjectile->SetActorTickEnabled(false);
			pProjectile->m_bIsInPool = true;
			pProjectile->SetOwnerAndMaterial(false, nullptr);
			m_TBulletPool.Add(pProjectile);
		}

	}
}

AProjectile* AUmbraGameMode::PopUpBulletFromPool(const FVector& vLocation, const FRotator& rRotation, float fSpeed, EBulletModifier eModifier, AActor* pOwner, bool bPlayerOwnerShip)
{
	if (m_TBulletPool.Num() == 0)
	{
		if (OnBulletPoolEmpty.IsBound())
			OnBulletPoolEmpty.Broadcast();
		return nullptr;
	}
	else if (!pOwner || pOwner->IsPendingKill())
	{
		return nullptr;
	}
	else {
		AProjectile* pProjectile = m_TBulletPool.Last();
		m_TBulletPool.Remove(pProjectile);
		pProjectile->m_fCurrentLife = 0.f;
		pProjectile->m_bWasLeavingPool = true;
		pProjectile->m_bIsInPool = false;
		pProjectile->AddTag(eModifier);
		pProjectile->SetOwnerAndMaterial(bPlayerOwnerShip, pOwner);
		pProjectile->SetActorLocation(vLocation);
		pProjectile->SetActorRotation(rRotation);
		pProjectile->m_fSpeed = fSpeed;
		pProjectile->SetActorHiddenInGame(false);
		pProjectile->SetActorEnableCollision(true);
		pProjectile->SetActorTickEnabled(true);
		pProjectile->PlayFlySound();
		return pProjectile;
	}
}

void AUmbraGameMode::BeginPlay()
{
	Super::BeginPlay();
	m_fCurrentRespawnTime = m_fDefaultRespawnTime;
	GetWorldTimerManager().SetTimer(m_FTimer, this, &AUmbraGameMode::StorePlayerLocationToRespawn, 0.2f, true);

}

void AUmbraGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AUmbraGameMode::ResetAllCheckpoints()
{

	for (int i = 0; i < m_TCheckpointRegister.Num(); ++i)
	{
		m_TCheckpointRegister[i]->ResetEnemies();
	}
	//UE_LOG(LogTemp,Error,TEXT("Enemies reset on checkpoints"))
	if (OnPlayerRespawned.IsBound())
		OnPlayerRespawned.Broadcast();
}

void AUmbraGameMode::StorePlayerLocationToRespawn()
{
	if (m_pCheckpoint)
	{
		if (m_pCheckpoint->PlayerIsFarFromCheckpoint())
		{
			AActor* pPlayer = m_pCheckpoint->GetOwner();
			FVector vLoc = FVector::ZeroVector;
			if (UNavigationSystemV1::K2_ProjectPointToNavigation(GetWorld(),pPlayer->GetActorLocation(),vLoc,nullptr,nullptr))
			{
				FVector vEnd = pPlayer->GetActorLocation() + (pPlayer->GetActorUpVector() * -300.f);
				TArray<AActor*> TIgnore;
				TIgnore.Add(m_pCheckpoint->GetOwner());
				FHitResult FHit;
				bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), pPlayer->GetActorLocation(), vEnd, ETraceTypeQuery::TraceTypeQuery1, false, TIgnore, EDrawDebugTrace::None, FHit, true);
				if (bHit && !Cast<AMovilePlatform>(FHit.Actor))
				{
					m_vDefaultLoc = m_pCheckpoint->GetOwner()->GetActorLocation();
				}
			}
		}
	}
}

void AUmbraGameMode::StopEnemiesLogic()
{
	for (int i = 0; i < m_TCheckpointRegister.Num(); ++i)
	{
		m_TCheckpointRegister[i]->StopEnemiesLogic();
	}
}

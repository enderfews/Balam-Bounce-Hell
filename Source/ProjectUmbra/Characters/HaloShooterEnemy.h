// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectUmbra/Characters/BaseEnemy.h"
#include "ProjectUmbra/Items/AProjectile.h"
#include "ProjectUmbra/Enums/EnemyMoveMode.h"
#include "HaloShooterEnemy.generated.h"

class UCapsuleComponent;
class UStaticMeshComponent;
class UArrowComponent;
class USphereComponent;
class UWidgetComponent;
class UHealthComponentCpp;

/**
 * 
 */
UCLASS()
class PROJECTUMBRA_API AHaloShooterEnemy : public ABaseEnemy
{
	GENERATED_BODY()
	
private:
	float m_fRemainingKnockBackDistance = 0.0f;
	float m_fRemainingStunDuration = 0.0f;
	float m_fKnockBackSpeed = 0.0f;
	FVector m_VKnockBackDirection = FVector::ZeroVector;
	bool m_bIsInKnockBack = false;

	UPROPERTY()
	USoundManager* m_USndManager;

	virtual void OnDeathCallback() override;
	UFUNCTION()
	void OnDamagedCallback(float _fCurrentHealth);
	
public:
	// Sets default values for this character's properties
	AHaloShooterEnemy();

	// Attributes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Movement mode")
		EEnemyMoveMode m_eMoveMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Tag")
		EBulletModifier m_eTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Bullet Amount")
		int m_iBulletAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Detection range")
		float m_fDetectionRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Shoot Delay")
		float m_fShootDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Idle Time")
		float m_fIdleTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Do not shoot")
		bool m_bDoNotShoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Bullet Speed")
		float m_fBulletSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds", DisplayName = "Shoot Sound")
	USoundBase* m_UShootSound;

	float m_fAngle;
	//AProjectile* m_FoundProjectile;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile", DisplayName = "Projectile to Spawn")
	TSubclassOf<AProjectile> ProjectileToSpawn;


	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Body")
		UStaticMeshComponent* mc_pCubeBody;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Debug Arrow")
		UArrowComponent* mc_pArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Muzzle")
		USphereComponent* mc_pMuzzle;

	//Sounds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sounds", meta=(DisplayName="Shoot FMOD Event"))
	UFMODEvent* m_UShootFMODEvent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sounds", meta=(DisplayName="Punch FMOD Event"))
	UFMODEvent* m_UPunchFMODEvent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sounds", meta=(DisplayName="Stun FMOD Event"))
	UFMODEvent* m_UStunFMODEvent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sounds", meta=(DisplayName="Hit FMOD Event"))
	UFMODEvent* m_UHitFMODEvent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sounds", meta=(DisplayName="Destroyed FMOD Event"))
	UFMODEvent* m_UDestroyedFMODEvent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles", DisplayName = "Death Particle System")
	UNiagaraSystem* m_UDeathParticleSystem;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	virtual void ShootBullet() override;

	virtual void DropObject() override;

	virtual void Tick(float DeltaTime) override;
	
	virtual void KnockBack(FVector _VKnockBackDirection, float _fKnockBackSpeed, float _fKnockBackDistance, float _fStunDuration) override;
	void UpdateKnockBack(float _fDeltaTime);

};

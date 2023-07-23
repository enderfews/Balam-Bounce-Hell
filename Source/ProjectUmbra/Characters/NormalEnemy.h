// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectUmbra/Characters/BaseEnemy.h"
#include "ProjectUmbra/Enums/ShootingType.h"
#include "ProjectUmbra/Enums/EnemyMoveMode.h"
#include "GameFramework/Character.h"
#include "ProjectUmbra/CustomDataType/BulletLUT.h"
#include "Components/TimelineComponent.h"
#include "NormalEnemy.generated.h"

class USoundManager;
class UFMODEvent;
class UObject;
class UCapsuleComponent;
class UStaticMeshComponent;
class UArrowComponent;
class UCurveFloat;
class USphereComponent;
class UWidgetComponent;
class UHealthComponentCpp;
class AProjectile;
class UDecalComponent;
class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class PROJECTUMBRA_API ANormalEnemy : public ABaseEnemy
{
	GENERATED_BODY()

public:
	bool m_bIsAbortedByCheckpoint;
	bool m_bIsCharging;
	bool m_bChargeStart;
	bool m_bIsEnemyAtLimit;
	bool m_bChargeWasCancelled;
	float m_fBrakingFrictionFactor;
	float m_fBrakingDecelerationWalking;
	float m_fGravityScale;
	float m_fTimeToResetChargeDuration;
	float m_fCurrentWalkSpeed;
	FVector m_vEndPosition;
	FVector m_vEndPositionAfterAdjustment;
	FVector m_vVelocity;
	// Sets default values for this character's properties
	ANormalEnemy();
	//Attributes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test", DisplayName = "Test mode? (no player controller)")
		bool m_bTestMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Movement mode")
		EEnemyMoveMode m_eMoveMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Can enemy shoot?")
		bool m_bCanShoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Can use Charge Attack?")
		bool m_bCanUseChargeAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Max charge distance")
		float m_fMaxChargeDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Charge speed")
		float m_fChargeSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Rotation rate")
		float m_fRotationRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Charge duration")
		float m_fChargeDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Charge rest duration")
		float m_fChargeRestDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Charge aim duration")
		float m_fChargeAimDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Prepare charge duration")
		float m_fPrepareChargeDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Charge distance variation")
		float m_fChargeDistanceVariation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Charge damage")
		float m_fChargeDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "predict loc to shoot")
		bool m_bPredictLocToShoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Patrol speed")
		float m_fPatrolSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Chasing speed")
		float m_fChasingSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Shooting Rate")
		float m_fShootingRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Aim Time")
		float m_fAimTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Shooting Type")
		EShootingType m_eShootingType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Tag")
		EBulletModifier m_eTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Shooting Range")
		float m_fShootingRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Rally Prob")
		float m_fRallyProb;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Burst num")
		int m_iBurstNum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Fire Rate")
		float m_fFireRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Bull Num")
		int m_iBullNum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Rally Prob Decrement")
		float m_fRallyProbDecrement;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Combat Range")
		float m_fCombatRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Pursuit Range")
		float m_fPursuitRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Bullet Speed")
		float m_fBulletSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Min distance between the player")
		float m_fMinDistanceBetweenPlayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Shooting Angle")
		float m_fShootingAngle;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Limited by its area")
	//	bool m_bLimitedByItsArea;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Punch Area Confing", DisplayName = "Cast time")
		float m_fCastTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Punch Area Confing", DisplayName = "Area CD")
		float m_fAreaCD;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Punch Area Confing", DisplayName = "Area Radius")
		float m_fAreaRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Punch Area Confing", DisplayName = "Area Damage")
		float m_fAreaDmg;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Punch Area Confing", DisplayName = "Stun time")
		float m_fStunTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Punch Area Confing", DisplayName = "Knock back time")
		float m_fKnockbackTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Punch Area Confing", DisplayName = "Punch Range")
		float m_fPunchRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Punch Area Confing", DisplayName = "Punch Speed")
		float m_fPunchSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Punch Area Confing", DisplayName = "Punch Trace Channel")
		TEnumAsByte<ETraceTypeQuery> m_EPunchTraceChannel;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles", meta = (DisplayName = "Stun Particle System"))
	UNiagaraSystem* m_UStunParticleSystem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles", meta = (DisplayName = "Death Particle System"))
	UNiagaraSystem* m_UDeathParticleSystem;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float m_fAngleBulletIncrement;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float m_fAngleBullet;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Class", DisplayName = "Bullet class")
		TSubclassOf<AProjectile> m_pProjectileClass;
	
	//Components
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Rally Capsule")
		UCapsuleComponent* mc_pRallyCapsule;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Cube Body")
		UStaticMeshComponent* mc_pCubeBody;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "DebugArrow")
		UArrowComponent* mc_pArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Muzzle")
		USphereComponent* mc_pMuzzle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Punch area decal")
		UDecalComponent* mc_pPunchAreaDecal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX", meta = (DisplayName = "Damaged Rim Color"))
	FColor m_FDamagedRimColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX", meta = (DisplayName = "Immunity Rim Color"))
	FColor m_FImmunityRimColor;

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

private:
	FTimeline m_TLRotateToFacePlayer;
	UFUNCTION()
		void RotateToFacePlayerFunc(float alpha);

	FTimeline m_TLPunch;
	UFUNCTION()
		void PunchFunc(float alpha);

	UPROPERTY()
	UCurveFloat* m_fPunchCurve;
	UPROPERTY()
	UCurveFloat* m_fRotateToFaceCurve;
	FTimerHandle m_Timer;

	FRotator m_RCurrent;
	FRotator m_RTarget;

	UPROPERTY()
	USoundManager* m_USndManager;

	bool m_bIsPunching;
	bool m_bIsPunchRunning;
	float m_fCurrentPunchDistance;

	float m_fRemainingKnockBackDistance = 0.0f;
	float m_fRemainingStunDuration = 0.0f;
	float m_fKnockBackSpeed = 0.0f;
	FVector m_VKnockBackDirection = FVector::ZeroVector;
	bool m_bIsInKnockBack = false;
	
	UNiagaraComponent* stunParticles = nullptr;
	bool m_bStartStun = true;

	UPROPERTY()
	UMaterialInstanceDynamic* m_UBodyMat;

	//Damage Visual Feedback
	void ChangeDamageImmunityColor(FColor _FColor);
	void ActivateDamageImmunity(float _fActive);

	//Health Callbacks
	UFUNCTION()
	void OnDamagedCallback(float _fCurrentHealth);
	UFUNCTION()
	void OnEndDamagedCallback();
	UFUNCTION()
	void OnEndImmunityCallback();
	
	virtual void OnDeathCallback() override;
	
	UFUNCTION()
		void OnPlayerKilled();
	UFUNCTION()
		void OnPlayerRespawned();
	UFUNCTION()
	void OnDestruction(AActor* _ADestroyedActor);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChargeEnded);

	UPROPERTY(BlueprintAssignable)
		FOnChargeEnded m_FOnChargeEnded;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRotationFaceEnded);

	UPROPERTY(BlueprintAssignable)
		FOnRotationFaceEnded m_FOnRotationFaceEnded;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void ShootBullet() override;
	virtual void DropObject() override;
	void ReturnDir(const AActor* _Projectile, FVector& Direction_);
	void SpawnProjectile(const FVector& _Position, const FRotator& _Rotation, EBulletModifier& _BulletType);
	void GetPunchKnockbackDir(const ACharacter* _Player, FVector& Dir_);

	UFUNCTION(BlueprintCallable)
		void ChargeAttack(ACharacter* pChar);

	void UpdateCharge(float DeltaTime);
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UFUNCTION()
	void OnDelayPunchCastTimeCompleted();
	UFUNCTION()
		void OnRotationFaceEnded();
	UFUNCTION(BlueprintCallable)
	void Punch();
	UFUNCTION(BlueprintCallable)
	void SmoothFacePlayer(FRotator& _Current,FRotator& _Target);

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void KnockBack(FVector _VKnockBackDirection, float _fKnockBackSpeed, float _fKnockBackDistance, float _fStunDuration) override;
	void UpdateKnockBack(float _fDeltaTime);
};

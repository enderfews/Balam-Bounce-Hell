// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FMODBlueprintStatics.h"
#include "ProjectUmbra/Characters/BaseEnemy.h"
#include "GameFramework/Character.h"
#include "ProjectUmbra/CustomDataType/BulletLUT.h"
#include "Components/TimelineComponent.h"
#include "ProjectUmbra/Enums/SniperTeleportPattern.h"
#include "SniperEnemy.generated.h"

class UFMODAudioComponent;
class USoundManager;
class UFMODEvent;
class UObject;
class AProjectile;
class UCapsuleComponent;
class UStaticMeshComponent;
class USceneComponent;
class UArrowComponent;
class UCurveFloat;
class UCurveLinearColor;
class USphereComponent;
class UWidgetComponent;
class UHealthComponentCpp;
class UNiagaraSystem;
class UNiagaraComponent;
class APlayerCharacter;
class USceneComponent;

UCLASS()
class PROJECTUMBRA_API ASniperEnemy : public ABaseEnemy
{
	GENERATED_BODY()
private:
	FTimeline m_TLRotateToFacePlayer;
	FTimeline m_TLChangeLaserColor;
	UFUNCTION()
		void RotateToFacePlayerFunc(float alpha);
	UCurveFloat* m_fRotateToFaceCurve;
	UFUNCTION()
		void ChangeLaserColorFunc( FLinearColor FColor);
	UFUNCTION()
		void OnLaserColorEnded();
	UCurveLinearColor* m_FChangeLaserCurve;
	FTimerHandle m_FTimer;
	FRotator m_rCurrent;
	FRotator m_rTarget;
	FTimerHandle m_FTeleportSoundTimerHandle;
	
	float m_fRemainingKnockBackDistance = 0.0f;
	float m_fRemainingStunDuration = 0.0f;
	float m_fKnockBackSpeed = 0.0f;
	FVector m_VKnockBackDirection = FVector::ZeroVector;
	bool m_bIsInKnockBack = false;
	UPROPERTY()
	APlayerCharacter* m_pPlayerRef = nullptr;
	UPROPERTY()
	UNiagaraComponent* m_pNiagaraComp = nullptr;
	UPROPERTY()
	UNiagaraComponent* m_pTeleportNiagaraComp = nullptr;
	UPROPERTY()
	UNiagaraComponent* m_pTeleportEndNiagaraComp = nullptr;

	UPROPERTY()
	USoundManager* m_USndManager;
	UPROPERTY()
	UFMODAudioComponent* m_UAimAudioComponent;
	UPROPERTY()
	UFMODAudioComponent* m_UTeleportAudioComponent;

	virtual void OnDeathCallback() override;
	
public:

	ASniperEnemy();

	//Attributes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Sniper teleport pattern")
		ESniperTeleportPattern m_eTeleportPattern;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Keep teleport location on leaving area")
		bool m_bKeepsTeleportLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Detection range")
		float m_fDetectionRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Cooldown")
		float m_fCooldown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Tag")
		EBulletModifier m_eTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Charge time")
		float m_fChargeTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Time after aiming")
		float m_fTimeAfterAiming;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Time until teleport")
		float m_fTimeUntilTeleport;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Max Teleport count")
		int m_iMaxTeleportCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Speed multiplier")
		float m_fSpeedMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Config", DisplayName = "Bullet Speed")
		float m_fBulletSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Config", DisplayName = "Time Offset After Aim Teleport Sound Starts")
		float m_fTimeOffsetAfterAimTeleportSoundStarts = 1.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Class", DisplayName = "Bullet class")
		TSubclassOf<AProjectile> m_pProjectileClass;

	//Sounds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds", DisplayName = "Shoot FMOD Event")
	UFMODEvent* m_UShootFMODEvent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds", DisplayName = "Aim FMOD Event")
	UFMODEvent* m_UAimFMODEvent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds", DisplayName = "Teleport FMOD Event")
	UFMODEvent* m_UTeleportFMODEvent;

	//Components
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Cube Body")
		UStaticMeshComponent* mc_pCubeBody;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Next teleport mark")
		UStaticMeshComponent* mc_pNextTeleportMark;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "DebugArrow")
		UArrowComponent* mc_pArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Muzzle")
		USphereComponent* mc_pMuzzle;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Laser Muzzle")
		USceneComponent* mc_pLaserMuzzle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles", DisplayName = "Laser Aiming")
		UNiagaraSystem* m_pLaserParticleSystem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles", DisplayName = "Teleport Particle System")
	UNiagaraSystem* m_pTeleportParticleSystem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles", DisplayName = "Teleport End Particle System")
	UNiagaraSystem* m_pTeleportEndParticleSystem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles", DisplayName = "Death Particle System")
	UNiagaraSystem* m_UDeathParticleSystem;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void ShootBullet() override;
	virtual void DropObject() override;

	UFUNCTION()
		void SmoothFacePlayer(FRotator& _CurrentRot, FRotator& _TargetRot);
	UFUNCTION()
		void SetLaserAim(APlayerCharacter* pTarget);
	virtual void KnockBack(FVector _VKnockBackDirection, float _fKnockBackSpeed, float _fKnockBackDistance, float _fStunDuration) override;
	void UpdateKnockBack(float _fDeltaTime);

	void AbortLaser();

	void PlayAimSound();
	void StopAimSound();
	void PlayTeleportSoundDelay(float _fTimeDelay);
	void PlayTeleportSound();
	void PlayTeleportEffect(FVector _FLocation);
	void StopTeleportEffect();
	bool IsTeleportEffectPlaying();
	void PlayTeleportEndEffect(FVector _FLocation);
	void StopTeleportEndEffect();
};


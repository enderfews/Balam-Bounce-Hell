// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "ProjectUmbra/Interfaces/IBateable.h"
#include "ExplosiveEnemy.generated.h"

class UNiagaraSystem;
class UFMODAudioComponent;
class USoundManager;
class UFMODEvent;
class UHealthComponentCpp;
class UWidgetComponent;
/**
 * 
 */
UCLASS()
class PROJECTUMBRA_API AExplosiveEnemy : public ABaseEnemy, public IIBateable
{
	GENERATED_BODY()

private:
	//Variables to control the explosion
	bool m_bExploding = false;
	float m_fCurrentTimeToExplode = 0;

	//Variables to control the enemy being bat
	bool m_bHit = false;
	FVector m_vHitDirection = FVector(0,0,0);

	bool m_bHasDropped = false;

	//SoundManager and SoundComponents
	UPROPERTY()
	USoundManager* m_USndManager;
	UPROPERTY()
	UFMODAudioComponent* m_UChaseAudioComponent;

	//Methods to control the explosion
	void ExplosionProgress(float _fTime);
	void Explode();

	//Helper functions for the AutoAim functionality
	//TODO: Try to move them to a functions class to be used by the bullet as well instead of having them duplicate
	FVector GetAutoAimDirection(const FVector& _vCurrentNewDirection, float _fAutoAimRange, float _AutoAimCos) const;
	bool GetInsideDotProductXY(const FVector _vLookDirection, const FVector _vTestDirection, const float _fThreshold) const;

	//Callback for getting damaged
	virtual void OnAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser) override;
	
	//Callback used to detect collision with walls and detonate the enemy instantly
	UFUNCTION()
	void OnCollisionHit(UPrimitiveComponent* _UHitComponent, AActor* _AOtherActor, UPrimitiveComponent* _UOtherComp, FVector _vNormalImpulse, const FHitResult& _FHit);

	//Implementation of IIBateable GetHit function
	virtual void GetHit_Implementation(FVector _vHitDirection, AActor* _ANewOwner, bool m_bPlayerOwnership, float _fAutoAimRange = 0.0f, float _AutoAimAngle = 0.0f) override;

	virtual void OnDeathCallback() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug", DisplayName="Draw Debugs")
	bool m_bDrawDebugs = false;
	
	//Object types Arrays to configure who is hit by the explosion
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Enemy Config", DisplayName="Objects Damaged")
	TArray<TEnumAsByte<EObjectTypeQuery>> m_TObjectsDamaged;

	virtual void BeginPlay() override;

public:
	//Scene components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components",DisplayName="Body")
	UStaticMeshComponent* mc_UBody;

	//Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy Config", DisplayName="Move Speed")
	float m_fMoveSpeed = 400.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Chasing Speed")
	float m_fChasingSpeed = 800.0f;
	//Detection ranges
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy Config", DisplayName="Detection Range")
	float m_fDetectionRange = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy Config", DisplayName="Explosion Detection Range")
	float m_fExplosionDetectionRange = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy Config", DisplayName="Explosion Range")
	float m_fExplosionRange = 300.0f;

	//Explosion
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy Config", DisplayName="Time to Explode")
	float m_fTimeToExplode = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy Config", DisplayName="Damage to Player")
	float m_fDamageToPlayer = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy Config", DisplayName="Damage to Enemies")
	float m_fDamageToEnemies = 1.0f;

	//Enemy bat
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy Config", DisplayName="Batted Speed")
	float m_fBattedSpeed = 1200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy Config", DisplayName="Time to Explode After Hit")
	float m_fTimeToExplodeAfterHit = 2.0f;

	//Effects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Particles", DisplayName="Explosion NiagaraSystem")
	UNiagaraSystem* m_UExplosionNiagaraSystem;

	//Sounds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sounds", DisplayName="Explode FMOD Event")
	UFMODEvent* m_UExplodeFMODEvent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sounds", DisplayName="Chase FMOD Event")
	UFMODEvent* m_UChaseFMODEvent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sounds", DisplayName="Bat FMOD Event")
	UFMODEvent* m_UBatFMODEvent;

	AExplosiveEnemy();

	//Performs the attack action in our case starts the explosion countdown
	virtual void ShootBullet() override;
	virtual void DropObject() override;
	
	virtual void Tick(float DeltaTime) override;

	void EnableChaseSound(bool _bEnable);

	UFUNCTION(BlueprintImplementableEvent,  Category = "OnExplosiveEnemyHit")
	void OnExplosiveEnemyHit();
};

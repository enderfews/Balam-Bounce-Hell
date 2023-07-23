
#pragma once

#include "GameFramework/Actor.h"
#include "ProjectUmbra/CustomDataType/BulletLUT.h"
#include "ProjectUmbra/Interfaces/IBateable.h"

#include "AProjectile.generated.h"

class UFMODAudioComponent;
class USoundManager;
class UFMODEvent;
class UNiagaraSystem;
class UNiagaraComponent;
class USphereComponent;

UCLASS()
class PROJECTUMBRA_API AProjectile : public AActor, public IIBateable
{
	GENERATED_BODY()

private:
	// TODO: Remove when Player and Enemies on C++
	bool m_bPlayerOwnership = false;
	float m_fAutoAimCos;

	bool m_bInElectricBehaviour = false;
	UPROPERTY()
	TArray<AActor*> m_TEnemiesHitByElectric;
	float m_fTimeToNextElectricTarget;
	UPROPERTY()
	UNiagaraComponent* mc_ElectricBounceComponent;
	FVector m_VLastTargetPosition = FVector::ZeroVector;
	UPROPERTY()
	AActor* ATargetHitByElectricHit;
	UPROPERTY()
	USoundManager* m_USndManager;
	UPROPERTY()
	UFMODAudioComponent* m_UFlyAudioComponent;
	

	//Helper functions
	//Returns a uint with the sum of the bullet tags
	uint8 GetTagsUint();
	
	//Returns a reflected vector given the Hit Result against a surface
	void BounceOnWall(const FHitResult _FHitResult);
	
	//Logic to damage Actors, including direct hit and explosion
	void DoDamage(AActor* _ATargetHitted);
	
	//Returns if the target hit is valid IE: a bullet shot by an enemy hits an enemy, the result will be false
	bool TargetIsValid(const AActor* _AHittedActor) const;
	
	//Returns the direction of auto aim, returns the same direction if no target is found
	FVector GetAutoaimDirection(const FVector _vCurrentNewDirection) const;
	
	//Returns the rotation to look from _vFrom to _vTo, al operations are done on the XY plane
	FRotator FindLookAtRotationXY(const FVector _vFrom, const FVector _vTo) const;
	
	//Methods to draw debug lines for auto aim
	//TODO: remove on the future
	void DrawDebugs(FVector _vDirection) const;
	void GetDebugForwardSegment(float _fSide, FVector _vDirection, FVector& _vSegmentEnd_) const;
	void GetDebugArcSegment(uint8 _uIndex, FVector _vDirection, FVector& _vSegmentStart_, FVector& _vSegmentEnd_) const;
	bool GetInsideDotProductXY(const FVector _vLookDirection, const FVector _vTestDirection, const float _fThreshold) const;
	
	//Callback for hit events
	UFUNCTION()
	void OnCollisionHit(UPrimitiveComponent* _UHitComponent, AActor* _AOtherActor, UPrimitiveComponent* _UOtherComp, FVector _vNormalImpulse, const FHitResult& _FHit);
	
	//Callback for overlap events
	UFUNCTION()
	void OnCollisionOverlap(UPrimitiveComponent* _UOverlappedComponent, AActor* _AOtherActor, UPrimitiveComponent* _UOtherComp, int32 _iOtherBodyIndex, bool _bFromSweep, const FHitResult& _FSweepResult);

	//Callback for overlap events
	//TODO: Remove when enemies and player on C++
	UFUNCTION()
	void OnCollisionOverlapPlayer(UPrimitiveComponent* _UOverlappedComponent, AActor* _AOtherActor, UPrimitiveComponent* _UOtherComp, int32 _iOtherBodyIndex, bool _bFromSweep, const FHitResult& _FSweepResult);

protected:
	virtual void BeginPlay() override;

public:
	//Scene Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", meta=(DisplayName="Collision"))
	USphereComponent* mc_UCollision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", meta=(DisplayName="Collision With Player"))
    USphereComponent* mc_UCollisionTwo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", meta=(DisplayName="Mesh"))
	UStaticMeshComponent* mc_UMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", meta=(DisplayName="Base Niagara Component"))
	UNiagaraComponent* m_UNiagaraComponent;

	//Assets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Materials", meta=(DisplayName="Enemy Material"))
	UMaterialInstance* m_UEnemyMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Materials", meta=(DisplayName="Player Material"))
	UMaterialInstance* m_UPlayerMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Particles", meta=(DisplayName="Hit Particles"))
	UNiagaraSystem* m_UHitParticleSystem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Particles", meta=(DisplayName="Explosion Niagara System"))
	UNiagaraSystem* m_UExplosionNiagaraSystem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Particles", meta=(DisplayName="Bounce Niagara System"))
	UNiagaraSystem* m_UBounceNiagaraSystem;

	//Tag configuration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tags Configuration", meta=(DisplayName="Modifiers"))
	TArray<EBulletModifier> m_EModifiers;
	UPROPERTY(EditAnywhere, Category="Tags Configuration", meta=(DisplayName="Collision"))
	UBulletLUT* m_UBulletLUT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tags Configuration", meta=(DisplayName="Bullet Data"))
	FBulletData m_FBulletData;

	//Auto aim variables, mainly for debug
	UPROPERTY(EditAnywhere, Category="Configuration", meta=(DisplayName="Autoaim Angle"))
	float m_fAutoAimAngle;
	UPROPERTY(EditAnywhere, Category="Configuration", meta=(DisplayName="Auotaim Range"))
	float m_fAutoAimRange;

	//Bullet travel speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration", meta=(DisplayName="Speed"))
	float m_fSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration", meta=(DisplayName="Stun Duration"))
	float m_fStunDuration = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration", meta=(DisplayName="Slow Duration"))
	float m_fSlowDuration = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration", meta=(DisplayName="Slow Amount", ClampMax=0.99f, ClampMin=0.0f))
	float m_fSlowAmount = 0.4f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration", meta=(DisplayName="Electric Tag Bounces"))
	int m_iElectricTagBounces = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration", meta=(DisplayName="Electric Tag Detection Radius", ClampMin=0.0f))
	float m_fElectricTagDetectionRadius = 600.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration", meta=(DisplayName="Electric Tag Bounce Duration Per Target", ClampMin=0.0f))
	float m_fElectricTagBounceDurationPerTarget = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration", meta=(DisplayName="Electric Tag Bounce Duration Per Target"))
	UNiagaraSystem* m_UElectricBounceSystem;

	//Object types for explosion damage
	UPROPERTY(EditAnywhere, Category="Damage Configuration", meta=(DisplayName="Objects Damaged By Player Explosion"))
	TArray<TEnumAsByte<EObjectTypeQuery>> m_TObjectsDamagedByPlayerExplosion;
	UPROPERTY(EditAnywhere, Category="Damage Configuration", meta=(DisplayName="Objects Damaged By Enemy Explosion"))
	TArray<TEnumAsByte<EObjectTypeQuery>> m_TObjectsDamagedByEnemyExplosion;

	//Sounds
	UPROPERTY(EditAnywhere, Category="Sounds", meta=(DisplayName="Fly FMOD Event"))
	UFMODEvent* m_UFlyFMODEvent;
	UPROPERTY(EditAnywhere, Category="Sounds", meta=(DisplayName="Bounce FMOD Event"))
	UFMODEvent* m_UBounceFMODEvent;
	UPROPERTY(EditAnywhere, Category="Sounds", meta=(DisplayName="Explode FMOD Event"))
	UFMODEvent* m_UExplodeFMODEvent;
	UPROPERTY(EditAnywhere, Category="Sounds", meta=(DisplayName="Bat FMOD Event"))
	UFMODEvent* m_UBatFMODEvent;
	UPROPERTY(EditAnywhere, Category="Sounds", meta=(DisplayName="Destroy FMOD Event"))
	UFMODEvent* m_UDestroyFMODEvent;

	AProjectile();

	virtual void Tick(float DeltaSeconds) override;

	//Function to call when player was respawned
	UFUNCTION()
		void RemoveBulletOnPlayerRespawned();
	//Pool functions
	UFUNCTION(BlueprintCallable)
		void SendBackToPool();
	UFUNCTION(BlueprintCallable)
		void PlayFlySound();
	//Pool variables
	bool m_bIsInPool=false;
	bool m_bWasLeavingPool = false;
	float m_fCurrentLife = 0.f;
	/**
	 * Adds a new tag to the bullet. If the tag already exists it won't be added.
	 * If there are two tags the first will be replaced with the new and the second one will be moved to the first position.
	 * 
	 * @param _ETagToAdd the new tag to add, if no tag is provided or the tag is None it won't be added
	 */
	UFUNCTION(BlueprintCallable)
	void AddTag(EBulletModifier _ETagToAdd);
	
	// /**
	//  * Sets the owner of the bullet and changes the material accordingly
	//  * 
	//  * @param _ANewOwner The new owner, usually the one who shoots or returns the bullet
	//  */
	// TODO: Return to old behaviour when Player and Enemies on C++
	// UFUNCTION(BlueprintCallable)
	// void SetOwnerAndMaterial(AActor* _ANewOwner);

	/**
	 * Sets the owner of the bullet and changes the material accordingly
	 * 
	 * @param _bPlayerOwnership If the new owner is the player
	 * @param _ANewOwner The new owner, usually the one who shoots or returns the bullet
	 */
	UFUNCTION(BlueprintCallable)
	void SetOwnerAndMaterial(bool _bPlayerOwnership, AActor* _ANewOwner);


	//Implementation of IBateable interface
	UFUNCTION(BlueprintCallable)
	virtual void GetHit_Implementation(FVector _vHitDirection, AActor* _ANewOwner, bool _bPlayerOwnership, float _fAutoAimRange = 0.0f, float _AutoAimAngle = 0.0f) override;

	UFUNCTION(BlueprintImplementableEvent,  Category = "OnBulletHit")
	void OnBulletHit();
	UFUNCTION(BlueprintImplementableEvent,  Category = "OnBulletExplode")
	void OnBulletExplode();
};


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectUmbra/CustomDataType/BulletLUT.h"
#include "UmbraGameMode.generated.h"


class ACheckpoint;
class ACharacter;
class AProjectile;
struct FTimerHandle;
class ASplineMovementCurve;
class ATriggerBox;
UCLASS()
class PROJECTUMBRA_API AUmbraGameMode : public AGameModeBase
{
	GENERATED_BODY()
private:
	FTimerHandle m_FTimer;
	FTimerHandle m_FRespawnTimer;
	FTimerHandle m_FResetTimer;
public:
	AUmbraGameMode();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game state", DisplayName = "Checkpoint register")
		TArray<ACheckpoint*> m_TCheckpointRegister;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game state", DisplayName = "Checkpoint ref")
		ACheckpoint* m_pCheckpoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game state", DisplayName = "Default Location")
		FVector m_vDefaultLoc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game state", DisplayName = "Default respawn time")
		float m_fDefaultRespawnTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game state", DisplayName = "Insta respawn when falling")
		bool m_bInstaRespawnWhenFalling;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Object Pool", DisplayName = "Bullet pool")
		TArray<AProjectile*> m_TBulletPool;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Pool", DisplayName = "Initial bullet amount")
		int m_iInitialBulletAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Pool", DisplayName = "Bullet pool bounds")
		ATriggerBox* m_pBulletPoolBounds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class", DisplayName = "Bullet class")
		TSubclassOf<AProjectile> m_TBulletClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration", DisplayName = "Remove Controls After Player falls into abyss")
		float m_fRemoveControlsAfterPlayerFallsIntoAbyss;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration", DisplayName = "Remove Controls After Respawn Duration")
	float m_fRemoveControlsAfterRespawnDuration;

	TArray<ASplineMovementCurve*> TCurvePool;
	float m_fCurrentRespawnTime;
	bool m_bCurrentDeathByDeathZone;
	bool m_bRespawnWasCalled;
	bool m_bCurrentScreenFade;
	ACharacter* m_pCurrentChar;
	FVector m_vCameraLoc;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerRespawned);

	UPROPERTY(BlueprintAssignable)
		FOnPlayerRespawned OnPlayerRespawned;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerKilled);

	UPROPERTY(BlueprintAssignable)
		FOnPlayerKilled OnPlayerKilled;
	void RespawnCharacter(ACharacter* _Char,bool bDeathByDeathZone=false);
	void SpawnCharacter();

	//Pool functions
	UFUNCTION(BlueprintCallable)
		void CreateInitialBullets();
	UFUNCTION(BlueprintCallable)
		AProjectile* PopUpBulletFromPool(const FVector& vLocation, const FRotator& rRotation,float fSpeed,EBulletModifier eModifier,AActor* pOwner,bool bPlayerOwnerShip);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBulletPoolEmpty);

	UPROPERTY(BlueprintAssignable)
		FOnBulletPoolEmpty OnBulletPoolEmpty;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void ResetAllCheckpoints();
	void StorePlayerLocationToRespawn();
	void StopEnemiesLogic();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectUmbra/Enums/EnemyType.h"
#include "GameFramework/Character.h"
#include "BaseEnemy.generated.h"

class ATagDrop;
class AHealthPack;
class UHealthComponentCpp;
class AWaypoint;
UCLASS()
class PROJECTUMBRA_API ABaseEnemy : public ACharacter
{
	GENERATED_BODY()
protected:
	UFUNCTION()
	virtual void OnDeathCallback(); 

public:
	// Sets default values for this character's properties
	ABaseEnemy();

	// UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Enemy Config",DisplayName="Health")
	// float m_fHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Enemy type")
	EEnemyType m_eEnemyType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Center patrol loc")
	FVector m_vCenterPatrolLoc = FVector(0,0,0);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Waypoint list")
		TArray<AWaypoint*> m_TWaypoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drops Config", DisplayName = "Drops tag")
	bool m_bDropsTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drops Config", DisplayName = "Health Pack Class")
	TSubclassOf<ATagDrop> m_TTagDropClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drops Config", DisplayName = "Drops health")
	bool m_bDropsHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config", DisplayName = "Waypoint pause")
		float m_fWaypointPause;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drops Config", DisplayName = "Health Pack Class")
	TSubclassOf<AHealthPack> m_THealthPackClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Health Component")
	UHealthComponentCpp* mc_pHealthComp;

	bool m_bDeathByDeathZone;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void ShootBullet();
	virtual void DropObject();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	virtual void OnAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	virtual void KnockBack(FVector _VKnockBackDirection, float _fKnockBackSpeed, float _fKnockBackDistance, float _fStunDuration);

	UFUNCTION(BlueprintImplementableEvent,  Category = "OnEnemyDamaged")
	void OnEnemyDamaged();
	UFUNCTION(BlueprintImplementableEvent,  Category = "OnEnemyDeath")
	void OnEnemyDeath();
	UFUNCTION(BlueprintImplementableEvent,  Category = "OnEnemyShoot")
	void OnEnemyShoot();

};

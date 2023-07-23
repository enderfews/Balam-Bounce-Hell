// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Flamethrower.generated.h"

class UNiagaraComponent;
class UFMODAudioComponent;
class UFMODEvent;

UCLASS()
class PROJECTUMBRA_API AFlamethrower : public AActor
{
	GENERATED_BODY()
private:
	//Sizes for effect control
	float m_fHalfWidth = 0.0f;
	float m_fHalfHeight = 0.0f;
	float m_fEffectLength = 0.0f;

	//Active control
	bool m_bIsActive = true;
	float m_fCurrentActiveTime = 0.0f;
	float m_fCurrentSleepTime = 0.0f;
	float m_fCurrentGlowEyesTime = 0.0f;

	//Progressive start distance
	float m_fCurrentDistance = 0.0f;

	float m_fFadeoutSpeed = 1.0f;
	float m_fCurrentFadeout = 1.0f;

	UPROPERTY()
	UFMODAudioComponent* mc_UFlamethrowerAudioComponent;

	FVector GetKnockBackDir(const AActor* _APlayer) const;
	void CalculateEffectDistance();
	void SetEffectDistance(float _fDistance);

	UFUNCTION()
	void PlayerOverlap(UPrimitiveComponent* _UOverlappedComponent, AActor* _AOtherActor, UPrimitiveComponent* _UOtherComp, int32 _iOtherBodyIndex, bool _bFromSweep, const FHitResult& _FSweepResult);
	UFUNCTION()
	void MapOverlap(UPrimitiveComponent* _UOverlappedComponent, AActor* _AOtherActor, UPrimitiveComponent* _UOtherComp, int32 _iOtherBodyIndex, bool _bFromSweep, const FHitResult& _FSweepResult);
	UFUNCTION()
	void EndMapOverlap(UPrimitiveComponent* _UOverlappedComponent, AActor* _AOtherActor, UPrimitiveComponent* _UOtherComp, int32 _iOtherBodyIndex);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", DisplayName="Pivot")
	USceneComponent* mc_UPivot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", DisplayName="Wall")
	UStaticMeshComponent* mc_UWall;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", DisplayName="Fire Start")
	USceneComponent* mc_UFireStart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", DisplayName="Damage Area")
	UBoxComponent* mc_UDamageArea;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", DisplayName="Map Collision Area")
	UBoxComponent* mc_UMapCollisionArea;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", DisplayName="Flamethrower Effect Component")
	UNiagaraComponent* mc_UFlamethrowerEffectComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", DisplayName="Left Eye Effect Component")
	UNiagaraComponent* mc_ULeftEyeEffectComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", DisplayName="Right Eye Effect Component")
	UNiagaraComponent* mc_URightEyeEffectComponent;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config", DisplayName="Length")
	float m_fLength = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config", DisplayName="Damage")
	float m_fDamage = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config", DisplayName="Active Time")
	float m_fActiveTime = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config", DisplayName="Sleep Time")
	float m_fSleepTime = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config", DisplayName="Start Active")
	bool m_bStartActive = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config", DisplayName="Knock Back Duration")
	float m_fKnockBackDuration = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config", DisplayName="Knock Back Speed")
	float m_fKnockBackSpeed = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config", DisplayName="Knock Back Stun Duration")
	float m_fKnockBackStunDuration = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config", DisplayName="Progressive Start Speed")
	float m_fProgressiveStartSpeed = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config", DisplayName="Time Offset Before Start")
	float m_fTimeOffsetBeforeStart = -1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound", DisplayName="Flamethrower FMOD Event")
	UFMODEvent* m_UFlamethrowerFMODEvent;
	
	// Sets default values for this actor's properties
	AFlamethrower();

	// Called every frame
	virtual void Tick(float _fDeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void Start();
	UFUNCTION(BlueprintCallable)
	void Stop();
};

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectUmbra/CustomDataType/BulletLUT.h"
#include "ProjectUmbra/Interfaces/IBateable.h"
#include "TagDrop.generated.h"

class USoundManager;
class UFMODEvent;
class USphereComponent;

UCLASS()
class PROJECTUMBRA_API ATagDrop : public AActor, public IIBateable
{
	GENERATED_BODY()

private:
	bool m_bInteracted = false;
	bool m_bActive = false;

	UPROPERTY()
	USoundManager* m_USndManager;

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", DisplayName="Pivot")
	USceneComponent* mc_UPivot = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", DisplayName="Collider")
	USphereComponent* mc_UCollider = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config", DisplayName="Tag")
	EBulletModifier m_ETag = EBulletModifier::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config", DisplayName="Life Time", meta=(ClampMin=0.1f));
	float m_fLifetime = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config", DisplayName="Time Until Active", meta=(ClampMin=0.1f));
	float m_fTimeUntilActive = 1.0f;

	UPROPERTY(EditAnywhere, Category="Sounds", meta=(DisplayName="Bat FMOD Event"))
	UFMODEvent* m_UBatFMODEvent;
	
	ATagDrop();

	virtual void Tick(float DeltaSeconds) override;

	void Collect(AActor* _ACollector);

	virtual void GetHit_Implementation(FVector _FHitDirection, AActor* _ANewOwner, bool _bPlayerOwnership, float _fAutoAimRange, float _fAutoAimAngle) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnBatHit();
	
};

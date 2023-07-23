#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObjectMovementComponent.generated.h"

UCLASS(ClassGroup=Movement, meta=(BlueprintSpawnableComponent))
class PROJECTUMBRA_API UObjectMovementComponent : public UActorComponent
{
	GENERATED_BODY()
private:
	int8 m_iDirection = 1;
	bool m_bIsMoving = false;
	float m_fProgress = 0.0f;

	FVector m_VWorldStartLocation;
	FVector m_VWorldEndLocation;
	
protected:
	virtual void BeginPlay() override;
	
public:
	UObjectMovementComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(EditAnywhere, Category="Config", DisplayName="End Location")
	FVector m_VEndLocation;
	UPROPERTY(EditAnywhere, Category="Config", DisplayName="Time To Move", meta=(ClampMin=0.0f))
	float m_fTimeToMove;

	UFUNCTION(BlueprintCallable)
	void Start();
	UFUNCTION(BlueprintCallable)
	void Stop();
	UFUNCTION(BlueprintCallable)
	void Reverse();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE( FOnMovementStart );
	UPROPERTY(BlueprintAssignable)
	FOnMovementStart OnMovementStart;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE( FOnMovementEnd );
	UPROPERTY(BlueprintAssignable)
	FOnMovementEnd OnMovementEnd;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE( FOnMovementReverseStart );
	UPROPERTY(BlueprintAssignable)
	FOnMovementStart OnMovementReverseStart;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE( FOnMovementReverseEnd );
	UPROPERTY(BlueprintAssignable)
	FOnMovementStart OnMovementReverseEnd;
};

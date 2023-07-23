#pragma once

#include "UObject/Interface.h"

#include "IBateable.generated.h"

UINTERFACE(Blueprintable)
class PROJECTUMBRA_API UIBateable : public UInterface
{
	GENERATED_BODY()
};

class IIBateable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void GetHit(FVector _FHitDirection, AActor* _ANewOwner, bool _bPlayerOwnership, float _fAutoAimRange = 0.0f, float _fAutoAimAngle = 0.0f);
};

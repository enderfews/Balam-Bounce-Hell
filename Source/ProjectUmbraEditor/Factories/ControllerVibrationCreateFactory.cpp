#include "ControllerVibrationCreateFactory.h"

#include "ProjectUmbra/CustomDataType/ControllerVibration.h"
#include "GameFramework/ForceFeedbackEffect.h"

UControllerVibrationCreateFactory::UControllerVibrationCreateFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UControllerVibration::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

UObject* UControllerVibrationCreateFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName,
                                                             EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UControllerVibration* NewControllerVibration = NewObject<UControllerVibration>(InParent, InClass, InName, Flags);
	NewControllerVibration->m_UForceFeedbackAsset = NewObject<UForceFeedbackEffect>(NewControllerVibration, UForceFeedbackEffect::StaticClass(),
		FName("ForceFeedback"), EObjectFlags::RF_NoFlags);
	return NewControllerVibration;
}

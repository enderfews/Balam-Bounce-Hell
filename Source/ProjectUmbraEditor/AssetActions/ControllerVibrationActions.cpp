#include "ControllerVibrationActions.h"

#include "ProjectUmbra/CustomDataType/ControllerVibration.h"


FControllerVibrationActions::FControllerVibrationActions(EAssetTypeCategories::Type _AssetCategory)
	: m_EAssetCategory(_AssetCategory)
{
}

FText FControllerVibrationActions::GetName() const
{
	return FText::FromString(FString(TEXT("Controller Vibration")));
}

FColor FControllerVibrationActions::GetTypeColor() const
{
	return FColor(125, 214, 47);
}

UClass* FControllerVibrationActions::GetSupportedClass() const
{
	return UControllerVibration::StaticClass();
}

uint32 FControllerVibrationActions::GetCategories()
{
	return m_EAssetCategory;	
}
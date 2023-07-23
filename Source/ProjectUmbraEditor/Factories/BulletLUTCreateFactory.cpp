#include "BulletLUTCreateFactory.h"

#include "Engine/UserDefinedEnum.h"
#include "ProjectUmbra/CustomDataType/BulletLUT.h"

UBulletLUTCreateFactory::UBulletLUTCreateFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UBulletLUT::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

UObject* UBulletLUTCreateFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
                                                   UObject* Context, FFeedbackContext* Warn)
{
	UBulletLUT* NewBulletLUT = NewObject<UBulletLUT>(InParent, InClass, InName, Flags);

	//Get num of values from Enum ignoring the last element "None"
	const int32 EnumNumValues = StaticEnum<EBulletModifier>()->NumEnums() - 2;

	for (uint8 i = 0; i < EnumNumValues; ++i)
	{
		uint8 Key = 1 << i;

		NewBulletLUT->BulletData.FindOrAdd(Key);

		for (uint8 j = i+1; j < EnumNumValues; ++j)
		{
			Key = 1 << i | 1 << j;

			NewBulletLUT->BulletData.FindOrAdd(Key);
		}
	}

	return NewBulletLUT;
}

#include "BulletLUTDetails.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "IStructureDetailsView.h"
#include "Modules/ModuleManager.h"
#include "ProjectUmbra/CustomDataType/BulletLUT.h"

TSharedRef<IDetailCustomization> FBulletLUTDetails::MakeInstance()
{
	return MakeShareable(new FBulletLUTDetails);
}

void FBulletLUTDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> Objects;
	DetailBuilder.GetObjectsBeingCustomized(Objects);

	if (Objects.Num() > 1)
	{
		return;
	}
	
	//GetObject selected
	const UBulletLUT* BulletLUT = static_cast<UBulletLUT*>(Objects[0].Get());

	//Hide default properties
	DetailBuilder.HideProperty(DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UBulletLUT, BulletData)));

	//Create a Category
	IDetailCategoryBuilder& Configurations = DetailBuilder.EditCategory("Configurations");

	//Get the number of values from the Enum
	const int32 NumEnums = StaticEnum<EBulletModifier>()->NumEnums() - 2;

	//Get all keys to iterate them
	TArray<uint8> Keys;
	BulletLUT->BulletData.GetKeys(Keys);

	bool ExpandFirst = true;

	//Iterate all the keys of the TMap
	for (const int Key : Keys)
	{
		FString StructName;

		//Get the name of the Enums using the uint8 value
		for (int32 i = 0; i < NumEnums; ++i)
		{
			if (((1 << i) & Key) > 0)
			{
				int32 Value = 1 << i;
				FString Name = StaticEnum<EBulletModifier>()->GetNameStringByValue(Value);
				StructName = StructName.Append(Name);
				StructName = StructName.Append(FString(" "));
			}
		}
		StructName = StructName.TrimEnd();

		//Create a FStructOnScope to insert on the category
		auto StructData = MakeShareable(
			new FStructOnScope(FBulletData::StaticStruct(), (uint8*)&BulletLUT->BulletData[Key]));

		//Insert the struct and modify its display name
		IDetailPropertyRow* DetailPropertyRow = Configurations.AddExternalStructure(StructData);
		DetailPropertyRow->DisplayName(FText::FromString(StructName));
		DetailPropertyRow->ShouldAutoExpand(ExpandFirst);
		ExpandFirst = false;
	}
}

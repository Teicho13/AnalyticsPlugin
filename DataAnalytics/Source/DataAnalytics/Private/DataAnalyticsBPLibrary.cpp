// Copyright Epic Games, Inc. All Rights Reserved.

#include "DataAnalyticsBPLibrary.h"
#include "DataAnalytics.h"

UDataAnalyticsBPLibrary::UDataAnalyticsBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UDataAnalyticsBPLibrary::DataToStringImpl(FProperty* prop, void* structPtr)
{
	//Reset all data like headers and value strings
	ResetValues();
	
	FStructProperty* StructProperty = CastField<FStructProperty>(prop);
	//Check if empty / null
	if(StructProperty)
	{
		//Go trough Struct
		for (TFieldIterator<FProperty> PropertyIt(StructProperty->Struct); PropertyIt; ++PropertyIt)
		{
			//Record header name
			HeaderNames.Append(*PropertyIt->GetAuthoredName());
			HeaderNames.Append(TEXT(", "));

			//Check value dimensions
			for (int i = 0; i < PropertyIt->ArrayDim; ++i)
			{
				//Get pointer to the value
				void* ValuePtr = PropertyIt->ContainerPtrToValuePtr<void>(structPtr,i);

				//Parse item
				ParseStructData(*PropertyIt,ValuePtr);
			}
		}

		//Remove last header comma
		int cutSize = HeaderNames.Len() - 2;
		HeaderNames.RemoveAt(cutSize);

		//Remove last Value comma
		cutSize = Values.Len() - 2;
		Values.RemoveAt(cutSize);
	}
}

void UDataAnalyticsBPLibrary::ParseStructData(FProperty* prop, void* valuePtr)
{
	//Supported Types
	
	float FloatValue;
	int32 IntValue;
	bool BoolValue;
	FString StringValue;
	FName NameValue;
	FText TextValue;

	//Check for integer / float
	if(FNumericProperty* NumericProperty = CastField<FNumericProperty>(prop))
	{
		//Is an integer
		if(NumericProperty->IsInteger())
		{
			IntValue = NumericProperty->GetSignedIntPropertyValue(valuePtr);
			Values.AppendInt(IntValue);
			Values.Append(", ");
		}
		//Is a float
		if(NumericProperty->IsFloatingPoint())
		{
			FloatValue = NumericProperty->GetFloatingPointPropertyValue(valuePtr);
			FString FloatStr = FString::SanitizeFloat(FloatValue);
			Values.Append(FloatStr);
			Values.Append(", ");
		}
	}
	//Check for boolean
	else if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(prop))
	{
		BoolValue = BoolProperty->GetPropertyValue(valuePtr);
		if(BoolValue)
		{
			//true
			Values.Append("True, ");
			Values.Append(", ");
		}else
		{
			//false
			Values.Append("False, ");
			Values.Append(", ");
		}
	}
	//Check for names
	else if (FNameProperty* NameProperty = CastField<FNameProperty>(prop))
	{
		NameValue = NameProperty->GetPropertyValue(valuePtr);
		Values.Append(NameValue.ToString());
		Values.Append(", ");
	}
	//Check for string
	else if (FStrProperty* StringProperty = CastField<FStrProperty>(prop))
	{
		StringValue = StringProperty->GetPropertyValue(valuePtr);
		Values.Append(StringValue);
		Values.Append(", ");
	}
	//Check for text
	else if (FTextProperty* TextProperty = CastField<FTextProperty>(prop))
	{
		TextValue = TextProperty->GetPropertyValue(valuePtr);
		Values.Append(TextValue.ToString());
		Values.Append(", ");
	}
	
}

void UDataAnalyticsBPLibrary::ResetValues()
{
	HeaderNames.Empty();
	Values.Empty();
}

FString UDataAnalyticsBPLibrary::GetHeaderNames()
{
	return HeaderNames;
}

FString UDataAnalyticsBPLibrary::GetValues()
{
	return Values;
}



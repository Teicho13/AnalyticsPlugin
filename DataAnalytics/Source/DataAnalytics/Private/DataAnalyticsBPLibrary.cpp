// Copyright Epic Games, Inc. All Rights Reserved.

#include "DataAnalyticsBPLibrary.h"
#include "DataAnalytics.h"
#include "Misc/FileHelper.h"

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

			FString test = "";
			//Check value dimensions
			for (int i = 0; i < PropertyIt->ArrayDim; ++i)
			{
				//Get pointer to the value
				void* ValuePtr = PropertyIt->ContainerPtrToValuePtr<void>(structPtr,i);

				//Parse item
				
				FString ParseString = ParseStructData(*PropertyIt,ValuePtr);
				Values.Append(ParseString);
				Values.Append(", ");
			}
		}

		//Remove last header comma
		int cutSize = HeaderNames.Len() - 2;
		HeaderNames.RemoveAt(cutSize);

		//Remove last Value comma
		cutSize = Values.Len() - 2;
		Values.RemoveAt(cutSize);

		CreateOutput(HeaderNames,Values);
	}
}

FString UDataAnalyticsBPLibrary::ParseStructData(FProperty* prop, void* valuePtr)
{
	//Supported Types
	
	float FloatValue;
	int32 IntValue;
	bool BoolValue;
	FString StringValue;
	FName NameValue;
	FText TextValue;
	FString TempString = "";

	//Check for integer / float
	if(FNumericProperty* NumericProperty = CastField<FNumericProperty>(prop))
	{
		//Is an integer
		if(NumericProperty->IsInteger())
		{
			IntValue = NumericProperty->GetSignedIntPropertyValue(valuePtr);
			TempString.AppendInt(IntValue);
			return TempString;
		}
		//Is a float
		if(NumericProperty->IsFloatingPoint())
		{
			FloatValue = NumericProperty->GetFloatingPointPropertyValue(valuePtr);
			FString FloatStr = FString::SanitizeFloat(FloatValue);
			TempString.Append(FloatStr);
			return TempString;
		}
	}
	//Check for boolean
	else if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(prop))
	{
		BoolValue = BoolProperty->GetPropertyValue(valuePtr);
		if(BoolValue)
		{
			//true
			TempString.Append("True");
			return TempString;
		}else
		{
			//false
			TempString.Append("False");
			return TempString;
		}
	}
	//Check for names
	else if (FNameProperty* NameProperty = CastField<FNameProperty>(prop))
	{
		NameValue = NameProperty->GetPropertyValue(valuePtr);
		TempString.Append(NameValue.ToString());
		return TempString;
	}
	//Check for string
	else if (FStrProperty* StringProperty = CastField<FStrProperty>(prop))
	{
		StringValue = StringProperty->GetPropertyValue(valuePtr);
		TempString.Append(StringValue);
		return TempString;
	}
	//Check for text
	else if (FTextProperty* TextProperty = CastField<FTextProperty>(prop))
	{
		TextValue = TextProperty->GetPropertyValue(valuePtr);
		TempString.Append(TextValue.ToString());
		return TempString;
	}
	//Check for Array
	else if (FArrayProperty* ArrayProperty = CastField<FArrayProperty>(prop))
	{
		//Get a helper to go trough array
		FScriptArrayHelper Helper(ArrayProperty,valuePtr);

		//Add quotes to keep values together in a single cell
		TempString.Append("\u0022");

		//Loop trough and parse all array values
		for (int i = 0, n = Helper.Num(); i < n; ++i)
		{
			TempString.Append(ParseStructData(ArrayProperty->Inner,Helper.GetRawPtr(i)));
			//Check if it is last value otherwise add a seperator
			if(i < n - 1)
			{
				TempString.Append(" | ");
			}
		}
		//Close quotes
		TempString.Append("\u0022");
		
		return TempString;
	}
	return "";
	
}

void UDataAnalyticsBPLibrary::ResetValues()
{
	HeaderNames.Empty();
	Values.Empty();
	OutString.Empty();
}

FString UDataAnalyticsBPLibrary::GetOutString()
{
	return OutString;
}

void UDataAnalyticsBPLibrary::WriteToFile(FString FilePath, FString InString, bool& outSuccess)
{
	if(!FFileHelper::SaveStringToFile(InString,*FilePath))
	{
		outSuccess = false;
		return;
	}
	outSuccess = true;
}

FString UDataAnalyticsBPLibrary::GetHeaderNames()
{
	return HeaderNames;
}

FString UDataAnalyticsBPLibrary::GetValues()
{
	return Values;
}

void UDataAnalyticsBPLibrary::CreateOutput(FString HeadersStr, FString ValuesStr)
{
	OutString = "";

	HeadersStr.Append("\n");
	OutString = HeadersStr;

	OutString.Append(ValuesStr);
}


